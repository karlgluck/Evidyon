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
#ifndef __ACTOR_H__
#define __ACTOR_H__
#pragma once


#include "actorcontroller.h"
#include "shared/networkdecl.h"
#include "shared/actor/actorsync.h"
#include "clientconfig.h"
#include "shared/actor/evidyon_actor.h"

#include "shared/actor/actorsyncdata.h"
#include "shared/actor/actorfastsync.h"
#include "shared/actor/actorslowsync.h"
#include "shared/actor/actordescription.h"
#include "shared/actor/actoraction.h"
#include "common/number.h"

#include "shared/event/eventindex.h"
#include "shared/event/eventid.h"
#include "shared/time/milliseconds.h"

#include "shared/world/mapindex.h"
#include "common/fracture/todo.h"

namespace Evidyon {


class ActorGroup;
class WorldRegion;
class Map;
namespace Server {
struct MapLocationInfo;
}
namespace Network {
namespace Packet {
struct ActorDescriptionData;
}
}
struct ServerActorDescriptionTemplate;
}

namespace Evidyon {


// add a "client owner" parameter and this structure could easily be adapted
// to allow enchanted/pet monsters!  this also streamlines giving normal
// characters exp from killing monsters, and redirects exp from pets to their
// owner.  if ai lifeforms search for enemies of their own faction, then to
// enchant an actor, all that would ahve to be done is give it a client owner
// and change its faction to be "good"!



// Represents a living, interactive part of the world.  Monsters, players,
// NPCs, merchants, etc. all use this class to represent themselves.  This is
// the most generic form of those types.
// An actor is a "dumb" class that is entirely beholden to its controller. 
// It does nothing other than maintain its own state, acccept modifications
// to it and ease the reporting and tabulation of values.  Be careful!
class ActorInstance {
public:
  static const Time::Milliseconds MAX_ACTION_DURATION = 650;
  enum Faction {
    GOOD,
    NEUTRAL,
    EVIL,
    GOOD_NPC,
    NEUTRAL_NPC,
    EVIL_NPC,
    CHAOTIC,
  };

private:
  struct UpdateMask {
    unsigned position : 1;
    unsigned __void : 26;
  };

  // These methods are called by the actor manager only.  The actor will enter
  // the local region upon calling this method.
  friend class GlobalActorManager;
  void acquire(ActorController* controller,
               int unique_id,
               Map* map,
               float x,
               float y);

  // Remove from all partitions
  void release();

public:

  // Synchronizes the state of this actor.  This method sends appropriate
  // state-change packets to clients as necessary.  Should be called by
  // the actor's controller class.
  // The coordinate parameters passed here are distinct from those passed
  // into the setPosition method.  These represent the actor's new interpolated
  // position, moved forward at the current speed.  setPosition(), on the other
  // hand, indicates a change in this movement and will generate an actor update
  // packet.
  // The 'left' and 'entered' parameters indicate world regions that this
  // actor is no longer a part of and has now entered, respectively.  This
  // method will automatically update the GlobalActorManager with its
  // region membership, and it is up to the controller to update its own.
  // The 'new_location' stores information about the new map location that the
  // actor occupies.  It can have special properties (such as being a linked
  // location) that need to be handled by the controller.
  // Returns 'true' if the region changed
  bool update(Number x, Number y,
              WorldRegion** left, int* number_left,
              WorldRegion** entered, int* number_entered);

  // This method checks for pure enemy and doesn't analyze group membership
  bool isEnemyOf(ActorInstance* actor) const;

  // Determines whether or not the given actor can be seen by this actor.  This
  // method should always be called on the client's actor.
  bool canSee(ActorInstance* actor) const;

  // Output:
  //  actor pointers in "actors_found"
  // Returns:
  //  The number of actors found, (<= to buffer_size)
  //int findAllNearbyActors(float radius_squared, ActorInstance** actors_found, int buffer_size);
  //int findGroupMembersNear(float x, float y, float radius_squared, ActorInstance** actors_found, int buffer_size);
  int findNearbyActorsInGroup(float radius_squared, ActorInstance** actors_found, int buffer_size);
  int findNearbyEnemies(float radius_squared, ActorInstance** actors_found, int buffer_size);
  int findEnemiesNear(float x, float y, float radius_squared, ActorInstance** actors_found, int buffer_size);
  int findEnemiesNear(ActorInstance* actor, float radius_squared, ActorInstance** actors_found, int buffer_size);

