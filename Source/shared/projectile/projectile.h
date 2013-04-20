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
#ifndef __SHARED_PROJECTILE_PROJECTILE_H__
#define __SHARED_PROJECTILE_PROJECTILE_H__
#pragma once

#include "shared/time/milliseconds.h"
#include "shared/event/eventindex.h"

namespace Evidyon {
namespace Projectile {


//----[  Projectile  ]---------------------------------------------------------
// This structure is contained within projectile magic and projectile weapons,
// and describes the behavior of the projectiles that magic or weapon create.
struct Projectile {
  Event::EventIndex event_index;
  float speed;
  Time::Milliseconds duration;
  float radius;
  bool homing;
  float homing_rate;
  bool bouncing;
  bool piercing;
  Time::Milliseconds piercing_period;  // 1.0f / speed (once per square)
};



}
}

#endif