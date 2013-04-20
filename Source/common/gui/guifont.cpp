//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
//                                                                           //
//  Permission is hereby granted, free of charge, to any person obtaining a  //
//  copy of this software and associated documentation files (the            //
//  "Software"), to deal in the Software without restriction, including      //
//  without limitation the rights to use, copy, modify, merge, publish,      //
//  distribute, sublicense, and/or sell copies of the Software, and to       //
//  permit persons to whom the Software is furnished to do so, subject to    //
//  the following conditions:                                                //
//                                                                           //
//  The above copyright notice and this permission notice shall be included  //
//  in all copies or substantial portions of the Software.                   //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  //
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               //
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     //
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     //
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#include <d3dx9.h>
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include "guielement.h"
#include <map>
#include "guivertex.h"
#include "guiindex.h"
#include "guilayer.h"
#include "guifont.h"



//-----------------------------------------------------------------------------
// Name: GUIFont
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIFont::GUIFont(int id)
{
  zero();
  myID = id;
}



//-----------------------------------------------------------------------------
// Name: create
// Desc: Initializes the font
//-----------------------------------------------------------------------------
bool GUIFont::create(LPDIRECT3DDEVICE9 d3dDevice, const char* fontName, DWORD height, bool bold, bool italic)
{
  // Validate parameters
  if (!d3dDevice || !fontName || !height) return false;

  // Make sure the font is clear
  destroy();

  // These variables are used within this method
  HFONT hFont = NULL;
  HFONT hFontOld = NULL;
  HDC hDC = NULL;
  HBITMAP hbmBitmap = NULL;
  HGDIOBJ hbmOld = NULL;

  // Assume that fonts drawn onto the texture do not need scaling unless the texture size
  // required is larger than the device's maximum
  myTextScale = 1.0f;

  // Create a device context for text to draw into
  hDC = CreateCompatibleDC(NULL);
  SetMapMode(hDC, MM_TEXT);

  // Generate the font
  if (!createGDIFont(hDC, &hFont, fontName, height, bold, italic ))
  {
    // Free our context
    DeleteDC(hDC);

    // Return failure
    return false;
  }

  // Set this new font up for use (but store the old one so we can restore it later
  hFontOld = (HFONT)SelectObject(hDC, hFont);

  // Get this device's ability information
  D3DCAPS9 d3dCaps;
  d3dDevice->GetDeviceCaps(&d3dCaps);

  // Usually the required texture is at least 128x128 pixels.
  myTexWidth = myTexHeight = 128;

  // Holds the result of painting the font
  PaintAlphabetResult par;

  // Calculate the size of the smallest power-of-two texture which will hold all the
  // printable characters we need.  First, we try making the texture larger until it gets
  // bigger than the device can support.  Then, we scale down the text to fit the largest
  // texture.
  do
  {
    // If the texture size matches the device's maximum and we've started another
    // iteration, that means the text is too big.  Scale it down.
    if (myTexWidth == d3dCaps.MaxTextureWidth)
      myTextScale *= 0.9f;

    // If this texture is smaller than the device's maximum size, scale it up
    if (myTexWidth < d3dCaps.MaxTextureWidth)
    {
      myTexWidth *= 2;
      myTexHeight *= 2;
    }

    // If the texture is larger than the device can support, scale the text a bit and
    // make the texture the maximum size.
    if (myTexWidth > d3dCaps.MaxTextureWidth)
    {
      // Calculate the text scaling amount
      myTextScale = (float)d3dCaps.MaxTextureWidth / (float)myTexWidth;
      myTexWidth = myTexHeight = d3dCaps.MaxTextureWidth;
    }

    // If we are at the maximum sized texture, the text has been scaled so it needs to
    // be recreated.
    if (myTexWidth == d3dCaps.MaxTextureWidth)
    {
      // Get rid of the new font and put the old font back in
      DeleteObject(SelectObject(hDC, hFontOld));

      // Create the new font
      if (!createGDIFont(hDC, &hFont, fontName, height, bold, italic))
      {
        // Free information
        DeleteDC(hDC);

        // Return failure
        return false;
      }

      // Select the new font
      hFontOld = (HFONT)SelectObject(hDC, hFont);
    }

  } while(PAR_FAILED_NOTENOUGHSPACE == (par = paintAlphabet(hDC, true)));

  // Check to make sure the function didn't fail
  if (par != PAR_SUCCEEDED)
  {
    // Put the old font back into the device context
    SelectObject(hDC, hFontOld);

    // Delete used information
    DeleteObject(hFont);
    DeleteDC(hDC);

    // Erase internal info
    zero();

    // This method failed
    return false;
  }

  // Create the font's texture
  if (FAILED(d3dDevice->CreateTexture(myTexWidth, myTexHeight, 1, 0, D3DFMT_A4R4G4B4,
                      D3DPOOL_MANAGED, &myTexture, NULL)))
  {
    // Put the old font back into the device context
    SelectObject(hDC, hFontOld);

    // Delete used information
    DeleteObject(hFont);
    DeleteDC(hDC);

    // Erase internal info
    zero();

    // Return failure
    return false;
  }

  // Prepare to create the bitmap
  DWORD* bitmapBits;
  BITMAPINFO bmi;
  ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
  bmi.bmiHeader.biSize    = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth     =  (int)myTexWidth;
  bmi.bmiHeader.biHeight    = -(int)myTexHeight;
  bmi.bmiHeader.biPlanes    = 1;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biBitCount  = 32;

  // Create a bitmap for the font
  hbmBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&bitmapBits, NULL, 0);
  hbmOld = SelectObject(hDC, hbmBitmap);

  // Set up the text properties
  SetTextColor(hDC, RGB(255,255,255));
  SetBkColor(  hDC, 0x00000000);
  SetTextAlign(hDC, TA_TOP);

  // Actually draw the alphabet
  if (PAR_SUCCEEDED != paintAlphabet(hDC, false))
  {
    // Get rid of created Windows information
    SelectObject(hDC, hbmOld);
    SelectObject(hDC, hFontOld);
    DeleteObject(hbmBitmap);
    DeleteObject(hFont);
    DeleteDC(hDC);

    // Free internal data
    destroy();

    // Return error
    return false;
  }

  // Lock the surface and manually write the alpha values for the pixels
  D3DLOCKED_RECT d3dlr;
  if (FAILED(myTexture->LockRect(0, &d3dlr, 0, 0)))
  {
    // Get rid of created Windows information
    SelectObject(hDC, hbmOld);
    SelectObject(hDC, hFontOld);
    DeleteObject(hbmBitmap);
    DeleteObject(hFont);
    DeleteDC(hDC);

    // Free internal data
    destroy();

    // Return error
    return false;
  }

  // This data is used to fill in the texture
  BYTE* destRow;
  destRow = (BYTE*)d3dlr.pBits;
  WORD* dest16;
  BYTE alpha;
  DWORD x, y;

  // Write the alpha values
  for (y = 0; y < myTexHeight; ++y)
  {
    dest16 = (WORD*)destRow;
    for (x = 0; x < myTexWidth; ++x)
    {
      // Determine whether or not a pixel was written here
      alpha = (BYTE)((bitmapBits[myTexWidth * y + x] & 0xFF) >> 4);

      // Write the alpha value
      if (alpha > 0)
        *dest16++ = ((WORD)(alpha << 12) | 0x0FFF); // updated to remove alpha b/c it causes rendering artifacts
      else
        *dest16++ = 0x0000;
    }

    // Move to the next row
    destRow += d3dlr.Pitch;
  }

  // Unlock the texture
  myTexture->UnlockRect(0);

  // Restore states and delete temporary information
  SelectObject(hDC, hbmOld);
  SelectObject(hDC, hFontOld);
  DeleteObject(hbmBitmap);
  DeleteObject(hFont);
  DeleteDC(hDC);

  // Success
  return true;
}




