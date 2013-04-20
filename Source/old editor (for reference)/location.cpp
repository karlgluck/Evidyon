//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#include "location.h"


dcIntegerWrapper::dcIntegerWrapper()
{
    myReference = 0;
}

dcIntegerWrapper::dcIntegerWrapper(int* reference)
{
    myReference = reference;
}

void dcIntegerWrapper::setValue(int value)
{
    *myReference = value;
}

int& dcIntegerWrapper::getValue() const
{
    return *myReference;
}

std::string dcIntegerWrapper::toString() const
{
    char stringValue[16];
    sprintf_s(stringValue, sizeof(stringValue), "%i", *myReference);
    return stringValue;
}
bool dcIntegerWrapper::save(dc::dcStreamOut* stream) {
  return stream->write(myReference, sizeof(int));
}

bool dcIntegerWrapper::load(dc::dcStreamIn* stream) {
  return stream->read(myReference, sizeof(int));
}

void dcIntegerWrapper::serialize(std::string* string) {
  if (!myReference) return;
  char str[64];
  string->assign(_itoa(*myReference, str, 10));
}

void dcIntegerWrapper::interpret(const char* string) {
  if (!myReference) return;
  sscanf_s(string, "%i", myReference);
}


/*
//------------------------------------------------------------------------------------------------
// Name:  Location
// Desc:  Initializes this resource
//------------------------------------------------------------------------------------------------
Location::Location() : myX(&myValue.first), myY(&myValue.second)
{
    myValue.first = 0;
    myValue.second = 0;

    member("x", &myX);
    member("y", &myY);
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string Location::toString() const
{
    char stringValue[32];
    sprintf_s(stringValue, sizeof(stringValue), "(%i, %i)", myValue.first, myValue.second);
    return stringValue;
}



//------------------------------------------------------------------------------------------------
// Name:  save
// Desc:  Saves this resource
//------------------------------------------------------------------------------------------------
bool Location::save(dcStreamOut* stream)
{
    // Write the coordinates
    if (!stream->write(&myValue.first, sizeof(myValue.first))) return false;
    if (!stream->write(&myValue.second, sizeof(myValue.second))) return false;

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  load
// Desc:  Loads this resource
//------------------------------------------------------------------------------------------------
bool Location::load(dcStreamIn* stream)
{
    // Read the coordinates
    if (!stream->read(&myValue.first, sizeof(myValue.first))) return false;
    if (!stream->read(&myValue.second, sizeof(myValue.second))) return false;

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  compareValue
// Desc:  Compares the value stored in this class to another value
//------------------------------------------------------------------------------------------------
int Location::compareValue(const Location::BasicType& otherValue) const
{
    if (myValue.first < otherValue.first) return -1;
    if (myValue.first > otherValue.first) return +1;
    if (myValue.second < otherValue.second) return -1;
    if (myValue.second > otherValue.second) return +1;
    return 0;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string Location::staticTypeName()
{
    return "Location";
}

*/






//------------------------------------------------------------------------------------------------
// Name:  Location
// Desc:  Initializes this resource
//------------------------------------------------------------------------------------------------
Location::Location()
{
    member("x", &myX);
    member("y", &myY);
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string Location::toString() const
{
    char stringValue[32];
    sprintf_s(stringValue, sizeof(stringValue), "(%i, %i)", getX(), getY());
    return stringValue;
}


//------------------------------------------------------------------------------------------------
// Name:  compareValue
// Desc:  Compares the value stored in this class to another value
//------------------------------------------------------------------------------------------------
int Location::compareTo( const dc::dcGenericResource* other ) const
{
    if (other->getManager()->uniqueID() != getManager()->uniqueID()) return dc::dcGenericResource::compareTo(other);

    const Location& otherLocation = *(Location*)other;
    if (myX.getValue() < otherLocation.myX.getValue()) return -1;
    if (myX.getValue() > otherLocation.myX.getValue()) return +1;
    if (myY.getValue() < otherLocation.myY.getValue()) return -1;
    if (myY.getValue() > otherLocation.myY.getValue()) return +1;
    return 0;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string Location::staticTypeName()
{
    return "Location";
}