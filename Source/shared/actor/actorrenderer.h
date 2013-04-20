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
#ifndef __SHARED_ACTOR_ACTORRENDERER_H__
#define __SHARED_ACTOR_ACTORRENDERER_H__
#pragma once




#include "actorprofileindex.h"
#include "shared/scenery/sceneryindex.h"
#include <dcx/memorypool>
#include "renderableactorinstance.h"
#include "actorid.h"
#include "actortemplateindex.h"
#include "shared/evidyon-limits.h"

namespace Evidyon {
namespace SkinnedMesh {
struct SkinnedMeshInstance;
class SkinnedMeshRenderer;
}
namespace Actor {
struct ActorProfile;
struct ActorTemplate;
}
namespace Sound {
class SoundManager;
}
namespace Client {
class ClientData;
}
namespace Scenery {
class SceneryRenderer;
}
namespace SpecialFX {
class SpecialFXManager;
}
}

namespace Evidyon {
namespace Actor {



//----[  ActorRenderer  ]------------------------------------------------------
// TODO: this is more of a manager than a renderer, since it does sound also
// and it handles network message (sync) data, and other stuff...
class ActorRenderer {
  static const int MAX_ACTOR_INSTANCES = Limits::ACTORS_ON_SCREEN;
  typedef dcx::dcxMemoryPool<RenderableActorInstance,
                             MAX_ACTOR_INSTANCES> RenderableInstances;

public:
  ActorRenderer();
  bool create(SkinnedMesh::SkinnedMeshRenderer* skinned_mesh_renderer,
              Scenery::SceneryRenderer* scenery_renderer,
              Sound::SoundManager* sound_manager,
              SpecialFX::SpecialFXManager* special_fx_manager);
  void destroy();

  // Fills the renderer with actor profile configurations
  bool initNumberOfActorProfiles(size_t count);
  bool initActorProfile(size_t index, const ActorProfile* actor_profile);
  void clearActorProfiles();

  // Gets the profile at the given index.  If the profile index is invalid,
  // this method returns NULL.
  const ActorProfile* actorProfile(ActorProfileIndex actor_profile);

  // Obtains an actor instance with the given template/profile/form.  If
  // too many actors are acquired, this method will fail by returning
  // NULL until some are released.
  // If the actor is dead (for sync messages check if the action is
  // ACTORACTION_DIE) the actor will be set at the final frame of one of
  // the death animations.  Otherwise, no animation will be set until
  // updateInstance is called.
  // IMPORTANT:  after calling this method, call updateEquipment on the
  // returned instance or none of the equipped items will show up.
  RenderableActorInstance* acquire(const ActorTemplate* actor_template,
                                   ActorProfileIndex actor_profile,
                                   SkinnedMesh::SkinnedMeshFormIndex form,
                                   bool dead);
  void release(RenderableActorInstance* instance);

  // Removes all acquired instances from the world.  Use with caution.
  void clearInstances();

  // Advances all variable attributes of the provided actor instance:
  //  - whether or not the actor is hidden (based on the map)
  //  - the actor's profile specialization type (based on map & description)
  //  - animation states
  //  - movement angle & location
  //  - equipped scenery transforms
  // Returns in *has_timed_out whether or not this instance should be erased.
  void updateInstance(RenderableActorInstance* instance,
                      double time,
                      double time_since_last_update,
                      Evidyon::Client::ClientData* client_data);

  // Checks to see if the items attached to the instance match those provided
  // as the equipped_items parameter.  If not, each mismatched entry's old
  // equipment scenery is released and new equipment scenery is allocated.
  // 'equipped_items' should be NUMBER_OF_ACTOR_EQUIPMENT_SLOTS
  // entries in length.
  // Passing NULL for equipped_items is valid.  This causes the renderer to
  // release and re-acquire all of the equipment on the instance.  It is much
  // preferable to pass a valid array because instances that stay the same
  // will not go through the reallocation process.
  // If reequip_all_items is set, equipment whose scenery index matches in
  // the passed array and the current set of equipment will still be
  // reallocated.  This flag should be set when an instance is being
  // created for the first time with a defined set of equipment, since 
  // all of its equipment needs to be allocated anyway.  This flag has no
  // effect if equipped_items is null.
  void updateEquipment(RenderableActorInstance* instance,
                       const Scenery::SceneryIndex* equipped_items,
                       bool reequip_all_items);

  // Changes the profile and/or form of an instance.
  void mutate(RenderableActorInstance* instance,
              ActorProfileIndex actor_profile,
              SkinnedMesh::SkinnedMeshFormIndex form);

  // Initializes the state of an instance with its very first sync.  It is
  // not necessary to follow this method with a call to sync().
  void firstSync(RenderableActorInstance* instance,
                 float x,
                 float y,
                 ActorSyncFlags flags);

  // Updates the attributes of this instance with sync data.  This data is
  // intended to come from the server; however, for testing purposes the
  // editor can spoof sync messages at a regular rate to preview how the
  // actor instance behaves.
  void sync(RenderableActorInstance* instance,
            float time_since_previous_sync,
            double time,
            float x,
            float y,
            ActorSyncFlags flags);

private:
  void zero();

  // Frees all of the internal objects associated with the given instance.
  void deallocate(RenderableActorInstance* instance);

  // Plays sound the actor makes when performing an action.
  void playActionSound(const RenderableActorInstance* instance,
                       const ActorProfile* instance_profile,
                       ActorAction action);

private:

  // Holds all allocated instances
  RenderableInstances renderable_instances_;

  // Draws the forms of the actors
  SkinnedMesh::SkinnedMeshRenderer* skinned_mesh_renderer_;

  // Renders equipped items
  Scenery::SceneryRenderer* scenery_renderer_;

  // Plays actor sounds based on their behavior; footsteps, get hit, etc.
  Sound::SoundManager* sound_manager_;

  // Allows effects to be attached to actors
  SpecialFX::SpecialFXManager* special_fx_manager_;

  ActorProfile* actor_profiles_;
  size_t number_of_actor_profiles_;

  ActorTemplate* actor_templates_;
  size_t number_of_actor_templates_;
};




}
}

#endif