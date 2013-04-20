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
#ifndef __SERVER_LIFEFORMAI_LIFEFORMAIINSTANCE_SIMPLEMELEEMONSTER_H__
#define __SERVER_LIFEFORMAI_LIFEFORMAIINSTANCE_SIMPLEMELEEMONSTER_H__
#pragma once

#include "lifeformaiinstance.h"
#include "shared/time/timer.h"
#include "server/actorpointer.h"
#include "server/actorcontroller.h"
#include "shared/avatar/avatarlevel.h"

namespace Evidyon {
class Map;
namespace LifeformAI {
struct LifeformAI_SimpleMeleeMonster;
}
}

namespace Evidyon {
namespace Server {




//----[  LifeformAIInstance_SimpleMeleeMonster  ]------------------------------
class LifeformAIInstance_SimpleMeleeMonster : public LifeformAIInstance,
                                              public ActorController {

  enum State {
    // pick a direction, stand still for a while
    PASSIVE_IDLE,
    // move in a straight line until something is hit, then go idle
    PASSIVE_WANDER,
    // Stand still, look for nearby client to target
    IDLE,
    MOVING,
    FOLLOWING_PATH,
    // Close enough to target to attack.  If the target is no longer in range,
    // switch to MOVING.
    ATTACKING,
    // When the state-change timer expires, erase this instance.
    DEAD,
    // When the state-change timer expires, erase this instance.  Exit
    // the world through a portal.
    DESPAWN,
    // Delete this instance
    UNINITIALIZED,
    TERMINATED,
  };


public:
  LifeformAIInstance_SimpleMeleeMonster(
    Avatar::AvatarLevel intended_level,
    Number x,
    Number z,
    const LifeformAI::LifeformAI_SimpleMeleeMonster* description);

public: // implement LifeformAIInstance
  virtual bool acquire(Map* map, GlobalActorManager* actor_manager);
  virtual bool update(double time, double time_since_last_update);
  virtual bool shouldReleaseWithWorldRegion(WorldRegion* region);
  virtual void release(GlobalActorManager* actor_manager);
  virtual void despawn();

public: // implement ActorController
  virtual Number changeHP(ActorInstance* source, bool magical, Number amount);
  virtual void fillDescription(Evidyon::Actor::ActorDescription* description);

private:
  void stateChange(State state, bool entering = true);
  void updateIdle(double time, double time_since_last_update);
  void updateMoving(double time, double time_since_last_update);
  void updateFollowingPath(double time, double time_since_last_update);
  void updateAttacking(double time, double time_since_last_update);
  void updateDeadOrDespawn(double time, double time_since_last_update);

  bool checkForCollisions(Number next_x, Number next_z);
  bool checkForActorCollision(Number next_x, Number next_z);
  bool checkForMapCollision(Number next_x, Number next_z);

  // Changes the dx_ and dz_ members so that they are pointing the monster
  // toward the given target location.  The distance to that location is
  // returned.
  Number setDXDZ(Number target_x, Number target_z);

  // Chooses counterclockwise or clockwise movement in order to
  // reach the target.
  bool pickPathDirection();

  // Call this method when the monster accidentally ends up somewhere
  // that it should not be (inside something).  This prevents monsters
  // from getting stuck.
  void bumpLocation();

private:
  State state_;
  ActorPointer actor_;
  const LifeformAI::LifeformAI_SimpleMeleeMonster* description_;
  Number x_, z_, dx_, dz_;
  Number target_x_, target_z_;
  bool navigate_cw_;

  // The level of the player that should kill this monster
  Avatar::AvatarLevel intended_level_;

  // When leaving the following-path state, if the distance to the target
  // increase, this actor will flip whether it is navigation clockwise
  // or counterclockwise.
  Time::Timer<Time::RESOLUTION_100_MS> state_change_timer_;
  Number hp_;
  ActorPointer target_;
  Time::Timer<Time::RESOLUTION_100_MS> next_begin_attack_timer_;
  Time::Timer<Time::RESOLUTION_10_MS> apply_attack_timer_;
};


}
}

#endif