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
#include "actor.h"
#include "actorgroup.h"
#include "../common/null.h"
#include "globalclientmanager.h"
#include "globalactormanager.h"
#include "worldregion.h"
#include "shared/actor/evidyon_actor.h"
#include "shared/evidyon_enchantments.h"
#include "encodepackets.h"
#include "globalworldmanager.h"
#include "client.h"
#include "shared/actor/actordescription.h"
#include "shared/evidyon_specialfx.h"
#include "shared/evidyon_network.h"
#include "shared/actor/actordescriptiontemplate.h"
#include "shared/packets/eventpackets.h"
#include "shared/client-server/packets.h"

#include <dc/debug>

#include <math.h>
#include <assert.h>

#include "common/fracture/todo.h"

namespace Evidyon {



  
//----[  acquire  ]------------------------------------------------------------
void ActorInstance::acquire(ActorController* controller,
                    int unique_id,
                    Map* map,
                    float x,
                    float y) {

  controller_ = controller;
  id_ = unique_id;
  faction_ = CHAOTIC;
  group_ = NULL;
  memset(&update_mask_, 0, sizeof(update_mask_));
  alive_ = true;
  setLivingFlag(true);
  x_ = x;
  y_ = y;
  level_ = 0;

  for (int i = 0; i < Evidyon::ClientConfig::SYNC_GROUPS+1; ++i) {
    sync_flags_info_[i].living = true;
    sync_flags_info_[i].teleport = true;
    sync_flags_info_[i].leaping = false;
    sync_flags_info_[i].flying_levitating = false;
    sync_flags_info_[i].falling = false;
    sync_flags_info_[i].moving = false;
    sync_flags_info_[i].swimming = false;
    sync_flags_info_[i].wading = false;
    sync_flags_info_[i].combat = false;
    sync_flags_info_[i].blood = false;
    sync_flags_info_[i].action = Actor::ACTORACTION_NONE;
    sync_flags_info_[i].action_duration = 0;
  }

  // should not be necessary, strictly speaking
  size_ = 0.5f;
  todo("karl","actor size");

  local_region_ = NULL;
  map_ = map;
}




//----[  release  ]------------------------------------------------------------
void ActorInstance::release() {
  setGroup(NULL); // make sure the actor isn't part of a group anymore
  if (local_region_ != NULL) {
    local_region_->leaveWorld(this);

    local_region_->removeReference();
    local_region_ = NULL;
  } else {
    DEBUG_INFO("ActorInstance @0x%X released without region", this);
  }

  { // cleanup code: not strictly necessary, but could help identify bugs
    map_ = NULL;
    controller_ = NULL;
  }
}




//----[  setGroup  ]-----------------------------------------------------------
bool ActorInstance::setGroup(ActorGroup* group) {
  if (group_ == group) return true;

  if (group_ != NULL) {
    group_->removeMember(this);
  }

  if (group != NULL) {
    group_ = group;
    if (group->addMember(this)) {
      group_ = group;
      return true;
    } else {
      group_ = NULL;
      return false;
    }
  } else {
    group_ = NULL;
    return true;
  }
}


//----[  getMapLocation  ]-----------------------------------------------------
void ActorInstance::getMapLocation(int* x, int* y) const {
  //*x = ((int)floor(x_)) >> Network::FLOAT_PACK_DECIMAL_BITS;
  //*y = ((int)floor(y_)) >> Network::FLOAT_PACK_DECIMAL_BITS;
  *x = ((int)floor(x_));
  *y = ((int)floor(y_));
}



//----[  getStrictRegion  ]----------------------------------------------------
void ActorInstance::getStrictRegion(int* map_x, int* map_y, WorldRegion** region) {
  int x, y;
  getMapLocation(&x, &y);
  if (local_region_->strictlyContainsPoint(x, y)) {
    *map_x = x;
    *map_y = y;
    *region = local_region_;
  } else {
    *map_x = x;
    *map_y = y;
    *region = map_->acquireRegionFromPoint(x, y);
    if (!(*region)) { *region = local_region_; }
  }
  assert(region);
}



//----[  faceLocation  ]-------------------------------------------------------
void ActorInstance::faceLocation(float x, float y) {
  float dx = x - x_;
  float dy = y - y_;
  float dist = sqrtf(dx*dx+dy*dy);
  static const float ATAN2_THREASHOLD = 0.01f;
  if (dist > ATAN2_THREASHOLD) { // make sure dist > 0
    direction_ = atan2f(dy, dx);
  }
}




//----[  faceLocation  ]-------------------------------------------------------
void ActorInstance::faceLocation(Number x, Number z) {
  direction_ = FixedPoint::atan2(z - Number(y_), x - Number(x_)).toFloat();
}



//----[  faceActor  ]----------------------------------------------------------
void ActorInstance::faceActor(ActorInstance* actor) {
  Number x, z;
  actor->getPosition(&x, &z);
  faceLocation(x, z);
}




//----[  angleTo  ]------------------------------------------------------------
float ActorInstance::angleTo(float x, float y) const {
  float dx = x - x_;
  float dy = y - y_;
  float dist = dx*dx+dy*dy;
  if (dist > 0.01f) { // make sure dist > 0
    return atan2f(dy, dx);
  } else {
    return 0.0f;
  }
}



//----[  angleTo  ]------------------------------------------------------------
float ActorInstance::angleTo(const ActorInstance* other) const {
  float dx = other->x_ - x_;
  float dy = other->y_ - y_;
  float dist = dx*dx+dy*dy;
  if (dist > 0.01f) { // make sure dist > 0
    return atan2f(dy, dx);
  } else {
    return 0.0f;
  }
}



//----[  isGlancingCollision  ]------------------------------------------------
bool ActorInstance::isGlancingCollision(
    const ActorInstance* bounding_overlap_collision,
    Number dx,
    Number dz) const {

  // get the distance to this actor
  float collider_x = bounding_overlap_collision->x_;
  float collider_y = bounding_overlap_collision->y_;
  float collider_dx = collider_x - x_,
        collider_dy = collider_y - y_;
  float collider_dd = sqrtf(collider_dx*collider_dx +
                            collider_dy*collider_dy);
  collider_dx /= collider_dd; // normalize
  collider_dy /= collider_dd;

  float dd = FixedPoint::sqrt(dx*dx+dz*dz).toFloat();
  float dot_x = dx.toFloat() / dd * collider_dx;
  float dot_y = dz.toFloat() / dd * collider_dy;

  // is the magnitude of the DP small? if so, ignore the collision
  static const float GLANCING_COLLISION_THREASHOLD = 0.4f;
  return dot_x < 0 || dot_y < 0 || (abs(dot_x + dot_y) < GLANCING_COLLISION_THREASHOLD);
  //float collision_dp_sq = dot_x * dot_x + dot_y * dot_y;
  //return collision_dp_sq < GLANCING_COLLISION_THREASHOLD;
}




//----[  update  ]-------------------------------------------------------------
bool ActorInstance::update(Number x,
                           Number y,
                           WorldRegion** left,
                           int* number_left,
                           WorldRegion** entered,
                           int* number_entered) {
  //////////////////////////////
  x = FixedPoint::maxfast(x, Number(0)); // prevent wrapping issues
  y = FixedPoint::maxfast(y, Number(0));
  //////////////////////////////

  if (update_mask_.position) {
    update_mask_.position = 0;
    x = x_;
    y = y_;
  }

  bool return_value = false;

  // Update the region that this actor is in
  WorldRegion* current_region = local_region_;
  Map* correct_map = map_;
  bool on_wrong_map = current_region == NULL ||
                      current_region->getMap() != correct_map;
  if (on_wrong_map) {
    if (current_region != NULL) {
      current_region->leaveWorld(this, left, number_left);
    } else {
      *number_left = 0;
    }

    WorldRegion* new_region = getController()->isClient() ? correct_map->acquireBorderedRegionFromPoint(x.toFloat(),y.toFloat()) : correct_map->acquireRegionFromPoint(x.toFloat(), y.toFloat());
    ASSERT(new_region) else return false;
    new_region->enterWorld(this, entered, number_entered);

    local_region_ = new_region;
    return_value = true;
    current_region = new_region;

  } else if (!current_region->containsPoint(x.toFloat(),y.toFloat())) {
    WorldRegion* new_region = getController()->isClient() ? correct_map->acquireBorderedRegionFromPoint(x.toFloat(),y.toFloat()) : correct_map->acquireRegionFromPoint(x.toFloat(), y.toFloat());
    current_region->moveToRegion(this, new_region, left, number_left, entered, number_entered);

    local_region_ = new_region;
    return_value = true;
    current_region = new_region;
  }

  x_ = x.toFloat();
  y_ = y.toFloat();

  return return_value;
}




//----[  isEnemyOf  ]----------------------------------------------------------
bool ActorInstance::isEnemyOf(ActorInstance* actor) const {
  static const unsigned int enemy_mask[] = {
    1<<EVIL | 1<<CHAOTIC | 1<<EVIL_NPC,// GOOD
    1<<CHAOTIC,// NEUTRAL
    1<<GOOD | 1<<CHAOTIC | 1<<GOOD_NPC,// EVIL
    1<<EVIL | 1<<EVIL_NPC, // GOOD_NPC
    1<<CHAOTIC,// NEUTRAL_NPC
    1<<GOOD | 1<<CHAOTIC | 1<<GOOD_NPC,// EVIL_NPC
    1<<CHAOTIC|1<<GOOD|1<<NEUTRAL|1<<EVIL|1<<GOOD_NPC|1<<NEUTRAL_NPC|1<<EVIL_NPC,// CHAOTIC
  };
  return (((enemy_mask[actor->faction_] >> faction_) & 1) == 0x1) &&
         !inSameGroupAs(actor);
}



//----[  canSee  ]-------------------------------------------------------------
bool ActorInstance::canSee(ActorInstance* actor) const {
  assert(local_region_);
  Map* map = local_region_ ? local_region_->getMap() : NULL;
  assert(map);
  return !actorIsOutOfRange(actor) &&
         (map && map->testLineOfSight(x_, y_, actor->x_, actor->y_));
}



//----[  findNearbyActorsInGroup  ]--------------------------------------------
int ActorInstance::findNearbyActorsInGroup(float radius_squared,
                   ActorInstance** actors_found, int buffer_size) {
  if (group_ == NULL) {
    actors_found[0] = this;
    return 1;
  } else {
    return group_->findMembersNear(this, radius_squared, actors_found, buffer_size);
  }
}




//----[  findNearbyEnemies  ]--------------------------------------------------
int ActorInstance::findNearbyEnemies(float radius_squared, ActorInstance** actors_found,
               int buffer_size) {
  return findEnemiesNear(x_, y_, radius_squared, actors_found, buffer_size);
}




//----[  findEnemiesNear  ]----------------------------------------------------
int ActorInstance::findEnemiesNear(float x, float y, float radius_squared,
               ActorInstance** actors_found, int buffer_size) {
  return group_ == NULL ?
    local_region_->findActorEnemiesNearby(this,
                                          x,
                                          y,
                                          radius_squared,
                                          actors_found,
                                          buffer_size) :
    group_->findEnemiesNear(this,
                            x,
                            y,
                            radius_squared,
                            actors_found,
                            buffer_size);
}





//----[  broadcastEvent_GenerateBasicProjectile  ]-----------------------------
void ActorInstance::broadcastEvent_GenerateBasicProjectile(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Number angle,
    Number x,
    Number z) {
  if (!local_region_) return;
  local_region_->broadcastEvent_GenerateBasicProjectile(
    event_id,
    event_index,
    angle,
    x,
    z);
}




//----[  broadcastEvent_GenerateTargetedSpecialFX  ]---------------------------
void ActorInstance::broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Actor::ActorID source_actor,
    Actor::ActorID target_actor) {
  if (!local_region_) return;
  local_region_->broadcastEvent_GenerateTargetedSpecialFX(
    event_id,
    event_index,
    source_actor,
    target_actor);
}




