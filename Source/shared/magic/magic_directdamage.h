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
#ifndef __SHARED_MAGIC_MAGIC_DIRECTDAMAGE_H__
#define __SHARED_MAGIC_MAGIC_DIRECTDAMAGE_H__
#pragma once

#include "shared/event/eventindex.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/time/milliseconds.h"

namespace Evidyon {
namespace Magic {

//----[  Magic_DirectDamage  ]-------------------------------------------------
// Single target; basic spell.
// Stage 0: Casting.  Move to stage 1 after delay.
// Stage 1: Apply damage.
struct Magic_DirectDamage {
  Time::Milliseconds effect_delay;
  Event::EventIndex event_index;
  float dps_to_damage_multiplier;
};

}
}

#endif