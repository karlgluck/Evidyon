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
#ifndef __SHARED_MAGIC_BUFFMASK_H__
#define __SHARED_MAGIC_BUFFMASK_H__
#pragma once

namespace Evidyon {
namespace Magic {
enum Buff;
}
}

namespace Evidyon {
namespace Magic {

  
//----[  BuffMask  ]-----------------------------------------------------------
// These entries are in exactly the same order as the Buff enumerated type.
union BuffMask {
  struct {
    unsigned max_hp  : 1;
    unsigned max_mp  : 1;
    unsigned defense  : 1;
    unsigned melee_damage   : 1;
    unsigned magic_damage   : 1;
    unsigned hp_steal : 1;
    unsigned mp_steal : 1;
    unsigned action_speed_modifier : 1;
    unsigned movement_speed : 1;
    unsigned hp_regen : 1;
    unsigned poison   : 1;
    unsigned mp_regen : 1;
    unsigned leech    : 1;
  } fields;
  unsigned int value;

  // Returns whether or not the corresponding field is set to 1
  bool get(Buff buff) const;

  // Changes the value of the specified field
  void set(Buff buff, bool enabled);
};


}
}

#endif