//----[  broadcastEvent_GenerateTargetedSpecialFX  ]---------------------------
void ActorInstance::broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Actor::ActorID source_actor,
    Number target_x,
    Number target_z,
    Actor::ActorID* target_actors,
    size_t number_of_targets) {
  if (!local_region_) return;
  local_region_->broadcastEvent_GenerateTargetedSpecialFX(
    event_id,
    event_index,
    source_actor,
    target_x,
    target_z,
    target_actors,
    number_of_targets);
}


//----[  broadcastEvent_GenerateTargetedSpecialFX  ]---------------------------
void ActorInstance::broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Number source_x,
    Number source_z,
    Number target_x,
    Number target_z,
    Actor::ActorID* target_actors,
    size_t number_of_targets) {
  if (!local_region_) return;
  local_region_->broadcastEvent_GenerateTargetedSpecialFX(
    event_id,
    event_index,
    source_x,
    source_z,
    target_x,
    target_z,
    target_actors,
    number_of_targets);
}


//----[  broadcastEvent_Terminate  ]-------------------------------------------
void ActorInstance::broadcastEvent_Terminate(Event::EventID event_id) {
  if (!local_region_) return;
  local_region_->broadcastEvent_Terminate(event_id);
}



//----[  findEnemiesNear  ]----------------------------------------------------
int ActorInstance::findEnemiesNear(ActorInstance* actor, float radius_squared,
               ActorInstance** actors_found, int buffer_size) {
  return findEnemiesNear(actor->x_, actor->y_, radius_squared, actors_found, buffer_size);
}



