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
#ifndef __GUIPICTUREELEMENT_H__
#define __GUIPICTUREELEMENT_H__
#pragma once


#include "guielement.h"
typedef unsigned long D3DCOLOR;
struct GUIRect;

/**
 * Puts a picture on the screen without distorting it
 */
class GUIPictureElement : public GUIElement
{
  public:

    /**
     * Initializes the class
     */
    GUIPictureElement();

    /**
     * Stores the texture for this element
     */
    void setTexture(int texture);

    /**
     * Calculates the texture coordinates for the the source area for this element using
     * the dimensions of the current texture.  This method will not function unless
     * the internal texture is currently valid.
     */
    void setSourceRegion(const GUIRect* sourceArea, GUILayer* layer);

    /**
     * Calculates the texture coordinates for the the source area for this element
     */
    void setSourceRegion(const GUIRect* sourceArea, GUISize textureDimensions);

  public:

    /**
     * Renders this text list into the given layer.
     */
    bool render(GUILayer* layer);

    /**
     * Gets the texture used by this fill element
     */
    int getTexture();

    /**
     * Gets the width of the picture this element represents
     */
    float getPictureWidth() const;

    /**
     * Gets the height of the picture this element represents
     */
    float getPictureHeight() const;


  protected:

    /**
     * Draws this picture at the provided coordinates
     */
    bool render(GUILayer* layer, float x, float y, float depth);

    /**
     * Draws this picture centered around the given coordinates
     */
    bool renderCentered(GUILayer* layer, float centerX, float centerY, float depth);


  protected:

    /// The texture used by this element to render
    int myTexture;

    /// The diffuse color for this element
    D3DCOLOR myDiffuseColor;

  private:

    /// The size of this picture
    float myPictureWidth, myPictureHeight;

    /// The element's texture coordinates
    ///  indexed by:  vertex row, vertex column, u/v
    float myTexcoords[2][2][2];
};




#endif