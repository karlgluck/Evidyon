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