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
#ifndef __SERVER_WORLD_MAPLOCATIONINFO_H__
#define __SERVER_WORLD_MAPLOCATIONINFO_H__
#pragma once

#include "shared/world/navigability.h"
#include "server/trigger/triggerinstance.h"
#include "shared/world/pathdirection.h"
#include "shared/zone/zonerules.h"
#include "shared/magic/magicindex.h"
#include "shared/geosid/geonite.h"
#include "shared/geosid/geosidindex.h"

namespace Evidyon {
namespace Server {



  
//----[  MapLocationInfo  ]----------------------------------------------------
struct MapLocationInfo {
  World::Navigability navigability; // full navigability definition
  Trigger::Server::TriggerInstance* trigger;
  Evidyon::Zone::ZoneRules zone_rules;
  Evidyon::Geosid::GeosidIndex nearby_geosid;
  unsigned int chaos_level;
  bool can_move_over;
  bool can_fly_over;
  bool can_teleport_over;
  bool occluding;

  // Effect triggered when player right-clicks (bursts) this location
  Evidyon::Magic::MagicIndex geonite_burst_magic;
  Evidyon::Geosid::Geonite geonite_burst_cost;

  // Taken directly from the game file.  0 is clockwise, 1 is ccw.
  Evidyon::World::PathDirection walking_path[2];
};



}
}

#endif