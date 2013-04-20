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
#include "server/player/player.h"
#include "server/client.h"
#include "server/clienttoclientitemtrade.h"
#include "server/clientitemsinterface.h"
#include "server/actorpointer.h"
#include "server/avatar/avatarinstance.h"
#include "server/avatar/avatarenchantments.h"
#include "server/avatar/avatarstats.h"
#include "server/player/playerrequest.h"

#include "server/actor.h" // for ActorInstance; TODO: rename actorinstance.h
#include "server/world/maplocationinfo.h"

#include "server/trigger/triggerinstance.h"
#include "shared/world/navigability.h"
#include "server/worldregion.h"
#include "server/map.h"
#include "server/item.h" // for ItemInstance; TODO: rename item.h->iteminstance.h
#include "shared/item/itemserverdescription.h"
#include "shared/design/design.h"
#include "shared/magic/magic.h"

#include "server/event/eventidgenerator.h"

#include "common/null.h"
#include <assert.h>

#include <dc/debug>

#include "server/globalgamefilemanager.h" // for Evidyon::SpellDescription
#include <math.h> // for cos/sin in updateRangedAttack--remove once I move to fixed point!
#include "common/randint32.h"
#include "common/randf.h"

// for getting the target map in OPEN_PORTAL_TO_DESTINATION
#include "server/globalworldmanager.h"

// for finding the guild's home point when using the return-to-spawn-point spell
#include "server/guild/guildinstance.h"


#include "common/fracture/todo.h"
#include "shared/design/newdesign.h"


namespace Evidyon {
namespace Server {


static const Time::Milliseconds FALLING_TIME_TO_DEATH_MS = 4500;
static const Time::Milliseconds DEAD_TIME_TO_RESPAWN_MS = 8500;
static const Time::Milliseconds LOGOFF_DELAY_MS = 3000;
static const Time::Milliseconds REGENERATION_PERIOD_MS = 2000;
static const float SWIMMING_SPEED_MULTIPLIER = 0.6;
static const float UNARMED_MELEE_ATTACK_RANGE = 0.75;
static const float MELEE_EFFECT_DELAY_PERCENT = 0.5;
static const Time::Milliseconds UNARMED_MELEE_REPEAT_DELAY = 1000;
static const Time::Milliseconds USE_ITEM_REPEAT_DELAY = 500;


//----[  acquire  ]------------------------------------------------------------
void Player::acquire(Client* client,
                     ActorPointer* actor,
                     PlayerRequest* request,
                     AvatarInstance* avatar,
                     AvatarEnchantments* enchantments,
                     AvatarStats* stats,
                     ClientItemsInterface* items,
                     ClientToClientItemTrade* trade) {
  todo("karl", "rename item.h -> iteminstance.h");
  todo("karl", "rename actor.h -> actorinstance.h");

  zero();
//  request->zero();

  client_ = client;
  actor_.copy(actor);
  request_ = request;
  avatar_ = avatar;
  enchantments_ = enchantments;
  stats_ = stats;
  items_ = items;
  trade_ = trade;

  request->reset(); // doubling this ensures it is cleared
  request->reset();

  ActorInstance* actor_instance = actor->dereference();
  assert(actor_instance);
  actor_instance->getPosition(&movement_.x, &movement_.z);

  { // recalculate avatar stats from scratch
    stats_->invalidateCalculatedMembers();
    enchantments_->zero();

    // this must be done first so that the previous "mutation" (nothing)
    // will be overwritten in the stats
    mutation_.mutated = true;
    mutation_.expiration_timer.trigger();

    calculateEquipment();
    calculateAvatarStats();

    // Do some spot-checks to make sure the calculations resulted in
    // usable data.
    assert(mutation_.mutated == false);
    assert(stats_->actor_profile != Actor::INVALID_ACTOR_PROFILE_INDEX);
    assert(stats_->specialization
            != Actor::INVALID_ACTOR_PROFILE_SPECIALIZATION);
    assert(stats_->skinned_mesh_form
            != SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX);
  }

  // If a player logs in with negative health, they are assumed to have logged
  // out while resurrected and are allowed to return to lying on the ground.
  if (stats_->hp.wholePart() <= 0) {
    stateChange(DEAD);
    dead_.resurrected = true;
    dead_.respawn_timer.reset();
  } else {
    stateChange(IDLE);
  }
}




//----[  release  ]------------------------------------------------------------
void Player::release() {
  if (state_ == DEAD) {
    // this tricky--the player is dead, but forcefully disconnected.  when
    // this happens, one of a few different situations have arisen:
    //  1- player d/c-ed intentionally while dead
    //  2- player d/c-ed by mistake while dead
    //  3- server was shutting down while player was dead
    // The first two situations are indistinguishable, so we have to assume
    // the player d/c-ed intentionally.  This could be used as a way to
    // prevent XP loss when dead if we don't assess the loss here.
    client_->respawn(true, true);
  }
  stateChange(INVALID);
  zero();
}





//----[  update  ]-------------------------------------------------------------
void Player::update(double time, double time_since_last_update) {
  assert(client_);
  assert(!actor_.invalid());

  if (stats_->hp.wholePart() <= 0) stateChange(DEAD);
  if (mutation_.expiration_timer.expired()) {
    calculateAvatarStats();
  }

  if (advance_portals_) {
    advance_portals_ = false;
    for (int i = 0; i < Magic::PORTALS_PER_PLAYER; ++i) {
      advancePortal(&portals_[i], time, time_since_last_update);
    }
  }
  if (advance_projectiles_) {
    advance_projectiles_ = false;
    for (int i = 0; i < Magic::PROJECTILES_PER_PLAYER; ++i) {
      advanceProjectile(&projectiles_[i], time, time_since_last_update);
    }
  }
  if (advance_traps_) {
    advance_traps_ = false;
    for (int i = 0; i < Magic::TRAPS_PER_PLAYER; ++i) {
      advanceTrap(&traps_[i], time, time_since_last_update);
    }
  }
  if (advance_dotaoebuff_) {
    advance_dotaoebuff_ = false;
    advanceDOTAOEBuff();
  }
  if (advance_dots_) {
    advance_dots_ = false;
    for (int i = 0; i < Magic::DOTS_PER_PLAYER; ++i) {
      advanceDOT(&dots_[i]);
    }
  }

  // Handle the current state
  switch (state_) {
  case IDLE:           updateIdle(time, time_since_last_update);         break;
  case FINDING_TRADE:  updateFindingTrade(time, time_since_last_update); break;
  case TRADING:        updateTrading(time, time_since_last_update);      break;
  case AT_STORAGE:     updateAtStorage(time, time_since_last_update);    break;
  case AT_BAZAAR:      updateAtBazaar(time, time_since_last_update);     break;
  //case AT_GEOSID:      updateAtGeosid(time, time_since_last_update);     break;
  case FALLING:        updateFalling(time, time_since_last_update);      break;
  case WALKING:        updateWalking(time, time_since_last_update);      break;
  case SWIMMING:       updateSwimming(time, time_since_last_update);     break;
  case FLYING:         updateFlying(time, time_since_last_update);       break;
  case ATTACK_RUN:     updateAttackRun(time, time_since_last_update);    break;
  case LEAP:           updateLeap(time, time_since_last_update);         break;
  case USING_MAGIC:    updateUsingMagic(time, time_since_last_update);   break;
  case MELEE_ATTACK:   updateMeleeAttack(time, time_since_last_update);  break;
  case RANGED_ATTACK:  updateRangedAttack(time, time_since_last_update); break;
  case DEAD:           updateDead(time, time_since_last_update);         break;
  case LEAVING_WORLD:  updateLeavingWorld(time, time_since_last_update); break;
  case GUILD_CREATE:   updateGuildCreate(time, time_since_last_update);  break;
  case GUILD_LEAD:     updateGuildLead(time, time_since_last_update);    break;
  }

  // Update equipped items and stats
  if (items_->hasEquipmentChanged()) calculateEquipment();
  if (stats_->recalculate_timer.poll()) calculateAvatarStats();

  // Regenerate *after* handing the current state, so that regeneration can't
  // prevent a player with < 0 HP from dying, but players can regenerate
  // while they are dead.
  if (regenerate_timer_.poll()) {
    regenerate_timer_.set(REGENERATION_PERIOD_MS);
    client_->changeHP(NULL, true, stats_->hp_regen);
    client_->changeMP(stats_->mp_regen);
  }

  // Update data based on the current location
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  const MapLocationInfo* location = my_actor->getMapLocationInfo();
  if (location && location != map_location_info_) {
    my_actor->setWadingFlag(location->navigability == World::NAVIGABILITY_WADE);
    movement_.swimming = location->navigability == World::NAVIGABILITY_SWIM;
    my_actor->setSwimmingFlag(movement_.swimming);
    falling_.falling = location->navigability == World::NAVIGABILITY_PIT;
    my_actor->setFallingFlag(falling_.falling);
    if (location->trigger) {
      location->trigger->onEnter(my_actor);
    }
    map_location_info_ = location;
  } else {
    //if (location->trigger) location->trigger->onUpdate();
  }


  // We should always update the actor because a trigger might go off that
  // causes us to change maps, another player could teleport us, an admin
  // could move us...there are all sorts of unknowns.
  WorldRegion* current_region = my_actor->getRegion();
  WorldRegion* regions_left[9];
  WorldRegion* regions_entered[9];
  int number_of_regions_left = 0, number_of_regions_entered = 0;
  if (my_actor->update(movement_.x,
                       movement_.z,
                       regions_left,
                       &number_of_regions_left,
                       regions_entered,
                       &number_of_regions_entered)) {
    WorldRegion* new_region = my_actor->getRegion();

    // Move the client to this new region
    current_region->changeRegionTo(client_,
                                   new_region,
                                   regions_left,
                                   number_of_regions_left,
                                   regions_entered,
                                   number_of_regions_entered);

    // Is this a new map?
    Map* new_map = new_region->getMap();
    if (current_region->getMap() != new_map) {
      // forge a stop action request so the player goes idle and blocks any
      // other actions
      request_->type = PlayerRequest::STOP_ACTION;
      client_->sendAsyncPacket_ChangeMap(new_map->getIndex());
    } else {
      // ...Nope, same ol' map, just a new region.  The client should still
      // know that we've changed regions though.
      client_->sendAsyncPacket_UpdateWorldRegion(new_region->getRegionID());
    }

    // When all regions have changed, the clients in the newly visible areas
    // are guaranteed not to know what this client's avatar looks like, so
    // they should get a copy of the description packet preemptively.
    // Similarly, this client won't know what others are in its own region
    // so we send all other avatars' descriptions to it.
    // This happens when doing a long teleport or changing maps.
    if (number_of_regions_left == 9) {
      assert(number_of_regions_entered == 9);
      my_actor->broadcastUpdateDescriptionPacket();
      new_region->tellAllNearbyClientFullDescriptions(client_);
    }

    for (int i = 0; i < number_of_regions_entered; ++i) {
      regions_entered[i]->sendLocalItemsOnGroundToClient(client_);
    }
  }

  // Retrieve the actor's new coordinates, since they could be different from
  // what we have stored internally if the actor was teleported.
  my_actor->getPosition(&movement_.x, &movement_.z);
}




//----[  isDead  ]-------------------------------------------------------------
bool Player::isDead() const {
  return state_ == DEAD;
}

//----[  isIdle  ]-------------------------------------------------------------
bool Player::isIdle() const {
  return state_ == IDLE;
}


//----[  isLookingForTrade  ]--------------------------------------------------
bool Player::isLookingForTrade() const {
  return state_ == FINDING_TRADE;
}



//----[  inTrade  ]------------------------------------------------------------
bool Player::inTrade() const {
  return state_ == TRADING;
}




//----[  atStorage  ]----------------------------------------------------------
bool Player::atStorage() const {
  return state_ == AT_STORAGE;
}




//----[  atBazaar  ]-----------------------------------------------------------
bool Player::atBazaar() const {
  return state_ == AT_BAZAAR;
}



//
////----[  atGeosid  ]-----------------------------------------------------------
//bool Player::atGeosid() const {
//  return state_ == AT_GEOSID;
//}




//----[  leaveWorld  ]---------------------------------------------------------
bool Player::leaveWorld() const {
  return state_ == LEAVING_WORLD &&
         leave_world_.logoff_timer.expired();
}



//----[  isInvitingGuildMembers  ]---------------------------------------------
bool Player::isInvitingGuildMembers() const {
  return state_ == GUILD_CREATE || state_ == GUILD_LEAD;
}


//----[  guildMemberJoined  ]--------------------------------------------------
bool Player::guildMemberJoined(char* create_guild_name, size_t length) {
  assert(isInvitingGuildMembers());
  bool create_guild = state_ == GUILD_CREATE;
  if (create_guild) {
    if (request_->type != PlayerRequest::CREATE_GUILD) {
      create_guild = false;
      assert(false && "guild_create request overwritten");
    } else {
      strncpy_s(create_guild_name,
                length,
                request_->create_guild.guild_name,
                length < Guild::MAX_NAME_LENGTH ? length : Guild::MAX_NAME_LENGTH);
    }
  }
  stateChange(IDLE);
  return create_guild;
}




//----[  resurrect  ]----------------------------------------------------------
bool Player::resurrect() {
  if (state_ != DEAD || dead_.resurrected) return false;
  dead_.resurrected = true;
  dead_.respawn_timer.reset();
  return true;
}



//----[  setAttackPlayers  ]---------------------------------------------------
bool Player::setAttackPlayers(bool attack_players) {
  bool return_value = attacking_players_ != attack_players;
  attacking_players_ = attack_players;
  return return_value;
}


//----[  zero  ]---------------------------------------------------------------
void Player::zero() {
  client_ = NULL;
  actor_.reset();
  request_ = NULL;
  avatar_ = NULL;
  enchantments_ = NULL;
  stats_ = NULL;
  items_ = NULL;
  trade_ = NULL;
  state_ = INVALID;
  map_location_info_ = NULL;
  attacking_players_ = false;
  movement_.dx = 0.0;
  movement_.dz = 0.0;
  movement_.stop_timer.reset();
  movement_.swimming = false;
  magic_.magic = NULL;
  magic_.parameter = 0;
  magic_.stage = 0;
  magic_.target_x = 0;
  magic_.target_z = 0;
  magic_.increment_stage_timer.reset();
  magic_.projectile_delivery.zero();
  for (int i = 0; i < Magic::MAX_TARGETS; ++i) {
    magic_.targeted_actors[i].reset();
  }
  magic_.next_targeted_actor = 0;
  attack_run_.speed = 0.0;
  attack_run_.first_hit_damage_bonus = 0.0;
  leap_.speed = 0.0;
  leap_.first_hit_damage_bonus = 0.0;
  attack_.target.reset();
  attack_.follow_range = 0.0;
  attack_.attack_range = 0.0;
  attack_.begin_attack_timer.reset();
  attack_.end_attack_timer.reset();
  attack_.effect.zero();
  falling_.falling = false;
  falling_.respawn_timer.reset();
  dead_.resurrected = false;
  dead_.respawn_timer.reset();
  mutation_.mutated = false;
  mutation_.can_speak = true;
  mutation_.can_cast_spells = true;
  mutation_.can_melee_attack = true;
  mutation_.hidden_from_monsters = false;
  mutation_.flying = false;
  mutation_.levitating = false;
  for (int i = 0; i <Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
    mutation_.show_equipped[i] = true;
  }
  mutation_.actor_profile = Actor::INVALID_ACTOR_PROFILE_INDEX;
  mutation_.skinned_mesh_form = SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX;
  mutation_.movement_speed = 0.0;
  mutation_.melee_effect_speed = 0.0;
  mutation_.armor = 0;
  mutation_.expiration_timer.reset();
  next_action_timer_.reset();
  regenerate_timer_.reset();
  next_triggered_event_timer_.reset();
  advance_portals_ = false;
  for (int i = 0; i < Magic::PORTALS_PER_PLAYER; ++i) {
    portals_[i].zero();
  }
  advance_projectiles_ = false;
  for (int i = 0; i < Magic::PROJECTILES_PER_PLAYER; ++i) {
    projectiles_[i].zero();
  }
  advance_traps_ = false;
  for (int i = 0; i < Magic::TRAPS_PER_PLAYER; ++i) {
    traps_[i].zero();
  }
  advance_dotaoebuff_ = false;
  dotaoebuff_.zero();
  advance_dots_ = false;
  for (int i = 0; i < Magic::DOTS_PER_PLAYER; ++i) {
    dots_[i].zero();
  }
}









//----[  stateChange  ]--------------------------------------------------------
bool Player::stateChange(State state, bool entering) {
  if (entering) {
    if (state == state_) return false; // prevent re-entrance
    stateChange(state_, false);
  }

  switch (state) {
  case IDLE:
    if (entering) {
      request_->reset(PlayerRequest::STOP_ACTION);
      movement_.dx = 0;
      movement_.dz = 0;
      movement_.stop_timer.trigger();
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setMovingFlag(false);
      my_actor->setAction(Actor::ACTORACTION_NONE, 0);
    } else {
    } break;
  case FINDING_TRADE:
    if (entering) {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      WorldRegion* region = my_actor->getRegion();

      // Find nearby clients that are also looking for a trade.  If this
      // trade initiation succeeds, once we enter the FINDING_TRADE
      // state, it will automatically switch us to the TRADING state.
      Client* other_client
        = region->findNearbyClientFindingTrade(client_, my_actor);
      if (other_client) client_->beginTrade(other_client);

    } else {
    } break;
  case TRADING:
    if (entering) {
    } else {
      client_->cancelTrade();
    } break;
  case AT_STORAGE:
    if (entering) {
      request_->reset(PlayerRequest::STORAGE_OPEN);
    } else {
    } break;
  case AT_BAZAAR:
    if (entering) {
      request_->reset(PlayerRequest::BAZAAR_OPEN);
    } else {
    } break;
  //case AT_GEOSID:
  //  if (entering) {
  //    request_->reset(PlayerRequest::GEOSID_OPEN);
  //  } else {
  //  } break;
  case FALLING:
    if (entering) {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setFallingFlag(true);
      movement_.x.int_value &= 0xFFFF0000;  // move to center of tile
      movement_.z.int_value &= 0xFFFF0000;
      movement_.x += 0.5;
      movement_.z += 0.5;
      my_actor->setMovingFlag(false);
      assert(falling_.falling);
      falling_.respawn_timer.enable();
      falling_.respawn_timer.set(FALLING_TIME_TO_DEATH_MS);
    } else {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setFallingFlag(false);
      falling_.respawn_timer.reset();
      falling_.respawn_timer.disable();
    } break;
  case WALKING:
    if (entering) {
    } else {
    } break;
  case SWIMMING:
    if (entering) {
    } else {
    } break;
  case FLYING:
    if (entering) {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setFlyingLevitatingFlag(true);
      actor_.dereferenceAssumingValid()->setAction(Actor::ACTORACTION_NONE, 0);
    } else {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setFlyingLevitatingFlag(false);
    } break;
  case ATTACK_RUN:
    if (entering) {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setCombatFlag(true);
      attack_.follow_range = avatar_->equipment.melee_follow_distance;
      attack_.attack_range = avatar_->equipment.melee_attack_distance;
      attack_.begin_attack_timer.set(0);
      attack_.end_attack_timer.reset();
    } else {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setCombatFlag(false);

      // Ne sure that the movement speeds are correct for the new
      // state.  This prevents attack-run into water from making the
      // player swim at attack-run speed...unless that's desireable?
    } break;
  case LEAP:
    if (entering) {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      //leap_.reset_leap_flag_timer.enable();
      my_actor->setLeapingFlag(true);
      my_actor->setCombatFlag(true);
      // prevents swimming if the end of the leap is over water
      my_actor->setFlyingLevitatingFlag(true);
    } else {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      //leap_.reset_leap_flag_timer.reset();
      //leap_.reset_leap_flag_timer.disable();
      my_actor->setLeapingFlag(false);
      my_actor->setCombatFlag(false);
      my_actor->setFlyingLevitatingFlag(false);
    } break;
  case USING_MAGIC:
    if (entering) {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      //my_actor->setMovingFlag(false);
      magic_.increment_stage_timer.enable();
    } else {
      magic_.increment_stage_timer.disable();
    } break;
  case MELEE_ATTACK:
    if (entering) {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setCombatFlag(true);
      attack_.begin_attack_timer.enable();
      attack_.end_attack_timer.enable();

      // make sure that a move request doesn't override the attack
      request_->reset(Server::PlayerRequest::MOVE);
    } else {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setCombatFlag(false);
      attack_.begin_attack_timer.disable();
      attack_.end_attack_timer.disable();
    } break;
  case RANGED_ATTACK:
    if (entering) {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setCombatFlag(true);
      attack_.end_attack_timer.enable();
    } else {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setCombatFlag(false);
      attack_.end_attack_timer.disable();
    } break;
  case DEAD:
    if (entering) {
      // this is a special case; it must be done early so that a resurrection
      // spell that is cast automatically on death can take effect
      state_ = DEAD;

      // get rid of the AOE buff if it is something like a fire cloud around
      // the player.
      if (dotaoebuff_.data &&
          dotaoebuff_.data->terminate_on_death) {
        releaseDOTAOEBuff(); // reset the AOE around the player
      }

      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setLivingFlag(false);
      my_actor->setMovingFlag(false);
      my_actor->setDead();
      dead_.resurrected = false;
      dead_.respawn_timer.enable();
      dead_.respawn_timer.set(DEAD_TIME_TO_RESPAWN_MS);

      Magic::MagicIndex magic_index;
      int wishing_percent = 0;
      if (items_->onDeath(&magic_index, &wishing_percent)) {
        Number x, z;
        my_actor->getPosition(&x, &z);
        const Evidyon::Magic::Magic* magic
          = GlobalGamefileManager::singleton()->getMagic(magic_index);
        if (magic) {
          initiateUseMagic(
            magic, avatar_->level, Actor::ACTORACTION_NONE, 0, 0, x, z, 0, my_actor, actor_, false);
          assert(state_ == DEAD);
        }
      }
      //if (enchantments_.onDeath(&magic_index)) {
      //  initiateUseMagic(magic_index);
      //}
      if (client_->getLocalZoneRules() == Zone::ZONERULES_PVP_ARENA) {
        // auto-resurrect in this area
        client_->resurrect(); // don't just set the flag--this sends out a packet
        assert(dead_.resurrected);
        enchantments_->clear();
        stats_->recalculate_timer.trigger();
      } else {
        if (falling_.falling) {
          client_->onAvatarDeath(actor_.dereferenceAssumingValid(), false, false);
        } else {
          // we're not over a pit, so drop items if the
          // player is not swimming either and the wishing
          // probability fails
          bool dont_drop_equipment = (wishing_percent > (rand_uint32() % 100));
          client_->onAvatarDeath(actor_.dereferenceAssumingValid(),
                                 !movement_.swimming,
                                 dont_drop_equipment);
        }
      }

      if (falling_.falling) {
        my_actor->setFallingFlag(true);
        // ouch, bad luck; dead while falling.  give them a bit more
        // time to think about what to do (allows resurrection)
        falling_.respawn_timer.enable();
        falling_.respawn_timer.set(FALLING_TIME_TO_DEATH_MS);
      }

      // Get rid of all portals directly targeting this player so that the
      // player isn't teleported while dead.  This also prevents the situation 
      // where a player portals, dies, respawns and then the portal finally
      // kicks in and portals them away from the respawn point.
      releaseSelfOnlyPortals();

    } else {
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setAlive();
      my_actor->setLivingFlag(true);
      falling_.respawn_timer.disable();
      dead_.respawn_timer.disable();
    } break;
  case LEAVING_WORLD:
    if (entering) {
      request_->reset(PlayerRequest::LEAVE_WORLD);
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->setAction(Actor::ACTORACTION_NONE, 0);
      my_actor->setMovingFlag(false);
      leave_world_.logoff_timer.enable();
      leave_world_.logoff_timer.set(LOGOFF_DELAY_MS);
    } else {
      if (leave_world_.logoff_timer.expired() == false) {
        ActorInstance* my_actor = actor_.dereferenceAssumingValid();
        my_actor->say("<Canceled Leaving World>");
      }
      leave_world_.logoff_timer.disable();
    } break;
  case GUILD_CREATE:
    if (entering) {
      // do not reset this request--it stores the name of the guild!
    } else {
      request_->reset(PlayerRequest::CREATE_GUILD);
    }
  case GUILD_LEAD:
    if (entering) {
      request_->reset(PlayerRequest::LEAD_GUILD);
    } else {
    }
  case INVALID:
    if (entering) {
      movement_.stop_timer.disable();
      next_action_timer_.disable();
      regenerate_timer_.disable();
      next_triggered_event_timer_.disable();
      stats_->recalculate_timer.disable();

      // ensure all other triggers are disabled
      magic_.increment_stage_timer.disable();
      dead_.respawn_timer.disable();
      falling_.respawn_timer.disable();
      leave_world_.logoff_timer.disable();
      for (int i = 0; i < Magic::PROJECTILES_PER_PLAYER; ++i) {
        releaseProjectileInstance(&projectiles_[i]);
      }
      releaseProjectileInstance(&magic_.projectile_delivery);
      for (int i = 0; i < Magic::PORTALS_PER_PLAYER; ++i) {
        releasePortalInstance(&portals_[i]);
      }
      for (int i = 0; i < Magic::DOTS_PER_PLAYER; ++i) {
        releaseDOTInstance(&dots_[i]);
      }
      releaseDOTAOEBuff();
      enchantments_->clear();
    } else {
      movement_.stop_timer.enable();
      next_action_timer_.enable();
      next_action_timer_.trigger(); // must be enabled to act
      regenerate_timer_.enable();
      regenerate_timer_.trigger(); // must be forced the first time
      next_triggered_event_timer_.enable();
      next_triggered_event_timer_.trigger(); // must be enabled
      stats_->recalculate_timer.enable();
      stats_->recalculate_timer.trigger();
    } break;
    break;
  default:
    assert(false && "Player encountered an unknown state");
  }

  if (entering) state_ = state;

  return true;
}







//----[  updateIdle  ]---------------------------------------------------------
void Player::updateIdle(double time, double time_since_last_update) {
  if (falling_.falling &&
      !(mutation_.flying || mutation_.levitating)) {
    stateChange(FALLING);
    return;
  }

  switch (request_->type) {
  case PlayerRequest::LEAVE_WORLD: stateChange(LEAVING_WORLD); break;
  case PlayerRequest::STOP_ACTION: {
    // cancel form enchantment
    // cancel all projectiles and portals
    for (int i = 0; i < Magic::PROJECTILES_PER_PLAYER; ++i) {
      releaseProjectileInstance(&projectiles_[i]);
    }
    releaseProjectileInstance(&magic_.projectile_delivery);
    for (int i = 0; i < Magic::PORTALS_PER_PLAYER; ++i) {
      releasePortalInstance(&portals_[i]);
    }
    releaseDOTAOEBuff();
    // don't reset DOTs--this might save the player =)
    request_->reset();
  } break;
  case PlayerRequest::STORAGE_OPEN: stateChange(AT_STORAGE); break;
  case PlayerRequest::BAZAAR_OPEN:  stateChange(AT_BAZAAR);  break;
  case PlayerRequest::USE_ITEM: processUseItemRequest(); break;
  case PlayerRequest::CAST:     processCastRequest(); break;
  case PlayerRequest::ATTACK:   processAttackRequest(); break;
  case PlayerRequest::MOVE:     processMoveRequest(); break;
  case PlayerRequest::SPEAK:    processSpeakRequest(true); break;
  case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE); break;
  case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD); break;
  default:
    // ignore this request
    request_->reset();
    break;
  }
}






