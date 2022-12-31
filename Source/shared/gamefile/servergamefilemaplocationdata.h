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