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
#include "../dc/string"


using namespace dc;





//-----------------------------------------------------------------------------
// Name:  dcString
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
dcString::dcString()
{
    myValue = "";
}


//-----------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//-----------------------------------------------------------------------------
std::string dcString::toString() const
{
    return myValue;
}



//----[  interpret  ]----------------------------------------------------------
void dcString::interpret(const char* string) {
  myValue = string;
}


//-----------------------------------------------------------------------------
// Name:  save
// Desc:  Saves this resource
//-----------------------------------------------------------------------------
bool dcString::save( dcStreamOut* stream )
{
    return stream->writeString( myValue );
}


//-----------------------------------------------------------------------------
// Name:  load
// Desc:  Loads this resource
//-----------------------------------------------------------------------------
bool dcString::load( dcStreamIn* stream )
{
    return stream->readString( &myValue );
}


//-----------------------------------------------------------------------------
// Name:  compareValue
// Desc:  Compares the value stored in this class to another value
//-----------------------------------------------------------------------------
int dcString::compareValue( const std::string& otherValue ) const
{
    return myValue.compare( otherValue );
}


//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//-----------------------------------------------------------------------------
std::string dcString::staticTypeName()
{
    return "String";
}