//----[  updateFindingTrade  ]-------------------------------------------------
void Player::updateFindingTrade(double time, double time_since_last_update) {
  if (falling_.falling ||
      request_->type != PlayerRequest::NONE) {
    stateChange(IDLE);
    updateIdle(time, time_since_last_update);
  }

  if (trade_->inProgress()) stateChange(TRADING);
}







//----[  updateTrading  ]------------------------------------------------------
void Player::updateTrading(double time, double time_since_last_update) {
  if (falling_.falling ||
      request_->type != PlayerRequest::NONE) {
    stateChange(IDLE);
    updateIdle(time, time_since_last_update);
  }

  // If the trade is no longer taking place, exit this state
  if (!trade_->inProgress()) stateChange(IDLE);
}









//----[  updateAtBazaar  ]-----------------------------------------------------
void Player::updateAtBazaar(double time, double time_since_last_update) {
  if (falling_.falling ||
      request_->type != PlayerRequest::NONE) {
    stateChange(IDLE);
    updateIdle(time, time_since_last_update);
  }
}







//
////----[  updateAtGeosid  ]-----------------------------------------------------
//void Player::updateAtGeosid(double time, double time_since_last_update) {
//  if (falling_.falling ||
//      request_->type != PlayerRequest::NONE) {
//    stateChange(IDLE);
//    updateIdle(time, time_since_last_update);
//  }
//}








//----[  updateAtStorage  ]----------------------------------------------------
void Player::updateAtStorage(double time, double time_since_last_update) {
  if (falling_.falling ||
      request_->type != PlayerRequest::NONE) {
    stateChange(IDLE);
    updateIdle(time, time_since_last_update);
  }
}









//----[  updateFalling  ]------------------------------------------------------
void Player::updateFalling(double time, double time_since_last_update) {
  if (mutation_.flying ||
      mutation_.levitating ||
      !falling_.falling) {
    assert(!movement_.swimming || !falling_.falling); // should never be both
    // falling is a non-movement state, so leaving it defaults to idle
    stateChange(IDLE);
    updateIdle(time, time_since_last_update);
    return;
  }

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();

  // respawn if we hit the bottom
  if (falling_.respawn_timer.poll()) {
    client_->onAvatarDeath(my_actor,
                           false, true); // don't drop items
    client_->respawn(true,  // restore hp/mp
                     true); // penalize experience
    stateChange(IDLE);
  }


  switch (request_->type) {
  case PlayerRequest::LEAVE_WORLD: stateChange(LEAVING_WORLD); break;
  // disabled: stop action, geosid, storage, bazaar
  case PlayerRequest::USE_ITEM: processUseItemRequest(); break;
  case PlayerRequest::CAST:     processCastRequest(); break;
  // disabled: move and attack
  case PlayerRequest::SPEAK:
    processSpeakRequest(false); // allow speech, but no trade dialog
    break;
  // disabled: create/lead guild
  default:
    // ignore this request
    request_->reset();
    break;
  }
}








//----[  updateWalking  ]------------------------------------------------------
void Player::updateWalking(double time, double time_since_last_update) {
  if (mutation_.flying) {
    stateChange(FLYING);
    updateFlying(time, time_since_last_update);
    return;
  }
  if (movement_.swimming) {
    stateChange(SWIMMING);
    updateSwimming(time, time_since_last_update);
    return;
  }
  if (falling_.falling &&
      !mutation_.levitating) {
    stateChange(FALLING);
    return;
  }

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();


  if (!movement_.stop_timer.expired()) {
    WorldRegion* current_region = my_actor->getRegion();
    Number x = movement_.x;
    Number z = movement_.z;
    Number dx = movement_.dx.toFloat() * time_since_last_update;
    Number dz = movement_.dz.toFloat() * time_since_last_update;

    // Get the next location's info
    const MapLocationInfo* next_map_location_info
      = current_region->getNextActorMapLocationInfo(x,
                                                    z,
                                                    dx,
                                                    dz);
    if (!next_map_location_info || // HACK!
        !next_map_location_info->can_move_over) {
      stateChange(IDLE);
    } else {

      // Get the actor's next location
      //Number dx = movement_.dx.toFloat() * time_since_last_update;
      //Number dz = movement_.dz.toFloat() * time_since_last_update;
      Number next_x = x + dx;
      Number next_z = z + dz;

      // Collide with actors
      ActorInstance* collision
        = current_region->collideActorWithActor(my_actor,
                                                x.toFloat(),
                                                z.toFloat(),
                                                next_x.toFloat(),
                                                next_z.toFloat());
      if (collision != NULL && !my_actor->isGlancingCollision(collision, dx, dz)) {
        stateChange(IDLE);
      } else {
        movement_.x = next_x; // set the actor's next coordinates so it keeps moving
        movement_.z = next_z;
      }
    }
  } else {
    stateChange(IDLE);

    // The client just stopped moving, so pick up items here
    int map_x, map_z;
    WorldRegion* strict_region = NULL;
    my_actor->getStrictRegion(&map_x, &map_z, &strict_region);

    // Get items
    CurrencyType currency = 0;
    if (!strict_region->pickUpItems(map_x, map_z, items_, &currency)) {
      my_actor->say("Inventory Full");
    }
    if (currency > 0) {
      client_->giveCurrency(currency);
      client_->syncCurrency();
    }
  }


  switch (request_->type) {
  case PlayerRequest::LEAVE_WORLD:  stateChange(LEAVING_WORLD); break;
  case PlayerRequest::STOP_ACTION:  request_->reset();
  case PlayerRequest::STORAGE_OPEN:
  case PlayerRequest::BAZAAR_OPEN:  stateChange(IDLE); break;
  case PlayerRequest::USE_ITEM:    processUseItemRequest(); break;
  case PlayerRequest::CAST:        processCastRequest(); break;
  case PlayerRequest::ATTACK:      processAttackRequest(); break;
  case PlayerRequest::MOVE:        processMoveRequest(); break;
  case PlayerRequest::SPEAK:       processSpeakRequest(true); break;
  case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE); break;
  case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD); break;
  default:
    // ignore this request
    request_->reset();
    break;
  }
}









//----[  updateSwimming  ]-----------------------------------------------------
void Player::updateSwimming(double time, double time_since_last_update) {
  if (mutation_.flying) {
    stateChange(FLYING);
    updateFlying(time, time_since_last_update);
    return;
  }
  if (mutation_.levitating) {
    // levitation makes us walk over water
    stateChange(WALKING);
    updateWalking(time, time_since_last_update);
    return;
  } else {
    // if we're not levitating or flying, there's a chance we could fall
    // off of a cliff!
    if (falling_.falling) {
      assert(!movement_.swimming); // should never be both
      stateChange(FALLING);
      return;
    }
  }
  if (!movement_.swimming) {
    // new state is now regular movement (default)
    stateChange(WALKING);
    updateIdle(time, time_since_last_update);
    return;
  }

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  if (!movement_.stop_timer.expired()) {
    WorldRegion* current_region = my_actor->getRegion();
    Number x = movement_.x;
    Number z = movement_.z;
    Number dx = movement_.dx.toFloat() * SWIMMING_SPEED_MULTIPLIER * time_since_last_update;
    Number dz = movement_.dz.toFloat() * SWIMMING_SPEED_MULTIPLIER * time_since_last_update;

    // Get the next location's info
    const MapLocationInfo* next_map_location_info
      = current_region->getNextActorMapLocationInfo(x,
                                                    z,
                                                    dx,
                                                    dz);
    bool continue_moving = next_map_location_info->can_move_over;
    if (continue_moving) {
      // Get the actor's next location
      Number next_x = x + dx;
      Number next_z = z + dz;

      // Collide with actors
      ActorInstance* collision
        = current_region->collideActorWithActor(my_actor,
                                                x.toFloat(),
                                                z.toFloat(),
                                                next_x.toFloat(),
                                                next_z.toFloat());
      continue_moving =
        collision == NULL || my_actor->isGlancingCollision(collision, dx, dz);
      if (continue_moving) {
        // set the actor's next coordinates so it keeps moving
        movement_.x = next_x;
        movement_.z = next_z;
      }
    }
    if (!continue_moving) {
      // the actor ran into something, so stop moving
      movement_.stop_timer.trigger();
      my_actor->setMovingFlag(false);
      stateChange(IDLE);
    }
  } else {
    // movement timer expired because we reached our destination
    movement_.stop_timer.trigger();
    my_actor->setMovingFlag(false);
    stateChange(IDLE);
  }


  switch (request_->type) {
  case PlayerRequest::LEAVE_WORLD:  stateChange(LEAVING_WORLD); break;
  case PlayerRequest::STOP_ACTION:  request_->reset();
  //case PlayerRequest::GEOSID_OPEN:
  case PlayerRequest::STORAGE_OPEN:
  case PlayerRequest::BAZAAR_OPEN:  stateChange(IDLE); break;
  case PlayerRequest::USE_ITEM:    processUseItemRequest(); break;
  // can't attack or cast while swimming
  case PlayerRequest::MOVE:        processMoveRequest(); break;
  case PlayerRequest::SPEAK:       processSpeakRequest(true); break;
  case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE); break;
  case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD); break;
  default:
    // ignore this request
    request_->reset();
    break;
  }

}









//----[  updateFlying  ]-------------------------------------------------------
void Player::updateFlying(double time, double time_since_last_update) {
  assert(false && "updateFlying incomplete");
}









//----[  updateAttackRun  ]----------------------------------------------------
void Player::updateAttackRun(double time, double time_since_last_update) {
  if (mutation_.flying) {
    stateChange(FLYING);
    updateFlying(time, time_since_last_update);
    return;
  }
  if (movement_.swimming) {
    stateChange(SWIMMING);
    updateSwimming(time, time_since_last_update);
    return;
  }
  if (falling_.falling && !mutation_.levitating) {
    stateChange(FALLING);
    return;
  }

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  ActorInstance* target = attack_.target.dereference();
  if (target) {
    if (target->isDead() || my_actor->actorIsOutOfRange(target)) {
      stateChange(IDLE);
      return;
    }

    Number separation_distance = my_actor->separationDistanceTo(target);
    if (attack_.follow_range < separation_distance) {
      // We are too far away to attack.  Move to get closer.
      my_actor->getPosition(&movement_.x, &movement_.z);
      Number offset_x, offset_z;
      target->getPosition(&offset_x, &offset_z);
      offset_x -= movement_.x;
      offset_z -= movement_.z;

      // Calculate distance from us -> target actor
      Number distance;
      distance.int_value
        = FixedPoint::fixmul<16>(offset_x.int_value, offset_x.int_value)
        + FixedPoint::fixmul<16>(offset_z.int_value, offset_z.int_value);
      distance = FixedPoint::sqrt(distance);

      // Make the actor face this location
      my_actor->setDirection(FixedPoint::atan2(offset_z, offset_x).toFloat());
      my_actor->setMovingFlag(true);

      // Calculate the rate at which to move on each axis.  We can ignore
      // the stop moving timer.
      movement_.dx.int_value
        = FixedPoint::fixmul<16>(attack_run_.speed.int_value,
                                  (offset_x / distance).int_value);
      movement_.dz.int_value
        = FixedPoint::fixmul<16>(attack_run_.speed.int_value,
                                 (offset_z / distance).int_value);
      movement_.stop_timer.trigger();

      // Get the next location's info
      Number dx = movement_.dx.toFloat() * time_since_last_update;
      Number dz = movement_.dz.toFloat() * time_since_last_update;
      WorldRegion* current_region = my_actor->getRegion();
      const MapLocationInfo* next_map_location_info
        = current_region->getNextActorMapLocationInfo(movement_.x,
                                                      movement_.z,
                                                      dx,
                                                      dz);
      if (!next_map_location_info || // wtf?
          !next_map_location_info->can_move_over) {
        stateChange(IDLE);
        my_actor->setMovingFlag(false);
        return;
      }

      // Get the actor's next location
      Number next_x = movement_.x + dx;
      Number next_z = movement_.z + dz;

      // Collide with actors
      ActorInstance* collision
        = current_region->collideActorWithActor(my_actor,
                                                movement_.x.toFloat(),
                                                movement_.z.toFloat(),
                                                next_x.toFloat(),
                                                next_z.toFloat());
      if (collision != NULL &&
          !my_actor->isGlancingCollision(collision, dx, dz)) {
        my_actor->setMovingFlag(false);
        if (target != collision) stateChange(IDLE);
      } else {
        movement_.x = next_x; // set the actor's next coordinates so it keeps moving
        movement_.z = next_z;
      }

    }
    if (separation_distance < attack_.attack_range) {
      // we are close enough to attack
      if (next_action_timer_.expired()) {
        my_actor->faceActor(target);
        attack_.end_attack_timer.set(stats_->melee_effect_delay);
        next_action_timer_.set(stats_->melee_repeat_delay);
        my_actor->setAction(Actor::ACTORACTION_ATTACK, stats_->melee_repeat_delay);
        attack_.effect.hp_change
          = -Design::AvatarMeleeDamage(
              avatar_->level,
              stats_->melee_min_damage.wholePart(),
              stats_->melee_max_damage.wholePart(),
              avatar_->attribute_values[Avatar::ATTRIBUTE_STRENGTH],
              avatar_->class_);
        attack_.effect.hp_change *= attack_run_.first_hit_damage_bonus;
        attack_.effect.hp_steal = stats_->hp_steal;
        attack_.effect.mp_steal = stats_->mp_steal;
        stateChange(avatar_->equipment.projectile
                      ? RANGED_ATTACK
                      : MELEE_ATTACK);
      }
    }
  } else {
    // we are running at a location, so this is essentially a fast walk
    if (!movement_.stop_timer.expired()) {
      WorldRegion* current_region = my_actor->getRegion();
      Number x = movement_.x;
      Number z = movement_.z;
      Number dx = movement_.dx.toFloat() * time_since_last_update;
      Number dz = movement_.dz.toFloat() * time_since_last_update;

      // Get the next location's info
      const MapLocationInfo* next_map_location_info
        = current_region->getNextActorMapLocationInfo(x,
                                                      z,
                                                      dx,
                                                      dz);
      if (!next_map_location_info || // hack...might happen if attack running into teleporter?
          !next_map_location_info->can_move_over) {
        movement_.stop_timer.trigger(); // stop moving
        my_actor->setMovingFlag(false);
      } else {

        // Get the actor's next location
        Number next_x = x + dx;
        Number next_z = z + dz;

        // Collide with actors
        ActorInstance* collision
          = current_region->collideActorWithActor(my_actor,
                                                  x.toFloat(),
                                                  z.toFloat(),
                                                  next_x.toFloat(),
                                                  next_z.toFloat());
        if (collision != NULL && !my_actor->isGlancingCollision(collision, dx, dz)) {
          movement_.stop_timer.trigger(); // stop moving
          my_actor->setMovingFlag(false);
        } else {
          movement_.x = next_x; // set the actor's next coordinates so it keeps moving
          movement_.z = next_z;
        }
      }
    } else {
      stateChange(IDLE);
      my_actor->setMovingFlag(false);
    }
  }


  switch (request_->type) {
  case PlayerRequest::LEAVE_WORLD:  stateChange(LEAVING_WORLD); break;
  case PlayerRequest::STOP_ACTION:  request_->reset();
  //case PlayerRequest::GEOSID_OPEN:
  case PlayerRequest::STORAGE_OPEN:
  case PlayerRequest::BAZAAR_OPEN:  stateChange(IDLE); break;
  case PlayerRequest::USE_ITEM:    processUseItemRequest(); break;
  case PlayerRequest::CAST:        processCastRequest(); break;
  case PlayerRequest::ATTACK:      processAttackRequest(); break;
  case PlayerRequest::MOVE:        processMoveRequest(); break;
  case PlayerRequest::SPEAK:       processSpeakRequest(true); break;
  case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE); break;
  case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD); break;
  default:
    // ignore this request
    request_->reset();
    break;
  }
}










