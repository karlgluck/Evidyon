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
#include "lifeformaiinstance_directdamagemonster.h"
#include "shared/lifeformai/lifeformai_directdamagemonster.h"
#include "server/globalactormanager.h"
#include "server/worldregion.h"
#include "server/world/maplocationinfo.h"
#include "server/map.h"
#include "server/worldregion.h"
#include "server/treasure/globaltreasuremanager.h"
#include "server/event/eventidgenerator.h"
#include "shared/design/monsterxp.h"
#include "shared/design/newdesign.h"

#include "server/client.h" // for giving xp

namespace Evidyon {
namespace Server {


static const Time::Milliseconds TIME_DEAD_BEFORE_RESPAWN_MS = 5000;
static const float MAX_PERSUIT_DISTANCE_SQ = 10.0f * 10.0f;

  
//----[  LifeformAIInstance_DirectDamageMonster  ]------------------------------
LifeformAIInstance_DirectDamageMonster::LifeformAIInstance_DirectDamageMonster(
    Avatar::AvatarLevel intended_level,
    Number x,
    Number z,
    const LifeformAI::LifeformAI_DirectDamageMonster* description) {
  state_ = UNINITIALIZED;
  actor_.reset();
  description_ = description;
  intended_level_ = intended_level;
  x_ = x;
  z_ = z;
  dx_ = 0;
  dz_ = 0;
  target_x_ = x_;
  target_z_ = z_;
  hp_ = Design::MonsterHP(intended_level, description->hp_multiplier);;
}




//----[  acquire  ]------------------------------------------------------------
bool LifeformAIInstance_DirectDamageMonster::acquire(
    Map* map,
    GlobalActorManager* actor_manager) {
  actor_manager->acquireActor(&actor_,
                              this,
                              description_->actor_template_index,
                              map,
                              x_.toFloat(),
                              z_.toFloat());
  if (actor_.invalid()) return false;
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  my_actor->setAlive();
  my_actor->setLevel(Number((int)intended_level_));
  my_actor->setMemberOfFaction(ActorInstance::EVIL);
  my_actor->setSize(description_->size);
  my_actor->enterWorld();
  stateChange(IDLE);
  navigate_cw_ = 0 == (my_actor->getIDNumber() & 1); // cheap randomize
  return true;
}




//----[  update  ]-------------------------------------------------------------
bool LifeformAIInstance_DirectDamageMonster::update(
    double time,
    double time_since_last_update) {
  if (description_ == NULL) return false;
  assert(actor_.invalid() == false);
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();


  switch (state_) {
    case IDLE:      updateIdle(time, time_since_last_update); break;
    case ATTACKING: updateAttacking(time, time_since_last_update); break;
    case MOVING: updateMoving(time, time_since_last_update); break;
    case FOLLOWING_PATH: updateFollowingPath(time, time_since_last_update); break;

    // These types have the same processing--invalidate the actor after the
    // timer expires.
    case DESPAWN:
    case DEAD:    updateDeadOrDespawn(time, time_since_last_update); break;
  }
  
  { // Advance the lifeform's state
    WorldRegion* regions_left[9];
    WorldRegion* regions_entered[9];
    int number_of_regions_left=0, number_of_regions_entered=0;
    if (my_actor->update(x_,
                         z_,
                         regions_left,
                        &number_of_regions_left,
                         regions_entered,
                        &number_of_regions_entered)) {
      // this is safe to ignore, but usually indicates a logic error somewhere
      //assert(number_of_regions_entered < 9);
    }
    const Evidyon::Server::MapLocationInfo* location
      = my_actor->getMapLocationInfo();
    if (!location || World::NavigabilitySolidToWalking(location->navigability)) {
      bumpLocation();
    } else {
      my_actor->setWadingFlag(location->navigability == World::NAVIGABILITY_WADE);
    }

    // Make the actor authoritative, so that our position is updated if we
    // get teleported or portaled somewhere.
    my_actor->getPosition(&x_, &z_);
  }

  // This lifeform is still active as long as it isn't in the TERMINATED state.
  return state_ != TERMINATED;
}




//----[  stateChange  ]--------------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::stateChange(State state, bool entering) {
  if (!description_ || (entering && state_ == state)) return;
  if (entering) stateChange(state_, false);
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  switch (state) {
  case IDLE:
    if (entering) {
      target_.reset();
      my_actor->setMovingFlag(false);
    } else {
    } break;
  case MOVING:
    if (entering) {
      ActorInstance* target_actor = target_.dereference();
      if (!target_actor || target_actor->getMap() != my_actor->getMap()) {
        target_.reset();
        my_actor->setMovingFlag(false);
        state = IDLE;
        break;
      }
      my_actor->setMovingFlag(true);
      target_actor->getPosition(&target_x_, &target_z_);
      state_change_timer_.trigger();
    } else {
      my_actor->setMovingFlag(false);
    } break;
  case FOLLOWING_PATH:
    if (entering) {
      if (target_.invalid()) {
        stateChange(IDLE);
        return;
      }
      navigate_cw_ = pickPathDirection();
      my_actor->setMovingFlag(true);
      state_change_timer_.reset();
      target_x_ = Number(x_.wholePart()) + Number(0.5);
      target_z_ = Number(z_.wholePart()) + Number(0.5);
      setDXDZ(target_x_, target_z_);
    } else {
      my_actor->setMovingFlag(false);
    } break;
  case ATTACKING:
    if (entering) {
      my_actor->setCombatFlag(true);
      next_begin_attack_timer_.trigger();
    } else {
      my_actor->setCombatFlag(false);
    } break;
  case DEAD:
    if (entering) {
      my_actor->setDead();
      WorldRegion* region;
      int map_x, map_z;
      my_actor->getStrictRegion(&map_x, &map_z, &region);
      GlobalTreasureManager::singleton()->dropTreasureItems(
        description_->treasure,
        intended_level_,
        region,
        map_x,
        map_z);
      state_change_timer_.set(TIME_DEAD_BEFORE_RESPAWN_MS);
    } else {
      my_actor->setAlive();
    } break;
  case DESPAWN:
    if (entering) {
      my_actor->setMovingFlag(false);
      WorldRegion* region = my_actor->getRegion();
      region->broadcastEvent_GenerateTargetedSpecialFX(
        GenerateEventID(),
        description_->despawn_event,
        my_actor->getIDNumber(),
        x_,
        z_,
        NULL,
        0);
      state_change_timer_.set(description_->despawn_delay);
    } break;
  case UNINITIALIZED:
    assert(!entering);
    if (!entering) {
      state_change_timer_.enable();
      state_change_timer_.trigger();
      target_.reset();
      next_begin_attack_timer_.enable();
      apply_attack_timer_.enable();
    } break;
  case TERMINATED:
    if (!entering) {
      // can't leave the terminate mode
      return;
    }
    state_change_timer_.disable();
    state_change_timer_.reset();
    target_.reset();
    next_begin_attack_timer_.disable();
    apply_attack_timer_.disable();
    description_ = NULL;
    break;
  }
  state_ = state;
}


//----[  updateIdle  ]---------------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::updateIdle(
    double time, double time_since_last_update) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  if (my_actor->getRegion()->findMonsterClientTarget(
      my_actor,
      x_.toFloat(),
      z_.toFloat(),
      &target_)) {
    stateChange(MOVING);
  }
}




