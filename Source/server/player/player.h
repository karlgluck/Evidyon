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
#ifndef __SERVER_PLAYER_PLAYER_H__
#define __SERVER_PLAYER_PLAYER_H__
#pragma once

#include "shared/time/timer.h"
#include "server/player/playerprojectileinstance.h"
#include "server/player/playerportalinstance.h"
#include "server/player/playertrapinstance.h"
#include "server/player/playerdotaoebuffinstance.h"
#include "server/player/playerdotinstance.h"
#include "server/actorpointer.h"
#include "shared/actor/actorprofileindex.h"
#include "shared/actor/actorequipmentslot.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"
#include "common/number.h"
#include "shared/magic/magiclimits.h"
#include "shared/actor/actoraction.h"


namespace Evidyon {
class Client;
class ClientItemsInterface;
class ClientToClientItemTrade;
struct SpellDescription;
namespace Magic {
struct Magic;
struct Spell;
struct Magic_Portal;
struct Magic_Buff;
struct AOEMagicData;
}
namespace Projectile {
struct Projectile;
}
namespace Server {
struct MapLocationInfo;
struct PlayerRequest;
struct AvatarInstance;
struct AvatarEnchantments;
struct AvatarStats;
}
}


namespace Evidyon {
namespace Server {


//----[  Player  ]-------------------------------------------------------------
class Player {
  enum State {
    IDLE,           // not doing anything
    FINDING_TRADE,  // idle, but looking for a trade
    TRADING,        // engaged in a trade
    AT_STORAGE,     // player is in the storage
    AT_BAZAAR,      // player is browsing the bazaar
//    AT_GEOSID,      // player is at a geosid
    FALLING,        // falling down a pit
    WALKING,        // moving on walkable ground
    SWIMMING,       // moving in swimming water
    FLYING,         // WALKING, but can move over short walls
    ATTACK_RUN,     // special movement: run at target -> MELEE_ATTACK
    LEAP,           // special movement: move to target location as if flying
    USING_MAGIC,    // casting a spell
    MELEE_ATTACK,   // swinging a melee attack
    RANGED_ATTACK,  // releasing a ranged projectile
    DEAD,           // player is lying on ground
    LEAVING_WORLD,  // waiting to leave the world
    GUILD_CREATE,   // waiting for player to join in order to create guild
    GUILD_LEAD,     // allow nearby players to join with g/join
    INVALID,        // client is not logged into world
  };


public:

  // Called when a client logs into the world
  void acquire(Client* client,
               ActorPointer* actor,
               PlayerRequest* request,
               AvatarInstance* avatar,
               AvatarEnchantments* enchantments,
               AvatarStats* stats,
               ClientItemsInterface* items,
               ClientToClientItemTrade* trade);

  // Invoked when a client leaves the world
  void release();

  // Pre-State Update:
  //  hp <= 0 -> DEAD
  //  advance projectiles, dot-aoe magic, portals, traps
  // Post-State Update:
  //  swimming = current_navigability == SWIM
  //  falling = current_navigability == PIT
  //  check (current location trigger) [[[enter if last location != current, etc...]]]
  void update(double time, double time_since_last_update);

  // These flags are polled by the client to determine how to handle
  // various network messages.
  bool isDead() const;
  bool isIdle() const;
  bool inTrade() const;
  bool isLookingForTrade() const;
  bool atStorage() const;
  bool atBazaar() const;
  //bool atGeosid() const;
  bool leaveWorld() const; // returns true after logoff counter expires

  // Returns 'true' if the player is leading or creating a guild
  bool isInvitingGuildMembers() const;

  // Fills create_guild_name and returns 'true' if a guild should be created
  bool guildMemberJoined(char* create_guild_name, size_t length);

  // Invoked when a resurrection spell is cast on this player.  Prevents them
  // from dying.  Returns 'true' if the resurrection had an effect so that
  // the Client invoking this method can send instructions to the remote
  // player.
  bool resurrect();

  // If this flag is set, AOEs will find players and projectiles will affect
  // players.  If the flag is cleared, no offensive AOE (spin/aoe/aoedot/etc)
  // will target a player, and no projectile will damage a client.
  // Returns 'true' if the flag was changed.
  bool setAttackPlayers(bool attack_players);

private:

