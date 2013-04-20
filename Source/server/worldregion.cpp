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
#include "worldregion.h"
#include <dc/debug>
#include "../common/null.h"
#include "globalworldmanager.h"
#include "actor.h"
#include "globalnetworkmanager.h"
#include "shared/actor/actordescription.h"
#include "globalactormanager.h"
#include "encodepackets.h"
#include "shared/client-server/packets.h"
#include "item.h"
#include "globalitemmanager.h"
#include "clientitemsinterface.h"
#include <assert.h>

#include "shared/server-editor/server_gamefile.h"
#include "globalgamefilemanager.h" // for the gold scenery

#include "common/fracture/todo.h"

namespace Evidyon {

static const int MAX_ITEMS_PICKED_UP_AT_ONCE = 6;



//----[  containsNoItems  ]----------------------------------------------------
bool WorldRegion::containsNoItems() const {
  return local_items_.empty();
}


//----[  acquire  ]------------------------------------------------------------
void WorldRegion::acquire(Map* map,
                          WorldRegion* neighbors[8],
                          short region_x,
                          short region_y,
                          World::RegionID region_id) {
  map_ = map;
  region_x_ = region_x;
  region_y_ = region_y;
  region_id_ = region_id;
  center_x_ = region_x * World::REGION_SIZE + 0.5f * World::REGION_SIZE;
  center_y_ = region_y * World::REGION_SIZE + 0.5f * World::REGION_SIZE;
  left_ = region_x * World::REGION_SIZE;
  top_  = region_y * World::REGION_SIZE;

  // add all of the valid neighbors
  references_ = 0;
  number_of_neighbors_ = 0;
  for (int i = 0; i < 8; ++i) {
    if (neighbors[i] == NULL) continue;
    linkNewNeighbor(neighbors[i]);
  }
  assert(local_actors_.empty());
  assert(local_clients_.empty());
}




//----[  release  ]------------------------------------------------------------
void WorldRegion::release() {
  CONFIRM(local_actors_.empty() && local_clients_.empty()) else {
    local_actors_.clear();
    local_clients_.clear();
  }

  // Remove currency from the ground
  currency_on_ground_.clear();

  { // Erase items from this region
    GlobalItemManager* item_manager = GlobalItemManager::singleton();
    item_manager->releaseWorldRegionItems(&local_items_);
  }

  // Remove neighbor links
  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    neighbors_[i] = NULL;
    CONFIRM(neighbor->removeNeighbor(this) == 1) else continue;
    neighbor->removeReference();
  }

  nearby_actors_.clear();
  nearby_clients_.clear();

  number_of_neighbors_ = 0;
}






//----[  enterWorld  ]---------------------------------------------------------
void WorldRegion::enterWorld(ActorInstance* actor) {
  local_actors_.insert(actor);
  nearby_actors_.insert(actor);

  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    neighbors_[i]->nearby_actors_.insert(actor);
  }
}





//----[  enterWorld  ]---------------------------------------------------------
void WorldRegion::enterWorld(ActorInstance* actor,
                             WorldRegion** regions_entered,
                             int* number_of_regions_entered) {
  local_actors_.insert(actor);
  nearby_actors_.insert(actor);

  const int number_of_neighbors = number_of_neighbors_;
  *number_of_regions_entered = 1 + number_of_neighbors;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    regions_entered[i] = neighbor;
    neighbor->nearby_actors_.insert(actor);
  }
  regions_entered[number_of_neighbors] = this;
}





//----[  enterWorld  ]---------------------------------------------------------
void WorldRegion::enterWorld(Client* client) {
  local_clients_.insert(client);
  nearby_clients_.insert(client);

  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    neighbor->nearby_clients_.insert(client);
  }
}





//----[  enterWorld  ]---------------------------------------------------------
void WorldRegion::enterWorld(Client* client,
                             WorldRegion** regions_entered,
                             int* number_of_regions_entered) {
  local_clients_.insert(client);
  nearby_clients_.insert(client);

  const int number_of_neighbors = number_of_neighbors_;
  *number_of_regions_entered = number_of_neighbors + 1;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    regions_entered[i] = neighbor;
    neighbor->nearby_clients_.insert(client);
  }
  regions_entered[number_of_neighbors] = this;
}



//----[  leaveWorld  ]---------------------------------------------------------
void WorldRegion::leaveWorld(ActorInstance* actor) {
  assert(local_actors_.find(actor) != local_actors_.end());
  local_actors_.erase(actor);
  nearby_actors_.erase(actor);

  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    assert(neighbors_[i]->nearby_actors_.find(actor) !=
           neighbors_[i]->nearby_actors_.end());
    neighbors_[i]->nearby_actors_.erase(actor);
  }
}



//----[  leaveWorld  ]---------------------------------------------------------
void WorldRegion::leaveWorld(ActorInstance* actor,
                             WorldRegion** regions_left,
                             int* number_of_regions_left) {
  assert(local_actors_.find(actor) != local_actors_.end());
  local_actors_.erase(actor);
  nearby_actors_.erase(actor);

  const int number_of_neighbors = number_of_neighbors_;
  *number_of_regions_left = 1 + number_of_neighbors;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    regions_left[i] = neighbor;
    neighbor->nearby_actors_.erase(actor);
  }
  regions_left[number_of_neighbors] = this;
}




//----[  leaveWorld  ]---------------------------------------------------------
void WorldRegion::leaveWorld(Client* client) {
  assert(local_clients_.find(client) != local_clients_.end());
  local_clients_.erase(client);
  nearby_clients_.erase(client);

  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    neighbors_[i]->nearby_clients_.erase(client);
  }
}





