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
#include "../dc/dcinternal_begin.h"


using namespace dc;



//-----------------------------------------------------------------------------
// Name:  uniqueID
// Desc:  Generates this resource's unique numeric identifier
//-----------------------------------------------------------------------------
unsigned long dcResourceManager::uniqueID() const
{
    return uniqueIDOfTypeName(typeName());
}



//-----------------------------------------------------------------------------
// Name:  deallocate
// Desc:  Global method to free the memory for a resource
//-----------------------------------------------------------------------------
void dcResourceManager::deallocate( dcGenericResource* resource )
{
    resource->getManager()->release( resource );
}


//-----------------------------------------------------------------------------
// Name:  uniqueIDOfTypeName
// Desc:  
//-----------------------------------------------------------------------------
unsigned long dcResourceManager::uniqueIDOfTypeName(const std::string& type_name) {

    // Make sure the length is aligned on a 16-bit boundary
    std::string tn = type_name; // todo: BAD but I don't have time to fix old code right now
    if( tn.length() % 2 == 1 )
        tn += '%';

    const unsigned short *data = (unsigned short*)tn.data();  // Pointer to the data to be summed
    size_t len = tn.length() / 2;      // Length in 16-bit words
    unsigned long sum1 = 0xffff, sum2 = 0xffff;

    while (len) {
        size_t tlen = len > 360 ? 360 : len;
        len -= tlen;
        do {
            sum1 += *data++;
            sum2 += sum1;
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    // Second reduction step to reduce sums to 16 bits
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);

    // Return the code
    return sum2 << 16 | sum1;
}