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
#include "regionswarmspawner.h"
#include "server/lifeformai/globallifeformaimanager.h"
#include "globalactormanager.h"
#include <memory.h>
#include <dc/debug>
#include "globalworldmanager.h"
#include "../common/null.h"
#include "client.h"
#include "encodepackets.h"
#include "globalgamefilemanager.h"
#include <assert.h>

namespace Evidyon {

  
//----[  RegionSwarmSpawner  ]-------------------------------------------------
RegionSwarmSpawner::RegionSwarmSpawner() {
  region_ = NULL;
}




//----[  create  ]-------------------------------------------------------------
void RegionSwarmSpawner::create(
    int region_left,
    int region_top,
    const GameFile::ServerGameFileMapRegionData* data) {
  for (int i = 0; i < NUMBER_OF_SWARM_SPAWNS; ++i) {
    swarms_[i] = data->swarm_spawn[i].lifeform_ai;
  }
  for (int i = 0; i < NUMBER_OF_SPAWN_LOCATIONS; ++i) {
    SpawnLocation* location = &spawn_locations_[i];
    location->x = region_left
                  + data->spawn_locations[i].x_subcoordinate
                  + 0.5;
    location->y = region_top
                  + data->spawn_locations[i].y_subcoordinate
                  + 0.5;
    location->swarms_mask = data->spawn_locations[i].swarms_mask;
    location->spawn_point_index = data->spawn_locations[i].spawn_point;
    location->intended_level = data->spawn_locations[i].intended_level;
  }
  for (int i = 0; i < NUMBER_OF_INHABITANTS; ++i) {
    inhabitants_[i].lifeform_type = data->inhabitants[i].lifeform_ai;
    inhabitants_[i].spawn_point_index = data->inhabitants[i].spawn_point;
    inhabitants_[i].x = data->inhabitants[i].x_subcoordinate
                        + region_left
                        + 0.5;
    inhabitants_[i].z = data->inhabitants[i].y_subcoordinate
                        + region_top
                        + 0.5;
  }
}


//----[  activate  ]-----------------------------------------------------------
void RegionSwarmSpawner::activate(WorldRegion* local_region) {
  region_ = local_region;
  just_activated_ = true;
  next_spawn_timer_.enable();
}



//----[  deactivateIfRegionIs  ]-----------------------------------------------
bool RegionSwarmSpawner::deactivateIfRegionIs(WorldRegion* local_region) {
  if (region_ == local_region) {
    next_spawn_timer_.disable();
    region_ = NULL;
    return true;
  } else {
    return false;
  }
}




//----[  update  ]-------------------------------------------------------------
void RegionSwarmSpawner::update(
    Server::GlobalLifeformAIManager* lifeform_ai_manager) {
  if (just_activated_) { // spawn a complete set of monsters
    just_activated_ = false;

    for (int i = 0; i < NUMBER_OF_INHABITANTS; ++i) {
      if (inhabitants_[i].lifeform_type == LifeformAI::INVALID_LIFEFORM_AI_INDEX) break;
      LifeformAI::SpawnPointIndex spawn_point_index
        = inhabitants_[i].spawn_point_index;
      //assert(!lifeform_ai_manager->spawnPointFilled(spawn_point_index));
      if (lifeform_ai_manager->spawnPointFilled(spawn_point_index)) continue;
      lifeform_ai_manager->spawn(
        inhabitants_[i].lifeform_type,
        spawn_point_index,
        region_->getMap(),
        1, // no intended avatar level
        inhabitants_[i].x,
        inhabitants_[i].z,
        false,
        NULL);
      assert(lifeform_ai_manager->spawnPointFilled(spawn_point_index));
    }

    for (int i = 0; i < NUMBER_OF_SPAWN_LOCATIONS; ++i) {
      SpawnLocation* location = &spawn_locations_[i];
      unsigned int mask = location->swarms_mask;
      if (!mask) continue;

      // pick a lifeform to spawn
      unsigned int rotate = rand()%32;
      mask = (mask << (32-rotate)) | (mask >> (rotate));

      // find the number of trailing zeros in 32-bit mask 
      unsigned int r;           // result goes here
      static const unsigned int MultiplyDeBruijnBitPosition[32] = {
        0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
        31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
      };
      r = MultiplyDeBruijnBitPosition[((unsigned __int32)((mask & -mask) * 0x077CB531U)) >> 27];
      LifeformAI::LifeformAIIndex lifeform_ai = swarms_[(r + rotate)%32];
      assert(lifeform_ai != LifeformAI::INVALID_LIFEFORM_AI_INDEX);

      // spawn a monster!  we have to check to see if it has already
      // been spawned, to cover this case:
      //  - monster is spawned
      //  - monster changes regions
      //  - region is erased, leaving monster
      //  - region is reinstantiated, spawning monster
      if (lifeform_ai_manager
            ->spawnPointFilled(location->spawn_point_index)) continue;
      Time::Milliseconds delay;
      lifeform_ai_manager->spawn(
        lifeform_ai,
        location->spawn_point_index,
        region_->getMap(),
        location->intended_level,
        location->x,
        location->y,
        false,
        &delay);
    }
    next_spawn_timer_.set(30000);
  } else { // spawn normally
    if (!next_spawn_timer_.expired()) return;

    for (int i = 0; i < NUMBER_OF_INHABITANTS; ++i) {
      if (inhabitants_[i].lifeform_type == LifeformAI::INVALID_LIFEFORM_AI_INDEX) break;
      LifeformAI::SpawnPointIndex spawn_point_index
        = inhabitants_[i].spawn_point_index;
      if (lifeform_ai_manager
            ->spawnPointFilled(spawn_point_index)) {
        continue;
      }
      lifeform_ai_manager->spawn(
        inhabitants_[i].lifeform_type,
        spawn_point_index,
        region_->getMap(),
        1, // no intended level
        inhabitants_[i].x,
        inhabitants_[i].z,
        true,
        NULL);
    }

    static const size_t SPAWN_LIMIT = 25;
    if (region_->numberOfNearbyActors() >= SPAWN_LIMIT) return;
    for (int i = 0; i < NUMBER_OF_SPAWN_LOCATIONS; ++i) {
      SpawnLocation* location = &spawn_locations_[i];
      LifeformAI::SpawnPointIndex spawn_point_index
        = location->spawn_point_index;
      unsigned int mask = location->swarms_mask;
      if (!mask || lifeform_ai_manager->spawnPointFilled(spawn_point_index)) continue;

      // pick a lifeform to spawn
      unsigned int rotate = rand()%32;
      mask = (mask << (32-rotate)) | (mask >> (rotate));

      // find the number of trailing zeros in 32-bit mask 
      unsigned int r;           // result goes here
      static const unsigned int MultiplyDeBruijnBitPosition[32] = {
        0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
        31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
      };
      r = MultiplyDeBruijnBitPosition[((unsigned __int32)((mask & -mask) * 0x077CB531U)) >> 27];
      LifeformAI::LifeformAIIndex lifeform_ai = swarms_[(r + rotate)%32];
      assert(lifeform_ai != LifeformAI::INVALID_LIFEFORM_AI_INDEX);

      // spawn a monster!
      Time::Milliseconds delay;
      lifeform_ai_manager->spawn(
        lifeform_ai,
        spawn_point_index,
        region_->getMap(),
        location->intended_level,
        location->x,
        location->y,
        true,
        &delay);
      //next_spawn_timer_.set(500+delay);
      next_spawn_timer_.set(30000);
      return;
    }
  }
}



}