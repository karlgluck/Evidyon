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
#include "colorkey.h"


//------------------------------------------------------------------------------------------------
// Name:  ColorKey
// Desc:  Sets up this color key with the provided name
//------------------------------------------------------------------------------------------------
ColorKey::ColorKey()
{
    member("Enable", &myEnableFlag);
    member("Color", &myColor);
}


//------------------------------------------------------------------------------------------------
// Name:  isEnabled
// Desc:  Determines whether or not this color key is enabled
//------------------------------------------------------------------------------------------------
bool ColorKey::isEnabled() const
{
    return myEnableFlag.getValue();
}



//------------------------------------------------------------------------------------------------
// Name:  getRGBColor
// Desc:  Obtains the COLORREF-formatted color of this color key
//------------------------------------------------------------------------------------------------
COLORREF ColorKey::getRGBColor() const
{
    return RGB(myColor.getX()->getValue(),
                myColor.getY()->getValue(),
                myColor.getZ()->getValue());
}



//------------------------------------------------------------------------------------------------
// Name:  getD3DColor
// Desc:  Gets the D3DCOLOR_XRGB-formatted color of this color key
//------------------------------------------------------------------------------------------------
D3DCOLOR ColorKey::getD3DColor() const
{
    return D3DCOLOR_XRGB( myColor.getX()->getValue(),
                           myColor.getY()->getValue(),
                           myColor.getZ()->getValue());
}



//------------------------------------------------------------------------------------------------
// Name:  setRGBColor
// Desc:  Sets the color of this colorkey by a COLORREF value
//------------------------------------------------------------------------------------------------
void ColorKey::setRGBColor(COLORREF color)
{
    myColor.getX()->setValue(GetRValue(color));
    myColor.getY()->setValue(GetGValue(color));
    myColor.getZ()->setValue(GetBValue(color));
}



//------------------------------------------------------------------------------------------------
// Name:  enable
// Desc:  Enables or disables this color key
//------------------------------------------------------------------------------------------------
void ColorKey::enable(bool en)
{
    myEnableFlag.setValue(en);
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class (needed for DC library)
//------------------------------------------------------------------------------------------------
std::string ColorKey::staticTypeName()
{
    return "ColorKey";
}