//----[  moveToRegion  ]-------------------------------------------------------
void WorldRegion::moveToRegion(ActorInstance* actor, WorldRegion* new_region,
                               WorldRegion** regions_left, int* number_of_regions_left,
                               WorldRegion** regions_entered, int* number_of_regions_entered) {

  // Change local region
  local_actors_.erase(actor);
  new_region->local_actors_.insert(actor);

  typedef std::set<WorldRegion*> WorldRegionSet;

  // Build a set of the new region's neighbors
  WorldRegionSet new_nearby_regions;
  {
    new_nearby_regions.insert(new_region);
    const int number_of_new_region_neighbors = new_region->number_of_neighbors_;
    WorldRegion** new_region_neighbors = new_region->neighbors_;
    for (int i = 0; i < number_of_new_region_neighbors; ++i) {
      new_nearby_regions.insert(new_region_neighbors[i]);
    }
  }

  // Build a set of our local regions
  WorldRegionSet nearby_regions;
  {
    nearby_regions.insert(this);
    const int number_of_neighbors = number_of_neighbors_;
    WorldRegion** neighbors = neighbors_;
    for (int i = 0; i < number_of_neighbors; ++i) {
      nearby_regions.insert(neighbors[i]);
    }
  }

  // Find entries in the new set that aren't part of the old set
  int nearby_regions_entered = 0;
  for (WorldRegionSet::iterator i = new_nearby_regions.begin();
     i != new_nearby_regions.end(); ++i) {
    WorldRegion* region = *i;
    if (nearby_regions.end() == nearby_regions.find(region)) {
      region->nearby_actors_.insert(actor);
      *(regions_entered++) = region;
      nearby_regions_entered++;
    }
  }
  *number_of_regions_entered = nearby_regions_entered;

  // Find entries in the old set that aren't part of the new set
  int nearby_regions_left = 0;
  for (WorldRegionSet::iterator i = nearby_regions.begin();
     i != nearby_regions.end(); ++i) {
    WorldRegion* region = *i;
    if (new_nearby_regions.end() == new_nearby_regions.find(region)) {
      region->nearby_actors_.erase(actor); ////////////////////////////// fixed this aug 8/09
      *(regions_left++) = region;
      nearby_regions_left++;
    }
  }
  *number_of_regions_left = nearby_regions_left;
}






//----[  changeRegionTo  ]-----------------------------------------------------
void WorldRegion::changeRegionTo(Client* client, WorldRegion* new_region,
                                 WorldRegion** regions_left, int number_of_regions_left,
                                 WorldRegion** regions_entered, int number_of_regions_entered) {

  // Change the local region
  local_clients_.erase(client);
  new_region->local_clients_.insert(client);

  // Switch the nearby regions
  for (int i = 0; i < number_of_regions_left; ++i) {
    regions_left[i]->nearby_clients_.erase(client);
  }
  for (int i = 0; i < number_of_regions_entered; ++i) {
    WorldRegion* region_entered = regions_entered[i];
    region_entered->nearby_clients_.insert(client);
  }
}



//----[  DynamicArrayNearbyActorDescriptionsFunctionoid  ]---------------------
class DynamicArrayNearbyActorDescriptionsFunctionoid
  : public DynamicArray<Evidyon::Actor::ActorDescription>::ElementFunctionoid {
public:
  virtual bool hasMoreElements() {
    return number_of_actors > 0;
  }
  virtual bool fill(size_t count, Evidyon::Actor::ActorDescription* element, bool* failed) {
    if (!element) return true; // terminated serialization? succeed
    if (!number_of_actors) return false;
    ActorInstance* actor = actor_manager->getActor(*actor_ids);
    --number_of_actors;
    if (nearby_actors->find(actor) == nearby_actors_end) {
      return fill(count, element, failed);
    }
    actor->fillDescription(element);
    return true;
  }
  GlobalActorManager* actor_manager;
  Evidyon::Actor::ActorID* actor_ids;
  size_t number_of_actors;
  WorldRegion::ActorSet* nearby_actors;
  WorldRegion::ActorSet::iterator nearby_actors_end;
};


//----[  tellNearbyActorFullDescriptions  ]------------------------------------
void WorldRegion::tellNearbyActorFullDescriptions(
    Evidyon::Actor::ActorID* actor_ids,
    size_t number_of_actors,
    Client* client) {
  using namespace ::Evidyon::Network::Packet;
  DynamicArrayNearbyActorDescriptionsFunctionoid functionoid;
  functionoid.actor_manager = GlobalActorManager::singleton();
  functionoid.actor_ids = actor_ids;
  functionoid.number_of_actors = number_of_actors;
  functionoid.nearby_actors = &nearby_actors_;
  functionoid.nearby_actors_end = nearby_actors_.end();
  while (functionoid.number_of_actors) {
    using namespace Evidyon::Actor;
    client->sendAsyncPacket(
      NetMsg_TellActorFullDescriptions::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        &DynamicArray<ActorDescription>(&functionoid)));
  }
}




//----[  DynamicArrayNearbyClientDescriptionsFunctionoid  ]--------------------
class DynamicArrayNearbyClientDescriptionsFunctionoid
  : public DynamicArray<Evidyon::Actor::ActorDescription>::ElementFunctionoid {
public:
  virtual bool hasMoreElements() {
    return next_nearby_client != nearby_clients_end;
  }
  virtual bool fill(size_t count, Evidyon::Actor::ActorDescription* element, bool* failed) {
    if (!element) return true; // out of room in this buffer? succeed
    if (next_nearby_client == nearby_clients_end) return false; // end list
    Client* client = *next_nearby_client;
    ++next_nearby_client;
    if (client == client_to_ignore) {
      return fill(count, element, failed);
    }
    client->getActor()->dereferenceAssumingValid()->fillDescription(element);
    return true;
  }
  Evidyon::Client* client_to_ignore;
  WorldRegion::ClientSet::iterator next_nearby_client;
  WorldRegion::ClientSet::iterator nearby_clients_end;
};



