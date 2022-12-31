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
#ifndef __SHARED_LIFEFORMAI_LIFEFORMAI_PROJECTILEMONSTER_H__
#define __SHARED_LIFEFORMAI_LIFEFORMAI_PROJECTILEMONSTER_H__
#pragma once


#include "shared/actor/actortemplateindex.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/treasure/treasureclassindex.h"
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"
#include "shared/projectile/projectile.h"

namespace Evidyon {
namespace LifeformAI {



//----[  LifeformAI_ProjectileMonster  ]--------------------------------------
struct LifeformAI_ProjectileMonster {
  Event::EventIndex despawn_event;
  Time::Milliseconds despawn_delay;

  Actor::ActorTemplateIndex actor_template_index;
  float size;
  Evidyon::Treasure::TreasureClassIndex treasure;
  float hp_multiplier, speed, xp_multiplier;
  Time::Milliseconds attack_period, attack_duration;

  // this multiplier takes into account the attack period and the editor-
  // specified dps modifier
  float dps_to_damage_multiplier;

  // determines the type of damage and whether the casting action is an
  // attack or a targeted spell-cast
  bool is_magic_projectile;
  float projectile_range_sq;
  Projectile::Projectile projectile;
};



}
}

#endif