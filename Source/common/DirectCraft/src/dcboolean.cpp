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