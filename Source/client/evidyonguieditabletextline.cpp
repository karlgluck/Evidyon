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

