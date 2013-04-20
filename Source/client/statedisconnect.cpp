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
#include "stdafx.h"




//-----------------------------------------------------------------------------
// Name:  stateDisconnect
// Desc:  
//-----------------------------------------------------------------------------
void EvidyonClient::stateDisconnect()
{
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;
    // We only need to run this state if the server is still valid
    if (!enet_server_) return;

    // Holds the event pulled by the ENet library
    ENetEvent netEvent;

    // Disconnect from the server
    enet_peer_disconnect(enet_server_, 0);

    // Allow up to 3 seconds for the disconnect to succeed, and drop any packets
    // that are received in the mean time
    bool disconnected = false;
    while(!disconnected && (enet_host_service(enet_client_, &netEvent, 3000) > 0))
    {
        switch(netEvent.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(netEvent.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                disconnected = true;
                //DEBUG_INFO("Disconnection succeeded");
                break;
        }
    }

    // The disconnect attempt hasn't succeeded yet, so force the connection to close
    if (!disconnected)
        enet_peer_reset(enet_server_);

    // Exit
    enet_server_ = NULL;
}