//----[  updateAttacking  ]----------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::updateAttacking(
    double time, double time_since_last_update) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  ActorInstance* target = target_.dereference();
  if (!target || !target->isOnSameMapAs(my_actor) || target->isDead()) {
    stateChange(IDLE);
    return;
  }
  if (apply_attack_timer_.poll() && my_actor->canSee(target)) {
    target->changeHP(
      my_actor,
      true,    // it's magical!
      Design::MonsterAttackDamage(
        intended_level_,
        description_->dps_to_damage_multiplier));
  }
  if (my_actor->distanceToSq(target) < description_->attack_distance_sq) {
    if (next_begin_attack_timer_.expired() && my_actor->canSee(target)) {
      Time::Milliseconds duration = description_->attack_duration;
      apply_attack_timer_.set(duration >> 1); // apply dmg halfway through
      next_begin_attack_timer_.set(description_->attack_period);
      my_actor->setAction(Actor::ACTORACTION_ATTACK, duration);
      target->broadcastEvent_GenerateTargetedSpecialFX(
        GenerateEventID(),
        description_->attack_event,
        my_actor->getIDNumber(),
        target->getIDNumber());
    }
  } else {
    stateChange(MOVING);
  }

  my_actor->faceActor(target);
}



//----[  updateMoving  ]-------------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::updateMoving(
    double time, double time_since_last_update) {
  // the actors are checked in the root update method
  ActorInstance* target_actor = target_.dereferenceAssumingValid();
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  WorldRegion* current_region = my_actor->getRegion();

  if (!target_actor || target_actor->isDead()) {
    stateChange(IDLE);
    return;
  }
  

  Number next_x = x_.toFloat() + (dx_.toFloat() * time_since_last_update);
  Number next_z = z_.toFloat() + (dz_.toFloat() * time_since_last_update);

  if (checkForMapCollision(next_x, next_z)) {
    // always go into the pathfinding mode if we hit something
    stateChange(FOLLOWING_PATH);
  } else if (checkForActorCollision(next_x, next_z)) {
    // either attack the target, if it is close enough, or try
    // to navigate away from it to try to find another way in
    target_actor->getPosition(&target_x_, &target_z_);
    Number offset_x = target_x_ - x_,
           offset_z = target_z_ - z_;
    Number distance = offset_x * offset_x + offset_z * offset_z;
    if (distance.toFloat() < description_->attack_distance_sq) {
      stateChange(ATTACKING);
    } else if (state_change_timer_.poll()) {
      Number angle_to_target = FixedPoint::atan2(offset_z, offset_x);
      static const Number ANGLE_OFFSET[3] = { 3.14 / 4, -3.14 / 4, 0 };
      angle_to_target += ANGLE_OFFSET[rand()%3];
      Number magnitude = distance + 1;
      setDXDZ(target_x_ - FixedPoint::cos(angle_to_target) * magnitude,
              target_z_ - FixedPoint::sin(angle_to_target) * magnitude);
      state_change_timer_.set(1000);
    }
  } else {
    if (state_change_timer_.poll()) {
      state_change_timer_.set(250);

      // If this actor is no longer on screen, try to find a closer target.
      // However, keep persuing the old target until we do, or until the
      // current target is so far out of range that we should just give up.
      if (my_actor->actorIsOutOfRange(target_actor)) {
        if (!my_actor->getRegion()->findMonsterClientTarget(
            my_actor,
            x_.toFloat(),
            z_.toFloat(),
            &target_)) {
          if (my_actor->distanceToSq(target_actor) > MAX_PERSUIT_DISTANCE_SQ) {
            stateChange(IDLE);
          }
        }
      }
      
      target_actor->getPosition(&target_x_, &target_z_);
      Number offset_x = target_x_ - x_,
             offset_z = target_z_ - z_;
      Number distance = offset_x * offset_x + offset_z * offset_z;

      if (distance.toFloat() < description_->attack_distance_sq) {
        // attack when we come into range
        stateChange(ATTACKING);
      } else {
        // Move in a direct path to the target
        setDXDZ(target_x_, target_z_);
      }
    }
    x_ = next_x;
    z_ = next_z;
  }
}



