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
#ifndef __SERVER_PLAYER_PLAYERPORTALINSTANCE_H__
#define __SERVER_PLAYER_PLAYERPORTALINSTANCE_H__
#pragma once


#include "shared/time/timer.h"
#include "shared/event/eventid.h"
#include "shared/magic/magic_portal.h"
#include "common/number.h"

namespace Evidyon {
class WorldRegion;
class Map;
}


namespace Evidyon {
namespace Server {

//----[  PlayerPortalInstance  ]-----------------------------------------------
struct PlayerPortalInstance {
  Event::EventID source_event_id, target_event_id;
  WorldRegion* source_region;
  Number source_x, source_z;
  Map* target_map;
  WorldRegion* target_region; // not necessairily valid
  Number target_x, target_z;
  Time::Timer<Time::RESOLUTION_100_MS> open_timer, close_timer;
  const Magic::Magic_Portal* data;

  // If this flag is set, the portal can cause monsters to move through it.
  // This is allowed whenever a portal has a destination on the same map.
  // NPCs cannot be portaled.
  bool portal_monsters;

  void zero();
  void enable();
  void disable();
};

}
}

#endif