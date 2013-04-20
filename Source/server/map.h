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
#ifndef __SERVER_MAP_H__
#define __SERVER_MAP_H__
#pragma once

#include "shared/evidyon_world.h"
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
#include "shared/world/mapindex.h"
#include "shared/world/maphash.h"

namespace Evidyon {
namespace Trigger {
namespace Server {
class TriggerInstance;
}
}
}

namespace Evidyon {
//namespace Server {


class ItemInstance;


  
//----[  ClearItemsOnMapLocation  ]--------------------------------------------
struct ClearItemsOnMapLocation {
  unsigned short x, y;
  unsigned int millisecond_timer_time;

  // these are used to check to make sure the location hasn't changed
  // since this clear was issued
  ItemIDType sum_of_unique_ids;

  bool operator < (const ClearItemsOnMapLocation& rhs) const {
    return millisecond_timer_time < rhs.millisecond_timer_time;
  }
};





//----[  Map  ]----------------------------------------------------------------
class Map {
  typedef std::map<Evidyon::World::RegionID,WorldRegion*> WorldRegions;
  typedef std::vector<RegionSwarmSpawner*> ActiveRegionSwarmSpawners;

  struct Inhabitant {
    int x, y;
    size_t lifeform_ai_type;

    // If this inhabitant is active, the actor pointer will be valid.
    ActorPointer actor;
  };

public:
  Map();

  bool create(World::MapIndex index,
              World::MapHash hash,
              int width,
              int height,
              GameFile::ServerGameFileMapLocationData* locations,
              size_t regions_wide,
              size_t regions_high,
              GameFile::ServerGameFileMapRegionData* regions,
              Evidyon::Trigger::Trigger* triggers,
              size_t number_of_triggers);

  void destroy();

  World::MapIndex getIndex() const;
  World::MapHash getHash() const;

  // Gets the allocated map information.  Used by the game file loader to
  // configure this map.
  Server::MapLocationInfo* getMapInfo();


  void createInhabitants(size_t number_of_inhabitants);
  void setInhabitant(size_t inhabitant_index, int x, int y, size_t lifeform_ai_type);

  // Looks through all of the map regions to see if any are completely out
  // of range of any client so that the region can be freed.
  void checkForUnusedMapRegions();

  // Gets rid of items on the map that have been there for a long time
  void clearOldItemsOnGround();

  // Updates the monster spawning
  void updateSpawn(double time);

  const Server::MapLocationInfo* getMapLocationInfo(int x, int y) const;

  bool canCreatePortalAt(Number x, Number z);

  bool isValidTeleport(ActorInstance* actor, float dest_x, float dest_y) const;
  bool isValidTeleport(Number source_x,
                       Number source_z,
                       Number target_x,
                       Number target_z) const;
  bool isValidMonsterWalkPath(Number source_x,
                              Number source_z,
                              Number target_x,
                              Number target_z) const;

  // Returns whether or not there is a direct line of sight between two points
  bool testLineOfSight(Number source_x,
                       Number source_z,
                       Number target_x,
                       Number target_z) const;

  void printDebugStats(char* buffer, size_t buffer_size);

  // Returns 'true' if a portal to the given location is valid
  bool validatePortalDestination(Number x,
                                 Number z);

  // Returns a random location in the chaos region of this map
  void getRandomChaosLocation(Number* x, Number* z);

public:

  // Obtains a world region containing the given point (if it already exists).
  // This method will not create a new one.
  WorldRegion* getRegionFromPoint(float x, float y);
  WorldRegion* getRegionFromPoint(int x, int y);

  // Acquires a world region, but does not check to make sure that it has
  // valid borders.  This is a necessary distinction from a bordered region
  // used by computer-controlled characters, since acquiring a bordered
  // region instantiates more AI actors in the bordered areas, which in
  // turn would acquire borders further out until the whole map was
  // acquired.
  WorldRegion* acquireRegionFromPoint(float x, float y);
  WorldRegion* acquireRegionFromPoint(int x, int y);

  // Similar to getRegionFromPoint, but these methods will create a new region
  // if none already exists.  These methods ensure that the region acquired
  // is bordered on all sides by valid regions.
  WorldRegion* acquireBorderedRegionFromPoint(float x, float y);
  WorldRegion* acquireBorderedRegionFromPoint(int x, int y);

  // If the point is outside of the current region, this method calls
  // getRegionFromPoint to access the updated region and returns 'true'.
  // Note that this will not create a new region or update any memberships.
  // This is intended for use by projectiles.
  bool updateRegion(float x, float y, const WorldRegion* current,
                    WorldRegion** updated);

  // Determines how much of *distance an actor can walk in the direction
  // (dx,dy) starting at (x,y).  If the return value is 'true', then
  // *distance specifies the maximum distance an actor can walk in the
  // given direction.
  bool validatePhysicalPath(float x, float y,
    float dx, float dy, float* distance);

  // Adds a clear request for the given location.  It is passed back to the
  // world region a few minutes in the future (depending on whether or not
  // it is important) when the region should be cleared of items.  The
  // 'summation' parameter is a rollover sum of all the item IDs on the
  // map location.  This is checked by the WorldRegion to make sure that
  // nothing has been added to or removed from the location since this
  // request was placed.
  void addClearItemsOnLocation(bool important, int x, int y, ItemIDType summation);


private:

  // Creates a new world region
  WorldRegion* createRegion(short region_x, short region_y);

  // Finds or creates the world region at (region_x, region_y), ensures it has
  // neighbors on all sides then returns that region.
  WorldRegion* acquireRegionAndNeighbors(World::RegionID region_id, short region_x, short region_y);

  // Obtains a region pointer, if the region exists
  WorldRegion* getRegion(short region_x, short region_y);

  // Erases the given region from the manager.  Performs all cleanup such as
  // removing AI lifeforms, freeing the spawn points, etc.
  void destroyRegion(WorldRegion* region);


private:
  WorldRegions regions_;  // Memory is pulled from the global world manager's pool
  Server::MapLocationInfo* locations_; // Data about each map location
  World::MapIndex map_index_; // Used to identify this map on the client
  World::MapHash map_hash_; // used to identify this map in the database
  int width_, height_; // todo: make square and just use size_

  // these structures control the creation of the mass mobs of monsters that
  // appear on the map for players to interact with.  A region swarm spawner
  // is activated when its associated WorldRegion is acquired.
  int regions_wide_, regions_high_;
  RegionSwarmSpawner* region_swarm_spawners_;
  ActiveRegionSwarmSpawners active_region_swarm_spawners_;

  // The inhabitants are permanent members of the world.  The controller for these
  // entries is created when the world region containing the spawn point is
  // activated and the associated actor does not exist.
  Inhabitant* inhabitants_;
  size_t number_of_inhabitants_;
  typedef std::multimap<World::RegionID,Inhabitant*> WorldRegionInhabitants;
  WorldRegionInhabitants world_region_inhabitants_;

  typedef std::priority_queue<ClearItemsOnMapLocation> ClearItemsOnLocationQueue;
  ClearItemsOnLocationQueue clear_items_on_location_queue_;

private:
  Trigger::Server::TriggerInstance** triggers_;
  size_t number_of_triggers_;
};

//}
}

#endif