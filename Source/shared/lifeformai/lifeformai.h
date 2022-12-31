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