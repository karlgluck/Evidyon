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
#include "stdafx.h"


//-----------------------------------------------------------------------------
// Name: EvidyonGUIListEntryText
// Desc: Initializes the class
//-----------------------------------------------------------------------------
EvidyonGUIListEntryText::EvidyonGUIListEntryText(GUIList::ListEntry* owner)
{
    myTextLabelCanvas.alignXLeft(0);
    myTextLabelCanvas.alignYTop(0);
    myTextLabelCanvas.relativeHeight(0);
    myTextLabelCanvas.relativeWidth(0);
    myTextLabelCanvas.relativeDepth(-0.01f);

    myTextLabelElement.setSourceCanvas(&myTextLabelCanvas);

    owner->addChildCanvas(&myTextLabelCanvas);
}



//-----------------------------------------------------------------------------
// Name: getCanvas
// Desc: Obtains the canvas for this element
//-----------------------------------------------------------------------------
GUICanvas* EvidyonGUIListEntryText::getCanvas()
{
    return &myTextLabelCanvas;
}



//-----------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font used to render this text.  The font must remain
//       valid.
//-----------------------------------------------------------------------------
void EvidyonGUIListEntryText::setFont(GUIFont* font)
{
    myTextLabelElement.setFont(font);
}



//-----------------------------------------------------------------------------
// Name: setText
// Desc: Changes the displayed text
//-----------------------------------------------------------------------------
void EvidyonGUIListEntryText::setText(const std::string& text)
{
    myTextLabelElement.setText(text);
}



//-----------------------------------------------------------------------------
// Name: setTextColor
// Desc: Changes this text's color
//-----------------------------------------------------------------------------
void EvidyonGUIListEntryText::setTextColor(D3DCOLOR color)
{
    myTextLabelElement.setTextColor(color);
}



//-----------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers the visible elements with the screen
//-----------------------------------------------------------------------------
void EvidyonGUIListEntryText::registerElements(GUILayer* layer)
{
    layer->registerElement(&myTextLabelElement);
}



//-----------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes elements from the given layer 
//-----------------------------------------------------------------------------
void EvidyonGUIListEntryText::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myTextLabelElement);
}





//-----------------------------------------------------------------------------
// Name: setClippingArea
// Desc: Sets the clipping region of the internal elements
//-----------------------------------------------------------------------------
void EvidyonGUIListEntryText::setClippingArea(const GUIRect* clippingArea)
{
    myTextLabelElement.setClippingArea(clippingArea);
}

