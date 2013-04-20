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
#include "globalnetworkmanager.h"
#include "../common/enet/enet.h"
#include "../common/null.h"
#include "globalclientmanager.h"
#include "../common/enet/enet.h"

#define ADDED_LATENCY_MILLISECONDS  200
#define AVERAGE_LATENCY_MILLISECONDS   250

#include <dc/debug>

namespace Evidyon {

GlobalNetworkManager* GlobalNetworkManager::singleton_ = NULL;

GlobalNetworkManager* GlobalNetworkManager::singleton() {
  return singleton_;
}

bool GlobalNetworkManager::create(size_t max_clients, unsigned short port) {
  // Load the ENet library
  if (APP_FATAL(0 > enet_initialize())("Unable to initialize the network library"))
    return false;

  // Initialize the ENet server object
  {
  // Initialize the host address
  ENetAddress address;
  address.host = ENET_HOST_ANY;
  address.port = (enet_uint16)port;

  // Set up the host server
  if (APP_ERROR(NULL == (host_ = enet_host_create(&address, max_clients, 0, 0)))
      ("An error occurred while trying to create an ENet server host"))
    return false;
  }

  singleton_ = this;

  return true;
}

void GlobalNetworkManager::destroy() {
  if (host_) {
    enet_host_destroy(host_);
    host_ = NULL;
  }
  enet_deinitialize();
}


void GlobalNetworkManager::updateDisconnectionsOnly(GlobalClientManager* client_manager) {
  ENetEvent net_event;
  while (enet_host_service(host_, &net_event, 0) > 0) {
    if (net_event.type == ENET_EVENT_TYPE_DISCONNECT)  {
        Client* client = (Client*)net_event.peer->data;
        if (client) client_manager->wasDisconnected(client);
    } else {
      enet_peer_disconnect(net_event.peer, 0);
    }

    if (net_event.packet) enet_packet_destroy(net_event.packet);
  }
}


void GlobalNetworkManager::update(GlobalClientManager* client_manager) {
  ENetEvent net_event;
  while (enet_host_service(host_, &net_event, 0) > 0) {
    // Almost every event will be the receive type.  To make processing faster, look for it
    // first.
    if (net_event.type == ENET_EVENT_TYPE_RECEIVE) {

      // Get the managing client structure
      Client* client = (Client*)net_event.peer->data;

      // Handle the message if the client is valid
      if (client && client->isValid())
        client->handleNetworkPacket(net_event.packet);
      else
        DEBUG_WARNING("Network receive event without a valid managing client class!");

      // Free packet memory
      enet_packet_destroy(net_event.packet);

    } else {
      switch(net_event.type) {
        // This event occurs when the client is connecting to the server for the first time.
        // IP bannings and other checks should occur here.
        case ENET_EVENT_TYPE_CONNECT:
        {
          // Get a client for this connection
          ENetPeer* peer = net_event.peer;
          Client* client = client_manager->acquire(peer);

          // If a client manager couldn't be found, disconnect this peer
          if (APP_WARNING(!client)("Couldn't get client container for new connection"))
          {
            // Disconnect this peer
            enet_peer_disconnect(peer, 0);

            // Reset this data so that no packets get processed while we're waiting
            // for the client to disconnect
            peer->data = NULL;
          }

        } break;

        // The user has disconnected.  This event shuts down the client, saving any data
        // that is valid to the database.
        case ENET_EVENT_TYPE_DISCONNECT:
        {
          Client* client = (Client*)net_event.peer->data;
          if (client) client_manager->wasDisconnected(client);

        } break;
      }
    }
  }
}


void GlobalNetworkManager::broadcast(NetworkPacket* packet) {
  enet_host_broadcast(host_, Network::ASYNC_PACKET_CHANNEL, packet);
}

void GlobalNetworkManager::broadcastOrphanedPacket(NetworkPacket* packet) {
  broadcast(packet);
  handleOrphanedPacket(packet);
}


  
void GlobalNetworkManager::sendPacket(Client* client, NetworkPacket* packet) {
  enet_peer_send(client->getPeer(), Network::NORMAL_PACKET_CHANNEL, packet);
}

void GlobalNetworkManager::sendPacket(NetworkPeer* peer, NetworkPacket* packet) {
  enet_peer_send(peer, Network::NORMAL_PACKET_CHANNEL, packet);
}

void GlobalNetworkManager::sendAsyncPacket(Client* client, NetworkPacket* packet) {
  enet_peer_send(client->getPeer(), Network::ASYNC_PACKET_CHANNEL, packet);
}

void GlobalNetworkManager::sendAsyncPacket(NetworkPeer* peer, NetworkPacket* packet) {
  enet_peer_send(peer, Network::ASYNC_PACKET_CHANNEL, packet);
}


void GlobalNetworkManager::handleOrphanedPacket(NetworkPacket* packet) {
  if (0 == packet->referenceCount) {
    enet_packet_destroy(packet);
  }
}

}
