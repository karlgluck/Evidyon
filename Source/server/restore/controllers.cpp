#include <dc/debug>
#include <dcx/managedmemorypool>

class Actor;
struct AttackData;


class ActorController {
public:
  virtual void willTakeDamage(int damage) = 0;
  virtual double getPredictiveDelay() = 0;
  virtual bool isClient() = 0;

  // Return 'true' if the actor is able to block an incoming physical attack
  // from the given source.
  // For client-handling actors, it is important that this method detect whether
  // the player is trying to do something with higher precedence--for example,
  // move away or cast a spell.
  virtual bool canBlockAttack(Actor* attacker, const AttackData* attack, double time) = 0;

  virtual double calculateSwingingTime(bool blocked) = 0;
  virtual double calculateBlockingTime() = 0;
  virtual double calculateRecoveryDelay() = 0;
  virtual double calculateRepeatedAttackDelay() = 0;

  // Fill the attack_data structure with an attack.  The result of this method
  // is passed to the target's blockAttack fuction.  If the return value is
  // 'false', the actor cannot attack the target.
  virtual bool calculateAttack(Actor* target, AttackData* attack_data) = 0;

  // The combat controller of an attacker calls this method when an attack
  // lands on this actor.  This method updates stats based on the attack.
  virtual void applyAttackDamage(Actor* attacker, AttackData* attack_data) = 0;

  // The combat controller invokes this method when the actor starts a swing at
  // the target.  The attack_data parameter will be filled by calculateAttack
  // if the attack wasn't blocked; otherwise, blocked_ will be set and none of
  // the other data members will be valid.
  // This member is responsible for broadcasting the attack visualization.
  // The parameter attack_time represents the duration of the attack
  virtual void combatControllerAttack(Actor* target,
    const AttackData* attack_data, double attack_time) = 0;

  // Similar to combatControllerAttack, but invoked for a blocker.
  virtual void combatControllerBlock(Actor* attacker, double block_time) = 0;

};



// controllers have no concept of other controllers and only
// execute a single "loop" of ther effect







class Actor;
class Client;
class CombatController;



//typedef double Time;
//inline Time convertSecondsToTime(


class MovementController {
public:
  MovementController();
  void update(double time, double timestep);
  float distanceToSq(const MovementController* other) const;
  bool withinRangeSq(const MovementController* other, float range_sq) const;

  inline float getX() const { return x_; }
  inline float getY() const { return y_; }

  inline void setLocation(float x, float y) { x_ = x; y_ = y; }
  inline void setMotion(float angle, float speed) { dx_ = cosf(angle)*speed; dy_ = sinf(angle)*speed; }
  inline void setMotionCartesian(float dx, float dy) { dx_=dx; dy_=dy; }
  inline void getMotionCartesian(float* dx, float* dy) { *dx=dx_; *dy=dy_; }

private:
  float x_, y_;

  // Add timestep*dx to x and timestep*dy to y each update
  // dx = cos(direction) * speed
  // dy = sin(direction) * speed
  float dx, dy;
};



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
  // by the owning Actor when another controller is attacking it.
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
  ActorController* owner_controller_;

  // The actor that this controller is currently attacking, and the one to
  // start attacking on the next attack round.
  Actor* current_target_;
  ActorController* current_target_controller_;

  // This flag is set if the actor should block an incoming attack from
  // the actor in attack_source_
  bool blocking_attack_;
  Actor* attack_source_;
  
  CurrentState current_state_;

  double time_;
  AttackData current_attack_;
};


class Actor {
  enum State {
    H2HCOMBAT_ATTACK_STATIONARY,      // the target is in range and this actor has stopped
    H2HCOMBAT_ATTACK_MOVINGTOTARGET,  // the target is within range, but not right next to the actor
    H2HCOMBAT_TARGETOUTOFRANGE,       // the target can't be reached, so move toward it
    MOVE,
  };
public:
  Actor();

  void acquire(ActorController* controller);
  void release();

  void update(double time);
  inline ActorController* getController() { return controller_; }

  inline bool blockAttack(Actor* attacker, const AttackData* attack, double time) {
      return controller_->canBlockAttack(attacker, attack, time) ? combat_.block(attacker, time) : false;
  }

private:
  Actor* target_;
  ActorController* controller_;
  CombatController combat_;
  MovementController movement_;
};





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

class Client : public ActorController {
public:
  // Sends a delta-HP packet to the client.  This is meant as a predictor
  // just to keep the game visually in sync.  The client's HP will be
  // update with an absolute value when damage is actually applied.
  virtual void willTakeDamage(int damage);

  // Calculates the amount of time it takes, in seconds, for a message to
  // reach the end host
  virtual double getPredictiveDelay();


  virtual bool isClient();

  // For client-handling actors, it is important that this method detect whether
  // the player is trying to do something with higher precedence--for example,
  // move away or cast a spell.
  virtual bool canBlockAttack(Actor* attacker, const AttackData* attack, double time);

  virtual double calculateSwingingTime(const AttackData* attack);
  virtual double calculateBlockingTime();
  virtual double calculateRecoveryDelay();
  virtual double calculateRepeatedAttackDelay();

  // Fill the attack_data structure with an attack.  The result of this method
  // is passed to the target's blockAttack fuction.  If the return value is
  // 'false', the actor cannot attack the target.
  virtual bool calculateAttack(Actor* target, AttackData* attack_data);