//----[  tellAllNearbyClientFullDescriptions  ]--------------------------------
void WorldRegion::tellAllNearbyClientFullDescriptions(Client* client) {
  using namespace ::Evidyon::Network::Packet;
  DynamicArrayNearbyClientDescriptionsFunctionoid functionoid;
  functionoid.client_to_ignore = client;
  functionoid.next_nearby_client = localClientsBegin();
  functionoid.nearby_clients_end = localClientsEnd();
  while (functionoid.next_nearby_client != functionoid.nearby_clients_end) {
    using namespace Evidyon::Actor;
    client->sendAsyncPacket(
      NetMsg_TellActorFullDescriptions::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        &DynamicArray<ActorDescription>(&functionoid)));
  }
}


//----[  addItemOnGround  ]----------------------------------------------------
bool WorldRegion::addItemOnGround(bool important, int x, int y, ItemPointer& item_pointer) {
  int local_x = x - left_;
  int local_y = y - top_;
  CONFIRM(local_x >= 0 &&
          local_y >= 0 &&
          local_x < World::REGION_SIZE &&
          local_y < World::REGION_SIZE &&
          item_pointer.invalid() == false) else return false;

  // Add to the region
  Sublocation local_coord(local_x, local_y);
  { // add the item, and be sure it's unique
    LocalItems::value_type value(local_coord, item_pointer);
    value.second.becomeUniquePointer();
    local_items_.insert(value);
  }

  // Add this clear to the global world manager
  addClearItemsOnLocationRequest(local_coord, important);

  return true;
}




//----[  broadcastUpdateItemsOnLocation  ]-------------------------------------
void WorldRegion::broadcastUpdateItemsOnLocation(int x, int y) {
  using namespace ::Evidyon::Network::Packet::Server;
  int local_x = x - left_;
  int local_y = y - top_;
  Sublocation local_coord(local_x, local_y);

  unsigned short item_types[MAX_ITEMS_VISIBLE_PER_LOCATION];
  int number_of_item_types = 0;

  CurrencyOnGround::iterator currency = currency_on_ground_.find(local_coord);
  if (currency != currency_on_ground_.end()) {
    item_types[0] = GlobalGamefileManager::singleton()->getGoldScenery();
    number_of_item_types = 1;
  }

  LocalItems::iterator lower = local_items_.lower_bound(local_coord);
  LocalItems::iterator upper = local_items_.upper_bound(local_coord);
  for (LocalItems::iterator i = lower;
       (i != upper) &&
       (number_of_item_types < MAX_ITEMS_VISIBLE_PER_LOCATION); ++i) {
    ItemInstance* item = i->second.dereference();
    CONFIRM(item) else { // damage control for invalid item
      GlobalItemManager::singleton()->releaseItem(&i->second);
      LocalItems::iterator next = i; ++next;
      local_items_.erase(i);
      i = next;
      continue;
    }
    item_types[number_of_item_types++] = item->getGroundType();
  }

  broadcastOrphanedNetworkPacket(Encode::tellItemsOnMapLocation(x,
                                                                y,
                                                                item_types,
                                                                number_of_item_types));

}




void WorldRegion::sendLocalItemsOnGroundToClient(Client* client) {
  using namespace ::Evidyon::Network::Packet::Server;
  LocalItems::iterator lower = local_items_.begin();
  LocalItems::iterator upper = local_items_.end();

  LocalItems::iterator i = lower;
  while (i != upper) {
    Sublocation current_local_coord = i->first;

    unsigned short item_types[MAX_ITEMS_VISIBLE_PER_LOCATION];
    int number_of_item_types = 0;
    { // add currency
      CurrencyOnGround::iterator currency = currency_on_ground_.find(current_local_coord);
      if (currency != currency_on_ground_.end()) {
        item_types[0] = GlobalGamefileManager::singleton()->getGoldScenery();
        number_of_item_types = 1;
      }
    }
    while ((i != upper) &&
           (current_local_coord == i->first) &&
           (number_of_item_types < MAX_ITEMS_VISIBLE_PER_LOCATION)) {
      ItemInstance* item = i->second.dereference();
      CONFIRM(item) else {
        todo("karl", "could do fixing (release item) here too");
        return;
      }
      item_types[number_of_item_types++] = item->getGroundType();
      ++i;
    }

    // skip past any other entries here
    if (number_of_item_types >= MAX_ITEMS_VISIBLE_PER_LOCATION) {
      while ((i != upper) && (current_local_coord == i->first)) ++i;
    }

    // calculate this location's actual coordinates
    int x = current_local_coord.first + left_;
    int y = current_local_coord.second + top_;

    // send the location's items
    client->sendAsyncPacket(Encode::tellItemsOnMapLocation(x,
                                                           y,
                                                           item_types,
                                                           number_of_item_types));
  }

  // send updates for items that have money but no regular items
  for (CurrencyOnGround::iterator i = currency_on_ground_.begin();
       i != currency_on_ground_.end(); ++i) {
    if (local_items_.find(i->first) != local_items_.end()) continue;

    // send the location's currency
    unsigned short currency_item_type
      = GlobalGamefileManager::singleton()->getGoldScenery();
    client->sendAsyncPacket(Encode::tellItemsOnMapLocation(i->first.first + left_,
                                                           i->first.second + top_,
                                                           &currency_item_type,
                                                           1));
  }
}