//----[  getDestroyActorPacket  ]----------------------------------------------
NetworkPacket* ActorInstance::getDestroyActorPacket() const {
  return ::Evidyon::Network::Packet::NetMsg_EraseActorFromWorld::writePacket(
    ENET_PACKET_FLAG_RELIABLE, id_);
}



//----[  broadcastDestroyActorPacket  ]----------------------------------------
void ActorInstance::broadcastDestroyActorPacket() {
  if (!local_region_) return;
  local_region_->broadcastOrphanedNetworkPacket(getDestroyActorPacket());
}


//----[  getDescriptionPacket  ]-----------------------------------------------
NetworkPacket* ActorInstance::getDescriptionPacket() {
  using namespace Evidyon::Network::Packet;
  Evidyon::Actor::ActorDescription description;
  fillDescription(&description);
  return NetMsg_TellActorDescription::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      &description);
}



//----[  fillDescription  ]----------------------------------------------------
void ActorInstance::fillDescription(
    Evidyon::Actor::ActorDescription* description) {
  sync_data_.packX(x_);
  sync_data_.packY(y_);
  memcpy(&description->sync,
         &sync_data_,
         sizeof(Evidyon::Actor::ActorSyncData));
  if (controller_) {
    description->id = id_;
    controller_->fillDescription(description);
  }
}