  void broadcastEvent_GenerateBasicProjectile(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Number angle,
    Number x,
    Number z);
  void broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Actor::ActorID source_actor,
    Actor::ActorID target_actor);
  void broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Actor::ActorID source_actor,
    Number target_x,
    Number target_z,
    Actor::ActorID* target_actors,
    size_t number_of_targets);
  void broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Number source_x,
    Number source_z,
    Number target_x,
    Number target_z,
    Actor::ActorID* target_actors,
    size_t number_of_targets);
  void broadcastEvent_Terminate(Event::EventID event_id);

  NetworkPacket* getDestroyActorPacket() const;
  void broadcastDestroyActorPacket();

  // This method creates a packet containing the full description of just this actor.
  NetworkPacket* getDescriptionPacket();
  void fillDescription(Evidyon::Actor::ActorDescription* description);

  // Sends out an updated description packet for this actor to its local region
  void broadcastUpdateDescriptionPacket();

  inline ActorController* getController();

  Map* getMap() const;
  World::MapIndex getMapIndex() const;
  const Server::MapLocationInfo* getMapLocationInfo() const;

  // Finds a region and adds this actor to it if the current region is invalid.
  void enterWorld();

  // Places text above this actor's head
  void say(const char* speech) const;

  // Places text above this actor's head that is intended to be heard by a
  // specific actor (usually a client's actor).  The text can be very long,
  // since the client will break it into phrases automatically.  Furthermore,
  // the client that is supposed to hear the text will *always* hear the
  // full speech (even if another say() from this actor comes along).
  void sayToActor(Evidyon::Actor::ActorID target_actor, const char* speech) const;


public:
  inline bool objectsCanMoveThrough() const; // returns true if the actor is dead, in spirit form, etc...
  inline bool isDead() const;
  inline bool isAlive() const;
  inline bool isInvisible() const;
  inline WorldRegion* getRegion();
  inline WorldRegion* getRegion() const;
  inline ActorGroup* getGroup();
  inline void getPosition(float* x, float* y) const;
  inline void getPosition(Number* x, Number* y) const;
  inline float getDirection() const;
  inline float distanceToSq(const ActorInstance* other) const;
  inline float distanceToSq(float x, float y) const;
  inline Number distanceToSq(Number x, Number z) const;
  inline Actor::ActorID getIDNumber() const;
  inline void tellName(Client* client);
  inline bool inSameGroupAs(ActorInstance* actor) const;
  inline void setLevel(Number level);
  inline Number getLevel() const;
  inline Number changeHP(ActorInstance* source, bool magic, Number amount);
  inline bool actorIsOutOfRange(ActorInstance* actor) const;
  inline bool locationIsOutOfRange(Number x, Number z) const;
  inline bool isOnSameMapAs(ActorInstance* other);

  // Changes the ActorGroup that this actor is a member of.  If the actor
  // is a member of a group currently, its membership is revoked.  If
  // the new group is valid (non-NULL), it is added to that group.
  bool setGroup(ActorGroup* group);

  // Gets the integral part of this actor's position
  void getMapLocation(int* x, int* y) const;

  // An actor doesn't immediately move to the next region when it crosses the
  // region's boundary.  This is so that an actor doesn't flip back and forth
  // between regions when making small movements.  Sometimes, though, it is
  // necessary to know *exactly* which region the actor is in.
  // For example, use this method when dropping items.
  // The output values are provided for reference, since they have to be found
  // in any case.
  // All parameters must be valid.  Be sure to check *region to see if it is
  // NULL before using.
  void getStrictRegion(int* map_x, int* map_y, WorldRegion** region);

  inline float getSize() const;
  inline void setSize(float size);

  // if the return value is < 0, the target's bounding circle is within
  // 'range' units of this actor's bounding circle.  'range' == 0.0f for
  // the check contact versions.  The return value has only relative
  // meaning; greater values indicate further distances
  inline float checkContactWith(float x, float y, float radius);
  inline float checkContactWith(const ActorInstance* other_actor);
  inline float scaleWithinRangeOf(float x, float y, float radius, float range);
  inline float scaleWithinRangeOf(const ActorInstance* other_actor, float range);

  // Finds out how much distance separates this actor's bounding circle and the
  // bounding circle of the target location (or actor).  These methods use a fast
  // approximation that is guaranteed to always return correct relative
  // magnitudes, but only gives accurate distances when the locations are
  // separated by an amount less than their combined radii.
  // For large separations, the returned distance is further than the actual
  // distance.
  inline float separationDistanceTo(float x, float y, float radius);
  inline float separationDistanceTo(const ActorInstance* other_actor);

public:

  // called before adding to a client's sync packet.  very fast comparison
  // that has no branch statements and uses the packed integer coordinates
  // in the actors' sync-data
  inline bool withinViewOf(const ActorInstance* client_actor) const;

