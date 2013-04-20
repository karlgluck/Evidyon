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
#ifndef __GUITEXTELEMENT_H__
#define __GUITEXTELEMENT_H__
#pragma once


#include "guielement.h"
class GUIFont;
typedef unsigned long D3DCOLOR;

/**
 * This element is used to render text of all sorts.  Implementing classes can be anything
 * from text labels, editable text lines or scrollable text boxes to lists of color-coded
 * entries (as in an inventory), map labels or player speech.
 */
class GUITextElement : public GUIElement
{
  protected:

    /**
     * This structure is filled by the derived class to define how to
     * render each line.  Note that the text will always be clipped to
     * the element's source canvas.
     */
    struct TextSegmentInfo
    {
      /// This text segment's screen location
      float x, y;

      /// Reference to the array of characters to draw
      const char* text;

      /// The maximum number of characters in this text line
      int lineLength;

      /// The amount to increase the Y-coordinate after this text renders
      int linesToIncrementAfterward;

      /// The color to render this text
      D3DCOLOR color;

      /// If this is set to a valid character, this will be rendered instead of each visible text characters
      char obscuringCharacter;
    };


  public:

    /**
     * Renders this text list into the given layer.
     */
    virtual bool render(GUILayer* layer);

    /**
     * Gets the font texture used by this element.  This method queries the getFont method
     * to obtain the font's texture.  It is invoked when the element is registered.
     */
    int getTexture();

    /**
     * Gets the font to use to render the segments of text
     */
    virtual GUIFont* getFont() = 0;


  protected:

    /**
     * Implemented by the derived class to get information about the next piece
     * of text to render.  This class calls this method with a 'null' parameter
     * to indicate that it is about to start a new render call and the subclass
     * should reset any necessary data.  This method will be called until the
     * return code is 'false'.  If the return code is 'true', it assumes the
     * information in nextSegmentInfo is correct and attempts to render it.
     */
    virtual bool getNextTextSegment(TextSegmentInfo* nextSegmentInfo) = 0;
};




#endif