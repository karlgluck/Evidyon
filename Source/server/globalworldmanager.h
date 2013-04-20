//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#ifndef __GLOBALWORLDMANAGER_H__
#define __GLOBALWORLDMANAGER_H__
#pragma once


#include "shared/evidyon_world.h"
#include <dcx/guaranteedmemorypool>
#include <map>
#include "worldregion.h"
#include "shared/evidyon_map.h"
#include "shared/server-editor/maplocationspecialfunction.h"
#include "regionswarmspawner.h"
#include <vector>
#include <queue>
#include "itemidtype.h"
#include "server/world/maplocationinfo.h"
#include "shared/trigger/trigger.h"
#include "shared/gamefile/servergamefilemaplocationdata.h"

#include "map.h" // only #included so that old code doesn't break; remove this dependancy!!


namespace Evidyon {



  
//----[  GlobalWorldManager  ]-------------------------------------------------
class GlobalWorldManager {
  static const int MAX_WORLD_REGIONS = 512;

  typedef dcx::dcxGuaranteedMemoryPool<WorldRegion,MAX_WORLD_REGIONS> WorldRegionPool;
  typedef std::map<Evidyon::World::RegionID,WorldRegion*> WorldRegions;
  typedef std::map<World::MapHash, World::MapIndex> MapHashToMapIndexTable;

  friend class WorldRegion;


public:
  static GlobalWorldManager* singleton();


public:
  GlobalWorldManager();
  bool create(int number_of_maps);
  void destroy();

  void setMapHashToMapIndex(World::MapHash hash, World::MapIndex index);

  // Obtains the map at the given index or permanent storage hash
  // TODO: move all this stuff into the database manager
  Map* getMapFromIndex(World::MapIndex index);
  Map* getMapFromHash(World::MapHash hash);
  World::MapIndex getMapIndexFromHash(World::MapHash hash);
  World::MapHash getMapHashFromIndex(World::MapIndex index);

  // Performs world-maintenance, updating things like erasing unused regions.
  void update(double time);

  void printDebugStats(char* buffer, size_t buffer_size);

  // Use the incremented parameter to check maps for unused regions
  void consolidateRegions(unsigned int incrementer);

protected:
  friend class Map;
  WorldRegion* allocateRegion(Map* map,
                              WorldRegion* neighbors[8],
                              short region_x,
                              short region_y,
                              World::RegionID region_id);
  void deallocateRegion(WorldRegion* region);


private:
  WorldRegionPool region_memory_pool_;
  int allocated_regions_;
  Map* maps_;
  int number_of_maps_;
  MapHashToMapIndexTable hash_to_index_table_;

private:
  static GlobalWorldManager* singleton_;
};


}


#endif