//----[  pickUpItems  ]--------------------------------------------------------
bool WorldRegion::pickUpItems(int x,
                              int y,
                              ClientItemsInterface* client_items,
                              CurrencyType* currency) {
  int local_x = x - left_;
  int local_y = y - top_;
  Sublocation local_coord(local_x, local_y);
  LocalItems::iterator lower = local_items_.lower_bound(local_coord);
  LocalItems::iterator upper = local_items_.upper_bound(local_coord);
  bool items_changed = false; // whether or not items were picked up

  bool return_value = true; // inventory could hold items?

  if (lower != upper) {
    int items_picked_up = 0;
    LocalItems::iterator i = lower;
    while ((i != upper) &&
           (items_picked_up < MAX_ITEMS_PICKED_UP_AT_ONCE)) {
      ItemPointer& local_item = i->second;
      if (APP_ERROR(local_item.invalid())("Invalid item on ground at %i, %i", x, y)) {
        items_changed = true;
        //LocalItems::iterator next = i; ++next;
        //local_items_.erase(i);
        //i = next;
        ++i; // this causes the i to be erased below
        break;
      }
      bool given = client_items->giveItem(&i->second, NULL);
      if (false == given) { return_value = false; break; }

      // move to the next item
      ++i;
      ++items_picked_up;
      items_changed = true;
    }

    // Erase the items that were picked up
    local_items_.erase(lower, i);
  }

  {
    // Get currency here
    CurrencyOnGround::iterator i = currency_on_ground_.find(local_coord);
    if (i != currency_on_ground_.end()) {
      *currency = i->second;
      currency_on_ground_.erase(i);
      items_changed = true;
    }
  }

  if (items_changed) {
    broadcastUpdateItemsOnLocation(x, y);

    WorldRegion_Sublocation local_coord(local_x, local_y);
    addClearItemsOnLocationRequest(local_coord, true);
  }

  return return_value;
}


void WorldRegion::clearItemsOnMapLocation(const ClearItemsOnMapLocation* clear_struct) {
  WorldRegion_Sublocation local_coord(clear_struct->x - left_, clear_struct->y - top_);
  LocalItems::iterator lower = local_items_.lower_bound(local_coord);
  LocalItems::iterator upper = local_items_.upper_bound(local_coord);
  if (lower != upper) {
    ItemIDType hash = calculateLocationItemsHash(local_coord);
    if (hash == clear_struct->sum_of_unique_ids) {
      GlobalItemManager::singleton()->releaseWorldRegionItems(&local_items_, lower, upper);

      // erase all items on the clients
      using namespace Evidyon::Network::Packet::Server;
      broadcastOrphanedNetworkPacket(Encode::tellItemsOnMapLocation(left_ + local_coord.first,
                                                                    top_ + local_coord.second,
                                                                    NULL,
                                                                    0));
    }
  }
}

void WorldRegion::addCurrencyOnGround(int x, int y, CurrencyType amount) {
  if (amount == 0) return; // don't drop nothing, that doesn't make sense
  WorldRegion_Sublocation local_coord(x - left_, y - top_);
  CurrencyOnGround::iterator i = currency_on_ground_.find(local_coord);
  if (i == currency_on_ground_.end()) {
    currency_on_ground_.insert(CurrencyOnGround::value_type(local_coord, amount));
    broadcastUpdateItemsOnLocation(x, y);
  } else {
    i->second += amount;
  }
}




//----[  findActorEnemiesNearby  ]---------------------------------------------
int WorldRegion::findActorEnemiesNearby(ActorInstance* enemies_of_actor,
                                        float x,
                                        float y,
                                        float radius_squared,
                                        ActorInstance** actors_found,
                                        int buffer_size) {
  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  int enemies_found = 0;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
       (i != end) &&
       (enemies_found < buffer_size); ++i) {
    ActorInstance* actor = *i;
    if ((actor != enemies_of_actor) &&
        actor->isAlive() &&
        actor->isEnemyOf(enemies_of_actor) &&
        actor->distanceToSq(x, y) < radius_squared) {
      *(actors_found++) = actor;
      ++enemies_found;
    }
  }
  return enemies_found;
}




//----[  findMonsterClientTarget  ]--------------------------------------------
bool WorldRegion::findMonsterClientTarget(ActorInstance* monster_actor,
                                          float x,
                                          float y,
                                          ActorPointer* client_actor) {
  WorldRegion::ClientSet::iterator end = nearbyClientsEnd();
  for (WorldRegion::ClientSet::iterator i = nearbyClientsBegin();
      (i != end); ++i) {
    Client* client_check = *i;
    ActorInstance* check_actor =
      client_check->getActor()->dereferenceAssumingValid();
    if (check_actor->isAlive() &&
        !check_actor->actorIsOutOfRange(monster_actor)) {
      client_actor->copy(client_check->getActor());
      return true;
    }
  }
  return false;
}




