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