//----[  broadcastUpdateDescriptionPacket  ]-----------------------------------
void ActorInstance::broadcastUpdateDescriptionPacket() {
  if (!local_region_) return;
  local_region_->broadcastOrphanedNetworkPacket(getDescriptionPacket());
}



//----[  getMap  ]-------------------------------------------------------------
Map* ActorInstance::getMap() const {
  CONFIRM(map_) else {
    CONFIRM(local_region_) else {
      return NULL;
    }
    return local_region_->getMap();
  }
  return map_;
}



//----[  getMapIndex  ]--------------------------------------------------------
World::MapIndex ActorInstance::getMapIndex() const {
  Map* map = getMap();
  return map ? map->getIndex() : World::INVALID_MAP_INDEX;
}


//----[  getMapLocationInfo  ]-------------------------------------------------
const Server::MapLocationInfo* ActorInstance::getMapLocationInfo() const {
  Map* map = getMap();
  return map->getMapLocationInfo(
    (int)floor(x_),
    (int)floor(y_));
}


//----[  enterWorld  ]---------------------------------------------------------
// this method was extracted from the constructor on 3/18/2009 because it is
// necessary to delay an actor's entrance into the world when spawning
// swarm monsters until their spawning effect finishes.
void ActorInstance::enterWorld() {
  CONFIRM(map_) else return;
  CONFIRM(!local_region_) else return;
  WorldRegion* local_region
    = controller_->isClient() ? map_->acquireBorderedRegionFromPoint(x_, y_)
                              : map_->acquireRegionFromPoint(x_, y_);
  local_region_ = local_region;
  local_region->enterWorld(this);
}




