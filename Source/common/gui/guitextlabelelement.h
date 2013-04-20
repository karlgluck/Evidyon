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
#ifndef __GUITEXTLABELELEMENT_H__
#define __GUITEXTLABELELEMENT_H__
#pragma once


#include "guitextelement.h"
#include <string>

enum Alignment {
  GUIALIGN_LEFT,
  GUIALIGN_CENTER,
  GUIALIGN_RIGHT,
  GUIALIGN_LEFT_TOP,
};

class GUIFont;

/**
 * Renders a single line of text that is centered horizontally within
 * the containing canvas.
 */
class GUITextLabelElement : public GUITextElement
{
  public:

    /**
     * Initializes the class
     */
    GUITextLabelElement();

    /**
     * Changes the font used to render this text
     */
    void setFont(GUIFont* font);

    /**
     * Changes the displayed text
     */
    void setText(const std::string& text);

    /**
     * Changes this text's color
     */
    void setTextColor(D3DCOLOR color);

    /**
     * Changes the alignment of this text
     */
    void setAlignment(Alignment alignment);

    const std::string& getText() const { return myText; }

    D3DCOLOR getTextColor() const { return myTextColor; }

    /**
     * Gets the font to use to render the this label
     */
    GUIFont* getFont();


  protected:

    /**
     * Returns the text for this label
     */
    bool getNextTextSegment(TextSegmentInfo* nextSegmentInfo);


  protected:

    /// The font to use to render this label
    GUIFont* myFont;

    /// The text to put in this label
    std::string myText;

    /// The color of text to render
    D3DCOLOR myTextColor;

    /// How to align the text in its source canvas
    Alignment myAlignment;
};




#endif