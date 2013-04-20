//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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