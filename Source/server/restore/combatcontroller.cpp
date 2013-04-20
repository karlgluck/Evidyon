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
//------------------------------------------------------------------------------------------------
// File:    combatcontroller.cpp
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "combatcontroller.h"
#include "../common/null.h"


CombatController::CombatController() {
  owner_ = NULL;
  owner_controller_ = NULL;
  current_target_controller_ = NULL;
  blocking_attack_ = false;
  attack_source_ = NULL;
  current_state_ = IDLE;
  time_ = 0.0;
}

void CombatController::setOwner(Actor* owner) {
  owner_ = owner;
  owner_controller_ = owner->getController();
}

bool CombatController::block(Actor* attacker, double time) {
  wakeUp(); // Make sure the controller isn't idle

  bool can_block = true;
  switch (current_state_) {

    // Don't block while attacking
    case UNBLOCKED_SWING_DELAY:
    case UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED:
    //case BLOCKED_SWING_DELAY:
      can_block = false;
      break;

    // Wait for the actor to resume the "action delay" state
    case RECOVERING:

    // Repeated blocking is allowed
    case BLOCKED_SWING_DELAY:
    case BLOCKING_ATTACK_DELAY:
      can_block = time_ <= time;
      break;

    case ACTION_DELAY:
      can_block = time_ > time || current_target_ == NULL;
      break;
  }

  // If we are able to block, do so during the next combat cycle
  if (can_block) {
    blocking_attack_ = true;
    attack_source_ = attacker;
  }

  return can_block;
}

void CombatController::enterBlockingAttackState(double time) {
  double block_delay = owner_controller_->calculateBlockingTime();
  time_ = time + block_delay;
  owner_controller_->combatControllerBlock(attack_source_, block_delay);
  current_state_ = BLOCKING_ATTACK_DELAY;
  blocking_attack_ = false;
  current_target_ = NULL;
  current_target_controller_ = NULL;
}

void CombatController::update_ActionDelay(double time) {

  if (blocking_attack_) {
    enterBlockingAttackState(time);
    return;
  }

  // If the target is invalid, we are in idle combat mode
  if (current_target_ == NULL) {
    current_state_ = IDLE;
    return;
  }

  // When the timer expires, swap in the next target.  If it's valid, execute
  // an attack swing.
  if (time_ <= time) {
    if (!owner_controller_->calculateAttack(current_target_, &current_attack_)) {
      current_target_ = NULL;
      return;
    }
    bool blocked = current_target_->blockAttack(owner_, &current_attack_, time);
    current_attack_.blocked = blocked;
    if (blocked) {
      current_state_ = BLOCKED_SWING_DELAY;
    } else {
      if (!current_target_controller_->isClient()) {
        current_state_ = UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED;
      } else {
        current_state_ = UNBLOCKED_SWING_DELAY;
      }
    }
    double swing_delay = owner_controller_->calculateSwingingTime(&current_attack_);
    time_ = time + swing_delay;
    owner_controller_->combatControllerAttack(current_target_, &current_attack_, swing_delay);
    return;
  }
}

void CombatController::update_BlockedSwingDelay(double time) {
  if (time_ > time) return;
  if (blocking_attack_) { // block!
    enterBlockingAttackState(time);
  } else {
    current_state_ = RECOVERING;
    time_ = time + owner_controller_->calculateRecoveryDelay();
  }
}

void CombatController::update_BlockingAttackDelay(double time) {
  if (time_ > time) return;
  if (blocking_attack_) { // block!
    enterBlockingAttackState(time);
  } else {
    current_state_ = RECOVERING;
    time_ = time + owner_controller_->calculateRecoveryDelay();
  }
}

void CombatController::update_UnblockedSwingDelay(double time) {

  // Wait until the time when we should send a damage message to the client so
  // that it arrives at the proper time
  if (time_ > time + current_target_controller_->getPredictiveDelay()) return;
  current_target_controller_->willTakeDamage(current_attack_.damage);
  current_state_ = UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED;
}

void CombatController::update_UnblockedSwingDelayDamageGenerated(double time) {
  if (time_ > time) return;
  current_target_controller_->applyAttackDamage(owner_, &current_attack_);
  current_state_ = RECOVERING;
  time_ = time + owner_controller_->calculateRecoveryDelay();
}

void CombatController::update_Recovering(double time) {
  if (time_ > time) return;
  current_state_ = ACTION_DELAY;
  time_ = time + owner_controller_->calculateRepeatedAttackDelay();
  current_target_ = NULL; // reset the target
  current_target_controller_ = NULL;
}

void CombatController::attack(Actor* target) {
  current_target_ = target;
  current_target_controller_ = target ? target->getController() : NULL;
  wakeUp();
}

bool CombatController::busy(double time) {
  switch (current_state_) {
    case IDLE: return false;
    case ACTION_DELAY: return current_target_ == NULL; break;
    case RECOVERING: return time_ < time;
    default: return true;
  }
}

void CombatController::wakeUp() {
  if (current_state_ == IDLE) {
    current_state_ = ACTION_DELAY;
    time_ = 0.0;
  }
}


void CombatController::update(double time) {
  switch (current_state_) {
    case ACTION_DELAY: update_ActionDelay(time); break;
    case BLOCKED_SWING_DELAY: update_BlockedSwingDelay(time); break;
    case UNBLOCKED_SWING_DELAY:update_UnblockedSwingDelay(time); break;
    case UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED:  update_UnblockedSwingDelayDamageGenerated(time); break;
    case BLOCKING_ATTACK_DELAY:update_BlockingAttackDelay(time); break;
    case RECOVERING: update_Recovering(time); break;

    default:
    case IDLE: break; // don't do anything in this state
  }
}