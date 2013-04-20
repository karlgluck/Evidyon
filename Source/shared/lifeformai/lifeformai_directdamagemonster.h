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
#ifndef __SHARED_LIFEFORMAI_LIFEFORMAI_DIRECTDAMAGEMONSTER_H__
#define __SHARED_LIFEFORMAI_LIFEFORMAI_DIRECTDAMAGEMONSTER_H__
#pragma once


#include "shared/actor/actortemplateindex.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/treasure/treasureclassindex.h"
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"


namespace Evidyon {
namespace LifeformAI {



//----[  LifeformAI_DirectDamageMonster  ]-------------------------------------
struct LifeformAI_DirectDamageMonster {
  Event::EventIndex despawn_event;
  Time::Milliseconds despawn_delay;

  Actor::ActorTemplateIndex actor_template_index;
  float size;
  Evidyon::Treasure::TreasureClassIndex treasure;
  float hp_multiplier, speed, xp_multiplier;

  // this multiplier takes into account the attack period and the editor
  // specified dps modifier
  float dps_to_damage_multiplier;

  // the direct damage monster moves toward the target until it is this far
  // away, then it will start using its spell.  It will not move closer than
  // this amount (so that melee monsters can get in!)
  float attack_distance_sq;
  Time::Milliseconds attack_period, attack_duration;
  Event::EventIndex attack_event;
};



}
}

#endif