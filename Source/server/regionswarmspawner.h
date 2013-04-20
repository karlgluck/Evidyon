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
#ifndef __REGIONSWARMSPAWNER_H__
#define __REGIONSWARMSPAWNER_H__
#pragma once


#include "actorpointer.h"

#include "shared/gamefile/servergamefilemapregiondata.h"
#include "shared/time/timer.h"
#include "shared/avatar/avatarlevel.h"


namespace Evidyon {

class WorldRegion;
namespace Server {
class GlobalLifeformAIManager;
}
}

namespace Evidyon {


  
//----[  RegionSwarmSpawner  ]-------------------------------------------------
class RegionSwarmSpawner {
  static const int NUMBER_OF_INHABITANTS =
    GameFile::ServerGameFileMapRegionData::NUMBER_OF_INHABITANTS;
  static const int NUMBER_OF_SWARM_SPAWNS =
    GameFile::ServerGameFileMapRegionData::NUMBER_OF_SWARM_SPAWNS;
  static const int NUMBER_OF_SPAWN_LOCATIONS =
    GameFile::ServerGameFileMapRegionData::NUMBER_OF_SPAWN_LOCATIONS;

  struct SpawnLocation {
    Avatar::AvatarLevel intended_level;
    unsigned int swarms_mask;
    LifeformAI::SpawnPointIndex spawn_point_index;
    float x, y;
  };

  // If thie lifeform type is not INVALID_LIFEFORM_INDEX, an inhabitant is
  // respawned whenever its actor is invalid.  The actor is also erased
  // if this region is deactivated.
  struct Inhabitant {
    LifeformAI::SpawnPointIndex spawn_point_index;
    LifeformAI::LifeformAIIndex lifeform_type;
    float x, z;
  };

public:
  RegionSwarmSpawner();

  void create(int region_left,
              int region_top,
              const GameFile::ServerGameFileMapRegionData* data);

  // When a spawner's associated world region is acquired, this method sets
  // up the spawner.
  void activate(WorldRegion* local_region);

  // If the parameter matches the internal region, this method clears the
  // internal reference and returns 'true'.
  bool deactivateIfRegionIs(WorldRegion* local_region);

  // Spawns monsters.  Should only be called when this region is active.
  void update(Server::GlobalLifeformAIManager* lifeform_ai_manager);

  //// Called with +1 for each client that enters a region or a neighboring
  //// region, and -1 for each one that enters this or a neighboring region.
  //void changeSpawnRate(int amount);

private:

  Time::Timer<Time::RESOLUTION_500_MS> next_spawn_timer_;

  // the part of the world into which this spawner spawns
  WorldRegion* region_;

  LifeformAI::LifeformAIIndex swarms_[NUMBER_OF_SWARM_SPAWNS];
  SpawnLocation spawn_locations_[NUMBER_OF_SPAWN_LOCATIONS];

  bool just_activated_; // 'true' between activate() and the first update()

  Inhabitant inhabitants_[NUMBER_OF_INHABITANTS];
};

}


#endif