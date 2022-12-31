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