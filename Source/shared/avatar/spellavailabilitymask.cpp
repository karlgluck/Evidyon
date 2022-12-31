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
#include "spellavailabilitymask.h"


namespace Evidyon {
namespace Avatar {


  
//----[  zero  ]---------------------------------------------------------------
void SpellAvailabilityMask::zero() {
  mask[0] = 0;
  mask[1] = 0;
  mask[2] = 0;
  mask[3] = 0;
}



//----[  setBit  ]-------------------------------------------------------------
void SpellAvailabilityMask::setBit(unsigned int bit) {
  mask[bit>>5] |= 1<<(bit%32);
}



//----[  clearBit  ]-----------------------------------------------------------
void SpellAvailabilityMask::clearBit(unsigned int bit) {
  mask[bit>>5] &= ~(1<<(bit%32));
}



//----[  isBitSet  ]-----------------------------------------------------------
bool SpellAvailabilityMask::isBitSet(unsigned int bit) const {
  return 1&(mask[bit>>5] >> (bit%32));
}



//----[  unionWith  ]----------------------------------------------------------
void SpellAvailabilityMask::unionWith(const SpellAvailabilityMask* other) {
  mask[0] |= other->mask[0];
  mask[1] |= other->mask[1];
  mask[2] |= other->mask[2];
  mask[3] |= other->mask[3];
}



//----[  equals  ]-------------------------------------------------------------
bool SpellAvailabilityMask::equals(const SpellAvailabilityMask* other) {
  return mask[0] == other->mask[0] &&
         mask[1] == other->mask[1] &&
         mask[2] == other->mask[2] &&
         mask[3] == other->mask[3];
}



//----[  copy  ]---------------------------------------------------------------
void SpellAvailabilityMask::copy(const SpellAvailabilityMask* other) {
  mask[0] = other->mask[0];
  mask[1] = other->mask[1];
  mask[2] = other->mask[2];
  mask[3] = other->mask[3];
}




//----[  compareCopy  ]--------------------------------------------------------
bool SpellAvailabilityMask::compareCopy(const SpellAvailabilityMask* other) {
  bool different =
    mask[0] != other->mask[0] ||
    mask[1] != other->mask[1] ||
    mask[2] != other->mask[2] ||
    mask[3] != other->mask[3];
  mask[0] = other->mask[0];
  mask[1] = other->mask[1];
  mask[2] = other->mask[2];
  mask[3] = other->mask[3];
  return different;
}



}
}