//----[  updateFollowingPath  ]------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::updateFollowingPath(
    double time, double time_since_last_update) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  const Server::MapLocationInfo* location = my_actor->getMapLocationInfo();

  // This timer limits the amount of time we can go in the wrong direction
  if (state_change_timer_.poll()) {
    stateChange(MOVING);
    return;
  }

  Number next_x = x_.toFloat() + (dx_.toFloat() * time_since_last_update);
  Number next_z = z_.toFloat() + (dz_.toFloat() * time_since_last_update);

  if (checkForActorCollision(next_x, next_z)) {
    // pick a random direction
    Number randx = 2*(rand()%3);
    Number randz = 2*(rand()%3);
    stateChange(MOVING);
    setDXDZ(x_.wholePart() + randx + Number(0.5),
            z_.wholePart() + randz + Number(0.5));
    state_change_timer_.set(1000);
  } else {
    WorldRegion* current_region = my_actor->getRegion();

    // If we reach the center of the square, start moving to the next element
    if (checkForMapCollision(next_x, next_z) ||
        (FixedPoint::square(x_ - target_x_) +
         FixedPoint::square(z_ - target_z_)) < 0.1) {

      // Do we have a direct path to the target?
      ActorInstance* target = target_.dereference();
      Map* map = current_region->getMap();
      if (!target || !target->isOnSameMapAs(my_actor)) { stateChange(IDLE); return; }
      target->getPosition(&target_x_, &target_z_);
      if (map->isValidMonsterWalkPath(x_, z_, target_x_, target_z_)) {
        stateChange(MOVING);
        return;
      }

      // No direct path yet, so just keep moving
      target_x_ = Number(x_.wholePart()) + Number(0.5);
      target_z_ = Number(z_.wholePart()) + Number(0.5);
      if (!World::OffsetByPathDirection(location->walking_path[navigate_cw_?0:1],
                                        &target_x_,
                                        &target_z_)) {
        stateChange(MOVING);
        return;
      }
      setDXDZ(target_x_, target_z_);
    } else {
      x_ = next_x;
      z_ = next_z;
    }
  }
}


