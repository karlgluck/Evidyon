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
#ifndef __SHARED_AVATAR_SPELLAVAILABILITYMASK_H__
#define __SHARED_AVATAR_SPELLAVAILABILITYMASK_H__
#pragma once


namespace Evidyon {
namespace Avatar {



// based on the availability mask size
static const int MAX_SPELL_TYPES = (32*4);


  
//----[  SpellAvailabilityMask  ]----------------------------------------------
struct SpellAvailabilityMask {
  unsigned long mask[4];
  void zero();
  void setBit(unsigned int bit);
  void clearBit(unsigned int bit);
  bool isBitSet(unsigned int bit) const;
  void unionWith(const SpellAvailabilityMask* other);
  bool equals(const SpellAvailabilityMask* other);
  void copy(const SpellAvailabilityMask* other);

  // Checks to see if the masks are different, and copies the other one in to
  // this mask and returns 'true' if they are (false indicates the masks were
  // the same)
  bool compareCopy(const SpellAvailabilityMask* other);
};



}
}

#endif