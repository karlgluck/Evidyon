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
#ifndef __SERVER_LIFEFORMAI_GLOBALLIFEFORMAIMANAGER_H__
#define __SERVER_LIFEFORMAI_GLOBALLIFEFORMAIMANAGER_H__
#pragma once

#include "shared/time/timer.h"
#include "common/number.h"
#include "shared/lifeformai/lifeformaiindex.h"
#include "shared/lifeformai/spawnpointindex.h"
#include "shared/avatar/avatarlevel.h"

#include <hash_map>

namespace Evidyon {
class GlobalActorManager;
class Map;
class WorldRegion;
namespace Server {
class LifeformAIInstance;
}
namespace LifeformAI {
struct LifeformAI;
}
}

namespace Evidyon {
namespace Server {



//----[  GlobalLifeformAIManager  ]--------------------------------------------
class GlobalLifeformAIManager {
  static const size_t MAXIMUM_ACTIVE_LIFEFORMS = 2048;
  static const size_t MAXIMUM_DELAYED_SPAWNS = 64;

  struct DelayedLifeformSpawn {
    LifeformAIInstance* instance;
    Time::Timer<Time::RESOLUTION_500_MS> enter_timer;
    Map* map;
    LifeformAI::SpawnPointIndex spawn_point_index;
  };

  struct ActiveLifeform {
    LifeformAIInstance* instance;
    LifeformAI::SpawnPointIndex spawn_point_index;
  };

  typedef stdext::hash_map<LifeformAI::SpawnPointIndex,
                           Server::LifeformAIInstance*> SpawnPointInstances;

public:
  GlobalLifeformAIManager();
  bool create(GlobalActorManager* actor_manager);
  void destroy();

  // Sets up the lifeform types using data from the game file
  bool initNumberOfDescriptions(size_t number);
  void initDescription(Evidyon::LifeformAI::LifeformAIIndex index,
                       const Evidyon::LifeformAI::LifeformAI* description);

  // Generates a new lifeform instance.  If the 'run_spawn_event' flag is set
  // the lifeform will be spawned after first creating the spawn event and
  // waiting the configured delay time for the lifeform.  Otherwise, the
  // lifeform is created immediately.
  // The number of milliseconds until the lifeform will exist is
  // returned in delay_before_spawn.
  void spawn(LifeformAI::LifeformAIIndex lifeform,
             LifeformAI::SpawnPointIndex spawn_point_index,
             Map* map,
             Avatar::AvatarLevel intended_level,
             Number x,
             Number z,
             bool run_spawn_event,
             Time::Milliseconds* delay_before_spawn);

  // Removes all lifeforms that are dependant on the given region.
  void releaseRegion(WorldRegion* region);

  // Updates all active lifeforms
  void update(double time, double time_since_last_update);

  // Returns whether or not there is currently a lifeform in the world
  // that was spawned at the given spawn point
  bool spawnPointFilled(LifeformAI::SpawnPointIndex spawn_point_index) const;

  // Erases the actor spawned by the given spawn point, if it exists.
  void releaseSpawnPoint(LifeformAI::SpawnPointIndex spawn_point_index);

private:
  void zero();

  // Gets the lifeform description for a given type index
  const Evidyon::LifeformAI::LifeformAI*
    lifeformAIDescription(Evidyon::LifeformAI::LifeformAIIndex index);
  
  // Puts a new lifeform into the delayed buffer.  If doing this causes an
  // entry to be pushed out, that entry is spawned immediately before being
  // overwritten.
  void addDelayedLifeformSpawn(
    LifeformAIInstance* instance,
    Map* map,
    LifeformAI::SpawnPointIndex spawn_point_index,
    Time::Milliseconds delay);

  // Generates a new active lifeform, if doing so is possible.  If there are
  // already too many lifeforms or the target region is invalid, this
  // method is a no-op.
  void acquireActiveLifeformAI(
    LifeformAIInstance* instance,
    Map* map,
    LifeformAI::SpawnPointIndex spawn_point_index);

  // Erases the lifeform at the target index from the active list using
  // a swap-down from the end of the list.  This will decrement the
  // number of active lifeforms.
  void removeActiveLifeformAtIndex(size_t index);

private:

  GlobalActorManager* actor_manager_;

  // All lifeform type descriptions, taken from the game file
  LifeformAI::LifeformAI* descriptions_;
  size_t number_of_descriptions_;

  // Array holding all of the lifeforms in the game.  All of these lifeforms
  // are updated each frame.
  ActiveLifeform* active_lifeforms_;
  size_t number_of_active_lifeforms_;
  
  // This is a buffer of lifeforms that are entering the world through a
  // portal.  If the buffer fills completely, spawning another monster will
  // force one of the previous lifeforms to spawn.
  DelayedLifeformSpawn delayed_spawns_[MAXIMUM_DELAYED_SPAWNS];
  size_t number_of_delayed_spawns_;

  // Pairs a spawn ID with all of the active instances so that the spawners
  // can easily check to see whether or not a given spawn point is being used.
  SpawnPointInstances spawn_point_instances_;


public:
  static GlobalLifeformAIManager* singleton();
private:
  static GlobalLifeformAIManager* singleton_;
};



}
}

#endif