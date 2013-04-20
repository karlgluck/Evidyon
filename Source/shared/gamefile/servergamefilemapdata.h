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
#ifndef __COMPLETE_EDITOR_GAMEFILE_SERVERGAMEFILEMAPDATA_H__
#define __COMPLETE_EDITOR_GAMEFILE_SERVERGAMEFILEMAPDATA_H__
#pragma once



#include "servergamefilemapregiondata.h"
#include "servergamefilemaplocationdata.h"
#include "shared/trigger/trigger.h"
#include "shared/world/maphash.h"
#include "shared/world/mapindex.h"


namespace Evidyon {
namespace GameFile {



//----[  ServerGameFileMapData  ]----------------------------------------------
struct ServerGameFileMapData {

  // This is the type that is stored in the server's database.  It is paired
  // with the map index at startup so that the map that a player is on
  // persists across map reorderings and deletions.
  World::MapHash map_hash;

  // Data about all of the locations in this map
  unsigned int locations_wide, locations_high;
  ServerGameFileMapLocationData* locations;

  unsigned int regions_wide, regions_high;
  ServerGameFileMapRegionData* regions;

  Evidyon::Trigger::Trigger* triggers;
  size_t number_of_triggers;
};



}
}

#endif