//----[  createOutlined  ]-----------------------------------------------------
bool GUIFont::createOutlined(LPDIRECT3DDEVICE9 d3d_device,
                             const char* font_name,
                             DWORD height,
                             bool bold,
                             bool italic,
                             D3DCOLOR outline_color,
                             int lower_threshold,
                             int upper_threshold) {
  if (!create(d3d_device, font_name, height, bold, italic)) return false;

  // Lock the surface and manually write the alpha values for the pixels
  D3DLOCKED_RECT d3dlr;
  if (FAILED(myTexture->LockRect(0, &d3dlr, 0, 0)))
  {
    // Free internal data
    destroy();

    // Return error
    return false;
  }

  // This data is used to fill in the texture
  BYTE* destRow;
  destRow = (BYTE*)d3dlr.pBits;
  WORD* dest16;
  BYTE alpha;
  DWORD x, y;

  // Write the alpha values
  for (y = 0; y < myTexHeight; ++y)
  {
    dest16 = (WORD*)destRow;
    for (x = 0; x < myTexWidth; ++x)
    {
      // Determine whether or not a pixel was written here
      alpha = 0;

      // Write the alpha value
      if (alpha > 0)
        *dest16++ = ((WORD)(0xFF << 12) | 0x0FFF); //((WORD)(alpha << 12) | 0x0FFF); // updated to remove alpha b/c it causes rendering artifacts
      else
        *dest16++ = 0x0000;
    }

    // Move to the next row
    destRow += d3dlr.Pitch;
  }

  // Unlock the texture
  myTexture->UnlockRect(0);

  return true;
}


