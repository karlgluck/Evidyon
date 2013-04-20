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