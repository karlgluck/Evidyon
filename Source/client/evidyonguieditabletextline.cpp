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
// Name: EvidyonGUIEditableTextLine
// Desc: Initializes this class
//-----------------------------------------------------------------------------
EvidyonGUIEditableTextLine::EvidyonGUIEditableTextLine()
{
    myNumbersOnlyFlag = false;
    myTextLengthLimit = -1;

    myBackground.setSourceCanvas(getCanvas());

    myTextElement.setSourceCanvas(&myTextCanvas);
    myTextElement.setClippingArea(myTextCanvas.getArea());

    myTextCanvas.setSourceCanvas(getCanvas());
    myTextCanvas.alignXCenter(0);
    myTextCanvas.alignYCenter(0);
    myTextCanvas.relativeWidth(-16);
    myTextCanvas.relativeHeight(-8);
    myTextCanvas.relativeDepth(-0.001f);

    myBackground.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
}



//-----------------------------------------------------------------------------
// Name: configure
// Desc: Calls the methods to set many text line parameters all at once
//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::configure(GUIFont* font,
    bool isPassword, bool restrictToNumbers, D3DCOLOR textColor)
{
    configure(font, isPassword, restrictToNumbers, textColor, -1);
}



//-----------------------------------------------------------------------------
// Name: configure
// Desc: Calls the methods to set many text line parameters all at once
//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::configure(GUIFont* font,
    bool isPassword, bool restrictToNumbers, D3DCOLOR textColor, int textLengthLimit)
{
    // Superclass methods
    setFont(font);
    setPasswordFlag(isPassword);
    numbersOnly(restrictToNumbers);
    setTextColor(textColor);
    setTextLengthLimit(textLengthLimit);
}






//-----------------------------------------------------------------------------
// Name: textLengthLimit
// Desc: Sets the number of characters this line can accept
//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::setTextLengthLimit(int characters)
{
    if (characters < 0)
        myTextLengthLimit = -1;
    else
        myTextLengthLimit = characters;
}



//-----------------------------------------------------------------------------
// Name: removeTextLengthLimit
// Desc: Makes this text line accept any number of characters (default behavior)
//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::removeTextLengthLimit()
{
    setTextLengthLimit(-1);
}



//-----------------------------------------------------------------------------
// Name: numbersOnly
// Desc: Changes the flag that restricts input so that only numbers are accepted
//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::numbersOnly(bool restrictToNumbers)
{
    myNumbersOnlyFlag = restrictToNumbers;
}



//-----------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers this component's visible elements with the given layer.  This method calls
//       the superclass method.
//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::registerElements(GUILayer* layer)
{
    GUIRect sourceArea;
    GUIPoint loc = { 0, 0 };
    GUISize size = { 32, 32 };
    sourceArea.set(loc, size);
    myBackground.setSourceRegion(&sourceArea, 8, EvidyonGUIDefs::TEXTURE_SIZE);

    layer->registerElement(&myBackground);
    GUIEditableTextLine::registerElements(layer);
}



//-----------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Unregisters this component's visible elements.  This method calls the superclass method.
//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::unregisterElements(GUILayer* layer)
{
    GUIEditableTextLine::unregisterElements(layer);
    layer->unregisterElement(&myBackground);
}


//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::setBackgroundColor(D3DCOLOR color) {
  myBackground.setColor(color);
}



//-----------------------------------------------------------------------------
// Name: onTypedPrintableCharacter
// Desc: Invoked when the user enters a valid character that should be added to the internal buffer
//-----------------------------------------------------------------------------
void EvidyonGUIEditableTextLine::onTypedPrintableCharacter(char printableCharacter)
{
    bool acceptCharacter = true;
    acceptCharacter = acceptCharacter && (myNumbersOnlyFlag ? (printableCharacter >= '0' && printableCharacter <= '9') : true);
    acceptCharacter = acceptCharacter && (myTextLengthLimit >= 0 ? (myText.length() < myTextLengthLimit) : true);
    if (acceptCharacter) GUIEditableTextLine::onTypedPrintableCharacter(printableCharacter);
}

