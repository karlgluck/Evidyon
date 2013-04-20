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
#include "globalclientmanager.h"
#include "globalnetworkmanager.h"
#include "../common/null.h"
#include "worldregion.h"
#include "shared/evidyon_network.h"
#include "clientconfig.h"


// This is necessary for slowsync flag grouping.  It would be nice to get rid
// of this dependancy.
#include "globalactormanager.h"

#include "globalgamefilemanager.h"
#include "globaldatabasemanager.h"


#include <math.h>
#include <dc/debug>

#include "uniqueavatarnamechecker.h"

// for admin commands
#include "encodepackets.h"
#include "globalworldmanager.h"

#include <windows.h> // for Sleep

#include "common/fracture/todo.h"


namespace Evidyon {


GlobalClientManager* GlobalClientManager::singleton_ = NULL;

GlobalClientManager* GlobalClientManager::singleton() {
  return singleton_;
}

GlobalClientManager::GlobalClientManager() {
  number_of_clients_in_world_ = 0;
}

bool GlobalClientManager::create() {
  if (!client_memory_pool_.create()) return false;
  singleton_ = this;
  memset(clients_in_world_, 0, sizeof(clients_in_world_));
  number_of_clients_in_world_ = 0;
  next_fast_sync_id_ = 0;

  return true;
}

void GlobalClientManager::destroy() {
  client_memory_pool_.destroy();
  singleton_ = NULL;
}

Client* GlobalClientManager::acquire(NetworkPeer* peer) {
  Client* client = client_memory_pool_.acquire();
  if (client == NULL) return NULL;
  client->acquire(peer);
  return client;
}



//----[  wasDisconnected  ]----------------------------------------------------
void GlobalClientManager::wasDisconnected(Client* client) {
  client->release();
  client_memory_pool_.release(client);
}



//----[  enteredWorld  ]-------------------------------------------------------
void GlobalClientManager::enteredWorld(Client* client) {
  clients_in_world_[number_of_clients_in_world_++] = client;
}



//----[  leftWorld  ]----------------------------------------------------------
void GlobalClientManager::leftWorld(Client* client) {
  const size_t number_of_clients_in_world = number_of_clients_in_world_;
  for (size_t i = 0; i < number_of_clients_in_world; ++i) {
    if (clients_in_world_[i] == client) {
      clients_in_world_[i] = clients_in_world_[--number_of_clients_in_world_];
      clients_in_world_[number_of_clients_in_world_] = NULL;
      next_fast_sync_id_ = 0; // reset to prevent a 1-cycle glitch in client stats due to swapping
      return;
    }
  }
}





//----[  update  ]-------------------------------------------------------------
void GlobalClientManager::update(double time, double time_since_last_update) {
  using namespace ::Evidyon::ClientConfig;

  // iterate down through the list so that removals don't mess up this iteration
  for (size_t i = number_of_clients_in_world_; i > 0; ) {
    Client* client = clients_in_world_[--i];
    client->update(time,
                   time_since_last_update);
  }
}


bool GlobalClientManager::accountIsActive(unsigned int account_id) {
  for (ClientMemoryPool::Iterator i(&client_memory_pool_);
                  i.hasNext();
                  i.advance()) {
  Client* client = i.get();
  if (client->getLoggedIntoAccountID() == account_id)
    return true;
  }

  return false;
}




//----[  fastSync  ]-----------------------------------------------------------
void GlobalClientManager::fastSync(unsigned int group,
                                   double time_since_last_sync) {
  for (size_t i = 0; i < number_of_clients_in_world_; ++i/*i += Evidyon::ClientConfig::SYNC_GROUPS*/) {
    Client* client = clients_in_world_[i];
    if (group == (client_memory_pool_.indexOf(client)%Evidyon::ClientConfig::SYNC_GROUPS)) {
      client->sendFastSync(time_since_last_sync, next_fast_sync_id_);
    }
  }
  next_fast_sync_id_ += (group == 0) ? 1 : 0;
}




//----[  slowSync  ]-----------------------------------------------------------
void GlobalClientManager::slowSync(unsigned int group,
                                   double time_since_last_sync) {
  { // Change the actor sync group
    GlobalActorManager* actor_manager = GlobalActorManager::singleton();
    actor_manager->setSyncGroup(group);
  }
  for (size_t i = group; i < number_of_clients_in_world_; i += Evidyon::ClientConfig::SYNC_GROUPS) {
    Client* client = clients_in_world_[i];
    client->sendSlowSync(time_since_last_sync);
  }
}





//----[  printDebugStats  ]----------------------------------------------------
void GlobalClientManager::printDebugStats(char* buffer, size_t buffer_size) {
  sprintf_s(buffer,
            buffer_size,
            "%lu clients allocated (%lu available); %lu in world",
            client_memory_pool_.numberOfAllocatedEntries(),
            client_memory_pool_.numberOfFreeEntries(),
            number_of_clients_in_world_);
}




//----[  logSnapshot  ]--------------------------------------------------------
void GlobalClientManager::logSnapshot() {
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client = clients_in_world_[i];
    client->logSnapshot();
  }
}




