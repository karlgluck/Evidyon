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
#ifndef __WORLDREGION_H__
#define __WORLDREGION_H__
#pragma once


#include "shared/evidyon_world.h"
#include <set>
#include "shared/networkdecl.h"
#include "shared/actor/actorid.h"
#include <map>
#include "worldregion_items.h"
#include <dcx/bitmask>
#include "itempointer.h"
#include "itemidtype.h"
#include "shared/client-server/bazaar.h" // for CurrencyType

#include "server/actorpointer.h"

#include "common/number.h"
#include "shared/event/eventindex.h"
#include "shared/event/eventid.h"

namespace Evidyon {
namespace Server {
struct MapLocationInfo;
class GuildInstance;
}
}

namespace Evidyon {

class ActorInstance;
class Client;
class Map;
struct ClearItemsOnMapLocation;
class ClientItemsInterface;




//----[  WorldRegion  ]--------------------------------------------------------
class WorldRegion {
public:
  typedef std::set<ActorInstance*> ActorSet;
  typedef std::set<Client*> ClientSet;
  typedef WorldRegion_Sublocation Sublocation;
  typedef WorldRegion_LocalItems LocalItems;
  typedef std::map<Sublocation, CurrencyType> CurrencyOnGround;

public:
  WorldRegion() {}
  ~WorldRegion() {}

  // Does a simple comparison to see if the given point is inside of
  // this region.
  inline bool containsPoint(float x, float y) const;

  // Determines if the given coordinate is exactly containsed by this region
  inline bool strictlyContainsPoint(int x, int y) const;

  // Determines whether or not this region completely contains the
  // given area, or whether it could possibly overlap another.  If
  // this returns 'true', a comparison with that area only has to
  // compare itself with entries inside of this region.
  // TODO: Right now this method isn't implemented, and always
  // returns false.
  inline bool uniquelyContainsArea(float x, float y, float radius_sq) const;

  inline void addReference();
  inline void removeReference();

  inline World::RegionID getRegionID() const;
  inline int getRegionX() const;
  inline int getRegionY() const;



  // Call this when an actor enters the world for the first time and is
  // located within this region
  void enterWorld(ActorInstance* actor);
  void enterWorld(ActorInstance* actor, WorldRegion** regions_entered, int* number_of_regions_entered);

  // Call this when a client enters the world for the first time and is
  // located within this region
  void enterWorld(Client* client);
  void enterWorld(Client* client, WorldRegion** regions_entered, int* number_of_regions_entered);

  // Call these when a structure that is currently located in this region
  // leaves the world completely
  void leaveWorld(ActorInstance* actor);
  void leaveWorld(ActorInstance* actor, WorldRegion** regions_left, int* number_of_regions_left);
  void leaveWorld(Client* client);

  // Finds out which regions change when an actor moves from one part of the
  // world to another, and updates the actor's membership in those regions.
  void moveToRegion(ActorInstance* actor, WorldRegion* new_region,
          WorldRegion** regions_left, int* number_of_regions_left,
          WorldRegion** regions_entered, int* number_of_regions_entered);

  // Changes the region that a client is a member of.  Since clients are
  // attached to an actor, this should be called after the actor's moveToRegion
  // has found the appropriate region membership changes.
  void changeRegionTo(Client* client, WorldRegion* new_region,
                      WorldRegion** regions_left, int number_of_regions_left,
                      WorldRegion** regions_entered, int number_of_regions_entered);


  // This method will send packets to the client of the activities of actors
  // that are local to this region.  This method should be called for each
  // newly visible region when moving between regions.
  //void sendActivityOfLocalActorsToClient(Client* client);

  // Sends the full description of all nearby neighbors to the given client. 
  // This should be called on a client's local region when it enters the
  // world for the first time.
  //void sendFullDescriptionOfNearbyActorsToClient(Client* client);

  void tellNearbyActorFullDescriptions(Evidyon::Actor::ActorID* actor_ids,
                                       size_t number_of_actors,
                                       Client* client);

  // Sends all of the descriptions of client actors in this region and its
  // neighbors to the client.  This is done when logging in or doing long
  // teleports to preemptively deliver data the client will need.
  void tellAllNearbyClientFullDescriptions(Client* client);