  // Resets all class members
  void zero();

  // Called to switch the current state of the player's avatar by setting
  // 'entering' to true.  If 'entering' is true, this method calls itself
  // with 'entering' false and 'state' equal to the state that is being
  // left before executing the new-state's code.
  // This method changes the state variable of the internal timer.
  // Returns 'true' if the state was modified.
  bool stateChange(State state, bool entering = true);


private:
  
  // Initiation:
  //  (default)
  // Info:
  //  STOP_ACTION here causes current form (morph enchantment) to expire
  //  polls the hasEquipmentChanged flag of the items to determine whether
  //    or not the bonuses from equipped items need to be adjusted
  // Termination:
  //  falling -> FALLING
  //  MOVE && flying -> FLYING
  //  MOVE && swimming -> SWIMMING
  //  MOVE -> WALKING
  //  USE_ITEM or CAST && can_act -> USING_MAGIC
  //  ATTACK && can_act -> MELEE_ATTACK / RANGED_ATTACK
  // GEOSID/BAZAAR/STORAGE -> AT_GEOSID / AT_BAZAAR / AT_STORAGE
  void updateIdle(double time, double time_since_last_update);

  // Initiation:
  //    SPEAK contains "trade"
  // Termination:
  //    any request -> IDLE
  void updateFindingTrade(double time, double time_since_last_update);

  // Initiation:
  //  When entering LOOK_FOR_TRADE, a nearby player also in LOOK_FOR_TRADE
  //  is found.
  // Info:
  //  Does nothing other than asynchronous updates (projectiles, portals, ...)
  //  any request removes trading flag
  // Termination:
  //  item trader indicates no longer in trade
  void updateTrading(double time, double time_since_last_update);

  // Initiation:
  //  BAZAAR_OPEN, GEOSID_OPEN, STORAGE_OPEN
  // Info:
  //  Does nothing other than asynchronous updates (projectiles, portals, ...)
  // Termination:
  //  any request -> IDLE
  void updateAtBazaar(double time, double time_since_last_update);
  //void updateAtGeosid(double time, double time_since_last_update);
  void updateAtStorage(double time, double time_since_last_update);

  // Initiation:
  //  MOVE
  // Termination:
  //  !falling -> cancel falling timer, IDLE
  //  flying -> cancel falling timer, FLYING
  //  falling timer expired ? respawn -> IDLE
  //  USE_ITEM or CAST && can_act -> USING_MAGIC
  void updateFalling(double time, double time_since_last_update);

  // Initiation:
  //  MOVE
  // Termination:
  //  flying -> FLYING
  //  swimming -> SWIMMING
  //  falling -> FALLING
  //  delay -> IDLE
  //  STOP_ACTION -> IDLE
  //  collision -> IDLE
  //  USE_ITEM or CAST && can_act && can_cast -> USING_MAGIC
  //  ATTACK && can_act -> MELEE_ATTACK / RANGED_ATTACK
  void updateWalking(double time, double time_since_last_update);

  // Initiation:
  //  MOVE
  // Termination:
  //  delay -> IDLE
  //  STOP_ACTION -> IDLE
  //  collision -> IDLE
  //  flying -> FLYING
  //  levitating -> WALKING
  //  !swimming -> WALKING
  //  USE_ITEM && can_act && can_cast -> USING_MAGIC
  void updateSwimming(double time, double time_since_last_update);

  // Initiation:
  //  MOVE
  // Termination:
  //  delay -> IDLE
  //  STOP_ACTION -> IDLE
  //  collision -> IDLE
  //  !flying -> SWIMMING, FALLING, WALKING
  void updateFlying(double time, double time_since_last_update);

  // Initiation:
  //  CAST(attack run)
  // Termination:
  //  STOP_ACTION -> IDLE
  //  collision w/ object -> IDLE
  //  collision w/ actor -> MELEE_ATTACK / RANGED_ATTACK
  //  flying -> FLYING
  //  swimming -> SWIMMING
  //  falling -> FALLING
  void updateAttackRun(double time, double time_since_last_update);

  // Initiation:
  //  CAST(leap)
  // Termination:
  //  collision w/ tall object -> IDLE
  //  collision w/ actor @ target -> MELEE_ATTACK / RANGED_ATTACK
  //  delay -> IDLE
  void updateLeap(double time, double time_since_last_update);