//-----------------------------------------------------------------------------
// Name: create
// Desc: 
//-----------------------------------------------------------------------------
bool GUIFont::create(LPDIRECT3DTEXTURE9 fontTexture, DWORD letterWidth, DWORD letterHeight,
            const DWORD characterIndices[128-32], float lineSpacingFactor, DWORD characterSpacing)
{
  destroy();

  if (!fontTexture || !characterIndices) return false;

  (myTexture = fontTexture)->AddRef();  // Store the texture internally

  D3DSURFACE_DESC desc;
  fontTexture->GetLevelDesc(0, &desc);
  myTexWidth = desc.Width;
  myTexHeight = desc.Height;

  myTextScale = 1.0f;
  myLineHeightInPixels = lineSpacingFactor * letterHeight;
  mySpacing = characterSpacing;

  int charactersWide = myTexWidth / letterWidth;
  int lastCharacterRowIndex = myTexHeight / letterHeight - 1;

  float x_pixel_to_texcoord = 1.0f / desc.Width;
  float y_pixel_to_texcoord = 1.0f / desc.Height;

  for (int character = 32; character < 128; ++character)
  {
    DWORD index = characterIndices[character-32];
    int characterX = index % charactersWide;
    int characterY = index / charactersWide;
    if (characterY > lastCharacterRowIndex) lastCharacterRowIndex;
    int x = (characterX * letterWidth);
    int y = (characterY * letterHeight);
    myTexCoords[character-32][0] = x_pixel_to_texcoord * (float)(x);
    myTexCoords[character-32][1] = y_pixel_to_texcoord * (float)(y);
    myTexCoords[character-32][2] = x_pixel_to_texcoord * (float)(x + letterWidth);
    myTexCoords[character-32][3] = y_pixel_to_texcoord * (float)(y + letterHeight);
  }

  return true;
}



//-----------------------------------------------------------------------------
// Name: destroy
// Desc: Clears out of the allocated memory for this font
//-----------------------------------------------------------------------------
void GUIFont::destroy()
{
  // Free the texture, if it exists
  if (myTexture) myTexture->Release();

  // Reset class members
  zero();
}



