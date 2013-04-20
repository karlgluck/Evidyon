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
#include "globalworldmanager.h"
#include "../common/null.h"
#include "shared/evidyon_map.h"
#include "globalgamefilemanager.h"
#include "globalclientmanager.h"
#include "globalactormanager.h"


#include <math.h>
#include <dc/debug>
#include <set>
#include "../common/safemacros.h"

#include "trigger/triggerinstance.h"

// need for GetTickCount
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


namespace Evidyon {



GlobalWorldManager*  GlobalWorldManager::singleton_ = NULL;

GlobalWorldManager* GlobalWorldManager::singleton() {
  return singleton_;
}



//----[  GlobalWorldManager  ]-------------------------------------------------
GlobalWorldManager::GlobalWorldManager() {
  number_of_maps_ = 0;
  maps_ = NULL;
  allocated_regions_ = 0;
}





//----[  create  ]-------------------------------------------------------------
bool GlobalWorldManager::create(int number_of_maps) {
  if (!region_memory_pool_.create()) return false;
  singleton_ = this;

  number_of_maps_ = number_of_maps;
  maps_ = new Map[number_of_maps];


  return true;
}



//----[  destroy  ]------------------------------------------------------------
void GlobalWorldManager::destroy() {
  if (number_of_maps_ > 0) {
    for (int i = 0; i < number_of_maps_; ++i) {
      maps_[i].destroy();
    }
    SAFE_DELETE_ARRAY(maps_);
    number_of_maps_ = 0;
  }
  region_memory_pool_.destroy();
  singleton_ = NULL;
}





//----[  setMapHashToMapIndex  ]-----------------------------------------------
void GlobalWorldManager::setMapHashToMapIndex(World::MapHash hash,
                                              World::MapIndex index) {
  MapHashToMapIndexTable::_Pairib result
    = hash_to_index_table_.insert(
        MapHashToMapIndexTable::value_type(hash, index));
  assert(result.second);
}




//----[  getMapFromIndex  ]----------------------------------------------------
Map* GlobalWorldManager::getMapFromIndex(World::MapIndex index) {
  //assert(index >= 0);
  //assert(index < number_of_maps_);
  if (index < 0 || index >= number_of_maps_) return NULL;
  return &maps_[index];
}



//----[  getMapFromHash  ]-----------------------------------------------------
Map* GlobalWorldManager::getMapFromHash(World::MapHash hash) {
  return getMapFromIndex(getMapIndexFromHash(hash));
}


//----[  getMapIndexFromHash  ]------------------------------------------------
World::MapIndex GlobalWorldManager::getMapIndexFromHash(World::MapHash hash) {
  MapHashToMapIndexTable::iterator i = hash_to_index_table_.find(hash);
  return i == hash_to_index_table_.end()? World::INVALID_MAP_INDEX : i->second;
}


//----[  getMapHashFromIndex  ]------------------------------------------------
World::MapHash GlobalWorldManager::getMapHashFromIndex(World::MapIndex index) {
  return (index < 0 || index >= number_of_maps_) ?
    World::INVALID_MAP_HASH : maps_[index].getHash();
}

//----[  update  ]-------------------------------------------------------------
void GlobalWorldManager::update(double time) {
  for (int i = 0; i < number_of_maps_; ++i) {
    maps_[i].updateSpawn(time);
  }
}




//----[  printDebugStats  ]----------------------------------------------------
void GlobalWorldManager::printDebugStats(char* buffer, size_t buffer_size) {
  sprintf_s(buffer,
            buffer_size,
            "%lu regions allocated / %lu (core) available\r\n",
            region_memory_pool_.numberOfAllocatedEntries(),
            region_memory_pool_.numberOfFreeEntries());

  for (int i = 0; i < number_of_maps_; ++i) {
    char internal_buffer[512];
    maps_[i].printDebugStats(internal_buffer, 512);
    strcat_s(buffer, buffer_size, internal_buffer);
    strcat_s(buffer, buffer_size, "\r\n");
  }
}





//----[  consolidateRegions  ]-------------------------------------------------
void GlobalWorldManager::consolidateRegions(unsigned int incrementer) {
  unsigned int map_index = incrementer % number_of_maps_;
  maps_[map_index].checkForUnusedMapRegions();
  maps_[map_index].clearOldItemsOnGround();
}




//----[  allocateRegion  ]-----------------------------------------------------
WorldRegion* GlobalWorldManager::allocateRegion(Map* map,
                                                WorldRegion* neighbors[8],
                                                short region_x,
                                                short region_y,
                                                World::RegionID region_id) {
  WorldRegion* region = region_memory_pool_.acquire();
  CONFIRM(region != NULL) else return NULL;
  region->acquire(map, neighbors, region_x, region_y, region_id);
  ++allocated_regions_;
  return region;
}



//----[  deallocateRegion  ]---------------------------------------------------
void GlobalWorldManager::deallocateRegion(WorldRegion* region) {
  region->release();
  CONFIRM(region_memory_pool_.release(region));
  --allocated_regions_;
}


}