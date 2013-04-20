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
#ifndef __SHARED_LIFEFORMAI_LIFEFORMAI_SIMPLEMELEETOWNGUARD_H__
#define __SHARED_LIFEFORMAI_LIFEFORMAI_SIMPLEMELEETOWNGUARD_H__
#pragma once


#include "shared/actor/actortemplateindex.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/treasure/treasureclassindex.h"
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"


namespace Evidyon {
namespace LifeformAI {



//----[  LifeformAI_SimpleMeleeTownGuard  ]-----------------------------------------
struct LifeformAI_SimpleMeleeTownGuard {
  Event::EventIndex despawn_event;
  Time::Milliseconds despawn_delay;

  Actor::ActorTemplateIndex actor_template_index;
  float size, reach;
  float speed;
  Time::Milliseconds attack_period, attack_duration;
  int hp;
  int attack_damage;

  // if the guard gets too far from this point or loses its target,
  // it will be teleported back here.
  float town_x, town_z;
};



}
}

#endif