//----[  updateLeap  ]---------------------------------------------------------
void Player::updateLeap(double time, double time_since_last_update) {
  if (mutation_.flying) {
    stateChange(FLYING);
    updateFlying(time, time_since_last_update);
    return;
  }

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();

  // Turn off the leaping flag after a certain amount of time so that
  // the player can come back down from the jump
  //if (leap_.reset_leap_flag_timer.poll()) {
  //  my_actor->setLeapingFlag(false);
  //}

  // We are jumping at a location, so this is basically like flying for a
  // short period.
  if (!movement_.stop_timer.expired()) {
    WorldRegion* current_region = my_actor->getRegion();
    Number x = movement_.x;
    Number z = movement_.z;
    Number dx = movement_.dx.toFloat() * time_since_last_update;
    Number dz = movement_.dz.toFloat() * time_since_last_update;

    // Get the next location's info
    const MapLocationInfo* next_map_location_info
      = current_region->getNextActorMapLocationInfo(x,
                                                    z,
                                                    dx,
                                                    dz);
    if (!next_map_location_info ||
        !next_map_location_info->can_fly_over) {
      movement_.stop_timer.trigger(); // stop moving
      my_actor->setMovingFlag(false);
    } else {

      // Get the actor's next location
      Number next_x = x + dx;
      Number next_z = z + dz;

      todo("karl", "collide with TALL actors");
      ActorInstance* collision = NULL;
        //= current_region->collideActorWithActor(my_actor,
        //                                        x.toFloat(),
        //                                        z.toFloat(),
        //                                        next_x.toFloat(),
        //                                        next_z.toFloat());
      if (collision != NULL && !my_actor->isGlancingCollision(collision, dx, dz)) {
        movement_.stop_timer.trigger(); // stop moving
        my_actor->setMovingFlag(false);
      } else {
        movement_.x = next_x; // set the actor's next coordinates so it keeps moving
        movement_.z = next_z;
      }
    }

    // While in the air, only process certain events
    switch (request_->type) {
    case PlayerRequest::LEAVE_WORLD:
    case PlayerRequest::STOP_ACTION:
      // end the leap asap
      my_actor->setLeapingFlag(false);
      my_actor->setFlyingLevitatingFlag(false);
      //leap_.reset_leap_flag_timer.trigger();
      request_->reset();
      stateChange(WALKING);
      break;
    //case PlayerRequest::GEOSID_OPEN:  // ignore these requests
    case PlayerRequest::STORAGE_OPEN:
    case PlayerRequest::BAZAAR_OPEN:  request_->reset(); break;
    case PlayerRequest::USE_ITEM:     break; // delay request
    case PlayerRequest::CAST:        processCastRequest(); break;
    case PlayerRequest::ATTACK:
      // delay this request
      break;
    case PlayerRequest::MOVE:
      // change the direction the player is leaping
      break;
    case PlayerRequest::SPEAK:
      // no speaking state-changes
      processSpeakRequest(false);
      break;
    case PlayerRequest::CREATE_GUILD:
    case PlayerRequest::LEAD_GUILD:
      // delay request
      break;
    default:
      // ignore this request
      request_->reset();
      break;
    }
  } else {
    // the leap has completed, so find the target near here

    // oct 14 2009 - disabled because this wasn't working anyway.

    //ActorInstance* target = attack_.target.dereference();
    //if (target && !(target->isDead() || my_actor->actorIsOutOfRange(target))) {

    //  Number separation_distance = my_actor->separationDistanceTo(target);
    //  if (separation_distance < attack_.attack_range) {
    //    // we are close enough to attack this target
    //    if (next_action_timer_.expired()) {
    //      my_actor->faceActor(target);
    //      attack_.end_attack_timer.set(stats_->melee_effect_delay);
    //      next_action_timer_.set(stats_->melee_repeat_delay);
    //      my_actor->setAction(Actor::ACTORACTION_ATTACK, stats_->melee_repeat_delay);
    //      attack_.effect.hp_change
    //        = -Design::AvatarMeleeDamage(
    //            avatar_->level,
    //            stats_->melee_min_damage.wholePart(),
    //            stats_->melee_max_damage.wholePart(),
    //            avatar_->attribute_values[Avatar::ATTRIBUTE_STRENGTH],
    //            avatar_->class_);
    //      attack_.effect.hp_change *= leap_.first_hit_damage_bonus;
    //      attack_.effect.hp_steal = stats_->hp_steal;
    //      attack_.effect.mp_steal = stats_->mp_steal;
    //      stateChange(avatar_->equipment.projectile
    //                    ? RANGED_ATTACK
    //                    : MELEE_ATTACK);
    //      // this return is important so that the state change doesn't get
    //      // overridden before it has a chance to happen!
    //      return;
    //    }
    //  }
    //}

    switch (request_->type) {
    case PlayerRequest::LEAVE_WORLD:  stateChange(LEAVING_WORLD); break;
    case PlayerRequest::STOP_ACTION:  request_->reset();
    //case PlayerRequest::GEOSID_OPEN:
    case PlayerRequest::STORAGE_OPEN:
    case PlayerRequest::BAZAAR_OPEN:  stateChange(IDLE); break;
    case PlayerRequest::USE_ITEM:    processUseItemRequest(); break;
    case PlayerRequest::CAST:        processCastRequest(); break;
    case PlayerRequest::ATTACK:      processAttackRequest(); break;
    case PlayerRequest::MOVE:        processMoveRequest(); break;
    case PlayerRequest::SPEAK:       processSpeakRequest(true); break;
    case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE); break;
    case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD); break;
    default:
      // ignore this request
      request_->reset();
      // we can't stay in the leaping state, and since the player isn't
      // doing anything else we have to simply go idle
      stateChange(IDLE);
      break;
    }
  }
}










//----[  updateUsingMagic  ]---------------------------------------------------
void Player::updateUsingMagic(double time, double time_since_last_update) {
  if (falling_.falling &&
      !(mutation_.flying || mutation_.levitating)) {
    // incrementing the magic stage here will cause whatever spell is being
    // used to act faster than it normally would, but it prevents delayed
    // spells from being cancelled entirely because the player is falling.
    // not a great solution, but meh...
    if (magic_.magic) incrementMagicStage();
    stateChange(FALLING);
    return;
  }
  if (movement_.swimming || !magic_.magic) {
    stateChange(IDLE);
    return;
  }

  if (magic_.increment_stage_timer.poll()) {
    incrementMagicStage();
    //if (!magic_.magic) stateChange(IDLE);
  }

  if (magic_.stage <= 0) {
    // stage 0 is uninterruptable--all "normal" requests are delayed until
    // the stage is finished.
    switch (request_->type) {
    case PlayerRequest::LEAVE_WORLD:  stateChange(LEAVING_WORLD); break;
    case PlayerRequest::STOP_ACTION:  stateChange(IDLE); break;
    //case PlayerRequest::GEOSID_OPEN:
    case PlayerRequest::STORAGE_OPEN:
    case PlayerRequest::BAZAAR_OPEN:
    case PlayerRequest::USE_ITEM:
    case PlayerRequest::CAST:
    case PlayerRequest::ATTACK:
    case PlayerRequest::MOVE:
      // delay this request, since we are currently casting a spell
      break;
    case PlayerRequest::SPEAK:
      processSpeakRequest(false); // no state change
      break;
    case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE);
    case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD);
      // delay the request
      break;
    default:
      // ignore this request
      request_->reset();
      break;
    }
  } else {
    // any stages above 0 can be interrupted by other requests
    switch (request_->type) {
    case PlayerRequest::LEAVE_WORLD:  stateChange(LEAVING_WORLD); break;
    case PlayerRequest::STOP_ACTION:  request_->reset();
    //case PlayerRequest::GEOSID_OPEN:
    case PlayerRequest::STORAGE_OPEN:
    case PlayerRequest::BAZAAR_OPEN:  stateChange(IDLE); break;
    case PlayerRequest::USE_ITEM:
      processUseItemRequest();
      if (!magic_.magic) stateChange(IDLE);
      break;
    case PlayerRequest::CAST:
      processCastRequest();
      // It is important to check for this; otherwise, an instant-spell that
      // interrupts a multistage spell (ex. multi-projectile) makes it
      // impossible to leave the usingMagic state.  This is because the stage
      // gets set to 0 and the increment stage timer is reset.
      if (!magic_.magic) stateChange(IDLE);
      break;
    case PlayerRequest::ATTACK:      processAttackRequest(); break;
    case PlayerRequest::MOVE: {
      // The MOVE request is special for this state.  Since we're casting
      // a multistage spell here, chances are the player wants to just
      // change direction and not cancel the spell.  To cancel the spell,
      // they can just stop action.
      request_->reset();
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->faceLocation(request_->move.x, request_->move.z);
    } break;

    case PlayerRequest::SPEAK:       processSpeakRequest(true); break;
    case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE); break;
    case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD); break;
    default:
      // ignore this request
      request_->reset();
      break;
    }
  }
}










//----[  updateMeleeAttack  ]--------------------------------------------------
void Player::updateMeleeAttack(double time, double time_since_last_update) {
  if (mutation_.flying) {
    stateChange(FLYING);
    updateFlying(time, time_since_last_update);
    return;
  }
  if (movement_.swimming) {
    stateChange(SWIMMING);
    updateSwimming(time, time_since_last_update);
    return;
  }
  if (falling_.falling && !mutation_.levitating) {
    stateChange(FALLING);
    return;
  }

  assert (!avatar_->equipment.projectile); // be sure this is a melee weapon

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  ActorInstance* target = attack_.target.dereference();
  if (!target || target->isDead() || my_actor->actorIsOutOfRange(target)) {
    stateChange(IDLE);
    return;
  }

  Number separation_distance = my_actor->separationDistanceTo(target);
  if (attack_.follow_range < separation_distance) {
    // We are too far away to attack.  Move to get closer.
    my_actor->getPosition(&movement_.x, &movement_.z);
    Number offset_x, offset_z;
    target->getPosition(&offset_x, &offset_z);
    offset_x -= movement_.x;
    offset_z -= movement_.z;

    // Calculate distance from us -> target actor
    Number distance;
    distance.int_value
      = FixedPoint::fixmul<16>(offset_x.int_value, offset_x.int_value)
      + FixedPoint::fixmul<16>(offset_z.int_value, offset_z.int_value);
    distance = FixedPoint::sqrt(distance);

    // Make the actor face this location
    my_actor->setDirection(FixedPoint::atan2(offset_z, offset_x).toFloat());
    my_actor->setMovingFlag(true);

    // Calculate the rate at which to move on each axis.  We can ignore
    // the stop moving timer.
    movement_.dx.int_value
      = FixedPoint::fixmul<16>(stats_->movement_speed.int_value,
                                (offset_x / distance).int_value);
    movement_.dz.int_value
      = FixedPoint::fixmul<16>(stats_->movement_speed.int_value,
                               (offset_z / distance).int_value);
    movement_.stop_timer.trigger();
    Number dx = movement_.dx.toFloat() * time_since_last_update;
    Number dz = movement_.dz.toFloat() * time_since_last_update;

    // Get the next location's info
    WorldRegion* current_region = my_actor->getRegion();
    const MapLocationInfo* next_map_location_info
      = current_region->getNextActorMapLocationInfo(movement_.x,
                                                    movement_.z,
                                                    dx,
                                                    dz);
    if (!next_map_location_info ||
        !next_map_location_info->can_move_over) {
      stateChange(IDLE);
      my_actor->setMovingFlag(false);
      return;
    }

    // Get the actor's next location
    Number next_x = movement_.x + dx;
    Number next_z = movement_.z + dz;

    // Collide with actors
    ActorInstance* collision
      = current_region->collideActorWithActor(my_actor,
                                              movement_.x.toFloat(),
                                              movement_.z.toFloat(),
                                              next_x.toFloat(),
                                              next_z.toFloat());
    if (collision != NULL &&
        !my_actor->isGlancingCollision(collision, dx, dz)) {
      my_actor->setMovingFlag(false);
      if (target != collision) stateChange(IDLE);
    } else {
      movement_.x = next_x; // set the actor's next coordinates so it keeps moving
      movement_.z = next_z;
    }

  }
  if (request_->type == PlayerRequest::NONE && // needed so that auto-attack doesn't block actions
      (separation_distance < attack_.attack_range)) {
    // we are close enough to attack
    if (next_action_timer_.expired()) {
      my_actor->faceActor(target);
      attack_.end_attack_timer.set(stats_->melee_effect_delay);
      next_action_timer_.set(stats_->melee_repeat_delay);
      my_actor->setAction(Actor::ACTORACTION_ATTACK, stats_->melee_repeat_delay);
      attack_.effect.hp_change
        = -Design::AvatarMeleeDamage(
            avatar_->level,
            stats_->melee_min_damage.wholePart(),
            stats_->melee_max_damage.wholePart(),
            avatar_->attribute_values[Avatar::ATTRIBUTE_STRENGTH],
            avatar_->class_);
      attack_.effect.hp_steal = stats_->hp_steal;
      attack_.effect.mp_steal = stats_->mp_steal;
    }
    if (attack_.end_attack_timer.poll()) {
      Number damage_dealt
        = target->changeHP(my_actor, false, attack_.effect.hp_change);
      client_->changeHP(NULL, true, -attack_.effect.hp_steal * damage_dealt);
      client_->changeMP(-attack_.effect.mp_steal * damage_dealt);
    }
  }


  switch (request_->type) {
  case PlayerRequest::LEAVE_WORLD:  stateChange(LEAVING_WORLD); break;
  case PlayerRequest::STOP_ACTION:  request_->reset();
  //case PlayerRequest::GEOSID_OPEN:
  case PlayerRequest::STORAGE_OPEN:
  case PlayerRequest::BAZAAR_OPEN:  stateChange(IDLE); break;
  case PlayerRequest::USE_ITEM:    processUseItemRequest(); break;
  case PlayerRequest::CAST:        processCastRequest(); break;
  case PlayerRequest::ATTACK:      processAttackRequest(); break;
  case PlayerRequest::MOVE:        processMoveRequest(); break;
  case PlayerRequest::SPEAK:       processSpeakRequest(true); break;
  case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE); break;
  case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD); break;
  default:
    // ignore this request
    request_->reset();
    break;
  }
}










//----[  updateRangedAttack  ]-------------------------------------------------
void Player::updateRangedAttack(double time, double time_since_last_update) {

  if (mutation_.flying) {
    stateChange(FLYING);
    updateFlying(time, time_since_last_update);
    return;
  }
  if (movement_.swimming) {
    stateChange(SWIMMING);
    updateSwimming(time, time_since_last_update);
    return;
  }
  if (falling_.falling && !mutation_.levitating) {
    stateChange(FALLING);
    return;
  }
  assert(avatar_->equipment.projectile); // be sure this is a ranged weapon

  ActorInstance* target = attack_.target.dereference();
  if (!target || target->isDead()) {
    stateChange(IDLE);
    return;
  }

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  Number separation_distance = my_actor->separationDistanceTo(target);
  if (separation_distance > attack_.follow_range) {
    // we are too far away--move to get closer.
    my_actor->getPosition(&movement_.x, &movement_.z);
    Number offset_x, offset_z;
    target->getPosition(&offset_x, &offset_z);
    offset_x -= movement_.x;
    offset_z -= movement_.z;

    // Calculate distance from us -> target actor
    Number distance;
    distance.int_value
      = FixedPoint::fixmul<16>(offset_x.int_value, offset_x.int_value)
      + FixedPoint::fixmul<16>(offset_z.int_value, offset_z.int_value);
    distance = FixedPoint::sqrt(distance);

    // Make the actor face this location
    my_actor->setDirection(FixedPoint::atan2(offset_z, offset_x).toFloat());
    my_actor->setMovingFlag(true);

    // Calculate the rate at which to move on each axis.  We can ignore
    // the stop moving timer.
    movement_.dx.int_value
      = FixedPoint::fixmul<16>(stats_->movement_speed.int_value,
                                (offset_x / distance).int_value);
    movement_.dz.int_value
      = FixedPoint::fixmul<16>(stats_->movement_speed.int_value,
                               (offset_z / distance).int_value);
    movement_.stop_timer.trigger();
    Number dx = movement_.dx.toFloat() * time_since_last_update;
    Number dz = movement_.dz.toFloat() * time_since_last_update;

    // Get the next location's info
    WorldRegion* current_region = my_actor->getRegion();
    const MapLocationInfo* next_map_location_info
      = current_region->getNextActorMapLocationInfo(movement_.x,
                                                    movement_.z,
                                                    dx,
                                                    dz);
    if (!next_map_location_info ||
        !next_map_location_info->can_move_over) {
      stateChange(IDLE);
      my_actor->setMovingFlag(false);
      return;
    }

    // Get the actor's next location
    Number next_x = movement_.x + dx;
    Number next_z = movement_.z + dz;

    // Collide with actors
    ActorInstance* collision
      = current_region->collideActorWithActor(my_actor,
                                              movement_.x.toFloat(),
                                              movement_.z.toFloat(),
                                              next_x.toFloat(),
                                              next_z.toFloat());
    if (collision != NULL &&
        !my_actor->isGlancingCollision(collision, dx, dz)) {
      my_actor->setMovingFlag(false);
      if (target != collision) stateChange(IDLE);
    } else {
      movement_.x = next_x; // set the actor's next coordinates so it keeps moving
      movement_.z = next_z;
    }

  }
  /*else*/
  if (request_->type == PlayerRequest::NONE && // needed so that auto-attack doesn't block actions
      (separation_distance < attack_.attack_range)) {
    // we are close enough to attack
    if (next_action_timer_.expired()) {
      my_actor->faceActor(target);
      attack_.end_attack_timer.set(stats_->melee_effect_delay);
      next_action_timer_.set(stats_->melee_repeat_delay);
      my_actor->setAction(Actor::ACTORACTION_ATTACK, stats_->melee_repeat_delay);
      attack_.effect.hp_change
        = -Design::AvatarMeleeDamage(
            avatar_->level,
            stats_->melee_min_damage.wholePart(),
            stats_->melee_max_damage.wholePart(),
            avatar_->attribute_values[Avatar::ATTRIBUTE_STRENGTH],
            avatar_->class_);
      attack_.effect.hp_steal = stats_->hp_steal;
      attack_.effect.mp_steal = stats_->mp_steal;
      attack_.effect.magic = false;
    }
    if (attack_.end_attack_timer.poll()) {
      // create the projectile
      PlayerProjectileInstance* projectile
        = acquireProjectileInstance(avatar_->equipment.projectile);
      if (projectile) {
        const Evidyon::Projectile::Projectile* data
          = avatar_->equipment.projectile;
        projectile->event_id = GenerateEventID();
        projectile->x = movement_.x;
        projectile->z = movement_.z;
        projectile->dx = cos(my_actor->getDirection()) * data->speed;
        projectile->dz = sin(my_actor->getDirection()) * data->speed;
        //projectile->dx = FixedPoint::cos(my_actor->getDirection())
        //projectile->dz = FixedPoint::sin(my_actor->getDirection())
        todo("karl", "todo....better?");
        projectile->effect = attack_.effect;
        projectile->expiration_timer.set(data->duration);
        projectile->homing.target_actor.copy(&attack_.target);
        target->getPosition(
          &projectile->homing.target_x,
          &projectile->homing.target_z);
        if (data->piercing) {
          projectile->piercing.next_pierce_hit_timer.trigger();
        } else {
          projectile->piercing.next_pierce_hit_timer.reset();
        }
        if (data->piercing || data->homing) {
          assert(false && "piercing/homing projectiles are non-functional");
        } else {
          todo("karl", "the direction of an actor should be a number anyway");
          my_actor->broadcastEvent_GenerateBasicProjectile(
            projectile->event_id,
            data->event_index,
            Number(my_actor->getDirection()),
            movement_.x,
            movement_.z);
        }
      }
    }
  }


  switch (request_->type) {
  case PlayerRequest::LEAVE_WORLD:  stateChange(LEAVING_WORLD); break;
  case PlayerRequest::STOP_ACTION:  request_->reset();
  //case PlayerRequest::GEOSID_OPEN:
  case PlayerRequest::STORAGE_OPEN:
  case PlayerRequest::BAZAAR_OPEN:  stateChange(IDLE); break;
  case PlayerRequest::USE_ITEM:    processUseItemRequest(); break;
  case PlayerRequest::CAST:        processCastRequest(); break;
  case PlayerRequest::ATTACK:      processAttackRequest(); break;
  case PlayerRequest::MOVE:        processMoveRequest(); break;
  case PlayerRequest::SPEAK:       processSpeakRequest(true); break;
  case PlayerRequest::CREATE_GUILD:stateChange(GUILD_CREATE); break;
  case PlayerRequest::LEAD_GUILD:  stateChange(GUILD_LEAD); break;
  default:
    // ignore this request
    request_->reset();
    break;
  }
}









