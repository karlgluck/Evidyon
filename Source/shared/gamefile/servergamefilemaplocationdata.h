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
#ifndef __COMPLETE_EDITOR_GAMEFILE_SERVERGAMEFILEMAPLOCATIONDATA_H__
#define __COMPLETE_EDITOR_GAMEFILE_SERVERGAMEFILEMAPLOCATIONDATA_H__
#pragma once


#include "shared/world/navigability.h"
#include "shared/trigger/triggerindex.h"
#include "shared/zone/zonerules.h"
#include "shared/world/pathdirection.h"
#include "shared/geosid/geonite.h"
#include "shared/geosid/geosidindex.h"
#include "shared/magic/magicindex.h"

namespace Evidyon {
namespace GameFile {


//----[  ServerGameFileMapLocationData  ]--------------------------------------
struct ServerGameFileMapLocationData {
  Evidyon::World::Navigability navigability;
  Evidyon::Trigger::TriggerIndex trigger;
  Evidyon::Zone::ZoneRules zone_rules;
  Evidyon::Geosid::GeosidIndex nearby_geosid;
  bool occluding;

  // Effect triggered when player right-clicks (bursts) this location
  Evidyon::Magic::MagicIndex geonite_burst_magic;
  Evidyon::Geosid::Geonite geonite_burst_cost;
  
  // 32-bit number indicating the chance of a chaos effect on this location
  // out of 0xFFFFFFFF.  For random chaos effects, choose a random 32-bit
  // value and execute if it is less than or equal to chaos_level.
  unsigned int chaos_level;

  // These values aid the AI in navigating around the map.
  Evidyon::World::PathDirection walking_path[2]; // 0 = clockwise, 1 = ccw
};


}
}

#endif