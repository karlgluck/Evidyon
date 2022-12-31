//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#ifndef __GUIBORDEREDFILLELEMENT_H__
#define __GUIBORDEREDFILLELEMENT_H__
#pragma once


#include "guielement.h"
typedef unsigned long D3DCOLOR;

/**
 * Fills the entire source region, stretching the borders of the element only along a single axis,
 * not stretching the corners at all, and stretching the middle to fill the center area.
 */
class GUIBorderedFillElement : public GUIElement
{
  public:

    /**
     * Initializes the class
     */
    GUIBorderedFillElement();

    /**
     * Stores the texture for this element
     */
    void setTexture(int texture);

    void setColor(D3DCOLOR color);

    /**
     * Calculates the texture coordinates for the the source area for this element using
     * the dimensions of the current texture.  This method will not function unless
     * the layer has a texture for the internal index.
     */
    void setSourceRegion(const GUIRect* sourceArea, int marginPixels, GUILayer* layer);

    /**
     * Calculates the texture coordinates for the the source area for this element
     */
    void setSourceRegion(const GUIRect* sourceArea, int marginPixels, GUISize textureDimensions);


  public:

    /**
     * Renders this text list into the given layer.
     */
    bool render(GUILayer* layer);

    /**
     * Gets the texture used by this fill element
     */
    int getTexture();


  protected:

    /// The texture used by this fill element to render
    int myTexture;

    /// The diffuse color for this element
    D3DCOLOR myDiffuseColor;


  private:

    /// The size of the margin for this element
    int myMarginPixels;

    /// The element's texture coordinates
    ///  indexed by:  quad row, quad column, vertex row, vertex column, u/v
    float myTexcoords[3][3][2][2][2];

    /// Texture dimensions
    float myTexcoordsPerPixelWidth, myTexcoordsPerPixelHeight;
};




#endif