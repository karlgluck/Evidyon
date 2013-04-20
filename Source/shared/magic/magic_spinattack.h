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
#ifndef __SHARED_MAGIC_MAGIC_SPINATTACK_H__
#define __SHARED_MAGIC_MAGIC_SPINATTACK_H__
#pragma once

#include "shared/event/eventindex.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/time/milliseconds.h"

namespace Evidyon {
namespace Magic {

//----[  Magic_SpinAttack  ]---------------------------------------------------
// Melee spin attack around the caster using the current weapon.  If it is a
// projectile weapon, a full spread of projectiles is created around the
// caster.
// Traveling Delay: time after which the damage is applied (melee) or the
//                  projectiles are created (ranged)
// repeat Delay:  time added to the player's innate recovery time between
//                  successive attacks
struct Magic_SpinAttack {
  Event::EventIndex event_index;
  Time::Milliseconds effect_delay;

  float damage_factor;      // multiplied by weapon damage
  int max_targets;

  // one of these should be true!
  bool allow_projectiles;
  bool allow_melee;
};

}
}

#endif