//----[  updateDeadOrDespawn  ]------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::updateDeadOrDespawn(
    double time, double time_since_last_update) {
  if (state_change_timer_.poll()) {
    stateChange(TERMINATED);
  }
}



//----[  checkForActorCollision  ]---------------------------------------------
bool LifeformAIInstance_DirectDamageMonster::checkForActorCollision(
    Number next_x,
    Number next_z) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  WorldRegion* region = my_actor->getRegion();
  ActorInstance* collision
    = region->collideActorWithActor(actor_.dereferenceAssumingValid(),
                                    x_.toFloat(),
                                    z_.toFloat(),
                                    next_x.toFloat(),
                                    next_z.toFloat());
  return (collision != NULL &&
          !my_actor->isGlancingCollision(collision, dx_, dz_));
}



//----[  checkForMapCollision  ]-----------------------------------------------
bool LifeformAIInstance_DirectDamageMonster::checkForMapCollision(
    Number next_x,
    Number next_z) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  WorldRegion* current_region = my_actor->getRegion();
  const MapLocationInfo* location = my_actor->getMapLocationInfo();
  if (!location || World::NavigabilitySolidToWalking(location->navigability)) {
    bumpLocation();
    return true;
  }

  // Get the next location's info
  //location = current_region->getNextActorMapLocationInfo(x_,
  //                                                       z_,
  //                                                       dx_,
  //                                                       dz_);
  location = current_region->getMap()->getMapLocationInfo(next_x.toFloat(), next_z.toFloat());
  return (!location ||
          World::NavigabilitySolidToWalking(location->navigability));
}

//----[  checkForCollisions  ]-------------------------------------------------
bool LifeformAIInstance_DirectDamageMonster::checkForCollisions(Number next_x,
                                                               Number next_z) {
  return checkForMapCollision(next_x, next_z) ||
         checkForActorCollision(next_x, next_z);
}


//----[  setDXDZ  ]------------------------------------------------------------
Number LifeformAIInstance_DirectDamageMonster::setDXDZ(Number target_x,
                                                      Number target_z) {
  // We are too far away to attack.  Move to get closer.
  Number offset_x = target_x - x_, offset_z = target_z - z_;

  // Calculate distance from us -> target actor
  Number distance;
  distance.int_value
    = FixedPoint::fixmul<16>(offset_x.int_value, offset_x.int_value)
    + FixedPoint::fixmul<16>(offset_z.int_value, offset_z.int_value);
  distance = FixedPoint::sqrt(distance);

  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  my_actor->setDirection(FixedPoint::atan2(offset_z, offset_x).toFloat());
  dx_.int_value
    = FixedPoint::fixmul<16>(Number(description_->speed).int_value,
                              (offset_x / distance).int_value);
  dz_.int_value
    = FixedPoint::fixmul<16>(Number(description_->speed).int_value,
                             (offset_z / distance).int_value);
  return distance;
}