//----[  findTownGuardEvilTarget  ]--------------------------------------------
bool WorldRegion::findTownGuardEvilTarget(ActorInstance* guard_actor,
                                          float x,
                                          float y,
                                          ActorPointer* evil_actor) {
  // search for an evil client before an evil monster
  {
    WorldRegion::ClientSet::iterator end = nearbyClientsEnd();
    for (WorldRegion::ClientSet::iterator i = nearbyClientsBegin();
        (i != end); ++i) {
      Client* client_check = *i;
      ActorInstance* client_actor =
        client_check->getActor()->dereferenceAssumingValid();
      if (client_actor->isAlive() &&
          client_check->isEvil() &&
          !client_actor->actorIsOutOfRange(guard_actor)) {
        evil_actor->copy(client_check->getActor());
        return true;
      }
    }
  }
  // now search for any evil thing
  {
    WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
    for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
        (i != end); ++i) {
      ActorInstance* actor_check = *i;
      if (!actor_check->isDead() &&
          !actor_check->getController()->isClient() &&
          actor_check->isEnemyOf(guard_actor) &&
          !actor_check->actorIsOutOfRange(guard_actor)) {
        GlobalActorManager::singleton()->getPointer(actor_check, evil_actor);
        return true;
      }
    }
  }
  return false;
}




//----[  findGeosidGuardianTarget  ]-------------------------------------------
bool WorldRegion::findGeosidGuardianTarget(ActorInstance* guardian_actor,
                                           float x,
                                           float y,
                                           Server::GuildInstance* guild_not_to_attack,
                                           ActorPointer* actor) {

  // search for a client not in the excluded guild
  {
    WorldRegion::ClientSet::iterator end = nearbyClientsEnd();
    for (WorldRegion::ClientSet::iterator i = nearbyClientsBegin();
        (i != end); ++i) {
      Client* client_check = *i;
      ActorInstance* client_actor =
        client_check->getActor()->dereferenceAssumingValid();
      if (client_actor->isAlive() &&
          !client_actor->actorIsOutOfRange(guardian_actor) &&
          (guild_not_to_attack == NULL ||
          client_check->getGuild() != guild_not_to_attack)) {
        actor->copy(client_check->getActor());
        return true;
      }
    }
  }
  // now search for any evil thing
  {
    WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
    for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
        (i != end); ++i) {
      ActorInstance* actor_check = *i;
      if (!actor_check->isDead() &&
          !actor_check->getController()->isClient() &&
          actor_check->isEnemyOf(guardian_actor) &&
          !actor_check->actorIsOutOfRange(guardian_actor)) {
        GlobalActorManager::singleton()->getPointer(actor_check, actor);
        return true;
      }
    }
  }
  return false;
}



//----[  findActorsNearby  ]---------------------------------------------------
int WorldRegion::findActorsNearby(ActorInstance* actor,
                                  float radius_squared,
                                  ActorInstance** actors_found,
                                  int buffer_size) {
  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  int number_of_actors_found = 0;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
       (i != end) &&
       (number_of_actors_found < buffer_size); ++i) {
    ActorInstance* test_actor = *i;
    if ((test_actor != actor) &&
        test_actor->distanceToSq(actor) < radius_squared) {
      *(actors_found++) = test_actor;
      ++number_of_actors_found;
    }
  }
  return number_of_actors_found;
}





//----[  findClosestActor  ]---------------------------------------------------
bool WorldRegion::findClosestActor(ActorInstance* actor,
                                   float search_radius_squared,
                                   ActorInstance** actor_found) {
  float closest_distance_sq = search_radius_squared;
  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  bool actor_was_found = false;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin(); (i != end); ++i) {
    ActorInstance* test_actor = *i;
    if (test_actor == actor) continue;
    float distance_sq = test_actor->distanceToSq(actor);
    if (test_actor->distanceToSq(actor) < closest_distance_sq) {
      *actor_found = test_actor;
      closest_distance_sq = distance_sq;
      actor_was_found = true;
    }
  }
  return actor_was_found;
}




//----[  pathIsClearOfActors  ]------------------------------------------------
bool WorldRegion::pathIsClearOfActors(Number start_x,
                                      Number start_z,
                                      Number end_x,
                                      Number end_z,
                                      Number path_radius,
                                      ActorInstance* actors_to_ignore[2]) {
  Number ox = end_x - start_x;
  Number oz = end_z - start_z;
  Number oo = start_x * end_z - end_x * start_z;
  Number ol = FixedPoint::sqrt(ox * ox + oz * oz);

  Number v_x = end_x - start_x;
  Number v_z = end_z - start_z;
  Number c2 = v_x * v_x + v_z * v_z;

  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  bool actor_was_found = false;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin(); (i != end); ++i) {
    ActorInstance* test_actor = *i;
    if (actors_to_ignore[0] == test_actor ||
        actors_to_ignore[1] == test_actor) {
      continue;
    }
    Number x, z;
    test_actor->getPosition(&x, &z);
    Number w_x = x - start_x;
    Number w_z = z - start_z;
    Number c1 = v_x * w_x + v_z * w_z;
    Number b = c1 / c2;
    Number b_x = start_x + b * v_x;
    Number b_z = start_z + b * v_z;
    Number line_distance_sq
      = (FixedPoint::square(x - b_x) + FixedPoint::square(x - b_z));
    Number collision_distance_sq
      = FixedPoint::square(path_radius + test_actor->getSize());
    if ((c1 > 0) && // inside starting point
        (c2 > c1) && // inside ending point
        (line_distance_sq < collision_distance_sq)) {
      // hit this actor
      float line_distance_sq_f = line_distance_sq.toFloat();
      float collision_distance_sq_f = collision_distance_sq.toFloat();
      return false;
    }
  }

  return true;
}


//----[  getNearbyActor  ]-----------------------------------------------------
bool WorldRegion::getNearbyActor(Actor::ActorID actor_id,
                                 ActorPointer* actor) {
  ActorPointer pointer;
  GlobalActorManager::singleton()->getPointer(actor_id, &pointer);
  if (pointer.invalid()) {
    actor->reset();
  } else {
    if (nearby_actors_.end() ==
        nearby_actors_.find(pointer.dereferenceAssumingValid())) {
      actor->reset();
    } else {
      actor->copy(&pointer);
      return true;
    }
  }
  return false;
}