//----[  say  ]----------------------------------------------------------------
void ActorInstance::say(const char* speech) const {
  WorldRegion* region = getRegion();
  if (region) {
    //using namespace Evidyon::Network::Packet::Server;
    //region->broadcastOrphanedNetworkPacket(
    //  Encode::actorSpeaks(getIDNumber(), NULL, speech));
    using namespace Evidyon::Network::Packet;
    size_t speech_length = strlen(speech);
    region->broadcastOrphanedNetworkPacket(
      NetMsg_Actor_Speaks::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        getIDNumber(),
        &CopiedArray<char>(speech, speech_length)));
  }
}




//----[  sayToActor  ]---------------------------------------------------------
void ActorInstance::sayToActor(Evidyon::Actor::ActorID target_actor, const char* speech) const {
  WorldRegion* region = getRegion();
  if (region) {
    using namespace Evidyon::Network::Packet;
    region->broadcastOrphanedNetworkPacket(
      NetMsg_Actor_Speaks_To_Actor::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        getIDNumber(),
        target_actor,
        &CopiedArray<char>(speech, strlen(speech))));
  }
}




//----[  initSyncFlags  ]------------------------------------------------------
void ActorInstance::initSyncFlags(int sync_group) {
  using namespace Evidyon::Actor;
  SyncFlagInfo* info = &sync_flags_info_[sync_group];
  ActorSyncFlags flags;
  flags.packAngle(direction_);
  flags.blood = sync_flags_info_[sync_group].blood;
  bool action_can_override = false;
  bool idle = false;
  if (info->living) {
    if (info->teleport) {
      flags.state = ActorSyncFlags::MOVEMENT_TELEPORT;
    } else if (info->leaping) {
      flags.state = ActorSyncFlags::MOVEMENT_LEAPING;
      // allow leap action through to specify duration; otherwise, block
      // all other actions.
      if (info->action == Actor::ACTORACTION_LEAP) {
        flags.packDuration(info->action_duration);
        flags.state = ActorSyncFlags::ACTION_LEAP;
      }
    } else if (info->flying_levitating) {
      flags.state =
        info->moving ? ActorSyncFlags::MOVEMENT_WALK
                     : ActorSyncFlags::MOVEMENT_IDLE;
      action_can_override = true;
    } else if (info->falling) {
      flags.state = ActorSyncFlags::MOVEMENT_FALLING;
    } else {
      if (info->moving) {
        if (info->swimming) {
          flags.state = ActorSyncFlags::MOVEMENT_SWIMMING;
        } else if (info->wading) {
          action_can_override = true;
          flags.state
            = info->combat ? ActorSyncFlags::MOVEMENT_WADING_ATTACK_RUN
                           : ActorSyncFlags::MOVEMENT_WADING;
        } else {
          action_can_override = true;
          flags.state
            = info->combat ? ActorSyncFlags::MOVEMENT_ATTACK_RUN
                           : ActorSyncFlags::MOVEMENT_WALK;
        }
      } else {
        idle = true;
        if (info->swimming) {
          action_can_override = true;
          flags.state = ActorSyncFlags::MOVEMENT_SWIMMING_IDLE;
        } else if (info->wading) {
          action_can_override = true;
          flags.state = ActorSyncFlags::MOVEMENT_WADING_IDLE;
        } else {
          action_can_override = true;
          flags.state = ActorSyncFlags::MOVEMENT_IDLE;
        }
      }
    }
  } else {
    flags.state = (info->wading ? ActorSyncFlags::MOVEMENT_DEAD_WADING
                                : ActorSyncFlags::MOVEMENT_DEAD);
    //if (info->teleport) {
    //  flags.state = ActorSyncFlags::MOVEMENT_TELEPORT;
    //} else {
    //  flags.state = ActorSyncFlags::MOVEMENT_IDLE;
    //}
  }

  if (action_can_override) {
    switch (info->action) {
    case ACTORACTION_SPIN_ATTACK:
        flags.packDuration(info->action_duration);
        flags.state = ActorSyncFlags::ACTION_SPIN_ATTACK;
      break;
    case ACTORACTION_CAST_AREA_SPELL:
        flags.packDuration(info->action_duration);
        flags.state = ActorSyncFlags::ACTION_CAST_AREA_SPELL;
      break;
    case ACTORACTION_CAST_TARGETED_SPELL:
        flags.packDuration(info->action_duration);
        flags.state = ActorSyncFlags::ACTION_CAST_TARGETED_SPELL;
      break;
    case ACTORACTION_ATTACK:
        flags.packDuration(info->action_duration);
        flags.state = ActorSyncFlags::ACTION_ATTACK;
      break;
    case Actor::ACTORACTION_GOT_HIT:
      if (idle) {
        flags.packDuration(info->action_duration);
        flags.state = ActorSyncFlags::ACTION_GOT_HIT;
      } break;
    }
  }

  sync_flags_[sync_group] = flags;
}



