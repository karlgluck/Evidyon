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
#include "actorrenderer.h"
#include "actorprofile.h"
#include "actortemplate.h"
#include "shared/skinned-mesh/skinnedmeshrenderer.h"
#include "shared/scenery/sceneryrenderer.h"
#include "shared/sound/soundmanager.h"
#include "shared/client/clientdata.h"
#include <assert.h>
#include "common/null.h"
#include "common/safemacros.h"
#include "renderableactorinstance.h"
#include "shared/skinned-mesh/skinnedmeshanimation.h"
#include "shared/binding/actorbinding.h"
#include "shared/binding/delaybinding.h"
#include "shared/specialfx/specialfxmanager.h"

#include "common/fracture/todo.h"

namespace Evidyon {
namespace Actor {


  
//----[  ActorRenderer  ]------------------------------------------------------
ActorRenderer::ActorRenderer() {
  zero();
}




//----[  create  ]-------------------------------------------------------------
bool ActorRenderer::create(
    SkinnedMesh::SkinnedMeshRenderer* skinned_mesh_renderer,
    Scenery::SceneryRenderer* scenery_renderer,
    Sound::SoundManager* sound_manager,
    SpecialFX::SpecialFXManager* special_fx_manager) {

  skinned_mesh_renderer_ = skinned_mesh_renderer;
  scenery_renderer_ = scenery_renderer;
  sound_manager_ = sound_manager;
  special_fx_manager_ = special_fx_manager;

  return renderable_instances_.create();
}





//----[  destroy  ]------------------------------------------------------------
void ActorRenderer::destroy() {
  clearInstances();
  renderable_instances_.destroy();
  clearActorProfiles();
  zero();
}







//----[  initNumberOfActorProfiles  ]------------------------------------------
bool ActorRenderer::initNumberOfActorProfiles(size_t count) {
  SAFE_DELETE_ARRAY(actor_profiles_);
  actor_profiles_ = new ActorProfile[count];
  assert(actor_profiles_);
  if (!actor_profiles_) { number_of_actor_profiles_ = 0; return false; }
  number_of_actor_profiles_ = count;
  return true;
}




//----[  initActorProfile  ]---------------------------------------------------
bool ActorRenderer::initActorProfile(size_t index, const ActorProfile* actor_profile) {
  assert(index < number_of_actor_profiles_);
  if (index >= number_of_actor_profiles_) return false;
  memcpy(&actor_profiles_[index], actor_profile, sizeof(ActorProfile));
  return true;
}




//----[  clearActorProfiles  ]-------------------------------------------------
void ActorRenderer::clearActorProfiles() {
  SAFE_DELETE_ARRAY(actor_profiles_);
  number_of_actor_profiles_ = 0;
}






//----[  actorProfile  ]-------------------------------------------------------
const ActorProfile* ActorRenderer::actorProfile(ActorProfileIndex actor_profile) {
  assert(actor_profile < number_of_actor_profiles_);
  if (actor_profile >= number_of_actor_profiles_) return NULL;
  return &actor_profiles_[actor_profile];
}









//----[  acquire  ]------------------------------------------------------------
RenderableActorInstance* ActorRenderer::acquire(
    const ActorTemplate* actor_template,
    ActorProfileIndex actor_profile,
    SkinnedMesh::SkinnedMeshFormIndex form,
    bool dead) {
  assert(actor_template);
  const ActorProfile* profile = actorProfile(actor_profile);
  assert(profile);
  if (!actor_template || !profile) return NULL;
  assert(skinned_mesh_renderer_);

  // Allocate the skinned mesh
  using namespace Evidyon::SkinnedMesh;
  SkinnedMeshInstance* skinned_mesh_instance
    = skinned_mesh_renderer_->acquire(profile->skinned_mesh, form);
  if (!skinned_mesh_instance) return NULL;
  skinned_mesh_instance->hidden = false;


  RenderableActorInstance* instance = renderable_instances_.acquire();
  if (!instance) {
    skinned_mesh_renderer_->release(skinned_mesh_instance);
    return NULL;
  }

  instance->reset();
  memcpy(&instance->base, actor_template, sizeof(ActorTemplate));
  instance->base.skinned_mesh_form = form;
  instance->base.actor_profile = actor_profile;
  instance->base_animation_state =
    RenderableActorInstance::INVALID_BASE_ANIMATION_STATE;
  instance->profile = profile;
  instance->skinned_mesh_instance = skinned_mesh_instance;
  instance->profile_specialization = actor_template->profile_specialization;

  // Put this actor in a death position if it is marked as dead.  This prevents
  // the dying animation from running on the first sync.
  if (dead) {
    skinned_mesh_renderer_->changeBasePose(
        profile->constant_animations[ANIMATION_DEATH_1+rand()%2],
        0.999,
        skinned_mesh_instance);
    instance->current_action = ACTORACTION_DIE;
    instance->base_animation_state = RenderableActorInstance::DEAD;
  } else {
    // this shouldn't be necessary, but was added to prevent
    // monsters from appearing huge when they first spawn
    skinned_mesh_renderer_->changeBasePose(
      profile->constant_animations[ANIMATION_IDLE],
      0.0,
      skinned_mesh_instance);
    instance->current_action = ACTORACTION_NONE;
  }

  // Generate the new actor instance
  instance->binding = Binding::ActorBinding::New(instance);

  // Return the newly created instance
  return instance;
}




//----[  release  ]------------------------------------------------------------
void ActorRenderer::release(RenderableActorInstance* instance) {
  assert(instance);
  deallocate(instance);
  renderable_instances_.release(instance);
}






//----[  clearInstances  ]-----------------------------------------------------
void ActorRenderer::clearInstances() {
  for (RenderableInstances::Iterator i(&renderable_instances_);
                                     i.hasNext();
                                     i.advance()) {
    RenderableActorInstance* instance = i.get();
    deallocate(instance);
  }
  renderable_instances_.releaseAll();
}








//----[  updateInstance  ]-----------------------------------------------------
void ActorRenderer::updateInstance(RenderableActorInstance* instance,
                                   double time,
                                   double time_since_last_update,
                                   Evidyon::Client::ClientData* client_data) {
  assert(instance);
  double time_since_last_sync = time - instance->last_sync.time_received;
  float distance_since_last_sync
    = (time_since_last_sync) * instance->last_sync.speed;
  float sync_x = instance->last_sync.x
                  + cosf(instance->last_sync.direction) * distance_since_last_sync;
  float sync_y = instance->last_sync.y
                  + sinf(instance->last_sync.direction) * distance_since_last_sync;
  float dx = sync_x - instance->current_movement_state.x;
  float dy = sync_y - instance->current_movement_state.y;
  float dd = sqrtf(dx*dx+dy*dy);

  static const float DISTANCE_THREASHOLD_STOP = 0.15f;
  static const float DISTANCE_THREASHOLD_TELEPORT = 1.0f;
  static const float TIME_THREASHOLD = 1.0f;
  bool last_sync_moving = instance->last_sync.moving;
  bool stop_moving = (last_sync_moving == false);
  stop_moving = stop_moving && ((dd < DISTANCE_THREASHOLD_STOP) ||
                                (time_since_last_sync > TIME_THREASHOLD));
  float current_speed;
  if (stop_moving) {
    current_speed = 0.0f;
    instance->current_movement_state.speed = 0.0f;
  } else {
    current_speed = instance->current_movement_state.speed;
    // is reaching the current location going to take less time than
    // a single sync?  if so, slow down; othwerise, go at the fastest speed
    float time_since_sync_before_last
      = instance->last_sync.time_since_previous_sync;
    if (dd/current_speed < time_since_sync_before_last) {
      current_speed = dd/time_since_sync_before_last;
    } else {
      current_speed = max(instance->last_sync.speed, current_speed);
    }
    instance->current_movement_state.speed = current_speed;
  }

  float current_angle = atan2f(dy,dx);
  instance->current_movement_state.angle = current_angle;

  float x = instance->current_movement_state.x;
  float z = instance->current_movement_state.y;
  float distance_to_move = current_speed * time_since_last_update;
  { // Above the teleport threshold, the actor speeds up
    bool out_of_sync = dd >= DISTANCE_THREASHOLD_TELEPORT;
    if (out_of_sync) distance_to_move *= 1.1f;
    distance_to_move = min(dd, distance_to_move);
    x += distance_to_move * cosf(current_angle);
    z += distance_to_move * sinf(current_angle);
    instance->current_movement_state.x = x;
    instance->current_movement_state.y = z;
  }

  // These integer coordinates are those of the tile on which the actor is
  // currently standing.
  int int_x = int(x), int_z = int(z);

  bool hidden = false;

  if (distance_to_move > 0.0f) {
    assert(client_data);

    // Play the walking sound
    if (instance->last_sync.leaping == false) {
      if (time - instance->last_footstep_sound_time > 1.5 / current_speed) {
        instance->last_footstep_sound_time = time;
        sound_manager_->playSound(Sound::SOUNDPRIORITY_AMBIENT,
                                  client_data->footstepSound(int_x, int_z));
      }
    }

    // Check to see whether or not this mesh is hidden
    hidden = !client_data->locationIsVisible(int_x, int_z);

    // The hidden state has changed!  Update the mesh's flag and all of the
    // attached scenery flags
    if (hidden != instance->skinned_mesh_instance->hidden) {
      instance->skinned_mesh_instance->hidden = hidden;
      for (int i = 0; i < NUMBER_OF_TOTAL_ACTOR_ATTACHMENT_POINTS; ++i) {
        Scenery::SceneryInstance* scenery = instance->attached_items[i].item_scenery;
        if (!scenery) continue;
        scenery->hidden = hidden;
      }
    }
  }

  bool actor_is_alive = instance->base_animation_state != RenderableActorInstance::DEAD;


  const ActorProfile* profile = instance->profile;
  const ActorProfileSpecialization* specialization
    = &profile->specializations[instance->profile_specialization];

  // Update the actor's animation
  if (instance->last_sync.swimming) {
    if (last_sync_moving) {
      if (current_speed != instance->last_sync.speed) {
        skinned_mesh_renderer_->changeBaseAnimationSpeed(
          time,
          current_speed,
          instance->skinned_mesh_instance);
      }
      if (instance->base_animation_state != RenderableActorInstance::SWIM) {
        skinned_mesh_renderer_->changeBaseAnimation(
          profile->constant_animations[ANIMATION_SWIM],
          time,
          current_speed,
          instance->skinned_mesh_instance);
        instance->base_animation_state = RenderableActorInstance::SWIM;
      }
    } else {
      if (instance->base_animation_state != RenderableActorInstance::SWIM_IDLE) {
        skinned_mesh_renderer_->changeBaseAnimation(
          profile->constant_animations[ANIMATION_SWIM],
          time,
          1.0f,
          instance->skinned_mesh_instance);
        instance->base_animation_state = RenderableActorInstance::SWIM_IDLE;
      }
    }
  } else {
    if (actor_is_alive) {
      if (last_sync_moving) {
        if (current_speed != instance->last_sync.speed) {
          skinned_mesh_renderer_->changeBaseAnimationSpeed(
            time,
            current_speed,
            instance->skinned_mesh_instance);
        }
        // animate walking
        if (instance->last_sync.moving && instance->last_sync.combat) {
          if (instance->base_animation_state != RenderableActorInstance::COMBAT_WALK) {
            skinned_mesh_renderer_->changeBaseAnimation(
              profile->constant_animations[ANIMATION_ATTACK_RUN],
              time,
              current_speed,
              instance->skinned_mesh_instance);
            instance->base_animation_state = RenderableActorInstance::COMBAT_WALK;
          }
        } else {
          if (instance->base_animation_state != RenderableActorInstance::MOVING) {
            skinned_mesh_renderer_->changeBaseAnimation(
              profile->constant_animations[ANIMATION_RUN],
              time,
              current_speed,
              instance->skinned_mesh_instance);
            instance->base_animation_state = RenderableActorInstance::MOVING;
          }
        }
      } else {
        // animate idle
        if (instance->in_combat) {
          if (instance->base_animation_state != RenderableActorInstance::COMBAT_IDLE) {
            skinned_mesh_renderer_->changeBaseAnimation(
              specialization->animations[ANIMATION_COMBAT_IDLE],
              time,
              1.0f,
              instance->skinned_mesh_instance);
            instance->base_animation_state = RenderableActorInstance::COMBAT_IDLE;
          }
        } else {
          if (instance->base_animation_state != RenderableActorInstance::IDLE) {
            skinned_mesh_renderer_->changeBaseAnimation(
              profile->constant_animations[ANIMATION_IDLE],
              time,
              1.0f,
              instance->skinned_mesh_instance);
            instance->base_animation_state = RenderableActorInstance::IDLE;
          }
        }
      }
    }
  }

  // Don't rotate if the actor is dead
  // Also, don't rotate if the actor is stopping: this prevents flipping direction on overshoot
  if (actor_is_alive &&
      (instance->movement_state_at_last_sync.speed == instance->last_sync.speed
       || last_sync_moving)) {

    // Find the shortest distance between the last sync's rendering angle
    // and the direction in which the actor is moving
    float angle = last_sync_moving ? current_angle : instance->last_sync.facing_angle;
    float rendering_angle = instance->movement_state_at_last_sync.rendering_angle;
    while (rendering_angle + D3DX_PI / 2 < angle) rendering_angle += 2 * D3DX_PI;
    while (rendering_angle - D3DX_PI / 2 > angle) rendering_angle -= 2 * D3DX_PI;

    bool do_smooth_rotation =
      (fabsf(angle - rendering_angle) < D3DX_PI*2.0f/3.0f) && last_sync_moving;
    //bool do_smooth_rotation = false;
    if (do_smooth_rotation) {
      // //double time_delta = 8.0f * (time - last_sync_.time_received);
      double time_delta = current_speed * 3.0f * (time - instance->last_sync.time_received);
      time_delta = min(1.0,time_delta);
      instance->current_movement_state.rendering_angle
        = rendering_angle * (1.0 - time_delta) +
                    angle * time_delta;
    } else {
      instance->current_movement_state.rendering_angle = angle;
    }
  }

  //// If the mesh is visible, update its animation state and the transforms
  //// for the attached scenery.  When invisible, none of this stuff should be
  //// drawn anyway.
  //if (!hidden) {

  // Always update the skinned mesh, even if it is hidden, so that bound effects
  // are updated correctly.
  // TODO: is this really necessary?
  {
    SkinnedMesh::SkinnedMeshInstance* skinned_mesh_instance
      = instance->skinned_mesh_instance;

    D3DXMATRIXA16 root_transform, temp;
    if (instance->last_sync.falling) {
      // fall down the pit.  this is (surprise, surprise!) a
      // special case where the actor needs to be in the same
      // position as the last sync in order to look right--
      // even though the actor isn't technically moving!
      instance->current_movement_state.x
        = (instance->last_sync.x - instance->current_movement_state.x)
            * 0.1f + instance->current_movement_state.x;
      instance->current_movement_state.y
        = (instance->last_sync.y - instance->current_movement_state.y)
            * 0.1f + instance->current_movement_state.y;
      instance->current_movement_state.vertical
        -= time_since_last_update;
    } else if (instance->last_sync.swimming) {
      static const float SWIMMING_HEIGHT = -0.25f;
      instance->current_movement_state.vertical
        = (SWIMMING_HEIGHT - instance->current_movement_state.vertical) * 0.1f
          + instance->current_movement_state.vertical;
    } else if (instance->last_sync.wading) {
      static const float WADING_HEIGHT = -1.0f;
      instance->current_movement_state.vertical
        = (WADING_HEIGHT - instance->current_movement_state.vertical) * 0.1f
          + instance->current_movement_state.vertical;
    }  else {
      // non-special-height state--blend to 0
      if (instance->current_movement_state.vertical < 0.0) {
        instance->current_movement_state.vertical = 0.0;
      } else {
        instance->current_movement_state.vertical
          = instance->current_movement_state.vertical * 0.9f;
      }
    }
    D3DXMatrixTranslation(&root_transform,
                          x,
                          instance->current_movement_state.vertical,
                          z);
    D3DXMatrixRotationY(&temp, -instance->current_movement_state.rendering_angle);
    D3DXMatrixMultiply(&root_transform, &temp, &root_transform);
    instance->binding->updateActorTransform(&root_transform);
    skinned_mesh_renderer_->updateAnimation(time,
                                            &root_transform,
                                            skinned_mesh_instance);

    // Update all attached items:  first the constant then the specialized
    int i;
    for (i = 0; i < NUMBER_OF_CONSTANT_ACTOR_ATTACHMENT_POINTS; ++i) {
      Scenery::SceneryInstance* scenery
        = instance->attached_items[i].item_scenery;
      if (!scenery) continue;
      const D3DXMATRIXA16* matrix
        = skinned_mesh_instance->attachment_point_matrices;
      matrix += profile->attachment_points[i];
      scenery->setTransform(matrix);
    }
    for (int j = 0;
             j < NUMBER_OF_SPECIALIZED_ACTOR_ATTACHMENT_POINTS;
           ++j, ++i) {
      assert(i < NUMBER_OF_TOTAL_ACTOR_ATTACHMENT_POINTS);
      Scenery::SceneryInstance* scenery
        = instance->attached_items[i].item_scenery;
      if (!scenery) continue;
      const D3DXMATRIXA16* matrix
        = skinned_mesh_instance->attachment_point_matrices;
      matrix += specialization->attachment_points[j];
      scenery->setTransform(matrix);
    }
  }
}









//----[  updateEquipment  ]----------------------------------------------------
void ActorRenderer::updateEquipment(
    RenderableActorInstance* instance,
    const Evidyon::Scenery::SceneryIndex* equipped_items,
    bool reequip_all_items) {
  assert(instance);

  // fill in all items
  for (int i = 0; i < NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
    Scenery::SceneryIndex scenery_index = instance->base.equipped_items[i];
    if (equipped_items) {
      if (!reequip_all_items && equipped_items[i] == scenery_index) {
        continue;
      } else {
        scenery_index = equipped_items[i];
        instance->base.equipped_items[i] = equipped_items[i];
      }
    }

    // Get the attachment location for this equipment slot
    RenderableActorInstance::AttachedItem* attached_item[2] = { NULL, NULL };
    switch (i) {
      case ACTOREQUIPMENTSLOT_PRIMARY:
        // specialized attachment point; listed after the constant ones
        attached_item[0]
          = &instance->attached_items
              [NUMBER_OF_CONSTANT_ACTOR_ATTACHMENT_POINTS
               + ACTORATTACHMENTPOINT_PRIMARY_WEAPON];
        break;
      case ACTOREQUIPMENTSLOT_ALTERNATE:
        // specialized attachment point; listed after the constant ones
        attached_item[0]
          = &instance->attached_items
              [NUMBER_OF_CONSTANT_ACTOR_ATTACHMENT_POINTS
               + ACTORATTACHMENTPOINT_SECONDARY_WEAPON_OR_SHIELD];
        break;
      case ACTOREQUIPMENTSLOT_HELMET:
        attached_item[0]
          = &instance->attached_items[ACTORATTACHMENTPOINT_HELMET];
        break;
        /*
        case ACTOREQUIPMENTSLOT_BOOTS:
        // there are two attachment points here!
        attached_item[0]
          = &instance->attached_items[ACTORATTACHMENTPOINT_LEFTBOOT];
        attached_item[1]
          = &instance->attached_items[ACTORATTACHMENTPOINT_RIGHTBOOT];
        break;
        */
    }
    assert(attached_item[0]);

    // release the old items
    if (attached_item[0]->item_scenery != NULL) {
      scenery_renderer_->release(attached_item[0]->item_scenery);
      attached_item[0]->item_scenery = NULL;
    }
    if (attached_item[1] != NULL &&
        attached_item[1]->item_scenery != NULL) {
      scenery_renderer_->release(attached_item[1]->item_scenery);
      attached_item[1]->item_scenery = NULL;
    }

    // acquire the new items
    if (scenery_index != Scenery::INVALID_SCENERY_INDEX) {
      attached_item[0]->item_scenery
        = scenery_renderer_->acquire(scenery_index);
      if (attached_item[1] != NULL) {
        attached_item[1]->item_scenery
          = scenery_renderer_->acquire(scenery_index);
      }
    }
  }
}





//----[  mutate  ]-------------------------------------------------------------
void ActorRenderer::mutate(RenderableActorInstance* instance,
                           ActorProfileIndex actor_profile,
                           SkinnedMesh::SkinnedMeshFormIndex form) {
  if (instance->base.actor_profile == actor_profile &&
      instance->base.skinned_mesh_form == form) return;

  const ActorProfile* profile = actorProfile(actor_profile);
  assert(profile);
  if (!profile) return;

  assert(profile == instance->profile);

  skinned_mesh_renderer_->mutate(instance->skinned_mesh_instance,
                                 profile->skinned_mesh,
                                 form);
  instance->base.skinned_mesh_form = form;
}





//----[  firstSync  ]----------------------------------------------------------
void ActorRenderer::firstSync(RenderableActorInstance* instance,
                              float x,
                              float y,
                              ActorSyncFlags flags) {
  assert(instance);

  // Although we can get the angle from the flags, we CANT get the
  // speed for this sync--setting the speed without setting the
  // last sync's receive time correctly would make the actor speed
  // off, since the delta between the current time and 0 
  // very high and this is multiplied by the speed in updateInstance.
  float angle = flags.unpackAngle();
  instance->last_sync.flags = flags;
  flags.unpackMovement(
    &instance->last_sync.alive,
    &instance->last_sync.moving,
    &instance->last_sync.teleport,
    &instance->last_sync.leaping,
    &instance->last_sync.wading,
    &instance->last_sync.combat,
    &instance->last_sync.swimming,
    &instance->last_sync.falling,
    &instance->last_sync.facing_angle);
  instance->base_animation_state
    = instance->last_sync.alive ? RenderableActorInstance::DEAD
                                : RenderableActorInstance::IDLE;
  instance->in_combat = instance->last_sync.combat;
  instance->last_sync.teleport = true;
  instance->last_sync.moving = false;

  instance->last_sync.time_received = 0.0f;
  instance->last_sync.time_since_previous_sync = 0.0f;
  instance->last_sync.x = x;
  instance->last_sync.y = y;
  instance->last_sync.direction = angle;
  instance->last_sync.facing_angle = angle;
  instance->last_sync.speed = 0.0f;
  instance->current_movement_state.x = x;
  instance->current_movement_state.y = y;
  instance->current_movement_state.vertical = 0.0f;
  instance->current_movement_state.angle = angle;
  instance->current_movement_state.rendering_angle = angle;
  instance->current_movement_state.speed = 0.0f;
  memcpy(&instance->movement_state_at_last_sync,
         &instance->current_movement_state,
         sizeof(RenderableActorInstance::MovementState));
}





//----[  sync  ]---------------------------------------------------------------
void ActorRenderer::sync(RenderableActorInstance* instance,
                         float time_since_previous_sync,
                         double time,
                         float x,
                         float y,
                         ActorSyncFlags flags) {
  assert(instance);

  bool swimming = instance->last_sync.swimming;
  bool last_moving = instance->last_sync.moving;
  flags.unpackMovement(
    &instance->last_sync.alive,
    &instance->last_sync.moving,
    &instance->last_sync.teleport,
    &instance->last_sync.leaping,
    &instance->last_sync.wading,
    &instance->last_sync.combat,
    &instance->last_sync.swimming,
    &instance->last_sync.falling,
    &instance->last_sync.facing_angle);

  bool actor_is_dead
    = instance->base_animation_state == RenderableActorInstance::DEAD;
  instance->last_sync.moving = instance->last_sync.moving && !actor_is_dead;

  // Invalidate the base state so that it is reset when the update() happens
  if (!actor_is_dead && swimming != instance->last_sync.swimming) {
    instance->base_animation_state
      = RenderableActorInstance::INVALID_BASE_ANIMATION_STATE;
  }

  if (flags.blood) {
    special_fx_manager_->generate(
      instance->profile->blood_special_fx,
      time,
      1.0f,
      x,
      y,
      instance->binding,
      x,
      y,
      NULL,
      0,
      0.5f,   // duration,
      NULL,   // we don't care about the bindings used
      false); // don't know if this is the client, but that's not important
  }

  float dx = x - instance->last_sync.x;
  float dy = y - instance->last_sync.y;
  instance->last_sync.direction = atan2f(dy, dx);
  instance->last_sync.speed
    = instance->last_sync.moving ? sqrtf(dx*dx+dy*dy)/time_since_previous_sync
                                 : 0.0f;
  static const float SPEED_CUTOFF = 0.25f;
  instance->last_sync.moving = instance->last_sync.speed > SPEED_CUTOFF;
  instance->last_sync.x = x;
  instance->last_sync.y = y;
  instance->last_sync.time_received = time;
  instance->last_sync.time_since_previous_sync = time_since_previous_sync;
 
  // Clear the in-combat flag when the player starts moving
  instance->in_combat
    = instance->last_sync.combat || instance->in_combat && !instance->last_sync.moving;

  if (instance->last_sync.teleport) {
    // jump instantly to the target location
    instance->current_movement_state.x = x;
    instance->current_movement_state.y = y;
    //instance->current_movement_state.vertical = 0.0f; // resets to ground
    instance->current_movement_state.speed = 0.0f;
    instance->last_sync.speed = 0.0f;
    instance->current_movement_state.angle = instance->last_sync.facing_angle;
    instance->current_movement_state.rendering_angle
      = instance->last_sync.facing_angle;
  }

  memcpy(&instance->movement_state_at_last_sync,
         &instance->current_movement_state,
         sizeof(RenderableActorInstance::MovementState)); 

  const ActorProfile* profile = instance->profile;
  const ActorProfileSpecialization* specialization
    = &profile->specializations[instance->profile_specialization];
  if (instance->last_sync.swimming) {
    // override the default specialization for swimming actors
    //specialization = &profile->specializations[Actor::SPECIALIZE_SWIMMING];
    return;
  }

  // This flag is set once the death animation has been initiated once.  It
  // prevents DIE from being played multiple times, since a dead actor simply
  // sends out ACTORACTION_DIE with every sync.
  bool current_action_is_invalid
    = instance->skinned_mesh_instance->action_animation.animation == NULL;
  if (actor_is_dead && instance->last_sync.alive) {
    actor_is_dead = false;
    instance->base_animation_state // set up a new base animation next update
      = RenderableActorInstance::INVALID_BASE_ANIMATION_STATE;
    instance->current_action = ACTORACTION_NONE;
    instance->lower_priority_action = ACTORACTION_NONE;
    current_action_is_invalid = true;
  } else if (!actor_is_dead && !instance->last_sync.alive) {
    skinned_mesh_renderer_->changeBaseRunOnceAndStop(
        profile->constant_animations[ANIMATION_DEATH_1+rand()%2],
        time,
        1.0f,
        instance->skinned_mesh_instance);
    instance->base_animation_state = RenderableActorInstance::DEAD;
    instance->last_sync.speed = 0.0f;
    instance->current_movement_state.speed = 0.0f;
    instance->movement_state_at_last_sync.speed = 0.0f;
    actor_is_dead = true;
  }
  if ((last_moving != instance->last_sync.moving) && instance->current_action != Actor::ACTORACTION_NONE) {
    //// make this animation end pronto
    //skinned_mesh_renderer_->changeActionAnimationSpeed(
    //  time,
    //  1000.0f,
    //  instance->skinned_mesh_instance);
  }
  if (actor_is_dead == false) {
    SkinnedMesh::SkinnedMeshAnimationIndex animation
      = SkinnedMesh::INVALID_SKINNED_MESH_ANIMATION_INDEX;
    const SkinnedMesh::SkinnedMeshAnimationIndex* specialized_animations
      = specialization->animations;

    Actor::ActorAction action = flags.unpackAction();
    if (action != Actor::ACTORACTION_NONE) {
      switch (action) {
      case ACTORACTION_SPIN_ATTACK:
        animation = specialized_animations[ANIMATION_SPIN_ATTACK];
        break;
      case ACTORACTION_CAST_AREA_SPELL:
        animation = profile->constant_animations[ANIMATION_CAST_AREA_SPELL];
        break;
      case ACTORACTION_CAST_TARGETED_SPELL:
        animation = profile->constant_animations[ANIMATION_CAST_TARGETED_SPELL];
        break;
      case ACTORACTION_ATTACK:
        animation = specialized_animations[ANIMATION_ATTACK_1 + rand()%2];
        instance->in_combat = true; // set the combat flag
        break;
      case ACTORACTION_GOT_HIT:
        animation = specialized_animations[ANIMATION_GET_HIT];
        instance->in_combat = true; // set the combat flag
        break;
      case ACTORACTION_LEAP:
        if (instance->current_action == ACTORACTION_LEAP) {
          // this is a second-stage leap, so do the extra special leap animation
          animation = profile->constant_animations[ANIMATION_LEAP_REPEATED];
        } else {
          animation = profile->constant_animations[ANIMATION_LEAP];
        }
        instance->in_combat = true; // set the combat flag
        break;
      }

      // assign this action
      if (animation != SkinnedMesh::INVALID_SKINNED_MESH_ANIMATION_INDEX) {
        skinned_mesh_renderer_->runActionAnimation(
          animation,
          time,
          flags.unpackSpeed(),
          instance->skinned_mesh_instance);
      }

      playActionSound(instance, profile, action);

      instance->current_action = action;
    } else {
      // reset the action if it is no longer playing
      if (instance->skinned_mesh_instance->action_animation.animation == NULL) {
        instance->current_action = Actor::ACTORACTION_NONE;
      }
    }
  }
}





//----[  zero  ]---------------------------------------------------------------
void ActorRenderer::zero() {
  assert(renderable_instances_.empty());
  skinned_mesh_renderer_ = NULL;
  scenery_renderer_ = NULL;
  sound_manager_ = NULL;
  special_fx_manager_ = NULL;
  actor_profiles_ = NULL;
  number_of_actor_profiles_ = 0;
}




//----[  deallocate  ]---------------------------------------------------------
void ActorRenderer::deallocate(RenderableActorInstance* instance) {
  instance->binding->releaseActor();
  instance->binding.reset();
  assert(scenery_renderer_);
  assert(skinned_mesh_renderer_);
  skinned_mesh_renderer_->release(instance->skinned_mesh_instance);
  for (int i = 0; i < NUMBER_OF_TOTAL_ACTOR_ATTACHMENT_POINTS; ++i) {
    Scenery::SceneryInstance* scenery
      = instance->attached_items[i].item_scenery;
    if (!scenery) continue;
    scenery_renderer_->release(scenery);
  }
}




//----[  playActionSound  ]----------------------------------------------------
void ActorRenderer::playActionSound(const RenderableActorInstance* instance,
                                    const ActorProfile* instance_profile,
                                    ActorAction action) {
  assert(sound_manager_);
  assert(instance);
  assert(instance->profile == instance_profile);
  ActorSound sound = INVALID_ACTOR_SOUND;
  switch (action) {
    case ACTORACTION_DIE:
      sound = ActorSound(ACTORSOUND_DIE_1 + (rand()%2));
      break;
    case ACTORACTION_SPIN_ATTACK:
    case ACTORACTION_ATTACK:
    case ACTORACTION_LEAP:
       sound = ActorSound(ACTORSOUND_ATTACK_1 + (rand()%3));
       break;
    case ACTORACTION_GOT_HIT:
       sound = ActorSound(ACTORSOUND_GET_HIT_1 + (rand()%2));
       break;
  }
  if (sound != INVALID_ACTOR_SOUND) {
    //if (instance == instance_for_clients_avatar_) {
    if (false) { // TODO: always play client's avatar's sound
      sound_manager_->playSound(
        Sound::SOUNDPRIORITY_SELF,
        instance_profile->sounds[sound]);
    } else {
      sound_manager_->playSound(
        Sound::SOUNDPRIORITY_NEARBY,
        instance->current_movement_state.x,
        instance->current_movement_state.y,
        instance_profile->sounds[sound]);
    }
  }
}



}
}

