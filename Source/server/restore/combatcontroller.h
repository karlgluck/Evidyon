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
#ifndef __COMBATCONTROLLER_H__
#define __COMBATCONTROLLER_H__

class Actor;

struct AttackData {
  bool blocked;
  int damage;
};

/**
 * Handles physical combat.  This class handles combat timing and flow only;
 * it has no concept of spatial separation, offensive/defensive capability,
 * special abilities, etc.  All of that should be handled by the Actor.
 */
class CombatController
{
public:
  CombatController();

  void setOwner(Actor* owner);
  void update(double time);

  // Changes the target that this controller is attacking.  This method should
  // be used with care--always find out if the controller is busy before
  // using this method, otherwise the results may be nonsensical.
  void attack(Actor* target);

  // Returns true if the controller is currently executing an attack or is
  // blocking an attack.  The controller is not busy when idle or it has
  // completed an attack and has not yet been assigned its next target.
  bool busy(double time);

  // This will wake up the controller if it is idle.  This method is invoked
  // by another CombatController when this controller's owner is being
  // attacked.
  bool block(Actor* attacker, double time);

private:

  void enterBlockingAttackState(double time);

  void update_ActionDelay(double time);
  void update_BlockingAttackDelay(double time);
  void update_BlockedSwingDelay(double time);
  void update_UnblockedSwingDelay(double time);
  void update_UnblockedSwingDelayDamageGenerated(double time);
  void update_Recovering(double time);

  // If the controller is IDLE, move into ACTION_DELAY
  void wakeUp();


private:

  enum CurrentState {
    BLOCKING_ATTACK_DELAY,
    BLOCKED_SWING_DELAY,
    UNBLOCKED_SWING_DELAY, // waiting to send a message to the target's client
    UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED,
    RECOVERING,
    ACTION_DELAY,
    IDLE,
  };

  // The actor that contains this controller
  Actor* owner_;

  // The actor that this controller is currently attacking, and the one to
  // start attacking on the next attack round.
  Actor* current_target_;

  // This flag is set if the actor should block an incoming attack from
  // the actor in attack_source_
  bool blocking_attack_;
  Actor* attack_source_;
  
  CurrentState current_state_;

  double time_;
  AttackData current_attack_;
};

#endif