//-----------------------------------------------------------------------------
// Name: renderText
// Desc: Renders text into the target layer's geometry buffers.  If the clipRect parameter
//     is valid, the text is cut (even mid-character) so that it fits inside of the rectangle.
//-----------------------------------------------------------------------------
bool GUIFont::renderText(GUILayer* targetLayer, const GUIRect* clipRect, float x, float y,
              float depth, const char* text, int chars, D3DCOLOR color, char obscuringCharacter)
{
  // Keeps track of how many characters have been processed/rendered
  int charsProcessed = 0, charsRendered = 0;

  // Set up the render locations
  x -= mySpacing;
  float startX = x;

  // Calculate the number of characters, if it's not specified
  if (chars < 0) chars = strlen(text);

  // This flag is set if text is being obscured
  bool obscuringText = obscuringCharacter >= 32 && obscuringCharacter < 128;

  // Calculate the number of renderable characters
  int renderableCharacters = 0;
  for (int i = 0; i < chars; ++i)
  {
    char c = text[i];
    if (c >= 32 && (c < 128 || obscuringText))
      ++renderableCharacters;
  }
  if (!renderableCharacters) return true;

  // Pointer to the destination vertices
  GUIVertex* vertices;

  // Lock the vertex buffer
  size_t availableQuads = 0;

  // This data is used for clipping pixels
  float texcoordsPerPixelWide = 1.0f / myTexWidth;
  float texcoordsPerPixelHigh = 1.0f / myTexHeight;

  // Local variable used to build vertices
  GUIVertex quadVertices[4];

  // Initialize vertex colors and depths
  for (int v = 0; v < 4; ++v)
  {
    quadVertices[v].depth = depth;
    quadVertices[v].color = color;
  }

  // Iterate through the text and add characters to the buffer.
  // Note that this process WILL render the ' ' character!
  while(*text && charsProcessed < chars)
  {
    // Get the character to render, then move to the next character in the text string
    char c = obscuringText ? obscuringCharacter : *text;
    text++;

    // Make sure there is room in the buffer for this
    if (!availableQuads)
    {
      // If we've rendered characters, flush the target layer's geometry because it's full
      if (charsRendered)
      {
        // If we failed to render, exit
        if (!targetLayer->flush()) return false;
      }

      // Calculate the number of quads we need
      size_t quadsRequested = (size_t)(renderableCharacters - charsRendered);
      availableQuads = quadsRequested;

      // Allocate more space for geometry
      if (!targetLayer->allocateQuadListGeometry(&availableQuads, &vertices))
      {
        // If there are no quads available, flush the buffer and try again
        if (!availableQuads) { targetLayer->flush(); availableQuads = quadsRequested; }

        // If the layer couldn't find quads to allocate, or the subsequent allocation fails,
        // this layer is invalid and we can't render anymore.  Bail out!
        if (!targetLayer->allocateQuadListGeometry(&availableQuads, &vertices) || !availableQuads)
          return false;
      }
    }

    // Increment the number of processed characters
    ++charsProcessed;

    // Reset on the next line if a newline character is encountered
    if (c == '\n')
    {
      x = startX;
      y += myLineHeightInPixels;
    }

    // Don't render characters outside of the given range
    if (c < 32 || c >= 128)
      continue;

    // Calculate texture coordinates
    float tx1 = myTexCoords[c-32][0];
    float ty1 = myTexCoords[c-32][1];
    float tx2 = myTexCoords[c-32][2];
    float ty2 = myTexCoords[c-32][3];

    // Get the width and height of this character
    float w = (tx2-tx1) *  myTexWidth / myTextScale;
    float h = (ty2-ty1) * myTexHeight / myTextScale;

    // Top-left vertex
    quadVertices[0].x = x + 0 - 0.5f;
    quadVertices[0].y = y + 0 - 0.5f;
    quadVertices[0].u = tx1;
    quadVertices[0].v = ty1;

    // Top-right vertex
    quadVertices[1].x = x + w - 0.5f;
    quadVertices[1].y = y + 0 - 0.5f;
    quadVertices[1].u = tx2;
    quadVertices[1].v = ty1;

    // Bottom-left vertex
    quadVertices[2].x = x + 0 - 0.5f;
    quadVertices[2].y = y + h - 0.5f;
    quadVertices[2].u = tx1;
    quadVertices[2].v = ty2;

    // Bottom-right vertex
    quadVertices[3].x = x + w - 0.5f;
    quadVertices[3].y = y + h - 0.5f;
    quadVertices[3].u = tx2;
    quadVertices[3].v = ty2;

    // Perform clipping of the rectangle, if necessary
    if (clipRect)
    {
      // Check the left vertices for left-clipping
      for (int i = 0; i < 4; i += 2)
      {
        if (quadVertices[i].x < clipRect->left)
        {
          float newX = min(clipRect->left, quadVertices[i+1].x);
          quadVertices[i].u += (newX - quadVertices[i].x) * texcoordsPerPixelWide;
          quadVertices[i].x = newX;
        }
      }

      // Check the right vertices for right-clipping
      for (int i = 1; i < 4; i += 2)
      {
        if (quadVertices[i].x > clipRect->right)
        {
          float newX = max(clipRect->right, quadVertices[i-1].x);
          quadVertices[i].u += (newX - quadVertices[i].x) * texcoordsPerPixelWide;
          quadVertices[i].x = newX;
        }
      }

      // Check the top vertices for top-clipping
      for (int i = 0; i < 2; i++)
      {
        if (quadVertices[i].y < clipRect->top)
        {
          float newY = min(clipRect->top, quadVertices[i+2].y);
          quadVertices[i].v += (newY - quadVertices[i].y) * texcoordsPerPixelHigh;
          quadVertices[i].y = newY;
        }
      }

      // Check the bottom vertices for bottom-clipping
      for (int i = 2; i < 4; i++)
      {
        if (quadVertices[i].y > clipRect->bottom)
        {
          float newY = max(clipRect->bottom, quadVertices[i-2].y);
          quadVertices[i].v += (newY - quadVertices[i].y) * texcoordsPerPixelHigh;
          quadVertices[i].y = newY;
        }
      }
    }

    // Copy the quad's vertices into the vertex buffer
    memcpy(vertices, &quadVertices, sizeof(quadVertices));

    // Advance to the next quad
    vertices += 4;

    // Advance the number of characters rendered and amount of buffer used
    availableQuads--;
    ++charsRendered;

    // Advance the x-coordinate
    x += w - (2 * mySpacing);
  }

  // Success
  return true;
}




