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
#ifndef __SHARED_AILIFEFORM_AILIFEFORM_H__
#define __SHARED_AILIFEFORM_AILIFEFORM_H__
#pragma once


#include "lifeformai_simplemeleemonster.h"
#include "lifeformai_interactivenpc.h"
#include "lifeformai_simplemeleetownguard.h"
#include "lifeformai_geosidguardian.h"
#include "lifeformai_projectilemonster.h"
#include "lifeformai_directdamagemonster.h"
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"

namespace Evidyon {
namespace LifeformAI {


//----[  LifeformAI  ]---------------------------------------------------------
struct LifeformAI {
  enum {
    LIFEFORMAI_SIMPLEMELEEMONSTER,
    LIFEFORMAI_INTERACTIVENPC,
    LIFEFORMAI_SIMPLEMELEETOWNGUARD,
    LIFEFORMAI_GEOSIDGUARDIAN,
    LIFEFORMAI_PROJECTILEMONSTER,
    LIFEFORMAI_DIRECTDAMAGEMONSTER,
  } type;
  union {
    LifeformAI_SimpleMeleeMonster simple_melee_monster;
    LifeformAI_InteractiveNPC interactive_npc;
    LifeformAI_SimpleMeleeTownGuard simple_melee_town_guard;
    LifeformAI_GeosidGuardian geosid_guardian;
    LifeformAI_ProjectileMonster projectile_monster;
    LifeformAI_DirectDamageMonster direct_damage_monster;
  };

  // The spawn event is initialized at the location where the lifeform is
  // spawning, and an instance of the lifeform is created after the delay.
  Event::EventIndex spawn_event;
  Time::Milliseconds spawn_delay;
};
 

}
}


#endif