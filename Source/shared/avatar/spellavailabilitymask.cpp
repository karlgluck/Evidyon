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