//----[  collideProjectileWithActor  ]-----------------------------------------
ActorInstance* WorldRegion::collideProjectileWithActor(
  float x, float y, float next_x, float next_y) {
  static const float PROJECTILE_COLLISION_RADIUS_SQ = 0.5f;

  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
     i != end; ++i) {
    ActorInstance* actor = *i;
    if (actor->objectsCanMoveThrough()) continue;
    todo("karl", "this can be optimized by doing integer comparisons, or "\
                 "doing long-range coarse checks.  right now this involves a "\
                 "lot of floating-point multiplications");
    float distance = actor->distanceToSq(next_x, next_y);
    if (distance < PROJECTILE_COLLISION_RADIUS_SQ &&
        distance < actor->distanceToSq(x,y)) {
      return actor;
    }
  }
  return NULL;
}






//----[  getNextActorMapLocationInfo  ]----------------------------------------
const Server::MapLocationInfo* WorldRegion::getNextActorMapLocationInfo(
    Number x,
    Number z,
    Number dx,
    Number dz) {
      /*
  // this was changed 
  x = ((float)x.wholePart()) + (x.fractionPart() - 0.5f) * 0.35f + 0.5f;
  z = ((float)z.wholePart()) + (z.fractionPart() - 0.5f) * 0.35f + 0.5f;
  x += FixedPoint::abs(dx) > FixedPoint::abs(dz) ? ((dx > 0) ? 0.55 : -0.55) : 0;
  z += FixedPoint::abs(dz) > FixedPoint::abs(dx) ? ((dz > 0) ? 0.55 : -0.55) : 0;
  return map_->getMapLocationInfo(x.wholePart(), z.wholePart());*/
  return map_->getMapLocationInfo((x+dx).wholePart(), (z+dz).wholePart());
}



//----[  collideActorWithActor  ]----------------------------------------------
ActorInstance* WorldRegion::collideActorWithActor(ActorInstance* actor,
                                          float x,
                                          float y,
                                          float next_x,
                                          float next_y) {
  //static const float ACTOR_COLLISION_RADIUS_SQ = 0.5f;

  float actor_radius = actor->getSize();

  WorldRegion::ActorSet::iterator begin, end;
  todo("karl", "enable this uniquelyContainsArea check to speed up processing!");
  /*if (uniquelyContainsArea(x, y, actor_radius_sq)) {
    begin = localActorsBegin();
    end = localActorsEnd();
  } else*/ {
    begin = nearbyActorsBegin();
    end = nearbyActorsEnd();
  }

  for (WorldRegion::ActorSet::iterator i = begin; i != end; ++i) {
    ActorInstance* check_collision_actor = *i;
    if (check_collision_actor == actor ||
        check_collision_actor->objectsCanMoveThrough()) continue;
    todo("karl","this can be optimized by doing integer comparisons/"\
                "long-range coarse checks");
    float range = check_collision_actor->checkContactWith(next_x, next_y, actor_radius);
    if (range < 0.0f &&
        range < check_collision_actor->checkContactWith(x, y, actor_radius)) {
      return check_collision_actor;
    }
  }
  return NULL;
}



//----[  findNearbyClientFindingTrade  ]---------------------------------------
Client* WorldRegion::findNearbyClientFindingTrade(Client* searching_client,
                                                  ActorInstance* client_actor) {
  WorldRegion::ClientSet::iterator end = nearbyClientsEnd();
  WorldRegion::ClientSet::iterator i = nearbyClientsBegin();
  for (; i != end; ++i) {
    Client* client = *i;
    if (searching_client->canBeginTradeWith(client)) return client;
  }
  return NULL;
}


// Finds a client that is not the searching client that is in the 'finding trade' state.
Client* WorldRegion::findNearbyClientInvitingGuildMembers(
    Client* searching_client,
    ActorInstance* client_actor) {
  WorldRegion::ClientSet::iterator end = nearbyClientsEnd();
  WorldRegion::ClientSet::iterator i = nearbyClientsBegin();
  for (; i != end; ++i) {
    Client* client = *i;
    if (searching_client->canJoinGuildOf(client)) return client;
  }
  return NULL;
}


//----[  findAOETargets  ]-----------------------------------------------------
void WorldRegion::findAOETargets(
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
    int* number_of_actors_found) {

  WorldRegion::ActorSet::iterator begin, end;
  todo("karl","enable this uniquelyContainsArea check to speed up processing!");
  /*if (uniquelyContainsArea(x, y, actor_radius_sq)) {
    begin = localActorsBegin();
    end = localActorsEnd();
  } else*/ {
    begin = nearbyActorsBegin();
    end = nearbyActorsEnd();
  }

  GlobalActorManager* actor_manager = GlobalActorManager::singleton();
  int actor_index = 0;

  todo("karl","unroll the 'if' statements inside into multiple loops");
  for (WorldRegion::ActorSet::iterator i = begin; i != end; ++i) {
    assert(actor_index < max_actors_to_find);
    ActorInstance* target = *i;
    if (target->isDead() ||
        (!target_source && target == source) ||
        (!target_friends && target->inSameGroupAs(source)) ||
        (!target_enemies && target->isEnemyOf(source)) ||
        (!target_players && target->getController()->isClient())) {
      continue;
    }
    Number target_x, target_z;
    target->getPosition(&target_x, &target_z);
    target_x -= x;
    target_z -= z;
    target_x *= target_x;
    target_z *= target_z;
    if ((target_x + target_z) > radius_sq) continue;

    // we found a match!
    actor_manager->getPointer(target, &actors_found[actor_index]);
    assert(!actors_found[actor_index].invalid());
    if (++actor_index >= max_actors_to_find) break;
  }

  *number_of_actors_found = actor_index;
}