  // The combat controller of an attacker calls this method when an attack
  // lands on this actor.  This method updates stats based on the attack.
  virtual void applyAttackDamage(Actor* attacker, AttackData* attack_data);

  // The combat controller invokes this method when the actor starts a swing at
  // the target.  The attack_data parameter will be filled by calculateAttack
  // if the attack wasn't blocked; otherwise, blocked_ will be set and none of
  // the other data members will be valid.
  // This member is responsible for broadcasting the attack visualization.
  // The parameter attack_time represents the duration of the attack
  virtual void combatControllerAttack(Actor* target,
    const AttackData* attack_data, double attack_time);

  // Similar to combatControllerAttack, but invoked for a blocker.
  virtual void combatControllerBlock(Actor* attacker, double block_time);
};

void Client::willTakeDamage(int damage) {
}

double Client::getPredictiveDelay() {
  return 0.0;
}

bool Client::isClient() {
  return true;
}

int globalTime = 0;
Client* actor_a = 0;

Actor::Actor() {
  combat_.setOwner(this);
}

bool Client::canBlockAttack(Actor* attacker, const AttackData* attack, double time) {
  return rand()%3 == 0;
}

double Client::calculateSwingingTime(const AttackData* attack) {
  return 0.4 + rand()/(double)RAND_MAX/2;
}

double Client::calculateBlockingTime() {
  return 0.3;
}

double Client::calculateRecoveryDelay() {
  return 0.0;
}

double Client::calculateRepeatedAttackDelay() {
  return 0.4;
}

bool Client::calculateAttack(Actor* target, AttackData* attack_data) {
  attack_data->blocked = false;
  attack_data->damage = rand()%10+5;
  return true;
}

void Client::applyAttackDamage(Actor* attacker, AttackData* attack_data) {
  printf("%6i: (%s) Took %i damage\n", globalTime, actor_a==this?"a":"b", attack_data->damage);
}

void Client::combatControllerAttack(Actor* target, const AttackData* attack_data,
                            double attack_time) {
  char data[512];
  if (attack_data->blocked)
    sprintf_s(data, 512, "attack was blocked!");
  else
    sprintf_s(data, 512, "dealing %i damage", attack_data->damage);
  printf("%6i: (%s) Attacking: %s\n", globalTime, actor_a==this?"a":"b", data);
}

void Client::combatControllerBlock(Actor* attacker, double block_time) {
  printf("%6i: (%s) Blocking\n", globalTime, actor_a==this?"a":"b");
}

Actor* global_blocker;


void Actor::acquire(ActorController* controller) {
  controller_ = controller;
  combat_.setOwner(this);
}

void Actor::release() {
  controller_ = NULL;
}

void Actor::update(double time) {
  if (!combat_.busy(time)) {
    combat_.attack(global_blocker == this ? NULL : global_blocker);
  }
  combat_.update(time);
}

#include <windows.h>
/*

static const int MAX_ACTOR_ENCHANTMENTS = 8;
static const int MAX_ACTOR_PERIODIC_EFFECTS = 4;

struct EffectGenerationDescription {
  enum Type {
    SELF, PROJECTILES, FAN_FORWARD, LOCAL_AREA, TARGET_AREA, TARGET_ACTOR
  } type;
};

struct EffectResultDescription {

  enum Type {
    // int_param[0]
    HP, MP, PHYSICAL_DEFENSE, MAGICAL_DEFENSE, CHANGE_FORM,
    // int_param[0] int_param[1]
    PHYSICAL_MAGICAL_DEFENSE,
    // float_param[0]
    DELTA_SPEED, SET_SPEED, LEAP,
    // float_param[0] float_param[1]
    TELEPORT,
    // float_param[0] int_param[1]
    SET_SPEED_CHANGE_FORM,
  } type;

  // Visualization applied when the effect is first activated.  Duration is set
  // to 1 period of the effect.
  int applied_visualization;

  // Visualization applied for the duration of the effect.  Duration is set to
  // 'duration'.
  int duration_visualization;

  double period;
  double duration;
};

struct EffectParameters {
  const EffectDescription* description;
  union {
    float float_params[2];
    int int_params[2];
  };
};

class EffectApplicationController {
  struct Enchantment {
    double expiration_time_;
    EffectParameters parameters_;
  };
  struct Periodic {
    double period_;
    double next_application_time_;
    int times_applied_;
    EffectParameters parameters_;
  };
public:

  // Calls methods on the owner actor to apply the result of an
  // enchantment or spell.
  void update(double time);

  void dispelEnchantment(int index);
  void dispelAllEnchantments();

private:
  Actor* owner_;
  Enchantment enchantments_[MAX_ACTOR_ENCHANTMENTS];
  Periodic periodics_[MAX_ACTOR_PERIODIC_EFFECTS];
};

class EffectGenerationController {
public:
public:
  void generate(const Targeter* source_targeter, EffectParameters* parameters);
private:
};














*/




void main() {
  Client client1, client2;
  Actor attacker, blocker;
  blocker.acquire( &client1 );
  attacker.acquire( &client2 );
  global_blocker = &blocker;
  actor_a = &client1;
  attacker.update(-1.0);
  blocker.update(-1.0);

  while (1) {
    globalTime = GetTickCount();
    attacker.update(globalTime / 1000.0f);
    blocker.update(globalTime / 1000.0f);
  }
}