//----[  updateDead  ]---------------------------------------------------------
void Player::updateDead(double time, double time_since_last_update) {

  // if resurrected, the respawn timer should be cleared so we don't have
  // to check that flag here
  if (falling_.respawn_timer.poll() ||
      dead_.respawn_timer.poll()) {
    ActorInstance* my_actor = actor_.dereferenceAssumingValid();
    client_->respawn(true,  // restore hp/mp
                     true); // penalize xp (if in arena, xp will not be penalized)
    stateChange(IDLE);
  }

  switch (request_->type) {
  case PlayerRequest::LEAVE_WORLD:

    // if a dead person wants to leave the world, just respawn them and allow it
    // to happen.
    if (dead_.resurrected) {
      client_->respawn(true, false);
      stateChange(LEAVING_WORLD);
      leave_world_.logoff_timer.trigger();
    }

    // it's really complicated to leave the world while dead, so just disallow
    // this from happening for now.

    ////// if logging off while dead, the player must leave the world IMMEDIATELY
    ////// on the next loop before any processing is done; otherwise, there is a
    ////// chance that the player could exit the DEAD state without respawning.
    ////// For this reason, leaving the world is only allowed if the player is
    ////// resurrected anyway.
    ////// This ties with the login logic--if a player logs in and is dead, they
    ////// are assumed to have been resurrected.
    ////// Furthermore, if a player is falling, resurrected and then logs out, they
    ////// are respawned before leaving the world so that they will log back in
    ////// at their safe zone.  This is because the login code will send the
    ////// player directly to this dead state, which won't detect them as falling.
    ////if (falling_.falling) client_->respawn(false, true);
    ////stateChange(LEAVING_WORLD);
    ////leave_world_.logoff_timer.trigger();
    break;
  case PlayerRequest::STANDUP:
    if (!dead_.resurrected) break;
    stateChange(IDLE);
    break;
  case PlayerRequest::RESPAWN:
    if (!dead_.resurrected) break;
    client_->respawn(true, false); // respawn(restore hp, penalize xp)
    stateChange(IDLE);
    break;
  // disabled: stop action, geosid, storage,
  //           bazaar, use item, cast, attack, move
  case PlayerRequest::SPEAK:
    processSpeakRequest(false); // allow speaking, but block state changes
    break;
  // disabled: create guild, lead guild
  default:
    // ignore this request
    request_->reset();
    break;
  }
}




//----[  updateLeavingWorld  ]-------------------------------------------------
void Player::updateLeavingWorld(double time, double time_since_last_update) { 
  if (request_->type == PlayerRequest::LEAVE_WORLD) request_->reset();
  if (request_->type == PlayerRequest::SPEAK) processSpeakRequest(true);
  if (falling_.falling || request_->type != PlayerRequest::NONE) {
    stateChange(IDLE);
    updateIdle(time, time_since_last_update);
  }
}




//----[  updateGuildCreate  ]--------------------------------------------------
void Player::updateGuildCreate(double time, double time_since_last_update) {
  if (falling_.falling || request_->type != PlayerRequest::CREATE_GUILD) {
    stateChange(IDLE);
    updateIdle(time, time_since_last_update);
  }
}




//----[  updateGuildLead  ]----------------------------------------------------
void Player::updateGuildLead(double time, double time_since_last_update) {
  if (falling_.falling || request_->type != PlayerRequest::NONE) {
    stateChange(IDLE);
    updateIdle(time, time_since_last_update);
  }
}




//----[  calculateEquipment  ]-------------------------------------------------
void Player::calculateEquipment() {
  avatar_->equipment.weapon_min_damage = 0;
  avatar_->equipment.weapon_max_damage = 0;
  avatar_->equipment.max_hp = avatar_->guild_max_hp;
  avatar_->equipment.max_mp = avatar_->guild_max_mp;
  avatar_->equipment.melee_repeat_delay = UNARMED_MELEE_REPEAT_DELAY;
  avatar_->equipment.movement_speed = avatar_->movement_speed;
  avatar_->equipment.defense = 0;
  avatar_->equipment.spell_availability.copy(&avatar_->spell_availability);
  avatar_->equipment.hp_steal = 0;
  avatar_->equipment.mp_steal = 0;
  avatar_->equipment.hp_regen = avatar_->hp_regen;
  avatar_->equipment.mp_regen = avatar_->mp_regen;

  // This structure is used to determine what equipment is valid
  struct {
    const Evidyon::Item::ItemServerDescription* main_hand;
    const Evidyon::Item::ItemServerDescription* off_hand;
    const Evidyon::Item::ItemServerDescription* two_hand;
    const Evidyon::Item::ItemServerDescription* shield;
    const Evidyon::Item::ItemServerDescription* armor;
    const Evidyon::Item::ItemServerDescription* helmet;
    const Evidyon::Item::ItemServerDescription* belt;
    const Evidyon::Item::ItemServerDescription* rings[6];
    int number_of_rings;
    const Evidyon::Item::ItemServerDescription* boots;
    const Evidyon::Item::ItemServerDescription* gloves;

    // multipliers for the base values
    float movement_speed_multiplier;
    float attack_delay_multiplier;
  } equipment;
  memset(&equipment, 0, sizeof(equipment));
  equipment.movement_speed_multiplier = 1.0f;
  equipment.attack_delay_multiplier = 1.0f;


  ItemInstance* equipped_item = NULL;
  unsigned int equipped_item_index = 0;
  while (items_->getNextEquippedItem(equipped_item_index,
                                    &equipped_item_index,
                                    &equipped_item)) {
    const Evidyon::Item::ItemServerDescription* description
      = equipped_item->getDescription();
    assert(description);

    // Note: all "goto EQUIPPED_ITEM_IS_INVALID" jump to the
    // end of this loop so that the item can be unequipped
    // without having to deal with a lot of flags, sentinel
    // if {} statements and other such nonsense.

    // Check item prerequisites.  If the item has prereqs the character
    // doesn't meet, it can't be equipped.
    if (description->requirements.level > avatar_->level) {
      goto EQUIPPED_ITEM_IS_INVALID;
    }
    for (int i = 0; i < Avatar::NUMBER_OF_AVATAR_ATTRIBUTES; ++i) {
      if (description->requirements.attribute_value[i] > avatar_->attribute_values[i]) {
        goto EQUIPPED_ITEM_IS_INVALID;
      }
    }

    switch (description->equipment_type) {
    default: assert(false); break;
    case Item::ITEMEQUIPMENTTYPE_1H:
      if (equipment.two_hand) goto EQUIPPED_ITEM_IS_INVALID;
      if (!equipment.main_hand) {
        equipment.main_hand = description;
      } else if (!equipment.off_hand &&
                 !equipment.shield &&
                 (equipment.main_hand->equipment_type == Item::ITEMEQUIPMENTTYPE_1H ||
                  equipment.main_hand->equipment_type == Item::ITEMEQUIPMENTTYPE_MAINHAND)) {
        equipment.off_hand = description;
      } else {
        goto EQUIPPED_ITEM_IS_INVALID;
      }
      break;
    case Item::ITEMEQUIPMENTTYPE_JAVELIN:
    case Item::ITEMEQUIPMENTTYPE_THROWN:
      if (equipment.off_hand) goto EQUIPPED_ITEM_IS_INVALID;
    case Item::ITEMEQUIPMENTTYPE_MAINHAND:
      if (equipment.two_hand ||
          equipment.main_hand) goto EQUIPPED_ITEM_IS_INVALID;
      equipment.main_hand = description;
      break;
    case Item::ITEMEQUIPMENTTYPE_OFFHAND:
      if (equipment.two_hand ||
          equipment.shield ||
          equipment.off_hand) goto EQUIPPED_ITEM_IS_INVALID;
      if (equipment.main_hand &&
          (equipment.main_hand->equipment_type == Item::ITEMEQUIPMENTTYPE_JAVELIN ||
           equipment.main_hand->equipment_type == Item::ITEMEQUIPMENTTYPE_THROWN)) {
        // can't equip an off-hand weapon with a thrown or javelin weapon
        goto EQUIPPED_ITEM_IS_INVALID;
      }
      equipment.off_hand = description;
      break;
    case Item::ITEMEQUIPMENTTYPE_SHIELD:
      if (equipment.two_hand ||
          equipment.off_hand ||
          equipment.shield) goto EQUIPPED_ITEM_IS_INVALID;
      equipment.shield = description;
      break;

    case Item::ITEMEQUIPMENTTYPE_2H:
    case Item::ITEMEQUIPMENTTYPE_POLEARM:
    case Item::ITEMEQUIPMENTTYPE_BOW:
    case Item::ITEMEQUIPMENTTYPE_CROSSBOW:
    case Item::ITEMEQUIPMENTTYPE_DUALWIELD:
    case Item::ITEMEQUIPMENTTYPE_UNARMED:
    case Item::ITEMEQUIPMENTTYPE_SPEAR:
      if (equipment.two_hand ||
          equipment.main_hand ||
          equipment.off_hand ||
          equipment.shield) goto EQUIPPED_ITEM_IS_INVALID;
      equipment.two_hand = description;
      break;

    case Item::ITEMEQUIPMENTTYPE_ARMOR:
      if (equipment.armor) goto EQUIPPED_ITEM_IS_INVALID;
      equipment.armor = description;
      break;
    case Item::ITEMEQUIPMENTTYPE_HELMET:
      if (equipment.helmet) goto EQUIPPED_ITEM_IS_INVALID;
      equipment.helmet = description;
      break;
    case Item::ITEMEQUIPMENTTYPE_BELT:
      if (equipment.belt) goto EQUIPPED_ITEM_IS_INVALID;
      equipment.belt = description;
      break;
    case Item::ITEMEQUIPMENTTYPE_RING:
      if (equipment.number_of_rings >= 6) goto EQUIPPED_ITEM_IS_INVALID;
      assert(!equipment.rings[equipment.number_of_rings]);
      equipment.rings[equipment.number_of_rings++] = description;
      break;
    case Item::ITEMEQUIPMENTTYPE_ACCESSORY: {
      todo("karl", "handle accessory item equipment types");
      goto EQUIPPED_ITEM_IS_INVALID;
    } break;
    case Item::ITEMEQUIPMENTTYPE_BOOTS:
      if (equipment.boots) goto EQUIPPED_ITEM_IS_INVALID;
      equipment.boots = description;
      break;
    case Item::ITEMEQUIPMENTTYPE_GLOVES:
      if (equipment.gloves) goto EQUIPPED_ITEM_IS_INVALID;
      equipment.gloves = description;
      break;
    case Item::ITEMEQUIPMENTTYPE_NONE:
      // cannot be equipped!
      goto EQUIPPED_ITEM_IS_INVALID;
      break;
    }

    avatar_->equipment.weapon_min_damage
      += description->equipped_avatar_modifiers.min_attack_damage;
    avatar_->equipment.weapon_max_damage
      += description->equipped_avatar_modifiers.max_attack_damage;
    avatar_->equipment.hp_regen
      += description->equipped_avatar_modifiers.hp_regen_rate;
    avatar_->equipment.mp_regen
      += description->equipped_avatar_modifiers.mp_regen_rate;
    avatar_->equipment.max_hp
      += description->equipped_avatar_modifiers.max_hp_added;
    avatar_->equipment.max_hp
      *= description->equipped_avatar_modifiers.max_hp_multiplier;
    avatar_->equipment.max_mp
      += description->equipped_avatar_modifiers.max_mp_added;
    avatar_->equipment.max_mp
      *= description->equipped_avatar_modifiers.max_mp_multiplier;
    equipment.movement_speed_multiplier
      *= description->equipped_avatar_modifiers.movement_speed_multiplier;
    equipment.attack_delay_multiplier
      *= description->equipped_avatar_modifiers.attack_delay_multiplier;
    avatar_->equipment.defense
      += description->equipped_avatar_modifiers.physical_defense;
    avatar_->equipment.hp_steal
      += description->equipped_avatar_modifiers.hp_steal;
    avatar_->equipment.mp_steal
      += description->equipped_avatar_modifiers.mp_steal;

    ++equipped_item_index;
    continue;

EQUIPPED_ITEM_IS_INVALID:
    items_->changeEquipped(equipped_item_index);
    ++equipped_item_index;
  }

  // guard against logical problems in the above code
  assert(equipment.main_hand == 0 || equipment.two_hand == 0);
  assert(equipment.off_hand == 0 || equipment.two_hand == 0);
  assert(equipment.shield == 0 || equipment.off_hand == 0);
  assert(equipment.shield == 0 || equipment.two_hand == 0);

  // Build the specialization type, the equipped items and the
  // actor's description information
  Evidyon::Actor::ActorProfileIndex actor_profile
    = Evidyon::Actor::INVALID_ACTOR_PROFILE_INDEX;
  Evidyon::SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form
    = Evidyon::SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX;
  Actor::ActorProfileSpecializationType specialization
    = Actor::SPECIALIZE_UNARMED;
  Scenery::SceneryIndex equipped[Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS] = {
    Scenery::INVALID_SCENERY_INDEX, // primary
    Scenery::INVALID_SCENERY_INDEX, // alternate
    Scenery::INVALID_SCENERY_INDEX, // helmet
  };
  avatar_->equipment.projectile = NULL; // by default, no projectile
  if (equipment.two_hand) {
    avatar_->equipment.melee_repeat_delay = equipment.two_hand->weapon.attack_duration;
    avatar_->equipment.melee_attack_distance = equipment.two_hand->weapon.range;
    equipped[Actor::ACTOREQUIPMENTSLOT_PRIMARY]
      = equipment.two_hand->equipped_actor_modifiers.equipped_scenery;
    switch (equipment.two_hand->equipment_type) {
    default: assert(false);
    case Item::ITEMEQUIPMENTTYPE_2H:
      specialization = Actor::SPECIALIZE_2HWEAPON;
      break;
    case Item::ITEMEQUIPMENTTYPE_POLEARM:
      specialization = Actor::SPECIALIZE_STAFF;
      break;
    case Item::ITEMEQUIPMENTTYPE_BOW:
      avatar_->equipment.projectile = &equipment.two_hand->projectile;
      specialization = Actor::SPECIALIZE_BOW;
      // bows are equipped on the off-hand
      equipped[Actor::ACTOREQUIPMENTSLOT_PRIMARY]
        = Scenery::INVALID_SCENERY_INDEX;
      equipped[Actor::ACTOREQUIPMENTSLOT_ALTERNATE]
        = equipment.two_hand->equipped_actor_modifiers.equipped_scenery;
      break;
    case Item::ITEMEQUIPMENTTYPE_CROSSBOW:
      avatar_->equipment.projectile = &equipment.two_hand->projectile;
      specialization = Actor::SPECIALIZE_CROSSBOW;
      // crossbows are equipped on the off-hand
      equipped[Actor::ACTOREQUIPMENTSLOT_PRIMARY]
        = Scenery::INVALID_SCENERY_INDEX;
      equipped[Actor::ACTOREQUIPMENTSLOT_ALTERNATE]
        = equipment.two_hand->equipped_actor_modifiers.equipped_scenery;
      break;
    case Item::ITEMEQUIPMENTTYPE_DUALWIELD:
      specialization = Actor::SPECIALIZE_DUAL_WIELD;
      break;
    case Item::ITEMEQUIPMENTTYPE_UNARMED:
      specialization = Actor::SPECIALIZE_UNARMED;
      break;
    case Item::ITEMEQUIPMENTTYPE_SPEAR:
      specialization = Actor::SPECIALIZE_SPEAR;
      break;
    }
  } else if (equipment.main_hand) { // check for main-hand item
    avatar_->equipment.melee_repeat_delay = equipment.main_hand->weapon.attack_duration;
    avatar_->equipment.melee_attack_distance = equipment.main_hand->weapon.range;
    equipped[Actor::ACTOREQUIPMENTSLOT_PRIMARY]
      = equipment.main_hand->equipped_actor_modifiers.equipped_scenery;
    if (equipment.main_hand->equipment_type
          == Item::ITEMEQUIPMENTTYPE_JAVELIN) {
      avatar_->equipment.projectile = &equipment.main_hand->projectile;
      specialization = Actor::SPECIALIZE_JAVELIN;
      if (equipment.shield) { // check for shield
        specialization = Actor::SPECIALIZE_1HWEAPON_WITH_SHIELD;
        equipped[Actor::ACTOREQUIPMENTSLOT_ALTERNATE]
          = equipment.shield->equipped_actor_modifiers.equipped_scenery;
      }
    } else if (equipment.main_hand->equipment_type
                == Item::ITEMEQUIPMENTTYPE_THROWN) {
      avatar_->equipment.projectile = &equipment.main_hand->projectile;
      specialization = Actor::SPECIALIZE_THROWN;
      if (equipment.shield) { // check for shield
        specialization = Actor::SPECIALIZE_1HWEAPON_WITH_SHIELD;
        equipped[Actor::ACTOREQUIPMENTSLOT_ALTERNATE]
          = equipment.shield->equipped_actor_modifiers.equipped_scenery;
      }
    } else {
      if (equipment.shield) { // check for shield
        specialization = Actor::SPECIALIZE_1HWEAPON_WITH_SHIELD;
        equipped[Actor::ACTOREQUIPMENTSLOT_ALTERNATE]
          = equipment.shield->equipped_actor_modifiers.equipped_scenery;
      } else {  // check for off-hand item
        if (equipment.off_hand) {
          equipped[Actor::ACTOREQUIPMENTSLOT_ALTERNATE]
            = equipment.off_hand->equipped_actor_modifiers.equipped_scenery;
          // specifically offhand items don't invoke dual-wielding
          specialization
            = (equipment.off_hand->equipment_type == Item::ITEMEQUIPMENTTYPE_OFFHAND)
                ? Actor::SPECIALIZE_1HWEAPON_NO_SHIELD
                : Actor::SPECIALIZE_DUAL_WIELD;
          if (equipment.off_hand->equipment_type != Item::ITEMEQUIPMENTTYPE_OFFHAND) {
            // Adding an off-hand item usually means the player is equipping two
            // weapons.  Unless we change the damage here, this will double the player's
            // damage straight up.  This is a hack that makes the item damages not
            // stack in this way.
            todo("karl","handle dual-wielding weapon damage less hackishly");
            avatar_->equipment.weapon_min_damage /= 1.75;
            avatar_->equipment.weapon_max_damage /= 1.75;
            avatar_->equipment.melee_repeat_delay += equipment.off_hand->weapon.attack_duration;
            avatar_->equipment.melee_repeat_delay /= 2;
          }
        } else {
          specialization = Actor::SPECIALIZE_1HWEAPON_NO_SHIELD;
        }
      }
    }
  } else {
    avatar_->equipment.melee_repeat_delay = UNARMED_MELEE_REPEAT_DELAY; // default delay
    avatar_->equipment.melee_attack_distance = UNARMED_MELEE_ATTACK_RANGE; // default range
    if (equipment.shield) { // is there a shield?
      todo("karl","this hack was added to make shields show up when "\
                  "the user equips a shield but has no mainhand weapon; "\
                  "since Erich made the anims, we should really have a "\
                  "SPECIALIZE_NOMAIN_SHIELD");
      specialization = Actor::SPECIALIZE_1HWEAPON_WITH_SHIELD;
      equipped[Actor::ACTOREQUIPMENTSLOT_ALTERNATE]
        = equipment.shield->equipped_actor_modifiers.equipped_scenery;
    } else {
      specialization = Actor::SPECIALIZE_UNARMED;
    }
  }
  assert(specialization != Actor::INVALID_ACTOR_PROFILE_SPECIALIZATION);
  if (equipment.armor) { // change form to account for armor
    // this is where one would apply set effects; ex. all bone armor?
    // turn into skeleton...etc.
    actor_profile
      = equipment.armor->equipped_actor_modifiers.actor_templates
        [avatar_->gender][avatar_->race][avatar_->class_].actor_profile;
    skinned_mesh_form
      = equipment.armor->equipped_actor_modifiers.actor_templates
        [avatar_->gender][avatar_->race][avatar_->class_].skinned_mesh_form;
  } else {
    actor_profile = avatar_->actor_profile;
    skinned_mesh_form = avatar_->skinned_mesh_form;
  }

  assert(actor_profile != Actor::INVALID_ACTOR_PROFILE_INDEX);
  assert(skinned_mesh_form != SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX);
  if (equipment.helmet) { // add the helmet to the equipment
    equipped[Actor::ACTOREQUIPMENTSLOT_HELMET]
      = equipment.helmet->equipped_actor_modifiers.equipped_scenery;
  }

  // make strength directly add to damage
  avatar_->equipment.weapon_min_damage += avatar_->attribute_values[Avatar::ATTRIBUTE_STRENGTH]-10;
  avatar_->equipment.weapon_max_damage += avatar_->attribute_values[Avatar::ATTRIBUTE_STRENGTH]-5;

  // Initialize combat/movement values
  avatar_->equipment.movement_speed = avatar_->movement_speed * equipment.movement_speed_multiplier;
  avatar_->equipment.melee_repeat_delay *= equipment.attack_delay_multiplier;
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
  avatar_->equipment.melee_effect_delay
    = Time::Milliseconds(MELEE_EFFECT_DELAY_PERCENT
                          * min(Evidyon::ActorInstance::MAX_ACTION_DURATION,
                                avatar_->equipment.melee_repeat_delay));
  avatar_->equipment.melee_follow_distance
    = avatar_->equipment.melee_attack_distance.toFloat() * 0.9;
  todo("karl", "fixed point mult by < 1 is messed up");
  assert(avatar_->equipment.melee_follow_distance > 0);

  // If we are attacking and have changed the equipment, the attack type
  // needs to be updated to account for the change--otherwise, swords
  // could shoot off projectiles that deliver sword-damage simply by
  // starting an attack with a bow and switching to a sword.
  if (state_ == MELEE_ATTACK || state_ == RANGED_ATTACK) {
    stateChangeAvatarAttackType();
  }

  // This is not necessary here since calculateAvatarStats will be called, and
  // it automatically compares the current form to the active form, taking
  // into account mutations.  Calling this here would make it so that
  // changing your equipment while mutated (which has no visual effect)
  // would still force a description update.
  //client_->combineUpdateActorDescriptionFlag(
  //  avatar_->equipment.actor_profile != actor_profile ||
  //  avatar_->equipment.skinned_mesh_form != skinned_mesh_form);

  avatar_->equipment.actor_profile = actor_profile;
  avatar_->equipment.skinned_mesh_form = skinned_mesh_form;
  avatar_->equipment.specialization = specialization;
  for (int i = 0; i < Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
    avatar_->equipment.equipped[i] = equipped[i];
  }

  // Make sure the client updates guild_max_hp and guild_max_mp
  client_->updateGeosidEffects();

  // Equipment has been updated
  items_->resetEquipmentChanged();
  stats_->recalculate_timer.trigger();

  // Update the spell availability on the client if it has changed
  if(stats_->spell_availability.compareCopy(
      &avatar_->equipment.spell_availability)) {
    if (state_ != INVALID) { // only send while active
      client_->sendSpellAvailabilityUpdate();
    }
  }
}