//----[  findSpinAttackTargets  ]----------------------------------------------
void WorldRegion::findSpinAttackTargets(
    ActorInstance* source,
    Number range,
    bool target_players,
    ActorPointer* actors_found,
    unsigned int max_actors_to_find,
    int* number_of_actors_found) {

  WorldRegion::ActorSet::iterator begin, end;
  todo("karl","enable this uniquelyContainsArea check to speed up processing!");
  /*if (uniquelyContainsArea(x, y, actor_radius_sq)) {
    begin = localActorsBegin();
    end = localActorsEnd();
  } else*/ {
    begin = nearbyActorsBegin();
    end = nearbyActorsEnd();
  }

  GlobalActorManager* actor_manager = GlobalActorManager::singleton();
  int actor_index = 0;

  float range_float = range.toFloat();//FixedPoint::square(range).toFloat();

  for (WorldRegion::ActorSet::iterator i = begin; i != end; ++i) {
    assert(actor_index < max_actors_to_find);
    ActorInstance* target = *i;
    if ((target == source) ||
        (target->inSameGroupAs(source)) ||
        (!target_players && target->getController()->isClient())) {
      continue;
    }
    //if (target->isDead() ||
    //target->separationDistanceTo(source) > range_float) continue;
    if (target->isDead() ||
        target->scaleWithinRangeOf(source, range_float) > 0.0) continue;
    //if (target->isDead() ||
    //    target->distanceToSq(source) > range_float) continue;

    // we found a match!
    actor_manager->getPointer(target, &actors_found[actor_index++]);
    if (actor_index >= max_actors_to_find) break;
  }

  *number_of_actors_found = actor_index;
  
}


//----[  broadcastNetworkPacket  ]---------------------------------------------
void WorldRegion::broadcastNetworkPacket(NetworkPacket* packet) {
  WorldRegion::ClientSet::iterator end = nearbyClientsEnd();
  for (WorldRegion::ClientSet::iterator i = nearbyClientsBegin();
      i != end; ++i) {
    Client* client = *i;
    GlobalNetworkManager::sendAsyncPacket(client, packet);
  }
}




//----[  broadcastOrphanedNetworkPacket  ]-------------------------------------
void WorldRegion::broadcastOrphanedNetworkPacket(NetworkPacket* packet) {
  broadcastNetworkPacket(packet);
  GlobalNetworkManager::handleOrphanedPacket(packet);
}


//----[  broadcastEvent_GenerateBasicProjectile  ]-----------------------------
void WorldRegion::broadcastEvent_GenerateBasicProjectile(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Number angle,
    Number x,
    Number z) {
  if (event_index == Evidyon::Event::INVALID_EVENT_INDEX) return;

  // check (again) that the packing doesn't destroy information
  assert(Evidyon::Packets::EventIndexPacketType(event_index) == event_index);

  using namespace Evidyon::Network::Packet;
  static const double TWO_PI = 3.1415926535 * 2;
  //assert(angle.toFloat() >= -TWO_PI / 2.0);
  //assert(angle.toFloat() <=  TWO_PI / 2.0);
  NetworkPacket* packet =
    NetMsg_Event_GenerateBasicProjectile::writePacket(
      ENET_PACKET_FLAG_UNSEQUENCED,
      event_id,
      Evidyon::Packets::EventIndexPacketType(event_index),
      unsigned char((angle / TWO_PI).int_value >> 8),
      unsigned short(unsigned(x.int_value) >> (16 - 5)),
      unsigned short(unsigned(z.int_value) >> (16 - 5)));

  broadcastOrphanedNetworkPacket(packet);
}




//----[  broadcastEvent_GenerateTargetedSpecialFX  ]---------------------------
void WorldRegion::broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Actor::ActorID source_actor,
    Actor::ActorID target_actor) {
  if (event_index == Evidyon::Event::INVALID_EVENT_INDEX) return;

  // check (again) that the packing doesn't destroy information
  assert(Evidyon::Packets::EventIndexPacketType(event_index) == event_index);

  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet =
    NetMsg_Event_GenerateTargetedSpecialFX_SrcToTgt::writePacket(
      ENET_PACKET_FLAG_UNSEQUENCED,
      event_id,
      event_index,
      source_actor,
      target_actor);
  broadcastOrphanedNetworkPacket(packet);
}




//----[  broadcastEvent_GenerateTargetedSpecialFX  ]---------------------------
void WorldRegion::broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Actor::ActorID source_actor,
    Number target_x,
    Number target_z,
    Actor::ActorID* target_actors,
    size_t number_of_targets) {
  if (event_index == Evidyon::Event::INVALID_EVENT_INDEX) return;

  //// THIS ASSERTION KEEPS GETTING HIT WHEN PLAYERS RIGHT-CLICK THE TELEPORTERS
  //assert(Evidyon::Packets::EventIndexPacketType(event_index) == event_index);
  NetworkPacket* packet;

  using namespace Evidyon::Network::Packet;
  if (number_of_targets == 0 || target_actors == NULL) {
    packet =
      NetMsg_Event_GenerateTargetedSpecialFX_TgtXZ::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        event_id,
        event_index,
        source_actor,
        unsigned short(unsigned(target_x.int_value) >> (16 - 5)),
        unsigned short(unsigned(target_z.int_value) >> (16 - 5)));
  } else {
    packet =
      NetMsg_Event_GenerateTargetedSpecialFX_TgtXZAOE::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        event_id,
        event_index,
        source_actor,
        unsigned short(unsigned(target_x.int_value) >> (16 - 5)),
        unsigned short(unsigned(target_z.int_value) >> (16 - 5)),
        &CopiedArray<Actor::ActorID>(target_actors, number_of_targets));
  }
  broadcastOrphanedNetworkPacket(packet);
}


