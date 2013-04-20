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
#ifndef __GUIFONT_H__
#define __GUIFONT_H__
#pragma once


#include <d3d9.h>

/**
 * This class is used to render text to the screen
 */
class GUIFont
{
  enum PaintAlphabetResult
  {
    PAR_SUCCEEDED,
    PAR_FAILED_FATAL,
    PAR_FAILED_NOTENOUGHSPACE,
  };

  public:

    /**
     * Initializes this class
     */
    GUIFont(int id);

    /**
     * Initializes the font
     */
    bool create(LPDIRECT3DDEVICE9 d3dDevice, const char* fontName, DWORD height, bool bold = false, bool italic = false);

    bool createOutlined(LPDIRECT3DDEVICE9 d3d_device,
                        const char* font_name,
                        DWORD height,
                        bool bold,
                        bool italic,
                        D3DCOLOR outline_color,
                        int lower_threshold,
                        int upper_threshold);
    /**
     * Create the font from a bitmapped font.  The character indices should specify the locations of ASCII
     * characters 32 through 128.
     */
    bool create(LPDIRECT3DTEXTURE9 fontTexture, DWORD letterWidth, DWORD letterHeight, const DWORD characterIndices[128-32], float lineSpacingFactor, DWORD characterSpacing);

    /**
     * Clears out of the allocated memory for this font
     */
    void destroy();

    /**
     * Renders text into the target layer's geometry buffers.  If the clipRect parameter
     * is valid, the text is cut (even mid-character) so that it fits inside of the rectangle.
     * If the obscuringCharacter parameter is valid (between 32 and 128), the given character
     * will be rendered instead of any character in the text input.
     */
    bool renderText(GUILayer* targetLayer, const GUIRect* clipRect, float x, float y,
             float depth, const char* text, int chars, D3DCOLOR color, char obscuringCharacter = '\0');

    /**
     * This method obtains the height, in pixels, of a line of text printed in this font
     */
    float getLineHeightInPixels() const;

    /**
     * Gets the texture used to render this font.  This method not increment the refcount.
     */
    LPDIRECT3DTEXTURE9 getTexture();

    /**
     * Obtains the ID into which this font's texture will be registered
     */
    int getTextureID() const;

    /**
     * Calculates the location at which to begin rendering so that the text string
     * will render centered horizontally inside of the given area.
     * @param chars If this is -1, the text is null-terminated; otherwise, defines the number of characters to examine
     * This method returns the size of the centered text
     */
    GUISize centerTextInArea(const char* text, int chars, const GUIRect* area, GUIPoint* renderPoint) const;

    /**
     * Determines the pixel size of the given segment of text
     * @param chars If this is -1, the text is null-terminated; otherwise, defines the number of characters to examine
     */
    bool getTextExtent(const char* text, int chars, GUISize* size) const;


    /**
     * Calculates the location at which to begin rendering so that the text string
     * will render centered vertically inside of the given area
     */
    //void centerTextVertically(const RECT* area, POINT* renderPoint);

    // Overwrites spaces in the text buffer where necessary to wrap the text
    // into the given area.  Very long lines of text with no spaces are not
    // broken up--they will extend past the width.  Carriage returns embedded
    // within the text are preserved.  The buffer must be null-terminated.
    void wrapTextToWidth(char* text_buffer, int width);

    /**
     * Turns all '\n' into ' '
     */
    //void unwrapText(char* textBuffer, int chars);

    /**
     * Calculates the size that the caret for editing this text should be
     */
    GUISize getCaretSize() const;

    /**
     * Determines which character's bounding square contains the provided location.  If the
     * location is outside of the segment of text, 'false' is returned; otherwise, 'true' is returned
     * and the character index is set.  This point should be relative to the point used to render
     * the segment of text (the top-left corner of the first character).
     * @param chars If this is -1, the text is null-terminated; otherwise, defines the number of characters to examine
     * @param relativePoint If a parameter is -1, it is not searched
     */
    bool getCharacterFromPoint(const char* text, int chars, GUIPoint relativePoint, int* characterIndex)  const;


  protected:

    /**
     * Creates a GDI font that will be used to render the alphabet for the texture
     */
    bool createGDIFont(HDC hDC, HFONT* font, const char* fontName, DWORD height, bool bold, bool italic);

    /**
     * Puts the font's alphabet on the texture
     */
    PaintAlphabetResult paintAlphabet(HDC hDC, bool measureOnly);

    /**
     * Resets all the members of this class
     */
    void zero();


  public:

    int myID;

    /// This is the texture created by the font class which holds the characters
    LPDIRECT3DTEXTURE9 myTexture;

    /// This is the font texture's width
    DWORD myTexWidth;

    /// The font texture's height
    DWORD myTexHeight;

    /// How much the font is scaled on the texture
    float myTextScale;

    /// The texture coordinates for each of the standard characters
    float myTexCoords[128-32][4];

    /// The height of a line of text
    float myLineHeightInPixels;

    /// How much spacing there is horizontally between each character
    DWORD mySpacing;
};




#endif