  // Initiation:
  //  CAST, USE_ITEM
  // Info:
  //  Moves avatar depending on whether the spell and current state
  //  allow it.  Handles falling.
  // Termination:
  //  flying -> FLYING
  //  swimming -> SWIMMING
  //  falling timer expired ? respawn -> IDLE
  //  STOP_ACTION -> IDLE
  //  USE_ITEM or CAST && can_act -> USING_MAGIC
  //  ATTACK && can_act -> MELEE_ATTACK / RANGED_ATTACK
  //  delay -> IDLE, WALKING, SWIMMING, FLYING
  void updateUsingMagic(double time, double time_since_last_update);

  // Initiation:
  //  ATTACK with melee weapon equipped
  // Info:
  //  Moves avatar
  // Termination:
  //  flying -> FLYING
  //  falling -> FALLING
  //  swimming -> SWIMMING
  //  STOP_ACTION -> IDLE
  //  CAST, USE_ITEM -> USING_MAGIC
  //  invalid target -> WALKING, SWIMMING, FLYING, IDLE
  void updateMeleeAttack(double time, double time_since_last_update);

  // Initiation:
  //  ATTACK with ranged weapon equipped
  // Info:
  //  Moves avatar
  // Termination:
  //  flying -> FLYING
  //  swimming -> SWIMMING
  //  falling -> FALLING
  //  STOP_ACTION -> IDLE
  //  CAST, USE_ITEM -> USING_MAGIC
  //  invalid target -> WALKING, SWIMMING, FLYING, IDLE
  void updateRangedAttack(double time, double time_since_last_update);

  // Initiation:
  //  hp < 0
  // Termination:
  //  falling timer expired ? respawn -> IDLE
  //  not resurrected: delay, then respawn -> IDLE
  //  resurrected && RESURRECT -> IDLE
  //  resurrected && RESPAWN -> respawn, IDLE
  void updateDead(double time, double time_since_last_update);

  // Initiation:
  //   player hits ESCAPE on their keyboard
  // Termination:
  //   logoff timer expired ? client will make the player leave the world
  //   any action -> IDLE
  void updateLeavingWorld(double time, double time_since_last_update);

  // Initiation:
  //  player types g/create [name] 
  // Termination:
  //  another player types g/join near this player and joins the guild
  //  if the request type is no longer CREATE_GUILD, -> IDLE
  void updateGuildCreate(double time, double time_since_last_update);

  // Initiation:
  //  player types g/lead or guild/lead, client sends NETMSG_GUILD_LEAD.
  // Termination:
  //  any action -> IDLE
  void updateGuildLead(double time, double time_since_last_update);


private:

  // The player's equipped items have changed, so their stats need to be
  // recalculated and added to the player.  This method will update the
  // form of the avatar as the armor dictates, and send an update-description
  // packet for the actor.
  // This is only done in the idle state.  The flag that triggers avatar stats
  // to be recalculated is set by this method.
  void calculateEquipment();

  // Recalculates the avatar's stats based on the avatar's attributes, equipment
  // and enchantments.
  void calculateAvatarStats();

  // Executes the per-timestep processing for projectiles, portals and traps.
  // Each advance*() method will set its corresponding flag if the object
  // it is passed is valid.  This optimizes the player's update() method so
  // that projectiles aren't processed if there are no valid projectiles,
  // portals are skipped if none of them are valid, etc.
  void advancePortal(PlayerPortalInstance* portal,
                     double time,
                     double time_since_last_update);
  void advanceProjectile(PlayerProjectileInstance* projectile,
                         double time,
                         double time_since_last_update);
  void advanceTrap(PlayerTrapInstance* trap,
                   double time,
                   double time_since_last_update);
  void advanceDOTAOEBuff();
  void advanceDOT(PlayerDOTInstance* dot);

  // Advances the state of the magic currently being cast
  void incrementMagicStage();

  // Sends a player's spoken text
  void processSpeakRequest(bool allow_state_change);

  // Handles a movement request from the client.  This will change the state
  // of the player to WALKING, FLYING or SWIMMING as appropriate.
  void processMoveRequest();

  // Handles an attack request from the client.  Depending on the equipped
  // weapon, this will send the avatar into melee or ranged combat.
  void processAttackRequest();