//----[  calculateAvatarStats  ]-----------------------------------------------
void Player::calculateAvatarStats() {

  bool buffs_changed, enchantments_changed;
  enchantments_->update(&stats_->recalculate_timer,
                        &buffs_changed,
                        &enchantments_changed);

  Number action_delay;
  Time::Milliseconds melee_effect_delay, melee_repeat_delay;
  Number magic_damage_multiplier = 1.0f;
  if (enchantments_->active_buffs.value != 0) {
    stats_->max_hp
      = avatar_->equipment.max_hp.toFloat()
        + enchantments_->buff_values[Magic::BUFF_MAX_HP];
    stats_->max_mp
      = avatar_->equipment.max_mp.toFloat()
        + enchantments_->buff_values[Magic::BUFF_MAX_MP];
    stats_->movement_speed
      = avatar_->equipment.movement_speed.toFloat()
        + enchantments_->buff_values[Magic::BUFF_MOVEMENT_SPEED];
    stats_->melee_min_damage
      = avatar_->equipment.weapon_min_damage.toFloat()
        * (1.0 + enchantments_->buff_values[Magic::BUFF_MELEE_DAMAGE]);
    stats_->melee_max_damage
      = avatar_->equipment.weapon_max_damage.toFloat()
        * (1.0 + enchantments_->buff_values[Magic::BUFF_MELEE_DAMAGE]);
    melee_effect_delay
      = (float(avatar_->equipment.melee_effect_delay)
         / (1.0 + enchantments_->buff_values[Magic::BUFF_ACTION_SPEED_MODIFIER]));
    melee_repeat_delay
      = (float(avatar_->equipment.melee_repeat_delay)
         / (1.0 + enchantments_->buff_values[Magic::BUFF_ACTION_SPEED_MODIFIER]));
    magic_damage_multiplier = enchantments_->buff_values[Magic::BUFF_MAGIC_DAMAGE];
    stats_->hp_regen
      = avatar_->equipment.hp_regen.toFloat()
        + enchantments_->buff_values[Magic::BUFF_HP_REGEN]
        - enchantments_->buff_values[Magic::BUFF_POISON];
    stats_->mp_regen
      = avatar_->equipment.mp_regen.toFloat()
        + enchantments_->buff_values[Magic::BUFF_MP_REGEN]
        - enchantments_->buff_values[Magic::BUFF_LEECH];
    stats_->hp_steal
      = avatar_->equipment.hp_steal.toFloat()
        + enchantments_->buff_values[Magic::BUFF_HP_STEAL];
    stats_->mp_steal
      = avatar_->equipment.mp_steal.toFloat()
        + enchantments_->buff_values[Magic::BUFF_MP_STEAL];
  } else {
    stats_->max_hp = avatar_->equipment.max_hp;
    stats_->max_mp = avatar_->equipment.max_mp;
    stats_->movement_speed = avatar_->equipment.movement_speed;
    stats_->melee_min_damage = avatar_->equipment.weapon_min_damage;
    stats_->melee_max_damage = avatar_->equipment.weapon_max_damage;
    melee_effect_delay = avatar_->equipment.melee_effect_delay;
    melee_repeat_delay = avatar_->equipment.melee_repeat_delay;
    stats_->hp_regen = avatar_->equipment.hp_regen;
    stats_->mp_regen = avatar_->equipment.mp_regen;
    stats_->hp_steal = avatar_->equipment.hp_steal;
    stats_->mp_steal = avatar_->equipment.mp_steal;
  }
//  DEBUG_INFO("%f - %f", stats_->melee_min_damage.toFloat(), stats_->melee_max_damage.toFloat());

  // if we are mutated, modify the stats accordingly
  if (mutation_.mutated) {
    if (mutation_.expiration_timer.poll()) {
      mutation_.mutated = false;
    } else {
      // use the mutated form
      stats_->can_speak = mutation_.can_speak;
      stats_->can_cast_spells = mutation_.can_cast_spells;
      stats_->can_melee_attack = mutation_.can_melee_attack;
      stats_->hidden_from_monsters = mutation_.hidden_from_monsters;
      stats_->flying = mutation_.flying;
      stats_->levitating = mutation_.levitating;
      stats_->movement_speed += mutation_.movement_speed;
      melee_effect_delay *= (1 - mutation_.melee_effect_speed).toFloat();
      action_delay *= 1 - mutation_.action_speed;
    }
  }

  if (!mutation_.mutated) {
    // use the default (natural, equipment-based) form
    stats_->can_speak = true;
    stats_->can_cast_spells = true;
    stats_->can_melee_attack = true;
    stats_->hidden_from_monsters = false;
    stats_->flying = false;
    stats_->levitating = false;
    mutation_.actor_profile = avatar_->equipment.actor_profile;
    mutation_.skinned_mesh_form = avatar_->equipment.skinned_mesh_form;
    mutation_.armor = 0; // this is added in below
    for (int i = 0; i < Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
      mutation_.show_equipped[i] = true;
    }
  }

  // Save the melee effect delay and action delays
  stats_->melee_effect_delay
    = Design::AvatarMeleeDelay(
        avatar_->level,
        melee_effect_delay,
        avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY],
        avatar_->class_);
  stats_->melee_repeat_delay
    = Design::AvatarMeleeDelay(
        avatar_->level,
        melee_repeat_delay,
        avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY],
        avatar_->class_);

  bool actor_description_changed = false;

  // Copy equipped items
  for (int i = 0; i < Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
    Scenery::SceneryIndex equipped_item
      = mutation_.show_equipped[i]
        ? avatar_->equipment.equipped[i]
        : Scenery::INVALID_SCENERY_INDEX;
    actor_description_changed =
      actor_description_changed || (equipped_item != stats_->equipped[i]);
    if (actor_description_changed) {
      stats_->equipped[i] = equipped_item;
    }
  }

  // Copy the specialization
  actor_description_changed
    = actor_description_changed
      || (stats_->specialization != avatar_->equipment.specialization);
  stats_->specialization = avatar_->equipment.specialization;

  // Copy the actor's body type
  client_->combineUpdateActorDescriptionFlag(
    actor_description_changed ||
    stats_->actor_profile != mutation_.actor_profile ||
    stats_->skinned_mesh_form != mutation_.skinned_mesh_form);
  stats_->actor_profile = mutation_.actor_profile;
  stats_->skinned_mesh_form = mutation_.skinned_mesh_form;

  // Did the buffs change?  Send an icon update.
  client_->combineUpdateEnchantmentIconsFlag(buffs_changed);
  stats_->buff_icons.value = enchantments_->active_buffs.value;

  // Did the visible enchantments change?  Send a description update.
  client_->combineUpdateVisibleEnchantmentsFlag(enchantments_changed);


  // Calculate the current defense damage reduction factors, and
  // determine if the enchantments need to be modified
  stats_->armor = 
    avatar_->equipment.defense
      + mutation_.armor
      + enchantments_->buff_values[Magic::BUFF_DEFENSE]
      + enchantments_->armor_enchantments[0].defense
      + enchantments_->armor_enchantments[1].defense
      + enchantments_->armor_enchantments[2].defense;
}




//----[  advancePortal  ]------------------------------------------------------
void Player::advancePortal(PlayerPortalInstance* portal,
                           double time,
                           double time_since_last_update) {
  assert(portal);
  if (portal->data == NULL) return;
  advance_portals_  = true;
  if (portal->open_timer.expired() == false) return;
  WorldRegion* source_region = portal->source_region;
  assert(source_region); // should always be valid, even if the portal is invalid
  if (!source_region) { portal->disable(); return; }
  Map* target_map = portal->target_map;
  if (!target_map) {
    // clear this portal's effect from the client since the target was invalid
    source_region->broadcastEvent_Terminate(portal->source_event_id);
    portal->disable();
    return;
  }
  const Magic::Magic_Portal* data = portal->data;
  switch (data->type) {
  default: assert(!"unknown portal type"); portal->disable(); break;
  case Magic::Magic_Portal::SELF_ONLY_TO_SPAWN_POINT:
  case Magic::Magic_Portal::SELF_ONLY_MARK1:
  case Magic::Magic_Portal::SELF_ONLY: {
    ActorInstance* my_actor = actor_.dereferenceAssumingValid();
    if (my_actor->isAlive()) { // only portal if still alive
      assert(portal->target_region);
      my_actor->changeMap(target_map,
                          portal->target_x.toFloat(),
                          portal->target_z.toFloat());
    }
    // the event is over!  since it is timed, though, we don't have to
    // send these terminate messages
    //my_actor->broadcastEvent_Terminate(portal->source_event_id);
    //target_region->broadcastEvent_Terminate(portal->target_event_id);
    portal->disable(); // finished!
    } break;
  case Magic::Magic_Portal::OPEN_PORTAL_MARK1:
  case Magic::Magic_Portal::OPEN_PORTAL:
  case Magic::Magic_Portal::DIMENSION_DOOR:
  case Magic::Magic_Portal::REVERSE_DIMENSION_DOOR: 
  case Magic::Magic_Portal::OPEN_PORTAL_TO_DESTINATION: {
    bool portal_monsters = portal->portal_monsters;
    for (WorldRegion::ActorSet::iterator i = source_region->nearbyActorsBegin();
                                        i != source_region->nearbyActorsEnd();
                                        ++i) {
      ActorInstance* actor = *i;
      assert(actor);
      Client* actor_client = actor->getController()->getClient();
      if ((actor_client ? (!actor_client->onSafeZone()) :
          (portal_monsters && actor->getController()->isMonster())) && // only portal players
          actor->isAlive() && // only portal living things
          actor->distanceToSq(portal->source_x,
                              portal->source_z) < data->radius_sq) {
        actor->changeMap(target_map,
                         portal->target_x,
                         portal->target_z);
      }
    }
    if (portal->close_timer.poll()) {
      // This portal is finished!  we shouldn't have to terminate
      // these events since they are timed to expire at this point
      // as well.
      //source_region->broadcastEvent_Terminate(portal->source_event_id);
      //target_region->broadcastEvent_Terminate(portal->target_event_id);
      portal->disable();
    }
    } break;
  }
}





//----[  advanceProjectile  ]--------------------------------------------------
void Player::advanceProjectile(PlayerProjectileInstance* projectile,
                               double time,
                               double time_since_last_update) {

  todo("karl", "this could be improved by setting a variable to indicate the "\
               "last-processed projectile, that way the loop that calls this "\
               "method could break at 1 or 2 instead of 16 most of the time");

  assert(projectile);
  if (projectile->data == NULL) return;
  Number x = projectile->x, z = projectile->z;
  projectile->x += projectile->dx.toFloat() * time_since_last_update;
  projectile->z += projectile->dz.toFloat() * time_since_last_update;

  // The assumption here is that if the player moves more than 1 region from
  // where projectiles are released, those projectiles will have been destroyed
  // anyway (as they are on a map change, log out of world, etc.)
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  Evidyon::WorldRegion* region = my_actor->getRegion();
  assert(region);
  ActorInstance* collision = region->collideProjectileWithActor(
    x.toFloat(),              // previous coordinates
    z.toFloat(),
    projectile->x.toFloat(),  // current coordinates
    projectile->z.toFloat());

  // Did we move into a new square?  Check to make sure it's passable.
  int projectile_x = projectile->x.wholePart(),
      projectile_z = projectile->z.wholePart();
  if (x.wholePart() != projectile_x ||
      z.wholePart() != projectile_z) {
    const Server::MapLocationInfo* location
      = region->getMap()->getMapLocationInfo(projectile_x, projectile_z);
    if (!location || World::NavigabilitySolidToFlying(location->navigability)) {
      todo("karl", "bouncing projectile here");
      region->broadcastEvent_Terminate(projectile->event_id);
      projectile->disable();
      return;
    }
  }

  // Don't collide with a group member; implicitly this means that the player
  // can't shoot himself, and can shoot *through* other group members
  if (collision && !collision->inSameGroupAs(my_actor)) {
    todo("karl", "piercing projectile here");

    if (attacking_players_ || !collision->getController()->isClient()) {
      Number damage_dealt
        = collision->changeHP(my_actor,
                              projectile->effect.magic,
                              projectile->effect.hp_change);
      client_->changeHP(NULL, true, -projectile->effect.hp_steal * damage_dealt);
      client_->changeMP(-projectile->effect.mp_steal * damage_dealt);

      // Erase this effect
      region->broadcastEvent_Terminate(projectile->event_id);
      projectile->disable();
      return;
    }
  }

  if (projectile->expiration_timer.poll()) {
    projectile->disable();
  } else {
    // This projectile is still alive!
    advance_projectiles_ = true;
  }
}





//----[  advanceTrap  ]--------------------------------------------------------
void Player::advanceTrap(PlayerTrapInstance* trap,
                         double time,
                         double time_since_last_update) {
  assert(trap);
}




//----[  advanceDOTAOEBuff  ]--------------------------------------------------
void Player::advanceDOTAOEBuff() {
  if (dotaoebuff_.expiration_timer.poll() || !dotaoebuff_.data) {
    dotaoebuff_.disable();
    return;
  }
  advance_dotaoebuff_ = true;
  if (dotaoebuff_.event_id == Event::INVALID_EVENT_ID ||
      dotaoebuff_.next_hit_timer.poll()) {
    const Magic::Magic_DOTAOEBuff* data = dotaoebuff_.data;
    dotaoebuff_.next_hit_timer.set(data->period);
    ActorPointer actors[Magic::MAX_TARGETS];
    int targets;
    Event::EventID event_id = dotaoebuff_.event_id;
    if (event_id == Event::INVALID_EVENT_ID) {
      // first instance of this effect
      event_id = GenerateEventID();
      dotaoebuff_.event_id = event_id;
      findAOETargets(&data->data,
                     dotaoebuff_.local_region,
                     NULL,
                     dotaoebuff_.x,
                     dotaoebuff_.z,
                     actors,
                     &targets,
                     data->data.event_index);
    } else {
      // continuing this effect
      findAOETargets(&data->data,
                     dotaoebuff_.local_region,
                     NULL,
                     dotaoebuff_.x,
                     dotaoebuff_.z,
                     actors,
                     &targets,
                     Event::INVALID_EVENT_INDEX);
    }

    // apply the damage
    Number damage
      = Design::AvatarSpellDamage(
          avatar_->level,
          data->dps_to_damage_multiplier,
          avatar_->attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE]);

    // targets that were hit with the buff
    Actor::ActorID buffed_targets[Magic::MAX_TARGETS];
    int number_of_buffed_targets = 0;

    ActorInstance* my_actor = actor_.dereferenceAssumingValid();

    // hit all of the targets
    for (int i = 0; i < targets; ++i) {
      ActorInstance* target = actors[i].dereferenceAssumingValid();
      ActorController* target_controller = target->getController();
      Client* target_client = target_controller->getClient();
      if (target_client) {
        if (target_client->applyBuff(&data->buff, avatar_)) {
          buffed_targets[number_of_buffed_targets++]
            = target->getIDNumber();
        }
      }
      
      // apply the damage
      Number damage_dealt
        = target_controller->changeHP(my_actor, true, -damage);
    }

    // create the buff effect
    if (number_of_buffed_targets) {
      dotaoebuff_.local_region->broadcastEvent_GenerateTargetedSpecialFX(
        GenerateEventID(),
        data->buff.event_index,
        my_actor->getIDNumber(),
        dotaoebuff_.x,
        dotaoebuff_.z,
        buffed_targets,
        number_of_buffed_targets);
    }
  }
}


