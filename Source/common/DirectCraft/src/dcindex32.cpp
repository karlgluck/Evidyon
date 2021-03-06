//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#include "../dc/index32"

using namespace dc;



//-----------------------------------------------------------------------------
// Name:  dcIndex32
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
dcIndex32::dcIndex32()
{
    myValue = 0;
}


//-----------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//-----------------------------------------------------------------------------
std::string dcIndex32::toString() const
{
    char stringValue[16];
    sprintf_s( stringValue, sizeof(stringValue), "%lu", myValue );
    return stringValue;
}



//----[  interpret  ]----------------------------------------------------------
void dcIndex32::interpret(const char* string) {
  int i;
  sscanf_s(string, "%i", &i);
  myValue = i;
}

//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//-----------------------------------------------------------------------------
std::string dcIndex32::staticTypeName()
{
    return "Index32";
}