  // Called repeatedly to add items that have been dropped due to a monster
  // dying, a character dropping items, etc.  If the return value is false,
  // then the given coordinates are outside of this region or the item is
  // invalid.
  // Parameters:
  //  important - if this is set, the item should stay on the ground for the
  //              maximum amount of time
  bool addItemOnGround(bool important, int x, int y, ItemPointer& item);
  void broadcastUpdateItemsOnLocation(int x, int y);
  void sendLocalItemsOnGroundToClient(Client* client);

  // Adds the items at (x, y) to the client's inventory.  If this can't be done
  // because there is not enough room, the method returns false.  If there are
  // no items on this location, *currency is left unchanged so be sure to
  // initialize it to zero.
  bool pickUpItems(int x,
                   int y,
                   ClientItemsInterface* client_items,
                   CurrencyType* currency);

  void clearItemsOnMapLocation(const ClearItemsOnMapLocation* clear_struct);

  // Drops currency.  Will causes a map location update if there wasn't already
  // currency on that location.
  void addCurrencyOnGround(int x, int y, CurrencyType amount);


public:

  // Returns a list of less than buffer_size actors in actors_found that are
  // members region, enemies of the enemies_of_actor and within
  // sqrt(radius_squared) of (x,y).
  int findActorEnemiesNearby(ActorInstance* enemies_of_actor,
                             float x,
                             float y,
                             float radius_squared,
                             ActorInstance** actors_found,
                             int buffer_size);

  bool findMonsterClientTarget(ActorInstance* monster_actor,
                               float x,
                               float y,
                               ActorPointer* client_actor);

  bool findTownGuardEvilTarget(ActorInstance* guard_actor,
                               float x,
                               float y,
                               ActorPointer* evil_actor);

  bool findGeosidGuardianTarget(ActorInstance* guardian_actor,
                                float x,
                                float y,
                                Server::GuildInstance* guild_not_to_attack,
                                ActorPointer* actor);

  // Returns a list of less than buffer_size actors in actors_found of actors
  // that are within sqrt(radius_squared) of the actor's position
  int findActorsNearby(ActorInstance* actor,
                       float radius_squared,
                       ActorInstance** actors_found,
                       int buffer_size);

  // Gets the actor closest to the argument within the given radius
  bool findClosestActor(ActorInstance* actor,
                        float search_radius_squared,
                        ActorInstance** actor_found);

  // Returns whether or not the path from a to b is clear on both sides
  // of collision with an actor within 'path_radius', excluding the
  // ignored actors.
  bool pathIsClearOfActors(Number start_x,
                           Number start_z,
                           Number end_x,
                           Number end_z,
                           Number path_radius,
                           ActorInstance* actors_to_ignore[2]);

  // Obtains an actor's instance if it within this region or one of its
  // neighbors.  If the ID number is invalid or the actor is not nearby,
  // false is returned.
  bool getNearbyActor(Actor::ActorID actor_id, ActorPointer* actor);

  // Searches the actor manager to see if a projectile has collided with any
  // ActorInstance.  A projectile must be heading *toward* an ActorInstance for it to be
  // considered colliding.  This way, launched projectiles won't collide
  // with the ActorInstance spawning them.
  ActorInstance* collideProjectileWithActor(float x, float y, float next_x, float next_y);

  inline Map* getMap();

  // Obtains the map location info for the tile that is half a tile in the
  // current direction of the actor or less ahead.
  const Server::MapLocationInfo* getNextActorMapLocationInfo(Number x,
                                                             Number z,
                                                             Number dx,
                                                             Number dz);


  // TODO: actors already within the radius of this actor are pushed away
  ActorInstance* collideActorWithActor(ActorInstance* actor, float x, float y, float next_x, float next_y);

  // Finds a client that is not the searching client that is in the 'finding trade' state.
  Client* findNearbyClientFindingTrade(Client* searching_client,
                                       ActorInstance* client_actor);

  // Finds a client that is not the searching client that is in the 'finding trade' state.
  Client* findNearbyClientInvitingGuildMembers(Client* searching_client,
                                               ActorInstance* client_actor);

  // Searches for actors to target with an AOE spell or effect.
  void findAOETargets(
    Number x,
    Number z,
    Number radius_sq,
    ActorInstance* source,
    bool target_source,
    bool target_friends,
    bool target_enemies,
    bool target_players,
    ActorPointer* actors_found,
    unsigned int max_actors_to_find,
    int* number_of_actors_found);

