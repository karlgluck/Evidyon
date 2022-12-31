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