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
#include "globalactormanager.h"
#include "worldregion.h"
#include "globalworldmanager.h"
#include "shared/actor/actordescriptiontemplate.h"
#include "../common/safemacros.h"

#include <dc/debug>

namespace Evidyon {

GlobalActorManager* GlobalActorManager::singleton_ = NULL;

GlobalActorManager* GlobalActorManager::singleton() { return singleton_; }

GlobalActorManager::GlobalActorManager() {
}

bool GlobalActorManager::create() {
  if (!actor_memory_pool_.create()) return false;
  next_available_actor_id_ = 0;
  singleton_ = this;
  return true;
}


void GlobalActorManager::destroy() {
  actor_memory_pool_.destroy();
  singleton_ = NULL;
}




//----[  acquireActor  ]-------------------------------------------------------
bool GlobalActorManager::acquireActor(
    ActorPointer* pointer,
    ActorController* controller,
    Evidyon::Actor::ActorTemplateIndex description_template,
    Map* map,
    float x,
    float y) {
  if (!actor_memory_pool_.acquire(pointer)) return false;
  Evidyon::Actor::ActorID id
    = Evidyon::Actor::PackActorID(++next_available_actor_id_,
                                  actor_memory_pool_.indexOf(pointer),
                                  description_template);
  ActorInstance* actor = pointer->dereferenceAssumingValid();
  actor->acquire(controller,
                 id,
                 map,
                 x,
                 y);
  active_actors_.insert(ActiveActors::value_type(id, actor));
  return true;
}




//----[  releaseActor  ]-------------------------------------------------------
void GlobalActorManager::releaseActor(ActorPointer* pointer) {
  ActorInstance* actor = pointer->dereference();
  if (actor) {
    WorldRegion* local_region = actor->getRegion();
    Map* map = local_region->getMap();
    active_actors_.erase(actor->getIDNumber());
    actor->release();
  }
  actor_memory_pool_.release(pointer);
}




//----[  getPointer  ]---------------------------------------------------------
void GlobalActorManager::getPointer(ActorInstance* actor, ActorPointer* pointer) {
  actor_memory_pool_.convert(actor, pointer);
}




//----[  getPointer  ]---------------------------------------------------------
void GlobalActorManager::getPointer(int actor_id, ActorPointer* pointer) {
  ActiveActors::iterator i = active_actors_.find(actor_id);
  if (i == active_actors_.end()) {
    pointer->reset();
  } else {
    actor_memory_pool_.convert(i->second, pointer);
  }
}

ActorInstance* GlobalActorManager::getActor(Evidyon::Actor::ActorID id) {
  ActiveActors::iterator i = active_actors_.find(id);
  return i == active_actors_.end() ? NULL : i->second;
}

void GlobalActorManager::setSyncGroup(int sync_group) {
  for (ActorMemoryPool::Iterator i(&actor_memory_pool_);
     i.hasNext();
     i.advance()) {
    ActorInstance* actor = i.get();
    //CONFIRM(actor->getRegion()) else return;
    actor->setSyncGroup(sync_group);
  }
}


void GlobalActorManager::printDebugStats(char* buffer, size_t buffer_size) {
  sprintf_s(buffer,
            buffer_size,
            "%lu actors allocated (%lu available); %lu active",
            actor_memory_pool_.numberOfAllocatedEntries(),
            actor_memory_pool_.numberOfFreeEntries(),
            active_actors_.size());
}


}