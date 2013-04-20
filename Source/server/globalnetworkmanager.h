//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __GLOBALNETWORKMANAGER_H__
#define __GLOBALNETWORKMANAGER_H__
#pragma once


#include "client.h"
#include "shared/networkdecl.h"


namespace Evidyon {

namespace Network {
static unsigned __int8 NORMAL_PACKET_CHANNEL = 0;
static unsigned __int8 ASYNC_PACKET_CHANNEL = 1;
}

class GlobalClientManager;

class GlobalNetworkManager {
public:
  static GlobalNetworkManager* singleton();

public:
  bool create(size_t max_clients, unsigned short port);
  void destroy();

  // Handles all incoming network messages
  void update(GlobalClientManager* client_manager);

  void broadcast(NetworkPacket* packet);
  void broadcastOrphanedPacket(NetworkPacket* packet);

  // Processes incoming network messages, but only allows disconnections
  // to occur.  All other message types are ignored.  This is used when
  // the server is shutting down.
  void updateDisconnectionsOnly(GlobalClientManager* client_manager);

public:
  static void trimPacket(NetworkPacket* packet, int new_byte_length);
  static void sendPacket(Client* client, NetworkPacket* packet);
  static void sendPacket(NetworkPeer* peer, NetworkPacket* packet);
  static void sendAsyncPacket(Client* client, NetworkPacket* packet);
  static void sendAsyncPacket(NetworkPeer* peer, NetworkPacket* packet);
  static void handleOrphanedPacket(NetworkPacket* packet);

private:
  NetworkHost* host_;

private:
  static GlobalNetworkManager* singleton_;
};


}

#endif