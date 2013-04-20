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
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guilayer.h"
#include "guipictureelement.h"
#include "guiquad.h"
#include "guitexture.h"


//-----------------------------------------------------------------------------
// Name: GUIPictureElement
// Desc: Initializes the class
//-----------------------------------------------------------------------------
GUIPictureElement::GUIPictureElement()
{
  myTexture = 0;
  myDiffuseColor = D3DCOLOR_XRGB(255,255,255);
  ZeroMemory(myTexcoords, sizeof(myTexcoords));
}


//-----------------------------------------------------------------------------
// Name: setTexture
// Desc: Stores the texture for this element
//-----------------------------------------------------------------------------
void GUIPictureElement::setTexture(int texture)
{
  myTexture = texture;
}



//-----------------------------------------------------------------------------
// Name: setSourceRegion
// Desc: Calculates the texture coordinates for the the source area for this element using
//     the dimensions of the current texture.  This method will not function unless
//     the internal texture is currently valid.
//-----------------------------------------------------------------------------
void GUIPictureElement::setSourceRegion(const GUIRect* sourceArea, GUILayer* layer) {
  //if (!layer || !sourceArea) {
  //  memset(myTexcoords, 0, sizeof(myTexcoords));
  //  return;
  //}

  GUITexture* gui_texture = layer->getTexture(myTexture);
  if (!gui_texture) return;
  setSourceRegion(sourceArea, gui_texture->getSize());
}



//-----------------------------------------------------------------------------
// Name: setSourceRegion
// Desc: Calculates the texture coordinates for the the source area for this element
//-----------------------------------------------------------------------------
void GUIPictureElement::setSourceRegion(const GUIRect* sourceArea, GUISize textureDimensions)
{
  // Calculate the texture coordinates from the pixel locations
  float u1, u2, v1, v2;

  // Use the whole region if the source area is invalid
  if (!sourceArea) {
    u1 = 0.0f; u2 = 1.0f;
    v1 = 0.0f; v2 = 1.0f;
    myPictureWidth = (float)textureDimensions.width;
    myPictureHeight = (float)textureDimensions.height;
  } else {

    float textureWidth  = (float)textureDimensions.width;
    float textureHeight = (float)textureDimensions.height;

    // Save the dimensions of the texture
    myPictureWidth  = (float)(sourceArea->right - sourceArea->left);
    myPictureHeight = (float)(sourceArea->bottom - sourceArea->top);

    // Calculate the texture coordinates from the pixel locations
    u1 = sourceArea->left / textureWidth;
    u2 = sourceArea->right / textureWidth;

    // Calculate the texture coordinates from the pixel locations
    v1 = sourceArea->top / textureHeight;
    v2 = sourceArea->bottom / textureHeight;
  }


  // Write each of the vertices
  for (int vertexRow = 0; vertexRow < 2; ++vertexRow)
  {
    // Set the v coordinate
    float v = vertexRow == 0 ? v1 : v2;

    for (int vertexCol = 0; vertexCol < 2; ++vertexCol)
    {
      // Set the u coordinate
      float u = vertexCol == 0 ? u1 : u2;

      // Save this texcoord
      myTexcoords[vertexRow][vertexCol][0] = u;
      myTexcoords[vertexRow][vertexCol][1] = v;
    }
  }
}



//-----------------------------------------------------------------------------
// Name: render
// Desc: Renders this text list into the given layer.
//-----------------------------------------------------------------------------
bool GUIPictureElement::render(GUILayer* layer)
{
  // Only render if the source canvas is valid
  if (!mySourceCanvas) return true;

  const GUIRect* destArea = mySourceCanvas->getArea();
  float depth = mySourceCanvas->getDepth();

  return render(layer, destArea->left, destArea->top, depth);
}



//-----------------------------------------------------------------------------
// Name: getTexture
// Desc: Gets the texture used by this fill element
//-----------------------------------------------------------------------------
int GUIPictureElement::getTexture()
{
  return myTexture;
}





//-----------------------------------------------------------------------------
// Name: render
// Desc: Draws this picture at the provided coordinates
//-----------------------------------------------------------------------------
bool GUIPictureElement::render(GUILayer* layer, float x, float y, float depth)
{
  // Only render if the source canvas is valid
  if (!mySourceCanvas) return true;

  // Get the target area
  D3DCOLOR color = myDiffuseColor;


  float x2 = x + myPictureWidth,
      y2 = y + myPictureHeight;

  // Allocate space for this element
  size_t quads = 1;
  GUIVertex* vertices;
  if (!layer->allocateQuadListGeometry(&quads, &vertices))
  {
    // Try flushing the buffer, and reallocating
    quads = 1;
    if (!layer->flush() || !layer->allocateQuadListGeometry(&quads, &vertices))
      return false; // There isn't enough space in the layer for this element
  }

  GUIQuad quad;
  quad.initialize(depth, color);

  // The vertex index being initialized
  int v = 0;
  for (int vertexRow = 0; vertexRow < 2; ++vertexRow)
  {
    // Set the y coordinate
    float yc = vertexRow == 0 ? y : y2;

    for (int vertexCol = 0; vertexCol < 2; ++vertexCol)
    {
      // Set the x coordinate
      float xc = vertexCol == 0 ? x : x2;

      // Set the vertex
      quad.vertices[v].x = xc;
      quad.vertices[v].y = yc;
      quad.vertices[v].depth = depth;
      quad.vertices[v].color = color;
      quad.vertices[v].u = myTexcoords[vertexRow][vertexCol][0];
      quad.vertices[v].v = myTexcoords[vertexRow][vertexCol][1];
      ++v;
    }
  }

  // Clip this picture
  if (myClippingArea)
    quad.clipSquare(myClippingArea, (myTexcoords[0][1][0] - myTexcoords[0][0][0]) / myPictureWidth,
                   (myTexcoords[1][0][1] - myTexcoords[0][0][1]) / myPictureHeight);

  // Copy the quad
  quad.copyTo(vertices);

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name: renderCentered
// Desc: Draws this picture centered around the given coordinates
//-----------------------------------------------------------------------------
bool GUIPictureElement::renderCentered(GUILayer* layer, float centerX, float centerY, float depth)
{
  // Only render if the source canvas is valid
  if (!mySourceCanvas) return true;

  // Get the centered coordinates of the destination area
  const GUIRect* destArea = mySourceCanvas->getArea();
  return render(layer, centerX - myPictureWidth  / 2,
              centerY - myPictureHeight / 2, depth);
}




//-----------------------------------------------------------------------------
// Name: getPictureWidth
// Desc: Gets the width of the picture this element represents
//-----------------------------------------------------------------------------
float GUIPictureElement::getPictureWidth() const
{
  return myPictureWidth;
}



//-----------------------------------------------------------------------------
// Name: getPictureHeight
// Desc: Gets the height of the picture this element represents
//-----------------------------------------------------------------------------
float GUIPictureElement::getPictureHeight() const
{
  return myPictureHeight;
}
