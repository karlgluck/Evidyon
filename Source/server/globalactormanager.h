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
#ifndef __GLOBALACTORMANAGER_H__
#define __GLOBALACTORMANAGER_H__
#pragma once


#include <map>
#include <hash_map>
#include <dcx/managedmemorypool>
#include "actor.h"
#include "shared/actor/actorid.h"

#include "shared/actor/actortemplateindex.h"


struct _iobuf;
typedef struct _iobuf FILE;

namespace Evidyon {

class WorldRegion;
class Map;

struct ServerActorDescriptionTemplate;



//----[  GlobalActorManager  ]-------------------------------------------------
class GlobalActorManager {

  // The maximum number of simultaneous actors is defined by the field in the
  // actor ID reserved for their index in this pool.
  typedef dcx::dcxManagedMemoryPool<ActorInstance, Evidyon::Actor::MAX_ACTORS_IN_WORLD> ActorMemoryPool;

  // It is safe to use the raw ActorInstance* pointer here, since this class controls
  // access to all of the ActorInstance* instances.  As a result, we can be 100%
  // confident that no actor will be removed without the pointers being
  // erased from these tables.

  // Contains all of the "spread" regions that the actor is inside
  typedef std::multimap<WorldRegion*,ActorInstance*> Regions;
  typedef std::map<Regions::value_type,Regions::iterator> RegionValueIterators;

  typedef stdext::hash_map<Evidyon::Actor::ActorID,
                           ActorInstance*> ActiveActors;

public:
  // This is a globally-standardized number of entries that should be allowed
  // in a search for actors; i.e. creating a static buffer of this size when
  // doing a search for actors is usually adequate.
  static const int STD_ACTOR_SEARCH_BUFFER_SIZE = 16;

public:
  typedef ActorMemoryPool::Pointer ActorPointer;

public:
  static GlobalActorManager* singleton();

public:

  GlobalActorManager();
  bool create();
  void destroy();

  // All non-sequential uses of any ActorInstance pointer should use the 'ActorPointer'
  // type.  Using this method allows such pointers to be automatically
  // invalidated when the actor is released.
  // The description_template parameter is a reference to the client-side
  // ClientActorDescriptionTemplate on which this actor is based.
  bool acquireActor(ActorPointer* pointer,
                    ActorController* controller,
                    Evidyon::Actor::ActorTemplateIndex description_template,
                    Map* map, float x, float y);
  void releaseActor(ActorPointer* pointer);
  void getPointer(ActorInstance* actor, ActorPointer* pointer);
  void getPointer(int actor_id, ActorPointer* pointer);

  ActorInstance* getActor(Evidyon::Actor::ActorID id);

  void setSyncGroup(int sync_group);


  // Writes information about this class useful to a debugging the server.  This data
  // is returned to the admin console.
  void printDebugStats(char* buffer, size_t buffer_size);

private:

  ActorMemoryPool actor_memory_pool_;
  int next_available_actor_id_;

  ActiveActors active_actors_;

private:
  static GlobalActorManager* singleton_;
};


}

#endif