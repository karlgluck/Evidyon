//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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