//----[  pickPathDirection  ]--------------------------------------------------
bool LifeformAIInstance_DirectDamageMonster::pickPathDirection() {
  assert(!target_.invalid());
  ActorInstance* target = target_.dereferenceAssumingValid();
  Map* map = target->getRegion()->getMap();
  Number target_x, target_z;
  target->getPosition(&target_x, &target_z);
  Number x_start, z_start;
  Number x = Number(x_.wholePart()) + Number(0.5),
         z = Number(z_.wholePart()) + Number(0.5);
  x_start = x;
  z_start = z;

  static const int MAX_STEPS = 10;

  // First, do clockwise
  for (int step = 0; step < MAX_STEPS; ++step) {
    const MapLocationInfo* location
      = map->getMapLocationInfo(x.wholePart(), z.wholePart());
    if (!location) { bumpLocation(); return false; }
    if ((step < 5 || ((step & 1) == 1)) && // check every other step
        map->isValidMonsterWalkPath(x, z, target_x, target_z)) {
      return true; // navigate clockwise, since it gets us to the target
    } else {
      World::OffsetByPathDirection(location->walking_path[0], &x, &z);
    }
  }
  Number cw_distance = target->distanceToSq(x, z);

  // Next, do ccw
  x = x_start;
  z = z_start;
  for (int step = 0; step < MAX_STEPS; ++step) {
    const MapLocationInfo* location
      = map->getMapLocationInfo(x.wholePart(), z.wholePart());
    if (!location) { bumpLocation(); return true; }
    if ((step < 5 || ((step & 1) == 1)) && // check every other step
        map->isValidMonsterWalkPath(x, z, target_x, target_z)) {
      return false; // navigate clockwise, since it gets us to the target
    } else {
      World::OffsetByPathDirection(location->walking_path[1], &x, &z);
    }
  }

  // Neither direction found the target...so pick whichever ends up
  // closer.
  return cw_distance < target->distanceToSq(x, z) ? true : false;
}





//----[  bumpLocation  ]-------------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::bumpLocation() {
  ActorInstance* actor = actor_.dereferenceAssumingValid();
  Map* map = actor->getRegion()->getMap();
  const Server::MapLocationInfo* location = NULL;
  int steps = 10;
  float step_x = dx_.toFloat()/5.0f;
  float step_z = dz_.toFloat()/5.0f;
  while (location == NULL ||
         World::NavigabilitySolidToWalking(location->navigability)) {
    x_ -= step_x;
    z_ -= step_z;
    location = map->getMapLocationInfo(x_.wholePart(), z_.wholePart());
    if (--steps <= 0) {
      stateChange(TERMINATED);
      return;
    }
  }
}


//----[  shouldReleaseWithWorldRegion  ]---------------------------------------
bool LifeformAIInstance_DirectDamageMonster::shouldReleaseWithWorldRegion(
  WorldRegion* region) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  return actor_.invalid() ||
         (my_actor->getRegion() == region);
}




//----[  release  ]------------------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::release(GlobalActorManager* actor_manager) {
  stateChange(TERMINATED);
  ActorInstance* my_actor = actor_.dereference();
  assert(my_actor);
  actor_manager->releaseActor(&actor_);
}




//----[  despawn  ]------------------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::despawn() {
}



//----[  changeHP  ]-----------------------------------------------------------
Number LifeformAIInstance_DirectDamageMonster::changeHP(ActorInstance* source, bool magical, Number amount) {
  amount = FixedPoint::minfast(Number(0), amount);
  hp_ += amount;
  if (hp_ < 0) {
    stateChange(DEAD);
    Client* killer_client = source->getController()->getClient();
    if (killer_client) killer_client->giveMonsterEXP(
      intended_level_,
      Design::MonsterXP(intended_level_,
                        description_->xp_multiplier));
  }
  if (!magical) {
    ActorInstance* my_actor = actor_.dereferenceAssumingValid();
    my_actor->setBloodFlag();
    my_actor->setAction(Actor::ACTORACTION_GOT_HIT, 300);
  }
  return amount;
}




//----[  fillDescription  ]----------------------------------------------------
void LifeformAIInstance_DirectDamageMonster::fillDescription(Evidyon::Actor::ActorDescription* description) {

}



////----[  pathIsClear  ]--------------------------------------------------------
//bool LifeformAIInstance_DirectDamageMonster::pathIsClear(Number x1,
//                                                        Number z1,
//                                                        Number x2,
//                                                        Number z2) {
//  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
//  WorldRegion* region = my_actor->getRegion();
//  Map* map = region->getMap();
//  if (!map->isValidMonsterWalkPath(x1, z1, x2, z2)) return false;
//
//  ActorInstance* ignore[] = { my_actor, target_.dereference() };
//  return region->pathIsClearOfActors(x1, z1, x2, z2, my_actor->getSize(), ignore);
//}
//
//




}
}


