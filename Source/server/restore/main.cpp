//#if 0
//#include <dc/debug>
//#include <dcx/managedmemorypool>
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//struct Actor;
//struct Client;
//class CombatController;
//
//
//
//struct AttackData {
//  bool blocked;
//  int damage;
//};
//
////typedef double Time;
////inline Time convertSecondsToTime(
//
//
//
//struct Client
//{
//  // Sends a delta-HP packet to the client.  This is meant as a predictor
//  // just to keep the game visually in sync.  The client's HP will be
//  // update with an absolute value when damage is actually applied.
//  void sendTakeDamageMessage(int damage);
//
//  // Calculates the amount of time it takes, in seconds, for a message to
//  // reach the end host
//  double getMessageDelay();
//};
//
//void Client::sendTakeDamageMessage(int damage) {
//}
//
//double Client::getMessageDelay() {
//  return 0.0;
//}
//
//
//
///**
// * Handles physical combat.  This class handles combat timing and flow only;
// * it has no concept of spatial separation, offensive/defensive capability,
// * special abilities, etc.  All of that should be handled by the Actor.
// */
//class CombatController
//{
//public:
//  CombatController();
//
//  void setOwner(Actor* owner);
//  void update(double time);
//
//  // When the controller is no longer busy, it starts attacking this
//  // target.  If the target is 'null', then it will stop attacking.
//  void changeTarget(Actor* target);
//
//  // Disables the controller, preventing it from attacking--but also
//  // preventing it from entering an idle state.
//  void enable(bool enabled);
//
//  // Returns true if the controller is currently executing an attack or is
//  // blocking an attack.  The controller is not busy when idle or waiting
//  // to attack again.
//  bool busy(double time);
//
//  // This will wake up the controller if it is idle.  This method is invoked
//  // by another CombatController when this controller's owner is being
//  // attacked.
//  bool block(Actor* attacker, double time);
//
//private:
//
//  void enterBlockingAttackState(double time);
//
//  void update_ActionDelay(double time);
//  void update_BlockingAttackDelay(double time);
//  void update_BlockedSwingDelay(double time);
//  void update_UnblockedSwingDelay(double time);
//  void update_UnblockedSwingDelayDamageGenerated(double time);
//  void update_Recovering(double time);
//
//  // If the controller is IDLE, move into ACTION_DELAY
//  void wakeUp();
//
//
//private:
//
//  enum CurrentState {
//    BLOCKING_ATTACK_DELAY,
//    BLOCKED_SWING_DELAY,
//    UNBLOCKED_SWING_DELAY, // waiting to send a message to the target's client
//    UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED,
//    RECOVERING,
//    ACTION_DELAY,
//    IDLE,
//  };
//
//  // The actor that contains this controller
//  Actor* owner_;
//
//  // Clear this flag if the actor is busy for a reason other than physical
//  // combat.  For example, when casting a spell.
//  bool enabled_;
//
//  // The actor that this controller is currently attacking, and the one to
//  // start attacking on the next attack round.
//  Actor* current_target_;
//  Actor* next_target_;
//
//  // This flag is set if the actor should block an incoming attack from
//  // the actor in attack_source_
//  bool blocking_attack_;
//  Actor* attack_source_;
//  
//  CurrentState current_state_;
//
//  double time_;
//  AttackData current_attack_;
//};
///*
//class RangedAttack {
//public:
//  void update(double time);
//  bool terminated(Actor* actor_hit);
//private:
//  //WorldPartition* current_partition_;
//  int projectile_id_;
//  double expiration_time_;
//  float speed_;
//  AttackData attack_data_;
//};
//
//class RangedCombatController {
//  enum CurrentState {
//    IDLE,
//    REPEAT_ATTACK_DELAY,
//    ATTACK_SHOOT_DELAY,
//  };
//  static const int MAXIMUM_OUTSTANDING_SHOTS = 3;
//public:
//  RangedCombatController();
//  void setOwner(Actor* owner);
//  void enable(bool enabled);
//  bool busy(double time);
//  void update(double time);
//  void changeTarget(Actor* target);
//
//private:
//  CurrentState current_state_;
//  Actor* owner_;
//  bool enabled_;
//  Actor* current_target_;
//  double time_;
//  RangedAttack ranged_attacks_[MAXIMUM_OUTSTANDING_SHOTS];
//};
//
//void RangedCombatController::update(double time) {
//  if (current_state_ == IDLE) return;
//  if (current_state_ == ATTACK_SHOOT_DELAY) {
//    if (time > time_) {
//      current_state_ = REPEAT_ATTACK_DELAY;
//      time_ = owner_->calculateRepeatedAttackDelay();
//    } else {
//      return;
//    }
//  }
//  // if (current_state_ == REPEAT_ATTACK_DELAY) { ... }
//  if (time > time_) {
//    time_ = owner_->calculateSwingingTime();
//  }
//}*/
//
//
//struct Actor
//{
//  Client* client;
//
//  Actor();
//
//  // Invoked by the attacker's H2H combat controller to execute gameplay logic.
//  // This method determines whether or not this actor can block a physical
//  // attack attack from the given attacker based on speed, avatar stats,
//  // shield, skills, etc.  If that is true, then this method invokes
//  // combat_.block and returns the value from it.
//  bool blockAttack(Actor* attacker, const AttackData* attack, double time);
//
//  double calculateSwingingTime(bool blocked);
//  double calculateBlockingTime();
//  double calculateRecoveryDelay();
//  double calculateRepeatedAttackDelay();
//
//  // Fill the attack_data structure with an attack.  The result of this method
//  // is passed to the target's blockAttack fuction.
//  void calculateAttack(Actor* target, AttackData* attack_data);
//
//  // The combat controller of an attacker calls this method when an attack
//  // lands on this actor.  This method updates stats based on the attack.
//  void applyAttackDamage(Actor* attacker, AttackData* attack_data);
//
//  // The combat controller invokes this method when the actor starts a swing at
//  // the target.  The attack_data parameter will be filled by calculateAttack
//  // if the attack wasn't blocked; otherwise, blocked_ will be set and none of
//  // the other data members will be valid.
//  // This member is responsible for broadcasting the attack visualization.
//  // The parameter attack_time represents the duration of the attack
//  void combatControllerAttack(Actor* target, const AttackData* attack_data,
//                              double attack_time);
//
//  // Similar to combatControllerAttack, but invoked for a blocker.
//  void combatControllerBlock(Actor* attacker, double block_time);
//
//  void update(double time);
//
//private:
//  CombatController combat_;
//};
//
//
//
//
//
//CombatController::CombatController() {
//  owner_ = NULL;
//  enabled_ = false;
//  current_target_ = NULL;
//  next_target_ = NULL;
//  blocking_attack_ = false;
//  attack_source_ = NULL;
//  current_state_ = IDLE;
//  time_ = 0.0;
//}
//
//void CombatController::setOwner(Actor* owner) {
//  owner_ = owner;
//}
//
//bool CombatController::block(Actor* attacker, double time) {
//  if (!enabled_) return false;
//  wakeUp(); // Make sure the controller isn't idle
//
//  bool can_block = true;
//  switch (current_state_) {
//
//    // Don't block while attacking
//    case UNBLOCKED_SWING_DELAY:
//    case UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED:
//      can_block = false;
//      break;
//
//    // Repeated blocking is allowed
//    case RECOVERING:
//    case BLOCKED_SWING_DELAY:
//    case BLOCKING_ATTACK_DELAY:
//      can_block = time_ <= time;
//      break;
//  }
//
//  // If we are able to block, do so during the next combat cycle
//  if (can_block) {
//    blocking_attack_ = true;
//    attack_source_ = attacker;
//  }
//
//  return can_block;
//}
//
//void CombatController::enterBlockingAttackState(double time) {
//  double block_delay = owner_->calculateBlockingTime();
//  time_ = time + block_delay;
//  owner_->combatControllerBlock(attack_source_, block_delay);
//  current_state_ = BLOCKING_ATTACK_DELAY;
//  blocking_attack_ = false;
//}
//
//void CombatController::update_ActionDelay(double time) {
//  if (!enabled_) {
//    current_state_ = IDLE;
//    return;
//  }
//
//  if (blocking_attack_) {
//    enterBlockingAttackState(time);
//    return;
//  }
//
//  // When the timer expires, swap in the next target.  If it's valid, execute
//  // an attack swing.
//  if (time_ <= time) {
//
//    // If the target is invalid, the controller is no longer attacking--go idle.
//    if (current_target_ == NULL) {
//      current_state_ = IDLE;
//      return;
//    }
//
//    owner_->calculateAttack(current_target_, &current_attack_);
//    bool blocked = current_target_->blockAttack(owner_, &current_attack_, time);
//    current_attack_.blocked = blocked;
//    if (blocked) {
//      current_state_ = BLOCKED_SWING_DELAY;
//    } else {
//      if (current_target_->client == NULL) {
//        current_state_ = UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED;
//      } else {
//        current_state_ = UNBLOCKED_SWING_DELAY;
//      }
//    }
//    double swing_delay = owner_->calculateSwingingTime(blocked);
//    time_ = time + swing_delay;
//    owner_->combatControllerAttack(current_target_, &current_attack_, swing_delay);
//    return;
//  }
//}
//
//
//void CombatController::update_BlockedSwingDelay(double time) {
//  if (time_ > time) return;
//  if (blocking_attack_) { // block!
//    enterBlockingAttackState(time);
//  } else {
//    current_state_ = RECOVERING;
//    time_ = time + owner_->calculateRecoveryDelay();
//  }
//}
//
//void CombatController::update_BlockingAttackDelay(double time) {
//  if (time_ > time) return;
//  if (blocking_attack_) { // block!
//    enterBlockingAttackState(time);
//  } else {
//    current_state_ = RECOVERING;
//    time_ = time + owner_->calculateRecoveryDelay();
//  }
//}
//
//void CombatController::update_UnblockedSwingDelay(double time) {
//
//  // Wait until the time when we should send a damage message to the client so
//  // that it arrives at the proper time
//  if (time_ > time + current_target_->client->getMessageDelay()) return;
//  current_target_->client->sendTakeDamageMessage(current_attack_.damage);
//  current_state_ = UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED;
//}
//
//void CombatController::update_UnblockedSwingDelayDamageGenerated(double time) {
//  if (time_ > time) return;
//  current_target_->applyAttackDamage(owner_, &current_attack_);
//  current_state_ = RECOVERING;
//  time_ = time + owner_->calculateRecoveryDelay();
//}
//
//void CombatController::update_Recovering(double time) {
//  if (time_ > time) return;
//  current_state_ = ACTION_DELAY;
//  time_ = time + owner_->calculateRepeatedAttackDelay();
//}
//
//void CombatController::changeTarget(Actor* target) {
//  next_target_ = target;
//  wakeUp();
//}
//
//void CombatController::enable(bool enabled) {
//  enabled_ = enabled;
//  if (enabled) wakeUp();
//}
//
//bool CombatController::busy(double time) {
//  switch (current_state_) {
//    case IDLE: return false;
//    case ACTION_DELAY: return current_target_ == NULL || next_target_ == NULL || time < time_; break;
//    case RECOVERING: return time_ < time;
//    default: return true;
//  }
//}
//
//void CombatController::wakeUp() {
//  if (current_state_ == IDLE) {
//    current_state_ = ACTION_DELAY;
//    time_ = 0.0;
//  }
//}
//
//
//void CombatController::update(double time) {
//  switch (current_state_) {
//    case ACTION_DELAY: update_ActionDelay(time); break;
//    case BLOCKED_SWING_DELAY: update_BlockedSwingDelay(time); break;
//    case UNBLOCKED_SWING_DELAY:update_UnblockedSwingDelay(time); break;
//    case UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED:  update_UnblockedSwingDelayDamageGenerated(time); break;
//    case BLOCKING_ATTACK_DELAY:update_BlockingAttackDelay(time); break;
//    case RECOVERING: update_Recovering(time); break;
//
//    default:
//    case IDLE: break; // don't do anything in this state
//  }
//}
//
//
//int globalTime = 0;
//Actor* actor_a = 0;
//
//Actor::Actor() {
//  combat_.setOwner(this);
//}
//
//bool Actor::blockAttack(Actor* attacker, const AttackData* attack, double time) {
//  if (rand()%3 == 0) return false;
//  return combat_.block(attacker, time);
//}
//
//double Actor::calculateSwingingTime(bool blocked) {
//  return 0.4 + (actor_a==this ? 0.2 : 0.0);
//}
//
//double Actor::calculateBlockingTime() {
//  return 0.4;//- (actor_a==this ? 0.2 : 0.0);
//}
//
//double Actor::calculateRecoveryDelay() {
//  return 0.0;
//}
//
//double Actor::calculateRepeatedAttackDelay() {
//  return 0.4 - (actor_a==this ? 0.1 : 0.0);
//}
//
//void Actor::calculateAttack(Actor* target, AttackData* attack_data) {
//  attack_data->blocked = false;
//  attack_data->damage = rand()%10+5;
//}
//
//void Actor::applyAttackDamage(Actor* attacker, AttackData* attack_data) {
//  printf("%6i: (%s) Took %i damage\n", globalTime, actor_a==this?"a":"b", attack_data->damage);
//}
//
//void Actor::combatControllerAttack(Actor* target, const AttackData* attack_data,
//                            double attack_time) {
//  char data[512];
//  if (attack_data->blocked)
//    sprintf_s(data, 512, "attack was blocked!");
//  else
//    sprintf_s(data, 512, "dealing %i damage", attack_data->damage);
//  printf("%6i: (%s) Attacking: %s\n", globalTime, actor_a==this?"a":"b", data);
//}
//
//void Actor::combatControllerBlock(Actor* attacker, double block_time) {
//  printf("%6i: (%s) Blocking\n", globalTime, actor_a==this?"a":"b");
//}
//
//Actor* global_blocker;
//void Actor::update(double time) {
//  if (time < 0.0) {
//    combat_.changeTarget(global_blocker == this ? actor_a : global_blocker);
//    combat_.enable(true);
//  }
//  else {
//    combat_.update(time);
//  }
//}
//
//#include <windows.h>
//
//
//void p() {
//  dcx::dcxManagedMemoryPool<Actor,512> actors;
//  actors.create();
//
//  dcx::dcxManagedMemoryPool<Actor,512>::Pointer actor, new_actor;
//  actors.acquire(&actor);
//  if (actor.invalid()) {
//  }
//  new_actor.copy(actor);
//  actors.release(&actor);
//  Actor* new_actor_p = new_actor.dereference();
//  new_actor_p = new_actor_p;
//  actors.destroy();
//}
//
//
//
//class SpellcastingController {
//};
//
//class MovementController {
//};
//
//
//
//
//
//void main() {
//  p();
//  Client client1;
//  Actor attacker, blocker;
//  blocker.client = &client1;
//  attacker.client = &client1;
//  global_blocker = &blocker;
//  actor_a = &attacker;
//  attacker.update(-1.0);
//  blocker.update(-1.0);
//
//  while (1) {
//    globalTime = GetTickCount();
//    attacker.update(globalTime / 1000.0f);
//    blocker.update(globalTime / 1000.0f);
//  }
//}
//#endif