//-----------------------------------------------------------------------------
// Name: getLineHeightInPixels
// Desc: This method obtains the height, in pixels, of a line of text printed in this font
//-----------------------------------------------------------------------------
float GUIFont::getLineHeightInPixels() const
{
  return myLineHeightInPixels;
}




//-----------------------------------------------------------------------------
// Name: getTexture
// Desc: Gets the texture used to render this font.  This method does not increment the refcount.
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 GUIFont::getTexture()
{
  return myTexture;
}


//-----------------------------------------------------------------------------
// Desc: Obtains the ID into which this font's texture will be registered
//-----------------------------------------------------------------------------
int GUIFont::getTextureID() const
{
  return myID;
}



//-----------------------------------------------------------------------------
// Name: centerTextInArea
// Desc: Calculates the location at which to begin rendering so that the text string
//     will render centered horizontally inside of the given area.
//-----------------------------------------------------------------------------
GUISize GUIFont::centerTextInArea(const char* text,
                                  int chars,
                                  const GUIRect* area,
                                  GUIPoint* renderPoint) const
{
  // Calculate this text's area
  GUISize s;
  getTextExtent(text, chars, &s);

  // Center the text area in the rectangle
  int areaCenterX = (area->right + area->left) / 2;
  int areaCenterY = (area->bottom + area->top) / 2;
  int textCenterX = (s.width) / 2;
  int textCenterY = (s.height) / 2;

  renderPoint->x = areaCenterX - textCenterX;
  renderPoint->y = areaCenterY - textCenterY;

  return s;
}



//-----------------------------------------------------------------------------
// Name: getTextExtent
// Desc: Determines the pixel size of the given segment of text
//-----------------------------------------------------------------------------
bool GUIFont::getTextExtent(const char* text, int chars, GUISize* size) const
{
  // This function fails if data isn't provided
  if (!text || !size) return false;

  // Variables used to calculate size
  float rowWidth  = 0.0f;
  float rowHeight = myLineHeightInPixels;
  float width   = 0.0f;
  float height  = rowHeight;
  int charsCalculated = 0;

  // Run through the text and calculate its size
  while(*text)
  {
    // If we've reached the calculation limit, exit the loop
    if ((chars != -1) && (charsCalculated > chars))
      break;

    // Get the character
    char c = *text++;

    // If this character breaks the line, move on
    if (c == '\n')
    {
      rowWidth = 0.0f;
      height += rowHeight;
    }

    // Increment the number of calculated characters
    charsCalculated++;

    // We can only process a certain range of characters
    if (c < 32 || c >= 128)
      continue;

    // Get texture coordinates
    float tx1 = myTexCoords[c-32][0];
    float tx2 = myTexCoords[c-32][2];

    // Increment the row width
    rowWidth += (tx2-tx1) * myTexWidth - 2 * mySpacing;

    // If the width is bigger than the current width, store the new width
    if (rowWidth > width)
      width = rowWidth;
  }

  // Store the size
  size->width  = (short)width;
  size->height = (short)height;

  // Success
  return true;
}



