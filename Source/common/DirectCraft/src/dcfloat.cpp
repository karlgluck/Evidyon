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
#include "../dc/float"


using namespace dc;




//-----------------------------------------------------------------------------
// Name:  dcFloat
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
dcFloat::dcFloat()
{
    myValue = 0.0f;
}


//-----------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//-----------------------------------------------------------------------------
std::string dcFloat::toString() const
{
    char stringValue[64];
    sprintf_s( stringValue, sizeof(stringValue), "%.04f", myValue );
    return stringValue;
}



//----[  serialize  ]----------------------------------------------------------
void dcFloat::serialize(std::string* string) {
  char value[64];
  sprintf_s(value, sizeof(value), "%f", myValue);
  string->assign(value);
}


//----[  interpret  ]----------------------------------------------------------
void dcFloat::interpret(const char* string) {
  sscanf_s(string, "%f", &myValue);
}

//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//-----------------------------------------------------------------------------
std::string dcFloat::staticTypeName()
{
    return "Float";
}
