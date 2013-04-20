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
#include "globallifeformaimanager.h"
#include "server/worldregion.h"
#include "server/map.h"
#include "lifeformaiinstance.h"
#include "common/null.h"
#include "common/safemacros.h"
#include "shared/lifeformai/lifeformai.h"
#include "server/event/eventidgenerator.h"
#include <assert.h>

#include <dc/debug>

namespace Evidyon {
namespace Server {



  
//----[  singleton_  ]---------------------------------------------------------
GlobalLifeformAIManager* GlobalLifeformAIManager::singleton_ = NULL;

  


//----[  GlobalLifeformAIManager  ]--------------------------------------------
GlobalLifeformAIManager::GlobalLifeformAIManager() {
  assert(singleton_ == NULL);
  zero();
  singleton_ = this;
}





//----[  create  ]-------------------------------------------------------------
bool GlobalLifeformAIManager::create(GlobalActorManager* actor_manager) {
  actor_manager_ = actor_manager;
  active_lifeforms_ = new ActiveLifeform[MAXIMUM_ACTIVE_LIFEFORMS];
  if (!active_lifeforms_) return false;
  memset(active_lifeforms_,
         0,
         sizeof(*active_lifeforms_)*MAXIMUM_ACTIVE_LIFEFORMS);
  number_of_active_lifeforms_ = 0;
  return true;
}





//----[  destroy  ]------------------------------------------------------------
void GlobalLifeformAIManager::destroy() {
  for (size_t i = number_of_active_lifeforms_; i > 0;) {
    --i;
    LifeformAIInstance* instance = active_lifeforms_[i].instance;
    active_lifeforms_[i].instance = NULL;
    instance->release(actor_manager_);
    SAFE_DELETE(instance);
  }
  for (size_t i = 0; i < MAXIMUM_DELAYED_SPAWNS; ++i) {
    DelayedLifeformSpawn* spawn = &delayed_spawns_[i];
    spawn->enter_timer.disable();
  }
  SAFE_DELETE_ARRAY(active_lifeforms_);
  SAFE_DELETE_ARRAY(descriptions_);
  assert(singleton_ == this);
  singleton_ = NULL;
  zero();
}





//----[  initNumberOfDescriptions  ]-------------------------------------------
bool GlobalLifeformAIManager::initNumberOfDescriptions(size_t number) {
  descriptions_ = new LifeformAI::LifeformAI[number];
  number_of_descriptions_ = number;
  return descriptions_ != NULL;
}





//----[  initDescription  ]----------------------------------------------------
void GlobalLifeformAIManager::initDescription(
    Evidyon::LifeformAI::LifeformAIIndex index,
    const Evidyon::LifeformAI::LifeformAI* description) {
  assert(index < number_of_descriptions_);
  memcpy(&descriptions_[index], description, sizeof(*description));
}







//----[  spawn  ]--------------------------------------------------------------
void GlobalLifeformAIManager::spawn(
    LifeformAI::LifeformAIIndex lifeform,
    LifeformAI::SpawnPointIndex spawn_point_index,
    Map* map,
    Avatar::AvatarLevel intended_level,
    Number x,
    Number z,
    bool run_spawn_event,
    Time::Milliseconds* delay_before_spawn) {
  const LifeformAI::LifeformAI* description = lifeformAIDescription(lifeform);
  assert(description);
  assert(!spawnPointFilled(spawn_point_index));
  if (!description ||
      (number_of_active_lifeforms_ >= MAXIMUM_ACTIVE_LIFEFORMS) ||
      spawnPointFilled(spawn_point_index)) {
    return;
  }
  WorldRegion* region = map->getRegionFromPoint(x.wholePart(), z.wholePart());
  assert(region);
  if (!region) return;
  LifeformAIInstance* instance
    = LifeformAIInstance::New(intended_level,
                              x,
                              z,
                              description);
  bool spawn_immediately = !run_spawn_event;
  if (run_spawn_event) {
    if (description->spawn_event != Event::INVALID_EVENT_INDEX) {
      region->broadcastEvent_GenerateTargetedSpecialFX(
        GenerateEventID(),
        description->spawn_event,
        x, z,
        x, z,
        NULL,
        0);
    }
    spawn_immediately = description->spawn_delay == 0;
    if (!spawn_immediately) {
      addDelayedLifeformSpawn(instance,
                              map,
                              spawn_point_index,
                              description->spawn_delay);
      if (delay_before_spawn) *delay_before_spawn = description->spawn_delay;
    }
  }
  
  if (spawn_immediately) {
    acquireActiveLifeformAI(instance,
                            map,
                            spawn_point_index);
    if (delay_before_spawn) *delay_before_spawn = NULL;
  }

  spawn_point_instances_.insert(
    SpawnPointInstances::value_type(spawn_point_index, instance));
}





//----[  releaseRegion  ]------------------------------------------------------
void GlobalLifeformAIManager::releaseRegion(WorldRegion* region) {
  for (size_t i = number_of_active_lifeforms_; i > 0;) {
    LifeformAIInstance* instance = active_lifeforms_[--i].instance; // <- iterator here
    assert(instance);
    if (instance->shouldReleaseWithWorldRegion(region)) {
      instance->release(actor_manager_);
      SAFE_DELETE(instance);
      removeActiveLifeformAtIndex(i);
    }
  }
}





//----[  update  ]-------------------------------------------------------------
void GlobalLifeformAIManager::update(double time,
                                     double time_since_last_update) {
  for (size_t i = number_of_active_lifeforms_; i > 0;) {
    LifeformAIInstance* instance = active_lifeforms_[--i].instance; // <- iterator here
    assert(instance);
    if (!instance->update(time, time_since_last_update)) {
      instance->release(actor_manager_);
      SAFE_DELETE(instance);
      removeActiveLifeformAtIndex(i);
    }
  }

  // Bring in delayed spawns.  We can't do this in any kind of ordered sequence
  // since the timers can expire at different times!
  size_t number_of_delayed_spawns = number_of_delayed_spawns_;
  for (size_t i = MAXIMUM_DELAYED_SPAWNS;
       number_of_delayed_spawns_ > 0 && i > 0;) {
    DelayedLifeformSpawn* spawn = &delayed_spawns_[--i]; // <- iterator here
    if (spawn->enter_timer.poll()) {
      assert(spawn->instance);
      spawn->enter_timer.disable();
      acquireActiveLifeformAI(spawn->instance,
                              spawn->map,
                              spawn->spawn_point_index);
      spawn->instance = NULL;
      --number_of_delayed_spawns_;
    }
  }
}




//----[  spawnPointFilled  ]---------------------------------------------------
bool GlobalLifeformAIManager::spawnPointFilled(
    LifeformAI::SpawnPointIndex spawn_point_index) const {
  return spawn_point_instances_.find(spawn_point_index)
            != spawn_point_instances_.end();
}




//----[  releaseSpawnPoint  ]--------------------------------------------------
void GlobalLifeformAIManager::releaseSpawnPoint(
    LifeformAI::SpawnPointIndex spawn_point_index) {
  SpawnPointInstances::iterator i
    = spawn_point_instances_.find(spawn_point_index);
  if (i != spawn_point_instances_.end()) {
    LifeformAIInstance* instance = i->second;
    instance->despawn();  // this object will be erased by the update loop
    spawn_point_instances_.erase(i);
  }
}


//----[  zero  ]---------------------------------------------------------------
void GlobalLifeformAIManager::zero() {
  actor_manager_ = NULL;
  descriptions_ = NULL;
  number_of_descriptions_ = 0;
  active_lifeforms_ = NULL;
  number_of_active_lifeforms_ = 0;
  memset(delayed_spawns_, 0, sizeof(delayed_spawns_));
  number_of_delayed_spawns_ = 0;
  for (size_t i = 0; i < MAXIMUM_DELAYED_SPAWNS; ++i) {
    DelayedLifeformSpawn* spawn = &delayed_spawns_[i];
    spawn->instance = NULL;
    spawn->map = NULL;
    spawn->spawn_point_index = LifeformAI::INVALID_SPAWN_POINT_INDEX;
    spawn->enter_timer.reset();
  }
  spawn_point_instances_.clear();
}





//----[  lifeformAIDescription  ]----------------------------------------------
const Evidyon::LifeformAI::LifeformAI*
  GlobalLifeformAIManager::lifeformAIDescription(
      Evidyon::LifeformAI::LifeformAIIndex index) {
  assert(index < number_of_descriptions_);
  return (index < number_of_descriptions_) ? &descriptions_[index] : NULL;
}






//----[  addDelayedLifeformSpawn  ]--------------------------------------------
void GlobalLifeformAIManager::addDelayedLifeformSpawn(
    LifeformAIInstance* instance,
    Map* map,
    LifeformAI::SpawnPointIndex spawn_point_index,
    Time::Milliseconds delay) {
  if (number_of_delayed_spawns_ == MAXIMUM_DELAYED_SPAWNS) {
    // spawn the next element immediately
    DelayedLifeformSpawn* spawn = &delayed_spawns_[0];
    acquireActiveLifeformAI(spawn->instance,
                            spawn->map,
                            spawn->spawn_point_index);
    // just write to the start, since we don't really have a choice of where
    // to put this element
    spawn->instance = instance;
    spawn->map = map;
    spawn->spawn_point_index = spawn_point_index;
    spawn->enter_timer.set(delay); // don't have to disable since we are reusing
  } else {
    for (size_t i = 0; i < MAXIMUM_DELAYED_SPAWNS; ++i) {
      DelayedLifeformSpawn* spawn = &delayed_spawns_[i];
      if (spawn->instance != NULL) continue;
      spawn->instance = instance;
      spawn->map = map;
      spawn->spawn_point_index = spawn_point_index;
      spawn->enter_timer.enable(delay);
      ++number_of_delayed_spawns_;
      break;
    }
  }
}






//----[  acquireActiveLifeformAI  ]--------------------------------------------
void GlobalLifeformAIManager::acquireActiveLifeformAI(
    LifeformAIInstance* instance,
    Map* map,
    LifeformAI::SpawnPointIndex spawn_point_index) {
  assert(instance);
  if (instance->acquire(map, actor_manager_)) {
    ActiveLifeform* active_lifeform
      = &active_lifeforms_[number_of_active_lifeforms_++];
    active_lifeform->instance = instance;
    active_lifeform->spawn_point_index = spawn_point_index;
  } else {
    SAFE_DELETE(instance);
    spawn_point_instances_.erase(spawn_point_index);
  }
}






//----[  removeActiveLifeformAtIndex  ]----------------------------------------
void GlobalLifeformAIManager::removeActiveLifeformAtIndex(size_t index) {
  ActiveLifeform* erased_element
    = &active_lifeforms_[index];
  spawn_point_instances_.erase(erased_element->spawn_point_index);
  memcpy(&active_lifeforms_[index],
         &active_lifeforms_[--number_of_active_lifeforms_],
         sizeof(*active_lifeforms_));
  erased_element = &active_lifeforms_[number_of_active_lifeforms_];
  erased_element->instance = NULL;
  erased_element->spawn_point_index = LifeformAI::INVALID_SPAWN_POINT_INDEX;
}




//----[  singleton  ]----------------------------------------------------------
GlobalLifeformAIManager* GlobalLifeformAIManager::singleton() {
  assert(singleton_);
  return singleton_;
}



}
}

