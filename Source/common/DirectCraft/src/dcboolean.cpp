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
#include "../dc/boolean"


using namespace dc;



//-----------------------------------------------------------------------------
// Name:  dcBoolean
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
dcBoolean::dcBoolean()
{
    myValue = false;
}


//-----------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//-----------------------------------------------------------------------------
std::string dcBoolean::toString() const
{
    return myValue ? "true" : "false";
}





//----[  interpret  ]----------------------------------------------------------
void dcBoolean::interpret(const char* string) {
  bool t = string[0] == 'T' || string[0] == 't' ||
           string[0] == '1';
  bool f = string[0] == 'F' || string[0] == 'f' ||
           string[0] == '0';
  if (t && !f) {
    myValue = true;
  } else if (f && !t) {
    myValue = false;
  } else {
    myValue = (stricmp(string, "false") != 0);
  }
}


//-----------------------------------------------------------------------------
// Name:  toggle
// Desc:  Swaps the value of this flag and returns the new value
//-----------------------------------------------------------------------------
bool dcBoolean::toggle()
{
    // Swap values
    myValue = !myValue;

    // Return the new value
    return myValue;
}


//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//-----------------------------------------------------------------------------
std::string dcBoolean::staticTypeName()
{
    return "Boolean";
}