//----[  broadcastEvent_GenerateTargetedSpecialFX  ]---------------------------
void WorldRegion::broadcastEvent_GenerateTargetedSpecialFX(
    Event::EventID event_id,
    Event::EventIndex event_index,
    Number source_x,
    Number source_z,
    Number target_x,
    Number target_z,
    Actor::ActorID* target_actors,
    size_t number_of_targets) {
  if (event_index == Evidyon::Event::INVALID_EVENT_INDEX) return;

  assert(Evidyon::Packets::EventIndexPacketType(event_index) == event_index);
  NetworkPacket* packet;

  using namespace Evidyon::Network::Packet;
  if (number_of_targets == 0 || target_actors == NULL) {
    packet =
      NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZ::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        event_id,
        event_index,
        unsigned short(unsigned(source_x.int_value) >> (16 - 5)),
        unsigned short(unsigned(source_z.int_value) >> (16 - 5)),
        unsigned short(unsigned(target_x.int_value) >> (16 - 5)),
        unsigned short(unsigned(target_z.int_value) >> (16 - 5)));
  } else {
    packet =
      NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZAOE::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        event_id,
        event_index,
        unsigned short(unsigned(source_x.int_value) >> (16 - 5)),
        unsigned short(unsigned(source_z.int_value) >> (16 - 5)),
        unsigned short(unsigned(target_x.int_value) >> (16 - 5)),
        unsigned short(unsigned(target_z.int_value) >> (16 - 5)),
        &CopiedArray<Actor::ActorID>(target_actors, number_of_targets));
  }
  broadcastOrphanedNetworkPacket(packet);
}


//----[  broadcastEvent_Terminate  ]-------------------------------------------
void WorldRegion::broadcastEvent_Terminate(Event::EventID event_id) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet =
    NetMsg_Event_Terminate::writePacket(
      ENET_PACKET_FLAG_UNSEQUENCED,
      event_id);

  broadcastOrphanedNetworkPacket(packet);
}


//----[  linkNewNeighbor  ]----------------------------------------------------
void WorldRegion::linkNewNeighbor(WorldRegion* neighbor) {

#if defined(EXTENDED_DEBUGGING)
  // Make sure this isn't already a neighbor
  for (int i = 0; i < number_of_neighbors_; ++i) {
    CONFIRM(neighbors_[i] != neighbor) else {
      return;
    }
  }
  CONFIRM(neighbor->number_of_neighbors_ < 8) else {
    DEBUG_INFO("%4i %4i X %4i %4i", region_x_, region_y_,
      neighbor->region_x_, neighbor->region_y_);
    return;
  }
#endif

  neighbor->addReference(); // link this->neighbor
  neighbors_[number_of_neighbors_++] = neighbor;

  addReference(); // link neighbor->this
  neighbor->neighbors_[neighbor->number_of_neighbors_++] = this;

  // Get local members
  copyNearbyRegionLocalsFrom(neighbor);
}





//----[  copyNearbyRegionLocalsFrom  ]-----------------------------------------
void WorldRegion::copyNearbyRegionLocalsFrom(WorldRegion* source) {
  nearby_actors_.insert(source->local_actors_.begin(), source->local_actors_.end());
  nearby_clients_.insert(source->local_clients_.begin(), source->local_clients_.end());
}




//----[  removeNeighbor  ]-----------------------------------------------------
int WorldRegion::removeNeighbor(WorldRegion* neighbor) {
  int links_removed = 0;
  for (int i = 0; i < number_of_neighbors_; ++i) {
    if (neighbors_[i] == neighbor) {
      neighbors_[i] = neighbors_[--number_of_neighbors_];
      neighbors_[number_of_neighbors_] = NULL;
      ++links_removed;
      neighbor->removeReference();
    }
  }
  assert(links_removed==1);
  return links_removed;
}




//----[  addClearItemsOnLocationRequest  ]-------------------------------------
void WorldRegion::addClearItemsOnLocationRequest(Sublocation& local_coord, bool important) {

  ItemIDType hash = calculateLocationItemsHash(local_coord);
  if (hash) {
    getMap()->addClearItemsOnLocation(important,
                                      local_coord.first + left_,
                                      local_coord.second + top_,
                                      hash);
  }
}


ItemIDType WorldRegion::calculateLocationItemsHash(Sublocation& local_coord) {
  LocalItems::iterator lower = local_items_.lower_bound(local_coord);
  LocalItems::iterator upper = local_items_.upper_bound(local_coord);

  if (lower != upper) {
    ItemIDType summation = 0;
    for (LocalItems::iterator i = lower; i != upper; ++i) {
      ItemPointer& item_ptr = i->second;
      if (item_ptr.invalid()) {
        GlobalItemManager::singleton()->releaseItem(&i->second);
        LocalItems::iterator next = i; ++next;
        local_items_.erase(i);
        i = next;
      } else {
        ItemInstance* item = item_ptr.dereferenceAssumingValid();
        summation += item->getUniqueID();
        //summation += (summation & 1) ? (item->getUniqueID()) : (~item->getUniqueID());
      }
    }

    // in the very unlikely event that the items' ids hash to 0, this
    // returns non-zero (since a zero hash indicates no items present
    return summation == 0 ? 1 : summation;
  }

  return 0;
}


}