//----[  wrapTextToWidth  ]----------------------------------------------------
void GUIFont::wrapTextToWidth(char* text_buffer, int width) {
  char* line_start = text_buffer;
  char* space_after_last_word = text_buffer;
  char* read_point = text_buffer;
  while (*read_point != '\0') {
    // move to to the next word
    while (*read_point != '\0' && *read_point != ' ') {
      ++read_point;
    }

    // Swap in an endline to make getTextExtent work
    char old_ch = *read_point;
    *read_point = '\0';

    // check the size of the word
    GUISize line_size;
    getTextExtent(line_start, -1, &line_size);
    *read_point = old_ch; // restore the read point character
    if (line_size.width >= width) { // too wide?
      if (space_after_last_word != line_start) {
        *space_after_last_word = '\n'; // turn the space into a carriage return
        *read_point = old_ch; // restore the character at the read point
        line_start = space_after_last_word + 1; // begin after last line
        space_after_last_word = line_start;
        while (*line_start == ' ') ++line_start; // skip spaces
      } else {
        // turn this location into a return, even though its past the line
        if (read_point != '\0') {
          *read_point = '\n';
          line_start = read_point;
        }
      }
      read_point = line_start; // start reading
    } else {
      space_after_last_word = read_point;
      while (*read_point == ' ') ++read_point; // move up to the next word
    }
  }

}




//-----------------------------------------------------------------------------
// Name: getCaretSize
// Desc: Calculates the size that the caret for editing this text should be
//-----------------------------------------------------------------------------
GUISize GUIFont::getCaretSize() const
{
  GUISize size;
  size.height = (short)((myTexCoords[0][3] - myTexCoords[0][1]) * myTexHeight);
  size.width  = (short)(size.height * 0.2f);
  return size;
}



//-----------------------------------------------------------------------------
// Name: getCharacterFromPoint
// Desc: Determines which character's bounding square contains the provided location.  If the
//     location is outside of the segment of text, 'false' is returned; otherwise, 'true' is returned
//     and the character index is set.  This point should be relative to the point used to render
//     the segment of text (the top-left corner of the first character)
//-----------------------------------------------------------------------------
bool GUIFont::getCharacterFromPoint(const char* text, int chars, GUIPoint relativePoint, int* characterIndex) const
{
  // Basically go through the same process as for calculating the size of a text string, but
  // when the point is between the top and bottom of the line and between the left and right
  // boundaries of a character, return it.

  // Validate parameters
  if (!text || !characterIndex) return false;

  // Set the default output
  if (relativePoint.x < 0)
  {
    *characterIndex = 0;
    return true;
  }

  // Variables used to calculate size
  float rowWidth  = 0.0f;
  float rowHeight = myLineHeightInPixels;
  int charsCalculated = 0;
  float height = rowHeight;

  // Run through the text and calculate its size
  while(*text)
  {
    // If we've reached the calculation limit, exit the loop
    if ((chars != -1) && (charsCalculated > chars))
      break;

    // Get the character
    char c = *text++;

    // If this character breaks the line, move on
    if (c == '\n')
    {
      rowWidth = 0.0f;
      height += rowHeight;
    }

    // If the user wants us to find both X and Y values and this row couldn't possibly
    // contain the point yet, move on
    if ((relativePoint.y != -1) && height < relativePoint.y)
      continue;

    // If the user wants us to search X and Y coordinates and this row is too far down,
    // we're done searching
    if ((relativePoint.y != -1) && ((height - rowHeight) > relativePoint.y))
      break;

    // Increment the number of calculated characters
    charsCalculated++;

    // We can only process a certain range of characters
    if (c < 32 || c >= 128)
      continue;

    // Get texture coordinates
    float tx1 = myTexCoords[c-32][0];
    float tx2 = myTexCoords[c-32][2];

    float charWidth = (tx2-tx1) * myTexWidth - 2 * mySpacing;

    // Determine if this character overlaps the point
    if ((rowWidth <= relativePoint.x) && (rowWidth + charWidth >= relativePoint.x))
    {
      // Store the character
      *characterIndex = charsCalculated - 1;
      
      // Found the character
      return true;
    }

    // Increment the row width
    rowWidth += charWidth;
  }

  // This point is somewhere beyond the last character
  *characterIndex = charsCalculated;

  // Couldn't find the point
  return false;
}



