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
#ifndef __GLOBALCLIENTMANAGER_H__
#define __GLOBALCLIENTMANAGER_H__
#pragma once


#include <dcx/memorypool>
#include "shared/networkdecl.h"
#include <vector>
#include "clientconfig.h"

namespace Evidyon {

class Client;
class WorldRegion;
class Map;
class GlobalDatabaseManager;

class GlobalClientManager {
public:
  static const int MAX_CLIENTS = 64;

private:
  typedef dcx::dcxMemoryPool<Client, MAX_CLIENTS> ClientMemoryPool;
  typedef std::vector<Client*> ClientsInWorld;

public:
  static GlobalClientManager* singleton();

public:
  GlobalClientManager();
  // Sets up the manager class
  bool create();
  void destroy();
  Client* acquire(NetworkPeer* peer);
  void wasDisconnected(Client* client);
  void enteredWorld(Client* client);
  void leftWorld(Client* client);
  void update(double time,
              double time_since_last_update);
  bool accountIsActive(unsigned int account_id);

  void fastSync(unsigned int group, double time_since_last_sync);
  void slowSync(unsigned int group, double time_since_last_sync);

  // Writes information about this class useful to a debugging the server.  This data
  // is returned to the admin console.
  void printDebugStats(char* buffer, size_t buffer_size);

  void logSnapshot();

public: // admin commands and utility methods
  void sendToAll(NetworkPacket* packet);
  void respawnAll();
  void disconnectAll();
  bool getCoordinatesOf(const char* avatar_name, Map** map, float* x, float* y);
  bool getClientOnAccount(unsigned int account_id, Client** client);
  bool getClientOnCharacter(unsigned int character_id, Client** client);

  // sends messages to the client about all clients that are in the world
  void admin_tellOnline(Client* admin_client);

  // Syncs all active client data with the database so that it can be
  // backed up with the current state of the world.
  void commitAllToDatabase(GlobalDatabaseManager* db);


private:
  ClientMemoryPool client_memory_pool_;

  Client* clients_in_world_[MAX_CLIENTS];
  size_t number_of_clients_in_world_;

  int next_fast_sync_id_;

private:
  static GlobalClientManager* singleton_;
};

}

#endif