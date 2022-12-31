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