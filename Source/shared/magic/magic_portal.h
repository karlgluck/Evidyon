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
#ifndef __SHARED_MAGIC_MAGIC_PORTAL_H__
#define __SHARED_MAGIC_MAGIC_PORTAL_H__
#pragma once

#include "shared/time/milliseconds.h"
#include "common/number.h"
#include "shared/event/eventindex.h"
#include "shared/world/mapindex.h"

namespace Evidyon {
namespace Magic {

//----[  Magic_Portal  ]-------------------------------------------------------
// Magic of this type causes a monster spawn when in chaos
struct Magic_Portal {
  enum Type {
    SELF_ONLY_MARK1,
    SELF_ONLY,
    OPEN_PORTAL_MARK1,
    OPEN_PORTAL,
    DIMENSION_DOOR,         // open portal to target point
    REVERSE_DIMENSION_DOOR, // open portal from target to source
    OPEN_PORTAL_TO_DESTINATION,
    SELF_ONLY_TO_SPAWN_POINT,
  } type;

  // all events have a defined duration, but can be cancelled early if a
  // portal is closed.
  Event::EventIndex source_event_index, // effect at the portal's entrance
                    target_event_index; // effect at the portal's target
  Event::EventIndex chaos_event_index; // used when the target no longer exists
  Time::Milliseconds open_delay;      // time until portal opens
  Time::Milliseconds close_delay;     // if not self-only, time until portal ends
  float radius_sq;                    // size of the portal

  // if the target is specified, this  is the destination
  World::MapIndex target_map;
  float target_x, target_z;
};

}
}

#endif