//----[  advanceDOT  ]---------------------------------------------------------
void Player::advanceDOT(PlayerDOTInstance* dot) {
  assert(dot);

  if (dot->expiration_timer.poll()) {
    releaseDOTInstance(dot);
    return;
  }

  if (dot->next_hit_timer.poll()) {
    dot->next_hit_timer.set(dot->data->period);
    ActorInstance* actor = dot->target.dereference();
    if (!actor) {
      releaseDOTInstance(dot);
      return;
    }
    actor->changeHP(actor_.dereferenceAssumingValid(), true, dot->target_hp_change);
  }

  advance_dots_ = true;
}



//----[  incrementMagicStage  ]------------------------------------------------
void Player::incrementMagicStage() {

  const Magic::Magic* magic = magic_.magic;
  assert(magic);
  if (!magic) return;
  //confirm(magic) else return;

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  switch (magic->type) {

  //----[  AOE  ]--------------------------------------------------------------
  case Magic::Magic::AOE: {
    const Magic::Magic_AOE* data = &magic->aoe;
    Number damage
      = Design::AvatarSpellDamage(
          magic_.casting_level,
          data->dps_to_damage_multiplier,
          avatar_->attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE]);
    for (int i = 0; i < magic_.next_targeted_actor; ++i) {
      ActorInstance* target = magic_.targeted_actors[i].dereference();
      if (!target ||
          my_actor->actorIsOutOfRange(target) ||
          !my_actor->canSee(target)) {
        continue;
      }
      Number damage_dealt = target->changeHP(my_actor, true, -damage);
      if (i == 0) { // steal on the first hit
        client_->changeHP(NULL, true, -attack_.effect.hp_steal * damage_dealt);
        client_->changeMP(-attack_.effect.mp_steal * damage_dealt);
      }
    }
    magic_.magic = NULL;
    } break;



  //----[  SPIN_ATTACK  ]------------------------------------------------------
  case Magic::Magic::SPIN_ATTACK: {
    Number min = stats_->melee_min_damage
                    * Number(magic->spin_attack.damage_factor);
    Number max = stats_->melee_max_damage
                    * Number(magic->spin_attack.damage_factor);
    Number damage
      = Design::AvatarMeleeDamage(
          avatar_->level,
          min.wholePart(),
          max.wholePart(),
          avatar_->attribute_values[Avatar::ATTRIBUTE_STRENGTH],
          avatar_->class_);
    if (avatar_->equipment.projectile) {
      if (!magic->spin_attack.allow_projectiles) {
        magic_.magic = NULL;
        break;
      }
      const Evidyon::Projectile::Projectile* data
        = avatar_->equipment.projectile;
      for (int i = 0; i < magic_.next_targeted_actor; ++i) {
        ActorInstance* target = magic_.targeted_actors[i].dereference();
        if (!target || !my_actor->canSee(target)) {
          continue;
        }
        // this is a ranged weapon--create the projectile
        PlayerProjectileInstance* projectile
          = acquireProjectileInstance(data);
        if (!projectile) { magic_.magic = NULL; break; }
        float angle = my_actor->angleTo(target);
        projectile->event_id = GenerateEventID();
        projectile->x = movement_.x;
        projectile->z = movement_.z;
        projectile->dx = cos(angle) * data->speed;
        projectile->dz = sin(angle) * data->speed;
        projectile->effect.zero();
        projectile->effect.hp_change = -damage;
        // october 15 2009 - steal on every hit
        // october 24 2009 - steal less on every hit
        projectile->effect.hp_steal = stats_->hp_steal / Number(2);
        projectile->effect.mp_steal = stats_->mp_steal / Number(2);
        projectile->effect.magic = false; // this is a melee attack
        projectile->expiration_timer.set(data->duration);
        projectile->homing.target_actor.copy(&magic_.targeted_actors[i]);
        if (target) {
          target->getPosition(&projectile->homing.target_x,
                              &projectile->homing.target_z);
        }
        if (data->piercing) {
          projectile->piercing.next_pierce_hit_timer.trigger();
        } else {
          projectile->piercing.next_pierce_hit_timer.reset();
        }
        if (data->piercing || data->homing) {
          assert(false && "piercing/homing projectiles are non-functional");
        } else {
          my_actor->broadcastEvent_GenerateBasicProjectile(
            projectile->event_id,
            data->event_index,
            Number(angle),
            movement_.x,
            movement_.z);
          todo("karl", "angle should be a Number, not a float");
        }
      }
    } else {
      if (!magic->spin_attack.allow_melee) {
        magic_.magic = NULL;
        break;
      }
      // this is a melee weapon
      for (int i = 0; i < magic_.next_targeted_actor; ++i) {
        ActorInstance* target = magic_.targeted_actors[i].dereference();
        if (!target || my_actor->actorIsOutOfRange(target)) {
          continue;
        }
        Number damage_dealt = target->changeHP(my_actor, false, -damage);
        //if (i == 0) { // steal on the first hit
        // oct 14 2009 - steal on every hit
          client_->changeHP(NULL, true, -attack_.effect.hp_steal * damage_dealt);
          client_->changeMP(-attack_.effect.mp_steal * damage_dealt);
        //}
      }
    }
    magic_.magic = NULL;
    } break;



  //----[  ATTACK_RUN  ]-------------------------------------------------------
  case Magic::Magic::ATTACK_RUN: { 
    assert(false);
    magic_.magic = NULL;
    } break;



  //----[  LEAP  ]-------------------------------------------------------------
  case Magic::Magic::LEAP: {
    assert(false);
    magic_.magic = NULL;
    } break;



  //----[  DIRECT_DAMAGE  ]------------------------------------------------------
  case Magic::Magic::DIRECT_DAMAGE: {
    ActorInstance* target = magic_.targeted_actors[0].dereference();
    if (!target ||
        target->isDead() ||
        my_actor->actorIsOutOfRange(target) ||
        !my_actor->canSee(target)) {
      magic_.magic = NULL;
      break;
    }
    const Magic::Magic_DirectDamage* data = &magic->direct_damage;
    Number damage
      = Design::AvatarSpellDamage(
          magic_.casting_level,
          data->dps_to_damage_multiplier,
          avatar_->attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE]);
    Number damage_dealt
      = target->changeHP(my_actor, true, -damage);
    client_->changeHP(NULL, true, -attack_.effect.hp_steal * damage_dealt);
    client_->changeMP(-attack_.effect.mp_steal * damage_dealt);
    magic_.magic = NULL;
    } break;

  //----[  BUFF  ]-------------------------------------------------------------
  case Magic::Magic::BUFF: {
    ActorInstance* target = magic_.targeted_actors[0].dereference();
    if (!target ||
        target->isDead() ||
        my_actor->actorIsOutOfRange(target) ||
        !my_actor->canSee(target)) {
      magic_.magic = NULL;
      break;
    }
    bool create_effect = false;
    Client* client = target->getController()->getClient();
    if (!client) { magic_.magic = NULL; break; }
    
    if (client->applyBuff(&magic->buff, avatar_)) {
      my_actor->broadcastEvent_GenerateTargetedSpecialFX(
        GenerateEventID(),
        magic->buff.event_index,
        my_actor->getIDNumber(),
        target->getIDNumber());
    }

    } break;
  


  //----[  AOE_BUFF  ]---------------------------------------------------------
  case Magic::Magic::AOE_BUFF: {
    for (int i = 0; i < magic_.next_targeted_actor; ++i) {
      ActorInstance* target = magic_.targeted_actors[0].dereference();
      Client* target_client;
      if (!target ||
          my_actor->actorIsOutOfRange(target) ||
          !my_actor->canSee(target)) {
        continue;
      }
      target_client = target->getController()->getClient();
      if (target_client) {
        target_client->applyBuff(&magic->aoe_buff.buff, avatar_);
      }
    }
    magic_.magic = NULL;
    } break;
  


  //----[  DOT  ]--------------------------------------------------------------
  case Magic::Magic::DOT: {
    ActorInstance* target = magic_.targeted_actors[0].dereference();
    const Magic::Magic_DOT* data = &magic->dot;
    if (target) {
      PlayerDOTInstance* dot
          = acquireDOTInstance(&magic->dot);
      assert(dot);
      dot->data = data;
      dot->target_hp_change
        = -Design::AvatarSpellDamage(
            magic_.casting_level,
            data->dps_to_damage_multiplier,
            avatar_->attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE]);
      dot->target.copy(&magic_.targeted_actors[0]);
      dot->next_hit_timer.set(data->period);
      dot->expiration_timer.set(data->duration);
      dot->event_id = GenerateEventID();
      target->broadcastEvent_GenerateTargetedSpecialFX(
        dot->event_id,
        data->event_index,
        my_actor->getIDNumber(),
        target->getIDNumber());
    }
    magic_.magic = NULL;
    } break;



  //----[  DOT_AOE_BUFF  ]-----------------------------------------------------
  case Magic::Magic::DOT_AOE_BUFF: {
    assert(false);
    magic_.magic = NULL;
    } break;
  


  //----[  PROJECTILE  ]-------------------------------------------------------
  case Magic::Magic::PROJECTILE: {
    const Magic::Magic_Projectile* magic_data = &magic->projectile;
    const Evidyon::Projectile::Projectile* data
      = &magic_data->projectile;
    if (magic_data->use_weapon_projectile) {
      data = avatar_->equipment.projectile;
      if (!data) { magic_.magic = NULL; break; }
    }
    float angle_between_projectiles
      = 2 * magic_data->half_spread / magic_data->projectiles_per_stage;
    float next_projectile_angle
      = my_actor->getDirection() 
          + randf() * angle_between_projectiles - magic_data->half_spread;
    ActorInstance* target = magic_.targeted_actors[0].dereference();
    for (int i = 0; i < magic_data->projectiles_per_stage; ++i) {
      // create the projectile
      PlayerProjectileInstance* projectile
        = acquireProjectileInstance(&magic_data->projectile);
      if (!projectile) { magic_.magic = NULL; break; }
      projectile->event_id = GenerateEventID();
      projectile->x = movement_.x;
      projectile->z = movement_.z;
      projectile->dx = cos(next_projectile_angle) * data->speed;
      projectile->dz = sin(next_projectile_angle) * data->speed;
      //projectile->dx = FixedPoint::cos(my_actor->getDirection())
      //projectile->dz = FixedPoint::sin(my_actor->getDirection())
      projectile->effect.zero();
      Number damage;
      if (magic_data->use_weapon_projectile) {
        damage =
          Design::AvatarMeleeDamage(
            magic_.casting_level,
            stats_->melee_min_damage.wholePart(),
            stats_->melee_max_damage.wholePart(),
            avatar_->attribute_values[Avatar::ATTRIBUTE_STRENGTH],
            avatar_->class_);
        damage *= magic_data->dps_to_damage_multiplier;
        projectile->effect.magic = false;
      } else {
        damage =
          Design::AvatarSpellDamage(
            magic_.casting_level,
            magic_data->dps_to_damage_multiplier,
            avatar_->attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE]);
        projectile->effect.magic = true;
      }
      projectile->effect.hp_change = -damage;
      // october 15 2009 - steal on every hit
      // october 24 2009 - steal less on every hit
      projectile->effect.hp_steal = stats_->hp_steal / Number(2);
      projectile->effect.mp_steal = stats_->mp_steal / Number(2);
      projectile->expiration_timer.set(data->duration);
      projectile->homing.target_actor.copy(&magic_.targeted_actors[0]);
      if (target) {
        target->getPosition(&projectile->homing.target_x,
                            &projectile->homing.target_z);
      }
      if (data->piercing) {
        projectile->piercing.next_pierce_hit_timer.trigger();
      } else {
        projectile->piercing.next_pierce_hit_timer.reset();
      }
      if (data->piercing || data->homing) {
        assert(false && "piercing/homing projectiles are non-functional");
      } else {
        my_actor->broadcastEvent_GenerateBasicProjectile(
          projectile->event_id,
          data->event_index,
          Number(next_projectile_angle),
          movement_.x,
          movement_.z);
        todo("karl", "projectile angle should be a Number");
      }
      next_projectile_angle += angle_between_projectiles;
    }

    if (magic_.stage + 1 >= magic_data->number_of_stages) {
      magic_.magic = NULL;
    } else {
      Time::Milliseconds delay = magic_data->stage_delay;
      magic_.increment_stage_timer.set(delay);
    }
    } break;


  //----[  BRANCHING  ]--------------------------------------------------------
  case Magic::Magic::BRANCHING: {
    } break;
  


  //----[  PORTAL  ]-----------------------------------------------------------
  case Magic::Magic::PORTAL: {
    // be sure the source is valid
    if (my_actor->locationIsOutOfRange(magic_.target_x, magic_.target_z)) {
      break;
    }
    const Magic::Magic_Portal* magic_data = &magic->portal;

    Time::Milliseconds delay = magic_data->open_delay;

    PlayerPortalInstance* portal = acquirePortalInstance(magic_data);
    if (!portal) {
      magic_.magic = NULL;
      return; // can't create any more portals
    }
    portal->source_event_id = GenerateEventID();
    portal->target_event_id = GenerateEventID();
    WorldRegion* source_region;
    Map* target_map = 0;
    WorldRegion* target_region = 0;
    Number source_x, source_z, target_x, target_z;
    bool open_portal = true;
    // the lazy-and-fast way of finding the source region (no lookups based
    // on coordinates) could cause problems if the regions are too small
    source_region = my_actor->getRegion();
    switch (magic_data->type) {
    case Magic::Magic_Portal::SELF_ONLY_TO_SPAWN_POINT: {
      // this toggles between the spawn point and the guild's home
      // point depending on which is closer.
      World::MapIndex map_index;
      float x, z;
      GlobalGamefileManager::singleton()->getPlayerRespawnLocation(
        client_->isEvil(), &map_index, &x, &z);
      //Number home_x, home_z;
      GuildInstance* guild = client_->getGuild();
      target_map = NULL;
      //if (guild && guild->goHome(&target_map, &home_x, &home_z)) {
      //  if (my_actor->getMapIndex() != target_map->getIndex() ||
      //      my_actor->locationIsOutOfRange(home_x, home_z)) {
      //    // go to the guild home
      //    target_x = home_x;
      //    target_z = home_z;
      //  } else {
      //    // go to the spawn point
      //    target_map = NULL;
      //  }
      //}
      if (!target_map) {
        target_map
          = GlobalWorldManager::singleton()->getMapFromIndex(map_index);
        target_x = x;
        target_z = z;
      }
      target_region // get the target region for the spawn point
        = target_map
          ? target_map->acquireRegionFromPoint(target_x.wholePart(),
                                               target_z.wholePart())
          : NULL;
      } break;
    case Magic::Magic_Portal::OPEN_PORTAL_MARK1:
    case Magic::Magic_Portal::SELF_ONLY_MARK1:
      magic_.parameter = 1; // force mark 1
    case Magic::Magic_Portal::OPEN_PORTAL:
    case Magic::Magic_Portal::SELF_ONLY:
      source_x = magic_.target_x; // meaningless for self-portals,
      source_z = magic_.target_z; // but setting these values can't hurt
      client_->getPortalMark(magic_.parameter,
                             &target_map,
                             &target_x,
                             &target_z);
      target_region // get the target region for this mark
        = target_map
          ? target_map->acquireRegionFromPoint(target_x.wholePart(),
                                               target_z.wholePart())
          : NULL;
      break;

    case Magic::Magic_Portal::DIMENSION_DOOR:
      // from caster's location to clicked point
      my_actor->getPosition(&source_x,
                            &source_z);
      target_x = magic_.target_x;
      target_z = magic_.target_z;
      target_region = my_actor->getRegion();
      target_map = target_region->getMap();
      open_portal
        = source_region->getMap()->isValidTeleport(
            source_x,
            source_z,
            target_x,
            target_z);
      break;

    case Magic::Magic_Portal::REVERSE_DIMENSION_DOOR:
      // from clicked point to caster's location
      my_actor->getPosition(&target_x,
                            &target_z);
      target_region = my_actor->getRegion();
      target_map = target_region->getMap();
      source_x = magic_.target_x;
      source_z = magic_.target_z;
      open_portal
        = source_region->getMap()->isValidTeleport(
            source_x,
            source_z,
            target_x,
            target_z);
      break;

    case Magic::Magic_Portal::OPEN_PORTAL_TO_DESTINATION: 
      // clarification: the magic "target" is where the user clicked
      // to open the portal, not where the portal is going
      source_x = magic_.target_x;
      source_z = magic_.target_z;
      target_map = GlobalWorldManager::singleton()->getMapFromIndex(magic_data->target_map);
      target_x = magic_data->target_x;
      target_z = magic_data->target_z;
      target_region // get the target region for this destination
        = target_map
          ? target_map->acquireRegionFromPoint(target_x.wholePart(),
                                               target_z.wholePart())
          : NULL;
      break;
    }

    // be sure that this portal can actually be created on this location
    switch (magic_data->type) {
    case Magic::Magic_Portal::OPEN_PORTAL:
    case Magic::Magic_Portal::OPEN_PORTAL_MARK1:
    case Magic::Magic_Portal::OPEN_PORTAL_TO_DESTINATION:
    case Magic::Magic_Portal::DIMENSION_DOOR:
      if (!source_region->getMap()->canCreatePortalAt(
            source_x, source_z)) {
        // disable this casting entirely
        magic_.magic = NULL;
        releasePortalInstance(portal);
        return;
      }
      break;
    }

    // create the effects
    if (open_portal && (!target_map || !target_region)) {
      assert(magic_data->type != Magic::Magic_Portal::DIMENSION_DOOR);
      assert(magic_data->type != Magic::Magic_Portal::REVERSE_DIMENSION_DOOR);
      // this portal's destination is a place that doesn't exist anymore, so
      // the portal will now be moved to chaos.
      client_->changePortalMarkToChaos(magic_.parameter,
                                       &target_map,
                                       &target_x,
                                       &target_z);

      // create the chaos version of the portal
      assert(source_region);
      source_region->broadcastEvent_GenerateTargetedSpecialFX(
        portal->source_event_id,
        magic_data->chaos_event_index,
        my_actor->getIDNumber(),
        magic_.target_x,
        magic_.target_z,
        NULL,
        0);

      // since we just got this portal's map, it should always be valid
      assert(target_map);
      assert(target_map->validatePortalDestination(target_x,
                                                   target_z));
      target_region
        =  target_map->acquireRegionFromPoint(
              target_x.wholePart(),
              target_z.wholePart());

      // don't actually create this portal right now--it's
      // to chaos, and we don't want a self-only port to a chaos
      // mark teleporting someone there the first time they use it
      open_portal = false;

    } else {
      assert(source_region);
      source_region->broadcastEvent_GenerateTargetedSpecialFX(
        portal->source_event_id,
        magic_data->source_event_index,
        my_actor->getIDNumber(),
        source_x,
        source_z,
        NULL,
        0);

      // Portals with an invalid destination that is not due to a chaos
      // map are still created.  This is by design; that way, dimension doors
      // to places inside of walls, etc. show up correctly but don't function
      open_portal =
        target_map && // this is possibly invalid
        target_map->validatePortalDestination(target_x,
                                              target_z);
    }
    if (open_portal) {
      assert(target_map);
      if (target_region) {
        // it's not necessary to have a target region yet--that just means
        // that there are no people there to see the target effect.
        target_region->broadcastEvent_GenerateTargetedSpecialFX(
          portal->target_event_id,
          magic_data->target_event_index,
          target_x,
          target_z,
          target_x,
          target_z,
          NULL,
          0);
      }
      portal->source_x = source_x;
      portal->source_z = source_z;
      portal->target_map = target_map;
      portal->target_region = target_region;
      portal->target_x = target_x;
      portal->target_z = target_z;
      portal->portal_monsters = source_region->getMap() == target_map;
      // portal->enable(); // done when acquired
      portal->close_timer.set(magic_data->close_delay);
    } else {
      // invalidate this portal so that it's destroyed when it opens
      portal->target_map = NULL;
      portal->target_region = NULL;
      portal->close_timer.trigger();
    }

    // The open timer and source region are always set, since we have created
    // a casting effect on the client at the source.  This allows invalid portals
    // to be erased when the portal would normally have opened.
    portal->source_region = source_region;
    portal->open_timer.set(magic_data->open_delay);

    magic_.magic = NULL;
    } break;
  

  //----[  TELEPORT  ]---------------------------------------------------------
  case Magic::Magic::TELEPORT: {
    if (my_actor->locationIsOutOfRange(magic_.target_x, magic_.target_z) ||
        !my_actor->getRegion()->getMap()->isValidTeleport(my_actor,
                                                         magic_.target_x.toFloat(),
                                                         magic_.target_z.toFloat())) {
      magic_.magic = NULL;
      break;
    }
    my_actor->faceLocation(magic_.target_x, magic_.target_z);
    my_actor->teleportTo(magic_.target_x.toFloat(), magic_.target_z.toFloat());
    magic_.magic = NULL;
    
    } break;



  //----[  MUTATE  ]-----------------------------------------------------------
  case Magic::Magic::MUTATE: {
    // TODO: fill this in
    magic_.magic = NULL;
    } break;
  


  //----[  SUMMON  ]-----------------------------------------------------------
  case Magic::Magic::SUMMON: {
    // TODO: fill this in
    magic_.magic = NULL;
    } break;
  


  //----[  TRAP_DIMENSION_DOOR  ]----------------------------------------------
  case Magic::Magic::TRAP_DIMENSION_DOOR: {
    // TODO: fill this in
    magic_.magic = NULL;
    } break;
  


  //----[  TRAP_BLACK_HOLE  ]--------------------------------------------------
  case Magic::Magic::TRAP_BLACK_HOLE: {
    // TODO: fill this in
    magic_.magic = NULL;
    } break;
  


  //----[  DISENCHANT  ]-------------------------------------------------------
  case Magic::Magic::DISENCHANT: {
    // TODO: fill this in
    magic_.magic = NULL;
    } break;
  


  //----[  INVISIBILITY  ]-----------------------------------------------------
  case Magic::Magic::INVISIBILITY: {
    // TODO: fill this in
    magic_.magic = NULL;
    } break;
  

  //----[  HEAL  ]-------------------------------------------------------------
  case Magic::Magic::HEAL: {
    const Magic::Magic_Heal* data = &magic->heal;
    ActorInstance* target = magic_.targeted_actors[0].dereference();
    if (!target ||
        (target->isDead() && !data->resurrects) ||
        my_actor->actorIsOutOfRange(target) ||
        !my_actor->canSee(target)) {
      break;
    }
    Client* target_client = target->getController()->getClient();
    assert(target_client); // checked by casting algorithm
    Number this_hp_restored = Number(data->hp_restored_percent * stats_->max_hp.toFloat());
    Number target_hp_restored
      = Number(data->hp_restored_percent * target_client->getAvatarStats()->max_hp.toFloat());
    Number amount_to_heal = FixedPoint::minfast(this_hp_restored,
                                                target_hp_restored);
    //amount_to_heal = avatar_->calculateAbilityEffect(
    //  Avatar::ATTRIBUTE_WISDOM,
    //  Avatar::ATTRIBUTE_WISDOM,
    //  amount_to_heal,
    //  amount_to_heal);
    target->changeHP(my_actor, true, amount_to_heal);
    if (data->resurrects) target_client->resurrect();
    magic_.magic = NULL;
    } break;


  //----[  ENCHANTMENT_ARMOR_0  ]----------------------------------------------
  case Magic::Magic::ENCHANTMENT_ARMOR_0: {
    assert(false);
    } break;
  


  //----[  ENCHANTMENT_ARMOR_1  ]----------------------------------------------
  case Magic::Magic::ENCHANTMENT_ARMOR_1: {
    assert(false);
    } break;
  


  //----[  ENCHANTMENT_ARMOR_2  ]----------------------------------------------
  case Magic::Magic::ENCHANTMENT_ARMOR_2: {
    assert(false);
    } break;
  


  //----[  ENCHANTMENT_SHIELD  ]-----------------------------------------------
  case Magic::Magic::ENCHANTMENT_SHIELD: {
    todo("karl","magic shield enchantment");
    } break;
  


  //----[  ENCHANTMENT_REACTIVE_HEALING  ]-------------------------------------
  case Magic::Magic::ENCHANTMENT_REACTIVE_HEALING: {
    todo("karl","reactive healing enchantment");
    } break;
  


  //----[  ENCHANTMENT_THORNS  ]-----------------------------------------------
  case Magic::Magic::ENCHANTMENT_THORNS: {
    todo("karl","thorns enchantment");
    } break;
  }

  // Move to the next stage
  magic_.stage++;
}