public: // should only be used by the actor's controller
  inline void setPosition(float x, float y);
  inline void teleportTo(float x, float y);
  inline void changeMap(Map* map, float x, float y);
  inline void changeMap(Map* map, Number x, Number z);
  inline void setDirection(float direction);
  inline void setAction(Actor::ActorAction action, Time::Milliseconds duration);
  inline void setMemberOfFaction(Faction faction);
  inline void setAlive() { alive_ = true; setLivingFlag(true); }
  inline void setDead() { alive_ = false; setLivingFlag(false); }

  void faceLocation(float x, float y);
  void faceLocation(Number x, Number z);
  void faceActor(ActorInstance* actor);

  float angleTo(float x, float y) const;
  float angleTo(const ActorInstance* other) const;
#pragma deprecated(angleTo)

  // Returns 'true' if the strict bounding-circle-overlap collision should be
  // ignored because this actor is passing the other and not trying to move
  // through it.
  bool isGlancingCollision(
    const ActorInstance* bounding_overlap_collision,
    Number dx,
    Number dz) const;

  // Gets the position change of the actor after a given amount of time
  void getPositionDelta(double delta_time, float* dx, float* dy);
#pragma deprecated(getPositionDelta)

protected:
  void reset(); // Called by owner when it acquires this structure

private:
  inline void initSyncFlags(int sync_group);
  inline void resetSyncFlags(int sync_group);


private:

  // The class that handles how this actor behaves, its stats, etc.
  ActorController* controller_;

  // A bit is set in this mask for each type of modification to the actor
  // so that an update message can be sent to clients informing them of
  // the actors' changed attributes.
  UpdateMask update_mask_;

  // how large this actor is, radially
  float size_;

  bool alive_;
  float x_, y_; // the actor's location in the world; copied to sync_data_ on update()
  float direction_;
  Number level_;

  // the map that the actor should be a part of.  this can be out-of-sync with
  // the local region (i.e. map doesn't contain region) for at most 1 update.
  Map* map_;  // considered authoratative
  WorldRegion* local_region_; // The region that the actor is directly inside

  // Classes of which this actor is a member
  Faction faction_;
  ActorGroup* group_;



public:
  // call before getSyncData to get the flags for a given group
  void setSyncGroup(int sync_group);
  void getSlowSyncData(Evidyon::Actor::ActorSlowSync* slow_sync);
  void getFastSyncData(Evidyon::Actor::ActorFastSync* fast_sync);
  const Evidyon::Actor::ActorSyncData* getSyncData();

  // changes various actor-state flags
  inline void setLivingFlag(bool living);
  inline void setMovingFlag(bool moving);
  inline void setCombatFlag(bool combat);
  inline void setTeleportFlag();
  inline void setWadingFlag(bool wading);
  inline void setFallingFlag(bool falling);
  inline void setSwimmingFlag(bool swimming);
  inline void setFlyingLevitatingFlag(bool flying_levitating);
  inline void setLeapingFlag(bool leaping);
  inline void setBloodFlag();

private:
  Evidyon::Actor::ActorID id_;

  // Data that defines this actor's state when updates are sent to a client.
  Evidyon::Actor::ActorSyncData sync_data_;

  // The group at index SYNC_GROUPS is the fastsync group.  It is only used
  // if this actor is fastsynced with a client.
  Evidyon::Actor::ActorSyncFlags sync_flags_[Evidyon::ClientConfig::SYNC_GROUPS+1];

  // The data in this structure is used to build the sync flags' "movement" member.
  // Since all of these flags are pretty much independent of one another,
  // it would be difficult to find a simple way to describe the actor's
  // state without a lot of complex logic going on whenever the state changes.
  // This makes the state changes much more simple, but makes the sync-flag
  // building a bit more complex.
  // The 'teleport' member is reset as the flags are "consumed" (passed to sync
  // group packets) since it should only affect one movement cycle, and it
  // dominates all other flags.
  struct SyncFlagInfo {
    bool living,    // not living? action = dead, movement = idle
         teleport,  // instead of interpolating, jump; dominant, auto-reset
         leaping,   // player is jumping; overrides all below, combat=true
         flying_levitating, // overrides falling, swimming, wading -> walking
         falling,   // falling down a pit; dominant, manual reset
         moving,    // not idle
         swimming,  // swimming.  no combat version.
         wading,    // if moving, movement is wading or wade-attack
         combat;    // turns any movement type -> attack run type
    bool blood;

    // next default action to perform
    Actor::ActorAction action;
    Time::Milliseconds action_duration;
         
    // note: flying and levitating are not states!  they are just walking
    // when normally one could not walk (over a pit, over water...)
  } sync_flags_info_[Evidyon::ClientConfig::SYNC_GROUPS+1];
};


#include "actor_inline.h"

}

#endif