//-----------------------------------------------------------------------------
// Name: createGDIFont
// Desc: Creates a GDI font that will be used to render the alphabet for the texture
//-----------------------------------------------------------------------------
bool GUIFont::createGDIFont(HDC hDC, HFONT* font, const char* fontName, DWORD height, bool bold, bool italic)
{
  // Get the font's size.  I don't actually know why this works.
  int size = -MulDiv(height, (int)(GetDeviceCaps(hDC, LOGPIXELSY) * myTextScale), 72);

  // Generate the font
  *font = CreateFont(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL,
            italic ? TRUE : FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
            VARIABLE_PITCH, fontName);
  
  // Make sure it worked
  return *font != NULL;
}



//-----------------------------------------------------------------------------
// Name: paintAlphabet
// Desc: Puts the font's alphabet on the texture
//-----------------------------------------------------------------------------
GUIFont::PaintAlphabetResult GUIFont::paintAlphabet(HDC hDC, bool measureOnly)
{
  // Set up variables used to paint the texture
  SIZE size;      // Holds calculated character sizes
  char str[2] = "x";  // Which character is being assigned

  // Calculate the spacing between characters based on line height
  if (0 == GetTextExtentPoint32(hDC, str, 1, &size)) return PAR_FAILED_FATAL;
  mySpacing = (DWORD)ceil(size.cy * 0.3f);

  // Set the painting start coordinates
  DWORD x = mySpacing;
  DWORD y = 0;

  // Draw the characters.  We're actually drawing the characters 32-127 but it is more
  // efficient to make this loop zero-based.
  for (char c = 0; c < 127 - 32; ++c)
  {
    // Set up the string to represent the character to paint
    str[0] = c + 32;

    // Get the size of this character
    if (0 == GetTextExtentPoint32(hDC, str, 1, &size)) return PAR_FAILED_FATAL;

    // Wrap characters around on the texture
    if ((DWORD)(x + size.cx + mySpacing) > myTexWidth)
    {
      x = mySpacing;
      y += size.cy + 1;
    }

    // Check to see if there is enough room to write characters
    if (y + size.cy > myTexHeight)
      return PAR_FAILED_NOTENOUGHSPACE;

    // If we are not just measuring, print the characters
    if (!measureOnly)
    {
      // Draw to the device context
      if (0 == ExtTextOut(hDC, x + 0, y + 0, ETO_OPAQUE, NULL, str, 1, NULL))
        return PAR_FAILED_FATAL;

      // Set up the texture coordinates
      myTexCoords[c][0] = ((float)(x + 0     - mySpacing))/myTexWidth;
      myTexCoords[c][1] = ((float)(y + 0     + 0     ))/myTexHeight;
      myTexCoords[c][2] = ((float)(x + size.cx + mySpacing))/myTexWidth;
      myTexCoords[c][3] = ((float)(y + size.cy + 0     ))/myTexHeight;
    }

    // Advance the character location
    x += size.cx + (2 * mySpacing);
  }

  // Save the height of a line of text
  myLineHeightInPixels = size.cy;

  // Success
  return PAR_SUCCEEDED;
}




//-----------------------------------------------------------------------------
// Name: zero
// Desc: Resets all the members of this class
//-----------------------------------------------------------------------------
void GUIFont::zero()
{
  myTexture = 0;
  myTexWidth = 0;
  myTexHeight = 0;
  myTextScale = 0.0f;
  ZeroMemory(myTexCoords, sizeof(float) * (128-32) * 4);
  myLineHeightInPixels = 0.0f;
  mySpacing = 0;
}