//----[  processSpeakRequest  ]------------------------------------------------
void Player::processSpeakRequest(bool allow_state_change) {
  assert(request_->type == PlayerRequest::SPEAK);
  if (mutation_.can_speak) {
    if (allow_state_change) {
      if (_stricmp(request_->speak.text.characters, "trade") == 0) {
        stateChange(FINDING_TRADE);
      }
    }
    actor_.dereferenceAssumingValid()->say(request_->speak.text.characters);
  }
  request_->reset();
}


//----[  processMoveRequest  ]-------------------------------------------------
void Player::processMoveRequest() {
  assert(request_->type == PlayerRequest::MOVE);
  request_->reset();
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();

  // Get the actor's location from the actor structure--it is authoritative.
  Number actor_x, actor_z;
  my_actor->getPosition(&actor_x, &actor_z);
  Number offset_x(request_->move.x),
         offset_z(request_->move.z);
  offset_x -= actor_x;
  offset_z -= actor_z;

  // Discard the request if its distance is too large.  This ensures that
  // the below logic doesn't mess up.
  if (offset_x > 10 || offset_z < -10 ||
      offset_z > 10 || offset_z < -10) {
    return;
  }

  // Calculate distance from here -> target
  Number distance;
  distance.int_value
    = FixedPoint::fixmul<16>(offset_x.int_value, offset_x.int_value)
    + FixedPoint::fixmul<16>(offset_z.int_value, offset_z.int_value);
  distance = FixedPoint::sqrt(distance);
  if (distance < Number(0.1)) return;


  // Make the actor face this location
  my_actor->setDirection(FixedPoint::atan2(offset_z, offset_x).toFloat());
  my_actor->setMovingFlag(true);

  // Calculate the rate at which to move on each axis and the time at which
  // to stop moving.
  movement_.dx.int_value
    = FixedPoint::fixmul<16>(stats_->movement_speed.int_value,
                              (offset_x / distance).int_value);
  movement_.dz.int_value
    = FixedPoint::fixmul<16>(stats_->movement_speed.int_value,
                             (offset_z / distance).int_value);
  movement_.stop_timer.set(
    (1000 * distance / stats_->movement_speed).wholePart());

  // Select the new state
  if (mutation_.flying) stateChange(FLYING);
  else if (movement_.swimming) stateChange(SWIMMING);
  else stateChange(WALKING);
}



//----[  processAttackRequest  ]-----------------------------------------------
void Player::processAttackRequest() {
  assert(request_->type == PlayerRequest::ATTACK);
  if (movement_.swimming || !mutation_.can_melee_attack) return;
  request_->reset();

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  ActorPointer new_target;
  if (my_actor->getRegion()->getNearbyActor(request_->attack.actor_id,
                                            &new_target)) {
    State required_state
      = avatar_->equipment.projectile ? RANGED_ATTACK : MELEE_ATTACK;
    if (required_state != state_ ||
        !new_target.equals(attack_.target)) {
      // the user actually picked a new state
      attack_.target.copy(new_target);
      stateChangeAvatarAttackType();
    } else {
      // ignore the move request that comes with this attack
      request_->reset(Server::PlayerRequest::MOVE);
    }
  }
}



//----[  processUseItemRequest  ]----------------------------------------------
void Player::processUseItemRequest() {
  assert(request_->type == PlayerRequest::USE_ITEM);
  if (next_action_timer_.expired() == false) return;
  request_->reset();

  Magic::MagicIndex magic_index;
  if (!items_->consumeItem(request_->use_item.inventory_index, &magic_index)) {
    return;
  }

  const Magic::Magic* magic
    = GlobalGamefileManager::singleton()->getMagic(magic_index);
  if (!magic) return; // confirm(magic) else return;

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  WorldRegion* region = my_actor->getRegion();
  ActorPointer target_pointer;
  region->getNearbyActor(request_->use_item.actor_id, &target_pointer);

  initiateUseMagic(magic,
                   avatar_->level,
                   Actor::ACTORACTION_CAST_TARGETED_SPELL,
                   USE_ITEM_REPEAT_DELAY,
                   0,
                   Number(request_->use_item.x),
                   Number(request_->use_item.z),
                   request_->use_item.parameter,
                   my_actor,
                   target_pointer,
                   true);
}





