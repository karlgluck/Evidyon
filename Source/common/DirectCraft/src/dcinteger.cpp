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
#include "../dc/integer"


using namespace dc;



//-----------------------------------------------------------------------------
// Name:  dcFileName
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
dcInteger::dcInteger()
{
    myValue = 0;
}


//-----------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//-----------------------------------------------------------------------------
std::string dcInteger::toString() const
{
    char stringValue[16];
    sprintf_s( stringValue, sizeof(stringValue), "%i", myValue );
    return stringValue;
}


//----[  interpret  ]----------------------------------------------------------
void dcInteger::interpret(const char* string) {
  int i;
  if (1 == sscanf_s(string, "%i", &i)) {
    myValue = i;
  } else {
    myValue = 0;
  }
}

//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//-----------------------------------------------------------------------------
std::string dcInteger::staticTypeName()
{
    return "Integer";
}
