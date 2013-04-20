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
#include "credits.h"



//----[  stateConnect  ]-------------------------------------------------------
void EvidyonClient::stateConnect() {
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;

  // Get the list of server IPs
  const char* serverAddress = config_.getSetting("Network", "Server1", "localhost");

  ENetAddress netAddress;
  ENetEvent netEvent;

  // Connect to the server
  enet_address_set_host(&netAddress, serverAddress);
  netAddress.port = Evidyon::Network::SERVER_PORT;

  Sleep(150); // Wait a little bit (just incase the server is restarting, so we don't get in by accident)
 
  // Initiate the connection with the remote host
  if (APP_ERROR(NULL == (enet_server_ = enet_host_connect(enet_client_, &netAddress, 2)))
    ("No available peers for initiating an ENet connection")) {
    myStateMachine.jumpState(VCS_SHUTDOWN);
    return;
  }

  // Display the main window
  ShowWindow(myMainWindow, SW_SHOWDEFAULT);
  UpdateWindow(myMainWindow);


  GUIFont font(EvidyonGUIDefs::WINDOW_FONT_TEXTURE_INDEX);
  if (APP_ERROR(!font.create(d3d_device_, "Tahoma", 10, true))("Couldn't create font")) return;
  gui_primary_layer_.setTexture(font.getTextureID(), font.getTexture());


  EvidyonGUIButton quit;
  {
    quit.setFont(&font);
    quit.setText("Exit");
    quit.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    quit.getCanvas()->alignXLeft(+10);
    quit.getCanvas()->alignYBottom(-10);
    quit.getCanvas()->absoluteHeight(35);
    quit.getCanvas()->absoluteWidth(200);
    quit.getCanvas()->absoluteDepth(0.5f);
    quit.registerElements(&gui_primary_layer_);
    quit.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
    quit.enable();
  }

  // If we fail to connect to the server, go to the correct state
  myStateMachine.jumpState(VCS_SHUTDOWN);

  // Wait for a connection reply from the server
  dcxWin32Clock clock;
  double originalStartTime = clock.getAccurateSystemTime();
  double startTime = clock.getAccurateSystemTime() - 10.0, currentTime = startTime, lastTime = startTime;
  while(windowsMessagePump() && !quit.wasPushed()) {

    // Get the current time
    currentTime = clock.getAccurateSystemTime();
    int number_of_dots = (int)floorf(2.0f*(float)(currentTime - originalStartTime));

    // Check to see if we got a reply
    if ((enet_host_service(enet_client_, &netEvent, 0) > 0) &&
        netEvent.type == ENET_EVENT_TYPE_CONNECT) {

      // Go to the next state
      myStateMachine.jumpState(VCS_ACCOUNT_LOGIN);

      // Exit, and proceed to the next state
      break;

    } else if (currentTime - startTime > 2.0) {
      // Try connecting again
      enet_peer_reset(enet_server_);
      if (APP_ERROR(NULL == (enet_server_ = enet_host_connect(enet_client_, &netAddress, 2)))
          ("No available peers for initiating an ENet connection")) {
        // Exit
        myStateMachine.jumpState(VCS_SHUTDOWN);
        return;
      }
      startTime = currentTime;
    }

    if (allowFrameToBeRendered(dcxWin32Clock::getAccurateSystemTime())) {

      // Clear the backbuffer and the zbuffer
      d3d_device_->Clear(
        0,
        NULL,
        D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
        D3DCOLOR_XRGB(0,0,0),
        1.0f,
        0);

      if (beginScene()) {

        CONFIRM(gui_primary_layer_.render());

        renderLoginBackground();

        renderstate_manager_.setStage0Texture(window_font_.getTexture());
        window_font_.renderText(&gui_primary_layer_,
                                NULL,
                                20.0f,
                                20.0f,
                                0.1f,
                                "(connecting to server...)",
                                -1,
                                D3DCOLOR_XRGB(255,255,255));
        GUIPoint pt;
        window_font_.centerTextInArea(Evidyon::Client::CREDITS_TEXT, -1, gui_primary_layer_.getPrimaryCanvas()->getArea(), &pt);
        window_font_.renderText(
          &gui_primary_layer_,
          NULL,
          pt.x,
          pt.y,
          0.1f,
          Evidyon::Client::CREDITS_TEXT,
          -1,
          D3DCOLOR_XRGB(255,255,255));
        gui_primary_layer_.flush();

        // End the scene
        endScene(VCS_CONNECT);
      }
    }
  }

  quit.unregisterElements(&gui_primary_layer_);
  gui_primary_layer_.removeTexture(font.getTextureID());
  font.destroy();
}