//----[  processCastRequest  ]-------------------------------------------------
void Player::processCastRequest() {
  assert(request_->type == PlayerRequest::CAST);
  if (next_action_timer_.expired() == false) {

    // only reset the cast request if we aren't leaping--this ensures that
    // players leaping repeatedly over a pit don't fall in
    if (state_ != LEAP &&
        state_ != RANGED_ATTACK &&
        state_ != MELEE_ATTACK) request_->reset();

    return;
  }
  request_->reset();
  assert(request_->cast.spell); // null spells should be ignored by client

  // check to see if the client has enough MP to cast the spell
  request_->reset();
  if (request_->cast.spell == NULL ||
      stats_->mp < request_->cast.spell->description.mp_cost ||
      !mutation_.can_cast_spells) return;

  const Evidyon::SpellDescription* spell = request_->cast.spell;
  assert(spell);

  // Get the magic to use for this spell.  Look backward so that we
  // find the best one this avatar is able to use.
  const Magic::Magic* magic = NULL;
  int stage = spell->description.number_of_stages;
  Avatar::AvatarLevel casting_level = 0;
  assert(stage);
  if (client_->isAdmin()) {
    // hack to make admin chars cast any spell
    stage = 0;
    magic = spell->magic[0];
    casting_level = avatar_->level;
  } else {
    while (stage > 0) {
      --stage;
      if (spell->description.stages[stage].level_obtained[avatar_->class_] <= avatar_->level) {
        magic = spell->magic[stage];
        casting_level 
          = spell->description.stages[stage].improvement_level_limit;
        break;
      }
    }
    if (casting_level > avatar_->level) {
      casting_level = avatar_->level;
    }
  }

  // should not be configured to use this spell at this level
  if (!magic) return;

  // don't allow casting while swimming unless it's a leap spell
  // (leaping will be checked inside)
  if (movement_.swimming && (magic->type != Magic::Magic::LEAP)) return;

  // check spin attacks
  if (magic->type == Magic::Magic::SPIN_ATTACK) {
    if ((avatar_->equipment.projectile &&
        !magic->spin_attack.allow_projectiles) ||
        (!avatar_->equipment.projectile &&
        !magic->spin_attack.allow_melee)) {
      // can't use a spin attack with a weapon of this type
      return;
    }
  }

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  WorldRegion* region = my_actor->getRegion();

  // this should usually never happen (since the distance is checked in the
  // cast message) but when it does, it's because the player cast a spell at max
  // range while running the opposite direction (possibly a bit of lag too)
  //assert(!my_actor->locationIsOutOfRange(request_->cast.x, request_->cast.z));

  ActorPointer target_pointer;
  region->getNearbyActor(request_->cast.actor_id, &target_pointer);


  Time::Milliseconds action_delay
    = Design::AvatarSpellDelay(
        casting_level,
        spell->description.stages[stage].action_delay,
        avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

  initiateUseMagic(magic,
                   casting_level,
                   spell->description.casting_action,
                   action_delay,
                   spell->description.mp_cost,
                   Number(request_->cast.x),
                   Number(request_->cast.z),
                   request_->cast.parameter,
                   my_actor,
                   target_pointer,
                   true);
}






//----[  initiateUseMagic  ]---------------------------------------------------
void Player::initiateUseMagic(const Evidyon::Magic::Magic* magic,
                              Avatar::AvatarLevel casting_level,
                              Actor::ActorAction cast_action,
                              Time::Milliseconds repeat_delay,
                              Number mp_cost,
                              Number x,
                              Number z,
                              unsigned int parameter,
                              ActorInstance* my_actor,
                              ActorPointer target_pointer,
                              bool allow_state_change) {

  // check to see if the client has enough MP and geonite to use the magic
  if (stats_->mp < mp_cost) return;

  magic_.magic = magic;
  magic_.parameter = parameter;
  magic_.stage = 0;
  magic_.casting_level = casting_level;
  //magic_.target_x = x;
  //magic_.target_z = z;

  WorldRegion* region = my_actor->getRegion();

  // this should never happen, since the distance is checked in the
  // network messages
  //assert(!my_actor->locationIsOutOfRange(x, z));

  ActorInstance* target = target_pointer.dereference();
  if (target && my_actor->actorIsOutOfRange(target)) return;

  magic_.stage = 0;
  magic_.next_targeted_actor = 0;
//  magic_.increment_stage_timer.reset();

  // This is where all the magic (get it?) happens.
  switch (magic->type) {
  default: assert(false && "unknown magic type"); break;



  //----[  AOE  ]--------------------------------------------------------------
  case Magic::Magic::AOE: {
    const Magic::Magic_AOE* data = &magic->aoe;
    if (!findAOETargets(&magic->aoe.data,
                        region,
                        target,
                        x,
                        z,
                        magic_.targeted_actors,
                        &magic_.next_targeted_actor,
                        data->data.event_index)) {
      break;
    }

    // no targets? cancel this casting
    if (magic_.next_targeted_actor == 0) return;


    Time::Milliseconds traveling_delay
      = Design::AvatarSpellDelay(
          casting_level,
          data->effect_delay,
          avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

    // we found targets, so enter the next stage--damage!
    //magic_.target_x = effect_x;
    //magic_.target_z = effect_z;
    if (!allow_state_change || traveling_delay == 0) {
      incrementMagicStage();
    } else {
      stateChange(USING_MAGIC); // resets increment timer
      magic_.increment_stage_timer.set(traveling_delay);
    }
    } break;



  //----[  SPIN_ATTACK  ]------------------------------------------------------
  case Magic::Magic::SPIN_ATTACK: {
    if (!mutation_.can_melee_attack) break;

    const Magic::Magic_SpinAttack* data = &magic->spin_attack;
    Number effect_x, effect_z;
    my_actor->getPosition(&effect_x, &effect_z);
    if (my_actor->locationIsOutOfRange(effect_x, effect_z)) {
      break;
    }

    // Find targets near this actor
    assert(data->max_targets <= Magic::MAX_TARGETS);
    region->findSpinAttackTargets(
      my_actor,
      avatar_->equipment.melee_attack_distance,
      attacking_players_, // should we find client targets?
      magic_.targeted_actors,
      data->max_targets,
      &magic_.next_targeted_actor);

    Actor::ActorID targets[Magic::MAX_TARGETS];
    for (int i = 0; i < magic_.next_targeted_actor; ++i) {
      targets[i]
       = magic_.targeted_actors[i].dereferenceAssumingValid()->getIDNumber();
    }

    // create the AOE's effect
    my_actor->broadcastEvent_GenerateTargetedSpecialFX(
      GenerateEventID(),
      data->event_index,
      my_actor->getIDNumber(),
      effect_x,
      effect_z,
      targets,
      magic_.next_targeted_actor);

    // set the action delay
    Time::Milliseconds traveling_delay
      = Design::AvatarSpellDelay(
          casting_level,
          data->effect_delay,
          avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

    cast_action = Actor::ACTORACTION_SPIN_ATTACK; // set at the end of switch()

    repeat_delay += stats_->melee_repeat_delay;

    if (magic_.next_targeted_actor > 0) {
      // we found targets, so enter the next stage
      magic_.target_x = effect_x;
      magic_.target_z = effect_z;
      if (!allow_state_change || traveling_delay == 0) { 
        incrementMagicStage();
      } else {
        stateChange(USING_MAGIC); // resets increment timer
        magic_.increment_stage_timer.set(traveling_delay);
      }
    }

    } break;



  //----[  ATTACK_RUN  ]-------------------------------------------------------
  case Magic::Magic::ATTACK_RUN: {
    if (!allow_state_change || !mutation_.can_melee_attack) break;

    const Magic::Magic_AttackRun* data = &magic->attack_run;
    attack_.target.copy(&target_pointer);
    Number target_x, target_z;

    attack_run_.speed
      = Number(stats_->movement_speed * data->speed_factor);
    attack_run_.first_hit_damage_bonus = data->first_hit_damage_bonus;

    // If the target is non-null, the attack run state constantly searches
    // for the target and moves toward it.  Because of this, we don't have
    // to calculate anything for that state here.  However, if the player
    // is running toward a location, this is essentially a fast movement
    // state so we need to use the timer.
    if (target == NULL) {
      target_x = x;
      target_z = z;
      Number actor_x, actor_z;
      ActorInstance* my_actor = actor_.dereferenceAssumingValid();
      my_actor->getPosition(&actor_x, &actor_z);
      Number offset_x = target_x - actor_x;
      Number offset_z = target_z - actor_z;

      // Calculate distance from here -> target
      Number distance;
      distance.int_value
        = FixedPoint::fixmul<16>(offset_x.int_value, offset_x.int_value)
        + FixedPoint::fixmul<16>(offset_z.int_value, offset_z.int_value);
      distance = FixedPoint::sqrt(distance);

      // Make the actor face this location
      my_actor->setDirection(FixedPoint::atan2(offset_z, offset_x).toFloat());
      my_actor->setMovingFlag(true);

      // Calculate the rate at which to move on each axis and the time at which
      // to stop moving.
      movement_.dx.int_value
        = FixedPoint::fixmul<16>(attack_run_.speed.int_value,
                                  (offset_x / distance).int_value);
      movement_.dz.int_value
        = FixedPoint::fixmul<16>(attack_run_.speed.int_value,
                                 (offset_z / distance).int_value);
      movement_.stop_timer.set(
        (1000 * distance / attack_run_.speed).wholePart());
    } else {
      movement_.stop_timer.trigger(); // ensure compliance in updateAttackRun
    }

    my_actor->broadcastEvent_GenerateTargetedSpecialFX(
      GenerateEventID(),
      data->event_index,
      my_actor->getIDNumber(),
      my_actor->getIDNumber());

    // Move to the attack-run state and reset the spell.  The attack run
    // never needs to be incremented.
    stateChange(ATTACK_RUN);
    cast_action = Actor::ACTORACTION_NONE;
    magic_.magic = NULL;

    } break;



  //----[  LEAP  ]-------------------------------------------------------------
  case Magic::Magic::LEAP: {
    if (!allow_state_change ||
        !mutation_.can_melee_attack) break;
    const Magic::Magic_Leap* data = &magic->leap;

    // We can keep leaping even if we're over a pit as long as the player
    // doesn't stop
    bool is_second_leap = false;
    if (falling_.falling || movement_.swimming) {
      if (state_ != LEAP) break;

      // leap event is played when it's being re-used
      my_actor->broadcastEvent_GenerateTargetedSpecialFX(
        GenerateEventID(),
        data->event_index,
        my_actor->getIDNumber(),
        my_actor->getIDNumber());

      // second leaps go the max distance, not to a target
      is_second_leap = true;
    }


    Number target_x = x, target_z = z;

    // validate the target location
    const MapLocationInfo* dest_location
      = region->getMap()->getMapLocationInfo(target_x.wholePart(),
                                             target_z.wholePart());
    if (!dest_location || !dest_location->can_move_over) {
      // can't jump into invalid spots
      break;
    }

    attack_.target.copy(&target_pointer);

    leap_.speed
      = Number(stats_->movement_speed * data->speed_factor);
    leap_.first_hit_damage_bonus = data->first_hit_damage_bonus;

    // Always leap at the target location.  Unlike attack run--even though
    // leap has a target actor--we always jump at a location so that the
    // leap doesn't get pulled around by the target's movement.  However,
    // should the target stay still it will get hit by the leap's bonus
    // attack damage.
    // We do reduce the distance by the bounding radii of the two actors
    // so that the player doesn't leap *inside* of the target actor.
    Number actor_x, actor_z;
    my_actor->getPosition(&actor_x, &actor_z);
    Number offset_x = target_x - actor_x;
    Number offset_z = target_z - actor_z;
    Number distance;
    distance.int_value
      = FixedPoint::fixmul<16>(offset_x.int_value, offset_x.int_value)
      + FixedPoint::fixmul<16>(offset_z.int_value, offset_z.int_value);
    distance = FixedPoint::sqrt(distance);
    distance -= target ? Number(my_actor->getSize() + target->getSize()) : Number(0);
    my_actor->setDirection(FixedPoint::atan2(offset_z, offset_x).toFloat());
    my_actor->setMovingFlag(true);
    movement_.dx.int_value
      = FixedPoint::fixmul<16>(leap_.speed.int_value,
                                (offset_x / distance).int_value);
    movement_.dz.int_value
      = FixedPoint::fixmul<16>(leap_.speed.int_value,
                               (offset_z / distance).int_value);
    if (is_second_leap) {
      distance = data->max_distance;
    } else {
      distance
        = FixedPoint::minfast(distance, Number(data->max_distance));
    }
    Number movement_delay = 1000 * distance / leap_.speed;
    movement_.stop_timer.set(movement_delay.wholePart());

    //my_actor->broadcastEvent_GenerateTargetedSpecialFX(
    //  GenerateEventID(),
    //  data->event_index,
    //  my_actor->getIDNumber(),
    //  my_actor->getIDNumber());

    stateChange(LEAP);

    // the action is set below
    cast_action = Actor::ACTORACTION_LEAP;
    repeat_delay = movement_delay.wholePart();
    magic_.magic = NULL;

    } break;



  //----[  DIRECT_DAMAGE  ]----------------------------------------------------
  case Magic::Magic::DIRECT_DAMAGE: {
    const Magic::Magic_DirectDamage* data = &magic->direct_damage;
    if (target == my_actor ||
        target == NULL) return;
    magic_.targeted_actors[0].copy(&target_pointer);
    my_actor->faceActor(target);
    my_actor->broadcastEvent_GenerateTargetedSpecialFX(
      GenerateEventID(),
      data->event_index,
      my_actor->getIDNumber(),
      target->getIDNumber());

    // get this effect's delay
    Time::Milliseconds traveling_delay
      = Design::AvatarSpellDelay(
          casting_level,
          data->effect_delay,
          avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

    if (!allow_state_change || traveling_delay == 0) {
      incrementMagicStage();
    } else {
      stateChange(USING_MAGIC); // resets increment timer
      magic_.increment_stage_timer.set(traveling_delay);
    }
    } break;


  //----[  BUFF  ]-------------------------------------------------------------
  case Magic::Magic::BUFF: {
    const Magic::Magic_Buff* data = &magic->buff;
    if (data->target_self) {
      target = my_actor;
      target_pointer.copy(client_->getActor());
    }
    if (target == NULL) return;
    magic_.targeted_actors[0].copy(&target_pointer);
    my_actor->faceActor(target);

    // note: we don't create the effect here!  this is so that the effect is
    // only created if it does something to the client--that way people can
    // "feel" when the buff has been fully applied, since it won't create
    // an effect.

    // get this effect's delay
    Time::Milliseconds traveling_delay
      = Design::AvatarSpellDelay(
          casting_level,
          data->effect_delay,
          avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

    if (!allow_state_change || traveling_delay == 0) {
      incrementMagicStage();
    } else {
      stateChange(USING_MAGIC); // resets increment timer
      magic_.increment_stage_timer.set(traveling_delay);
    }
    
    } break;
  


  //----[  AOE_BUFF  ]---------------------------------------------------------
  case Magic::Magic::AOE_BUFF: {
    const Magic::Magic_AOEBuff* data = &magic->aoe_buff;
    if (!findAOETargets(&magic->aoe_buff.data,
                        region,
                        target,
                        x,
                        z,
                        magic_.targeted_actors,
                        &magic_.next_targeted_actor,
                        data->buff.event_index)) {
      break;
    }

    // create the area effect
    my_actor->broadcastEvent_GenerateTargetedSpecialFX(
      GenerateEventID(),
      data->data.event_index,
      my_actor->getIDNumber(),
      x,
      z,
      NULL,
      0);

    // get this effect's delay
    Time::Milliseconds traveling_delay
      = Design::AvatarSpellDelay(
          casting_level,
          data->effect_delay,
          avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

    if (!allow_state_change || traveling_delay == 0) {
      incrementMagicStage();
    } else {
      stateChange(USING_MAGIC); // resets increment timer
      magic_.increment_stage_timer.set(traveling_delay);
    }
    } break;



  //----[  DOT  ]--------------------------------------------------------------
  case Magic::Magic::DOT: {
    const Magic::Magic_DOT* data = &magic_.magic->dot;
    if (target_pointer.invalid()) break;
    magic_.targeted_actors[0].copy(target_pointer);

    // get this effect's delay
    Time::Milliseconds traveling_delay
      = Design::AvatarSpellDelay(
          casting_level,
          data->effect_delay,
          avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

    if (!allow_state_change || traveling_delay == 0) {
      incrementMagicStage();
    } else {
      stateChange(USING_MAGIC);
      magic_.increment_stage_timer.set(traveling_delay);
    }
  } break;



  //----[  DOT_AOE_BUFF  ]-----------------------------------------------------
  case Magic::Magic::DOT_AOE_BUFF: {
    releaseDOTAOEBuff();
    const Magic::Magic_DOTAOEBuff* data = &magic->dot_aoe_buff;
    advance_dotaoebuff_ = true;
    dotaoebuff_.data = data;
    dotaoebuff_.x = x;
    dotaoebuff_.z = z;
    dotaoebuff_.local_region = region;
    dotaoebuff_.enable();
    dotaoebuff_.expiration_timer.set(data->duration);
    // do not trigger the next hit immediately!  this would make players
    // able to use this like a direct AOE by casting it over and over
    dotaoebuff_.next_hit_timer.set(data->period);
    advanceDOTAOEBuff();
    next_action_timer_.set(
      Design::AvatarSpellDelay(
        casting_level,
        data->action_delay,
        avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]));
    } break;


  //----[  PROJECTILE  ]-------------------------------------------------------
  case Magic::Magic::PROJECTILE: {
    const Magic::Magic_Projectile* data = &magic->projectile;
    magic_.target_x = x;
    magic_.target_z = z;
    magic_.targeted_actors[0].copy(target_pointer);
    my_actor->setMovingFlag(false);
    movement_.dx = 0;
    movement_.dz = 0;
    movement_.stop_timer.trigger();
    my_actor->faceLocation(x, z);
    incrementMagicStage();
    if (data->use_weapon_projectile) {
      repeat_delay += stats_->melee_repeat_delay;
    }
    if (allow_state_change && magic_.magic) stateChange(USING_MAGIC);
    } break;
  


  //----[  BRANCHING  ]--------------------------------------------------------
  case Magic::Magic::BRANCHING: {
    } break;
  


  //----[  PORTAL  ]-----------------------------------------------------------
  case Magic::Magic::PORTAL: {
    magic_.target_x = x;
    magic_.target_z = z;
    my_actor->faceLocation(x, z);
    incrementMagicStage();
    if (!allow_state_change && magic_.magic) stateChange(USING_MAGIC);
    } break;
  


  //----[  TELEPORT  ]---------------------------------------------------------
  case Magic::Magic::TELEPORT: {
    const Magic::Magic_Teleport* data = &magic->teleport;
    Number target_x = x,
           target_z = z;
    if (my_actor->locationIsOutOfRange(target_x, target_z)) {
      break;
    }
    my_actor->broadcastEvent_GenerateTargetedSpecialFX(
      GenerateEventID(),
      magic->teleport.event_index,
      my_actor->getIDNumber(),
      target_x,
      target_z,
      NULL, // target actors
      0);   // # of target actors
    magic_.target_x = target_x;
    magic_.target_z = target_z;
    Time::Milliseconds traveling_delay
      = Design::AvatarSpellDelay(
          casting_level,
          data->effect_delay,
          avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

    if (!allow_state_change || traveling_delay == 0) {
      incrementMagicStage();
    } else {
      stateChange(USING_MAGIC); // resets increment timer
      magic_.increment_stage_timer.set(traveling_delay);
    }
    } break;
  


  //----[  MUTATE  ]-----------------------------------------------------------
  case Magic::Magic::MUTATE: {
    } break;
  


  //----[  SUMMON  ]-----------------------------------------------------------
  case Magic::Magic::SUMMON: {
    } break;
  


  //----[  TRAP_DIMENSION_DOOR  ]----------------------------------------------
  case Magic::Magic::TRAP_DIMENSION_DOOR: {
    } break;
  


  //----[  TRAP_BLACK_HOLE  ]--------------------------------------------------
  case Magic::Magic::TRAP_BLACK_HOLE: {
    } break;
  


  //----[  DISENCHANT  ]-------------------------------------------------------
  case Magic::Magic::DISENCHANT: {
    } break;
  


  //----[  INVISIBILITY  ]-----------------------------------------------------
  case Magic::Magic::INVISIBILITY: {
    } break;
  


  //----[  HEAL  ]-------------------------------------------------------------
  case Magic::Magic::HEAL: {
    const Magic::Magic_Heal* data = &magic->heal;
    if (data->target_self) {
      target = my_actor;
      target_pointer.copy(client_->getActor());
    }
    if (target == NULL) return; // no target? exit
    if (!target->getController()->isClient()) return; // not a client? exit
    Number mp_consumed = Number(data->mp_consumed_percent * stats_->max_mp.toFloat());
    if (mp_consumed > stats_->mp) return;
    mp_cost = mp_consumed.wholePart();
    magic_.targeted_actors[0].copy(&target_pointer);
    my_actor->faceActor(target);
    my_actor->broadcastEvent_GenerateTargetedSpecialFX(
      GenerateEventID(),
      data->event_index,
      my_actor->getIDNumber(),
      target->getIDNumber());

    // get this effect's delay
    Time::Milliseconds traveling_delay
      = Design::AvatarSpellDelay(
          casting_level,
          data->effect_delay,
          avatar_->attribute_values[Avatar::ATTRIBUTE_AGILITY]);

    if (!allow_state_change || traveling_delay == 0) {
      incrementMagicStage();
    } else {
      stateChange(USING_MAGIC); // resets increment timer
      magic_.increment_stage_timer.set(traveling_delay);
    }
    } break;
  

  //----[  ENCHANTMENT_ARMOR_0  ]----------------------------------------------
  case Magic::Magic::ENCHANTMENT_ARMOR_0: {
    const Magic::Magic_Enchantment_Armor* data = &magic->enchantment_armor;
    enchantments_->armor_enchantments[0].defense = data->defense;
    enchantments_->armor_enchantments[0].expire_timer.disable();
    enchantments_->armor_enchantments[0].expire_timer.enable(data->duration);
    enchantments_->enchantments[Magic::ENCHANTMENT_ARMOR_0]
      = data->enchantment_type;
    stats_->recalculate_timer.trigger();
    client_->combineUpdateVisibleEnchantmentsFlag(true);
    } break;
  


  //----[  ENCHANTMENT_ARMOR_1  ]----------------------------------------------
  case Magic::Magic::ENCHANTMENT_ARMOR_1: {
    const Magic::Magic_Enchantment_Armor* data = &magic->enchantment_armor;
    enchantments_->armor_enchantments[1].defense = data->defense;
    enchantments_->armor_enchantments[1].expire_timer.disable();
    enchantments_->armor_enchantments[1].expire_timer.enable(data->duration);
    enchantments_->enchantments[Magic::ENCHANTMENT_ARMOR_1]
      = data->enchantment_type;
    stats_->recalculate_timer.trigger();
    client_->combineUpdateVisibleEnchantmentsFlag(true);
    } break;
  


  //----[  ENCHANTMENT_ARMOR_2  ]----------------------------------------------
  case Magic::Magic::ENCHANTMENT_ARMOR_2: {
    const Magic::Magic_Enchantment_Armor* data = &magic->enchantment_armor;
    enchantments_->armor_enchantments[2].defense = data->defense;
    enchantments_->armor_enchantments[2].expire_timer.disable();
    enchantments_->armor_enchantments[2].expire_timer.enable(data->duration);
    enchantments_->enchantments[Magic::ENCHANTMENT_ARMOR_2]
      = data->enchantment_type;
    stats_->recalculate_timer.trigger();
    client_->combineUpdateVisibleEnchantmentsFlag(true);
    } break;
  


  //----[  ENCHANTMENT_SHIELD  ]-----------------------------------------------
  case Magic::Magic::ENCHANTMENT_SHIELD: {
    } break;
  


  //----[  ENCHANTMENT_REACTIVE_HEALING  ]-------------------------------------
  case Magic::Magic::ENCHANTMENT_REACTIVE_HEALING: {
    } break;
  


  //----[  ENCHANTMENT_THORNS  ]-----------------------------------------------
  case Magic::Magic::ENCHANTMENT_THORNS: {
    } break;
  }

  // consume MP and/or geonite
  client_->changeMP(-mp_cost);

  // Delay the next action based on the amount of time it takes to cast this
  next_action_timer_.set(repeat_delay);

  // Set the actor's action
  if (cast_action != Actor::ACTORACTION_NONE) {
    my_actor->setAction(cast_action, repeat_delay);
  }
}



//----[  stateChangeAvatarAttackType  ]----------------------------------------
void Player::stateChangeAvatarAttackType() {
  if (stateChange(avatar_->equipment.projectile ? RANGED_ATTACK : MELEE_ATTACK)) {
    // only reset the timers if the state was modified, so that people clicking
    // repeatedly can keep attacking
    attack_.begin_attack_timer.set(0);
    attack_.end_attack_timer.reset();
  }
  attack_.follow_range = avatar_->equipment.melee_follow_distance;
  attack_.attack_range = avatar_->equipment.melee_attack_distance;
}




//----[  acquireProjectileInstance  ]------------------------------------------
PlayerProjectileInstance* Player::acquireProjectileInstance(
    const Projectile::Projectile* data) {
  advance_projectiles_ = true;
  todo("karl","optimize acquireProjectileInstance");
  for (int i = 0; i < Magic::PROJECTILES_PER_PLAYER; ++i) {
    if (projectiles_[i].data == NULL) {
      projectiles_[i].enable();
      projectiles_[i].data = data;
      return &projectiles_[i];
    }
  }
  return NULL;
}



//----[  releaseProjectileInstance  ]------------------------------------------
void Player::releaseProjectileInstance(PlayerProjectileInstance* projectile) {
  if (projectile->data == NULL) return;
  ActorInstance* my_actor = actor_.dereference();
  WorldRegion* region = my_actor ? my_actor->getRegion() : NULL;
  if (region) region->broadcastEvent_Terminate(projectile->event_id);
  projectile->disable();
}




//----[  acquirePortalInstance  ]----------------------------------------------
PlayerPortalInstance* Player::acquirePortalInstance(
    const Magic::Magic_Portal* data) {
  advance_portals_ = true;
  for (int i = 0; i < Magic::PORTALS_PER_PLAYER; ++i) {
    if (portals_[i].data == NULL) {
      portals_[i].enable();
      portals_[i].data = data;
      return &portals_[i];
    }
  }
  return NULL;
}


//----[  releasePortalInstance  ]----------------------------------------------
void Player::releasePortalInstance(PlayerPortalInstance* portal) {
  if (portal->data == NULL) return;
  if (portal->source_region) {
    portal->source_region->broadcastEvent_Terminate(portal->source_event_id);
  }
  if (portal->target_region) {
    portal->target_region->broadcastEvent_Terminate(portal->target_event_id);
  }
  portal->disable();
}



//----[  releaseSelfOnlyPortals  ]---------------------------------------------
void Player::releaseSelfOnlyPortals() {
  if (!advance_portals_) return;
  for (int i = 0; i < Magic::PORTALS_PER_PLAYER; ++i) {
    PlayerPortalInstance* portal = &portals_[i];
    const Magic::Magic_Portal* data = portal->data;
    if (data) {
      switch (data->type) {
      case Magic::Magic_Portal::SELF_ONLY_TO_SPAWN_POINT:
      case Magic::Magic_Portal::SELF_ONLY_MARK1:
      case Magic::Magic_Portal::SELF_ONLY:
        releasePortalInstance(portal);
        break;
      }
    }
  }
}


//----[  releaseDOTAOEBuff  ]--------------------------------------------------
void Player::releaseDOTAOEBuff() {
  if (dotaoebuff_.data == NULL) return;
  if (dotaoebuff_.local_region) {
    dotaoebuff_.local_region->broadcastEvent_Terminate(dotaoebuff_.event_id);
  }
  dotaoebuff_.disable();
}




//----[  acquireDOTInstance  ]-------------------------------------------------
PlayerDOTInstance* Player::acquireDOTInstance(const Magic::Magic_DOT* data) {
  advance_dots_ = true;
  for (int i = 0; i < Magic::DOTS_PER_PLAYER; ++i) {

    // always acquire a DOT
    if ((i + 1 == Magic::DOTS_PER_PLAYER) &&
        (dots_[i].data != NULL)) {
      releaseDOTInstance(&dots_[i]);
    }

    if (dots_[i].data == NULL) {
      dots_[i].enable();
      dots_[i].data = data;
      return &dots_[i];
    }
  }
  assert(!"A DOT instance should never fail to be acquired");
  return NULL;
}


//----[  releaseDOTInstance  ]-------------------------------------------------
void Player::releaseDOTInstance(PlayerDOTInstance* instance) {
  if (instance->data == NULL) return;
  ActorInstance* target = instance->target.dereference();
  if (target && instance->event_id != Event::INVALID_EVENT_ID) {
    target->broadcastEvent_Terminate(instance->event_id);
  }
  instance->disable();
}




//----[  findAOETargets  ]-----------------------------------------------------
bool Player::findAOETargets(const Magic::AOEMagicData* data,
                            WorldRegion* region,
                            ActorInstance* target,
                            Number target_x,
                            Number target_z,
                            ActorPointer* targeted_actors,
                            int* number_of_targeted_actors,
                            Event::EventIndex per_target_event) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  Number source_x, source_z;
  Number effect_x, effect_z;
  bool source_location = false, target_location = true;
  switch (data->source) { // find the point at which to set the source
  case Magic::AOEMagicData::SELF_LOCATION:
    source_location = true;
  case Magic::AOEMagicData::SELF:
    my_actor->getPosition(&source_x, &source_z);
    break;
  case Magic::AOEMagicData::TARGET:
    if (target) {
      source_location = true;
      target->getPosition(&source_x, &source_z);
    } else {
      source_x = target_x;
      source_z = target_z;
    } break;
  case Magic::AOEMagicData::TARGET_ACTOR:
    if (!target) return false;
    source_location = true;
    target->getPosition(&source_x, &source_z);
    break;
  case Magic::AOEMagicData::TARGET_LOCATION:
    source_location = true;
    source_x = target_x;
    source_z = target_z;
    break;
  }
  switch (data->effect) { // find the point at which to find targets
  case Magic::AOEMagicData::SELF_LOCATION:
  case Magic::AOEMagicData::SELF:
    my_actor->getPosition(&effect_x, &effect_z);
    break;
  case Magic::AOEMagicData::TARGET:
    if (target) {
      target->getPosition(&effect_x, &effect_z);
    } else {
      effect_x = target_x;
      effect_z = target_z;
    } break;
  case Magic::AOEMagicData::TARGET_ACTOR:
    if (!target) return false;
    target->getPosition(&effect_x, &effect_z);
    break;
  case Magic::AOEMagicData::TARGET_LOCATION:
    effect_x = target_x;
    effect_z = target_z;
    break;
  }

  // Find targets near this actor
  assert(data->max_targets <= Magic::MAX_TARGETS);
  int targets_found = 0;
  region->findAOETargets(
    effect_x,
    effect_z,
    Number(data->radius_sq),
    my_actor,
    data->target_caster,
    data->target_friends,
    data->target_enemies,
    attacking_players_,
    targeted_actors,
    data->max_targets,
    &targets_found);
  *number_of_targeted_actors = targets_found;

  Actor::ActorID target_ids[Magic::MAX_TARGETS];
  assert(targets_found <= Magic::MAX_TARGETS);
  for (int i = 0; i < targets_found; ++i) {
    target_ids[i]
     = targeted_actors[i].dereferenceAssumingValid()->getIDNumber();
  }

  // create the AOE's effect
  if (per_target_event != Event::INVALID_EVENT_INDEX) {
    if (source_location) {
      my_actor->broadcastEvent_GenerateTargetedSpecialFX(
        GenerateEventID(),
        per_target_event,
        source_x,
        source_z,
        effect_x,
        effect_z,
        target_ids,
        targets_found);
    } else {
      my_actor->broadcastEvent_GenerateTargetedSpecialFX(
        GenerateEventID(),
        per_target_event,
        my_actor->getIDNumber(),
        effect_x,
        effect_z,
        target_ids,
        targets_found);
    }
  }

  return targets_found != 0;
}




}
}

