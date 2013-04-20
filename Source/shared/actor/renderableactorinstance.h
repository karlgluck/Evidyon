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
#ifndef __RENDERABLEACTORINSTANCE_H__
#define __RENDERABLEACTORINSTANCE_H__
#pragma once



#include "actorsyncflags.h"
#include "actorid.h"
#include "actortemplate.h"
#include "actoraction.h"
#include "actorprofilespecializationtype.h"
#include "constantactorattachmentpoint.h"
#include "shared/binding/actorbindingptr.h"


namespace Evidyon {
namespace SkinnedMesh {
struct SkinnedMeshInstance;
}
namespace Actor {
struct ActorProfile;
}
namespace Scenery {
struct SceneryInstance;
}
}


namespace Evidyon {
namespace Actor {



  
//----[  NUMBER_OF_TOTAL_ACTOR_ATTACHMENT_POINTS  ]----------------------------
static const unsigned int NUMBER_OF_TOTAL_ACTOR_ATTACHMENT_POINTS
  = NUMBER_OF_SPECIALIZED_ACTOR_ATTACHMENT_POINTS
   + NUMBER_OF_CONSTANT_ACTOR_ATTACHMENT_POINTS;


//----[  RenderableActorInstance  ]--------------------------------------------
// This structure is allocated by the actor manager and represents a complex
// animated character controlled by messages from the server.
struct RenderableActorInstance {

  // Changes the contents of this structure to a default state
  void reset();

  // This flag is set when the actor runs the "get hit" animation so that
  // idles are done in the combat pose.  This is cleared when the player moves.
  bool in_combat;

  enum BaseAnimationState {
    DEAD,
    MOVING,
    COMBAT_WALK,
    IDLE,
    COMBAT_IDLE,
    SWIM,
    SWIM_IDLE,
    INVALID_BASE_ANIMATION_STATE,
  } base_animation_state;

  struct MovementState {
    float x, y, angle, speed, rendering_angle;
    float vertical;
  };
  MovementState current_movement_state, movement_state_at_last_sync;

  // This is a list of all items attached to the instance.  The constant
  // attachment points are first in this array, followed by the specialized
  // ones.
  struct AttachedItem {
     Scenery::SceneryInstance* item_scenery;
  };
  AttachedItem attached_items[NUMBER_OF_TOTAL_ACTOR_ATTACHMENT_POINTS];

  // When an actor is moving, its footsteps are played.  This controls a splash
  // sound when in water.
  double last_footstep_sound_time;

  // Basic quasi-stateless description of this actor's attributes.  Used to
  // instantiate the actor, and holds some of its core information such as
  // its name, action speeds, alignment and profile type.
  ActorTemplate base;

  // This structure holds the information most recently received from the server
  // about the actor's attributes.
  struct {
    double time_received;   // timestamp when the sync was received
    float x, y;             // actor's world location
    float direction;        // calculated from the movement since last sync
    float speed;            // calculated
    float time_since_previous_sync; // slows down the actor if it is arriving
    float facing_angle; // the angle that the actor is looking
    bool alive;
    bool moving;
    bool teleport;
    bool leaping;
    bool wading;
    bool combat;
    bool swimming;
    bool falling;
    ActorSyncFlags flags;
  } last_sync;

  // What action animation the actor is executing.  If the skinned mesh
  // instance has no action animation referenced, this value should be ignored.
  // It is used to allow higher-priority actions to execute before lower-
  // priority ones.  The lower priority action is an action that arrived during
  // but did not replace the current action.  It is executed if the current
  // action terminates within 2 syncs of the low action being received.
  ActorAction current_action, lower_priority_action;

  // The current profile.  This determines the skinned mesh type and all of the
  // different animation sets.
  const ActorProfile* profile;

  // The instance of the animated mesh used to render this actor
  SkinnedMesh::SkinnedMeshInstance* skinned_mesh_instance;

  // This is the current profile specialization.  Its default value is that
  // defined in the base template, which is derived from the actor's equipment.
  // However, other specializations require this value to be changed.  For
  // example, actors that swim.
  ActorProfileSpecializationType profile_specialization;

  // This binding is used to attach special effects to the actor.  Every actor
  // has one of these that is updated whenever the actor moves.
  Binding::ActorBindingPtr binding;
};



}
}

#endif