  // Handles a use-item or cast spell request.  These will check to be sure
  // that the player is allowed its next action before executing the request.
  void processUseItemRequest();
  void processCastRequest();

  // Dereferences target_pointer and checks to make sure it is in range
  // The allow_state_change variable is not set, this effect cannot have
  // a delayed effect.
  // The repeat_delay value should be configured based on the source
  // that is generating the magic and the avatar's stats.  For example,
  // a spell should pass in the spell's repeat delay times the avatar's
  // magic delay multiplier.  An item should probably just pass a constant
  // value.  Other magics, such as those from geosids, are also likely
  // constant.
  // The casting level is limited for spells at a certain point, so although
  // is is usually the caster's level, this value can be less.
  void initiateUseMagic(const Evidyon::Magic::Magic* magic,
                        Avatar::AvatarLevel casting_level,
                        Actor::ActorAction cast_action,
                        Time::Milliseconds repeat_delay,
                        Number mp_cost,
                        Number x,
                        Number z,
                        unsigned int parameter,
                        ActorInstance* my_actor,
                        ActorPointer target_pointer,
                        bool allow_state_change);

  // Changes the avatar to the attack type for the weapon it has equipped.  All
  // necessary data is brought into the attack parameters from the avatar's
  // description.
  // This is called not only by processAttackRequest, but also when the
  // client is in an attack state and changes its equipment.
  void stateChangeAvatarAttackType();

  // Sets the "advance projectiles" flag, then returns the next available
  // projectile.  If no projectile is unused, this method returns NULL.
  PlayerProjectileInstance* acquireProjectileInstance(
    const Projectile::Projectile* data);

  // If the given projectile is valid, this method will send event-termination
  // messages to the nearby clients and it will disable the projectile.
  void releaseProjectileInstance(PlayerProjectileInstance* projectile);

  // Sets the "advance portals" flag, then returns the next available portal.
  // If none are available, NULL is returned.
  PlayerPortalInstance* acquirePortalInstance(const Magic::Magic_Portal* data);

  // If the given portal instance is valid, this method terminates the portal's
  // events on the client-side, invalidates the portal and disables its timers.
  void releasePortalInstance(PlayerPortalInstance* portal);

  // Erases all portals that are self-only; this is called when the player dies
  void releaseSelfOnlyPortals();

  // Erases the instance and sends a terminate packet
  void releaseDOTAOEBuff();

  PlayerDOTInstance* acquireDOTInstance(const Magic::Magic_DOT* data);
  void releaseDOTInstance(PlayerDOTInstance* instance);

  // Searches for targets when casting the given AOE.  If false is returned,
  // no effect was created.
  bool findAOETargets(const Magic::AOEMagicData* data,
                      WorldRegion* region,
                      ActorInstance* target,
                      Number target_x,
                      Number target_z,
                      ActorPointer* targeted_actors,
                      int* number_of_targeted_actors,
                      Event::EventIndex per_target_event);

private:

  // These structures are saved in the client (so that they can be updated
  // with the database as necessary) but are affected by the player's
  // actions.
  Client* client_;
  ActorPointer actor_;
  PlayerRequest* request_;
  AvatarInstance* avatar_;
  AvatarEnchantments* enchantments_;
  AvatarStats* stats_;
  ClientItemsInterface* items_;
  ClientToClientItemTrade* trade_;

  // What the player is currently doing.
  State state_;

  // Saves the actor's location on the map.  This variable is handled by the
  // update method.
  const MapLocationInfo* map_location_info_;

  // Saves the attacking-players flag
  bool attacking_players_;

  struct {
    Number x, z;
    Number dx, dz;
    Time::Timer<Time::RESOLUTION_1_MS> stop_timer;
    bool swimming; // flying > levitating > swimming, falling > walking
  } movement_;

  struct {
    const Evidyon::Magic::Magic* magic;
    unsigned int parameter;
    int stage;
    Number target_x, target_z;
    Avatar::AvatarLevel casting_level;

    // This timer advances a spell's current stage.
    //  single-stage, standard spell:  apply effect
    //  multi-projectile spell:  generate next projectile
    //  multi-stage spell:  erase primary stage (effect expired)
    Time::Timer<Time::RESOLUTION_1_MS> increment_stage_timer;

