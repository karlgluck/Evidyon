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