//----[  sendToAll  ]----------------------------------------------------------
void GlobalClientManager::sendToAll(NetworkPacket* packet) {
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client = clients_in_world_[i];
    client->sendAsyncPacket(packet);
  }
  GlobalNetworkManager::singleton()->handleOrphanedPacket(packet);
}



//----[  respawnAll  ]---------------------------------------------------------
void GlobalClientManager::respawnAll() {
  GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
  float x, y, evil_x, evil_y;
  World::MapIndex map, evil_map;
  gamefile_manager->getPlayerRespawnLocation(false, &map, &x, &y);
  gamefile_manager->getPlayerRespawnLocation(true, &evil_map, &evil_x, &evil_y);

  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client = clients_in_world_[i];
    if (client->isEvil()) {
      client->moveTo(evil_map, evil_x, evil_y);
    } else {
      client->moveTo(map, x, y);
    }
  }
}



//----[  disconnectAll  ]------------------------------------------------------
void GlobalClientManager::disconnectAll() {
  commitAllToDatabase(GlobalDatabaseManager::singleton());

  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client = clients_in_world_[i];
    client->sendDisconnectionPacket();
  }

  GlobalNetworkManager* network_manager = GlobalNetworkManager::singleton();
  int time_waiting = 0;
  static const int MAXIMUM_MS_TO_WAIT = 5000;
  static const int SLEEP_MS_PER_CYCLE = 25;
  while ((false == client_memory_pool_.empty()) &&
         (time_waiting++ < MAXIMUM_MS_TO_WAIT/SLEEP_MS_PER_CYCLE)) {
    Sleep(SLEEP_MS_PER_CYCLE);
    network_manager->updateDisconnectionsOnly(this);
  }

  // Everyone should now be disconnected
  CONFIRM(client_memory_pool_.empty());
  CONFIRM(number_of_clients_in_world_ == 0);
}




//----[  getCoordinatesOf  ]---------------------------------------------------
bool GlobalClientManager::getCoordinatesOf(const char* avatar_name, Map** map, float* x, float* y) {
  std::string reduced_name;
  UniqueAvatarNameChecker::reduceName(avatar_name, &reduced_name);
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    const Client* client = clients_in_world_[i];
    if (client->loggedIntoAvatar(reduced_name)) {
      const ActorInstance* actor = client->getActor()->dereference();
      CONFIRM(actor) else return false;
      Map* actor_map = actor->getMap();
      if (!actor_map) return false;
      actor->getPosition(x, y);
      *map = actor_map;
      return true;
    }
  }
  return false;
}

bool GlobalClientManager::getClientOnAccount(unsigned int account_id, Client** client) {
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client_in_world = clients_in_world_[i];
    if (client_in_world->getLoggedIntoAccountID() == account_id) {
      *client = client_in_world;
      return true;
    }
  }
  return false;
}


//----[  getClientOnCharacter  ]-----------------------------------------------
bool GlobalClientManager::getClientOnCharacter(unsigned int character_id, Client** client) {
  todo("karl","optimize getClientOnCharacter search");
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client_in_world = clients_in_world_[i];
    if (client_in_world->getLoggedIntoCharacterID() == character_id) {
      *client = client_in_world;
      return true;
    }
  }
  return false;
}




//----[  admin_tellOnline  ]---------------------------------------------------
void GlobalClientManager::admin_tellOnline(Client* admin_client) {
  using namespace ::Evidyon::Network::Packet::Server;
  char message[256];
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client_in_world = clients_in_world_[i];
    const ActorInstance* actor = client_in_world->getActor()->dereference();
    World::MapIndex map;
    float x, y;
    Map* actor_map = NULL;
    if (actor) actor_map = actor->getMap();
    if (actor_map) {
        actor->getPosition(&x, &y);
        map = actor_map->getIndex();
    } else {
      map = -1;
      x = 0.0f;
      y = 0.0f;
    }
    sprintf_s(message,
              256,
              "%7lu - %9lu - %s (%2lu: %.02f, %.02f)",
              client_in_world->getLoggedIntoAccountID(),
              client_in_world->getLoggedIntoCharacterID(),
              client_in_world->getName(),
              map, x, y);
    admin_client->sendPacket(Encode::admin_serverMessage(message));
  }

  sprintf_s(message, 256, "%lu client(s) online", number_of_clients_in_world_);
  admin_client->sendPacket(Encode::admin_serverMessage(message));
}


void GlobalClientManager::commitAllToDatabase(GlobalDatabaseManager* db) {
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client_in_world = clients_in_world_[i];
    client_in_world->syncToDatabase(db);
  }
}


}