//----[  resetSyncFlags  ]-----------------------------------------------------
void ActorInstance::resetSyncFlags(int sync_group) {
  sync_flags_info_[sync_group].teleport = false;
  sync_flags_info_[sync_group].action = Evidyon::Actor::ACTORACTION_NONE;
  sync_flags_info_[sync_group].blood = false;
}



//----[  setSyncGroup  ]-------------------------------------------------------
void ActorInstance::setSyncGroup(int sync_group) {
  initSyncFlags(sync_group);
  sync_data_.packX(x_);
  sync_data_.packY(y_);
  sync_data_.flags = sync_flags_[sync_group];
  resetSyncFlags(sync_group);
}



//----[  getSlowSyncData  ]----------------------------------------------------
void ActorInstance::getSlowSyncData(Evidyon::Actor::ActorSlowSync* slow_sync) {
  slow_sync->id = id_;
  memcpy(&slow_sync->data, &sync_data_, sizeof(sync_data_));
}



//----[  getFastSyncData  ]----------------------------------------------------
void ActorInstance::getFastSyncData(Evidyon::Actor::ActorFastSync* fast_sync) {
  fast_sync->data.packX(x_);
  fast_sync->data.packY(y_);
  initSyncFlags(Evidyon::ClientConfig::SYNC_GROUPS);
  fast_sync->data.flags = sync_flags_[Evidyon::ClientConfig::SYNC_GROUPS];
  resetSyncFlags(Evidyon::ClientConfig::SYNC_GROUPS);
}




//----[  getSyncData  ]--------------------------------------------------------
const Actor::ActorSyncData* ActorInstance::getSyncData() {
  return &sync_data_;
}



}