    // This instance holds the unusual type of projectile that
    // is used as the delivery method for a spell.  This handles,
    // for example, a spell which you cast and swaps positions of
    // you and whoever it hits, or a spell that when cast allows you
    // to cast again and move to its position.
    // This projectile is always destroyed if the spell changes.
    PlayerProjectileInstance projectile_delivery;

    // This list saves the targets of the last AOE or branching magic effect
    // so that if the client casts an effect repeatedly (which is common)
    // the server doesn't have to go searching for targets every time.  The
    // algorithm simply validates the targets in this list, incrementing the
    // next targeted actor each time.  If a target is found to be invalid,
    // the algorithm stops using the old targets and searches for new ones
    // again.
    ActorPointer targeted_actors[Magic::MAX_TARGETS];
    int next_targeted_actor;
  } magic_;

  struct {
    Number speed;
    float first_hit_damage_bonus;
  } attack_run_;


  struct {
    Number speed;
    float first_hit_damage_bonus;
    //Time::Timer<Time::RESOLUTION_1_MS> reset_leap_flag_timer;
  } leap_;

  struct {
    ActorPointer target;
    Number follow_range;
    Number attack_range;
    Time::Timer<Time::RESOLUTION_1_MS> begin_attack_timer;
    Time::Timer<Time::RESOLUTION_1_MS> end_attack_timer;
    PlayerAttackEffect effect;
  } attack_;

  struct {
    bool falling; // flying > levitating > falling
    Time::Timer<Time::RESOLUTION_100_MS> respawn_timer;
  } falling_;

  struct {
    bool resurrected;  // when setting this flag, clear the respawn timer
    Time::Timer<Time::RESOLUTION_100_MS> respawn_timer;
  } dead_;

  // This structure handles a non-natural mutation of the player's avatar,
  // such as becoming a cloud of gas or turning into a scorpion.  Such
  // mutations offer benefits and changes to the player that override
  // or modify the player's natural stats.
  struct {
    bool mutated; // set to recognize mutation
    bool can_speak;
    bool can_cast_spells;
    bool can_melee_attack;
    bool hidden_from_monsters;
    bool flying, levitating;
    bool show_equipped[Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS];
    Actor::ActorProfileIndex actor_profile;
    SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form;
    Number movement_speed;      // speed += this
    Number melee_effect_speed;  // melee effect delay *= 1 - this
    Number action_speed;        // action delay *= 1 - this
    Number armor;               // total armor += this
    Time::Timer<Time::RESOLUTION_100_MS> expiration_timer;
  } mutation_;

  struct {
    Time::Timer<Time::RESOLUTION_500_MS> logoff_timer;
  } leave_world_;

  // Controls the delay between repeated voluntary actions
  Time::Timer<Time::RESOLUTION_1_MS> next_action_timer_;

  // Regenerate every time this timer goes off
  Time::Timer<Time::RESOLUTION_500_MS> regenerate_timer_;

  // A player can't be portaled until this timer expires.  It is set for a
  // second or two when the player goes through a portal or changes maps
  // so that they can't be port-PKed by someone creating a portal on one
  // of their marks, by someone creating a portal at the bottom of stairs
  // or other similar tricks.
  // This also prevents a player from triggering a player-made trap.
  Time::Timer<Time::RESOLUTION_1_MS> next_triggered_event_timer_;

  // Holds all data for the client's open portals.  These need to be updated
  // in every loop.
  bool advance_portals_;
  PlayerPortalInstance portals_[Magic::PORTALS_PER_PLAYER];

  // All projectiles the user has created go here.  They should be advanced
  // and collided each update.
  bool advance_projectiles_;
  PlayerProjectileInstance projectiles_[Magic::PROJECTILES_PER_PLAYER];

  // All the traps this client has set
  bool advance_traps_;
  PlayerTrapInstance traps_[Magic::TRAPS_PER_PLAYER];

  // The client's active cloud effect
  bool advance_dotaoebuff_;
  PlayerDOTAOEBuffInstance dotaoebuff_;

  // All the traps this client has set
  bool advance_dots_;
  PlayerDOTInstance dots_[Magic::DOTS_PER_PLAYER];
};



}
}

#endif