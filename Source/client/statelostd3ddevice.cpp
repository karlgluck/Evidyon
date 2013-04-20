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
#include "stdafx.h"




//-----------------------------------------------------------------------------
// Name:  stateConnect
// Desc:  
//-----------------------------------------------------------------------------
void EvidyonClient::stateLostD3DDevice()
{
    // Get rid of everything
    //global_actor_manager_.onLostDevice();
    renderstate_manager_.resetState();
    freeResources();

    // Wait for a connection reply from the server
    HRESULT hr;
    while(D3DERR_DEVICELOST == (hr = d3d_device_->TestCooperativeLevel()))
    {
        if (!windowsMessagePump())
        {
            myStateMachine.jumpState(VCS_SHUTDOWN);
            return;
        }

        // Poll the connection to keep it alive.  Watch for disconnection.
        ENetEvent netEvent;
        while(enet_host_service(enet_client_, &netEvent, 0) > 0) {
            if (netEvent.type == ENET_EVENT_TYPE_DISCONNECT) {
                myStateMachine.jumpState(VCS_SHUTDOWN);
                return;
            }
            // Free packet memory
            if (netEvent.packet) enet_packet_destroy(netEvent.packet);
        }

        // Don't overload the program
        Sleep(200);
    }

    CONFIRM(hr == D3DERR_DEVICENOTRESET && SUCCEEDED(d3d_device_->Reset(&d3d_params_))) else {
        myStateMachine.jumpState(VCS_SHUTDOWN);
        return;
    }

    // Reset everything
    if (APP_FATAL(!acquireResources())("The game couldn't load its resource file")) {
        myStateMachine.jumpState(VCS_DISCONNECT);
        myStateMachine.jumpState(VCS_SHUTDOWN);
        return;
    }
}