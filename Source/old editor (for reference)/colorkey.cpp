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