  // Searches for actors to target with a spin.  This is different because the
  // effect uses the separation distance around the source, instead of just a
  // radial distance from a point.  This is because the weapon's range works
  // differently.
  // This method automatically skips friends, targets enemies, will not hit the
  // source, and will find players based on the flag.
  void findSpinAttackTargets(
    ActorInstance* source,
    Number range,
    bool target_players,
    ActorPointer* actors_found,
    unsigned int max_actors_to_find,
    int* number_of_actors_found);

public:

  // Sends a packet to all of the nearby clients
  void broadcastNetworkPacket(NetworkPacket* packet);

  // Same as broadcastNetworkPacket, but will release the packet if it has no
  // references left.
  void broadcastOrphanedNetworkPacket(NetworkPacket* packet);

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

public:
  inline ActorSet::iterator localActorsBegin();
  inline ActorSet::iterator localActorsEnd();
  inline ActorSet::iterator nearbyActorsBegin();
  inline ActorSet::iterator nearbyActorsEnd();
  inline ClientSet::iterator localClientsBegin();
  inline ClientSet::iterator localClientsEnd();
  inline ClientSet::iterator nearbyClientsBegin();
  inline ClientSet::iterator nearbyClientsEnd();

  // Finds out how many clients are somewhere around this region.  This is
  // primarily used to find out when this region should be deallocated.
  // The number of local clients is necessairily less than or equal to
  // the returned value.
  inline size_t numberOfNearbyClients() const;

  // Finds out how many actors are in a region near this one
  inline size_t numberOfNearbyActors() const;

  // When this method returns false, it is safe to assume that anything in
  // this region is not visible to a client
  inline bool neighboringRegionsHaveNoNearbyClients() const;

  // Returns 'true' if this region still has items on it.  It is important
  // not to erase a region until all of the items have been freed, because
  // otherwise death stacks often disappear instantaneously.
  /*inline*/ bool containsNoItems() const;


friend class GlobalWorldManager;
protected:

  // Initializes this region.  Any non-NULL neighbor must be an unlinked
  // world region bordering this one.
  void acquire(Map* map,
               WorldRegion* neighbors[8],
               short region_x,
               short region_y,
               World::RegionID region_id);

  // Extracts this region from its neighbors.  Should only be called once
  // this region has no local entries; i.e. all actors, clients, etc. should
  // have removed themselves from this region.
  void release();


// Internal-only methods
private:

  // Does all of the initialization to link this region to a neighbor.  This
  // method will insert 'neighbor' in the internal neighbors list, and
  // increment reference counts and number_of_neighbors_ for both classes
  // appropriately.  Additionally, this method calls copyNearbyRegionLocalsFrom
  // to add entries to the local_* sets.
  void linkNewNeighbor(WorldRegion* neighbor);

  // Puts all of the "local" members of the source region into the "nearby"
  // members of this region.
  // This method is used when adding a new neighbor.
  void copyNearbyRegionLocalsFrom(WorldRegion* source);

  // Erases the given neighbor from this region's neighbor list.  The return
  // value is the number of references to this neighbor that were erased,
  // and should always be 1 if the program is working correctly.
  int removeNeighbor(WorldRegion* neighbor);

  void addClearItemsOnLocationRequest(Sublocation& local_coord, bool important);

  // Returns a number unique to the particular combination of items present
  // on this map location at the given time.  This method returns 0 only if
  // there are no items on the location.
  ItemIDType calculateLocationItemsHash(Sublocation& local_coord);



private:
  Map* map_;  // the map that this region is a part of
  World::RegionID region_id_;
  float center_x_, center_y_; // World-coordinates of the region's center
  int region_x_, region_y_; // Region-coordinates of this region
  int references_;
  int left_, top_;

  WorldRegion* neighbors_[8];
  int number_of_neighbors_;

  ActorSet local_actors_;   // Actors inside of this region
  ActorSet nearby_actors_;  // Actors in this region or neighbors
  ClientSet local_clients_;   // Clients in this region
  ClientSet nearby_clients_;  // Clients in this region or neighbors

  // This set of items maps subregion coordinate offsets
  LocalItems local_items_;

  // This map stores where currency has been dropped on the map.  It was an
  // active decision NOT to store this like items.
  // Note: this structure isn't cleared until the region is deallocated.  This
  // has the cool side-effect of making gold indicative of how recently a
  // region has been active.
  CurrencyOnGround currency_on_ground_;
};


#include "worldregion_inline.h"


#endif