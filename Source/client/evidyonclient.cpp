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
#include "evidyonguidefs.h"
#include "shared/client-server/admin.h" // for admin console commands
#include <assert.h>
#include "shared/item/itemclientdescription.h"
#include "client/clientfacade.h"

#include <shlobj.h> // for finding screenshots directory


//-----------------------------------------------------------------------------
// Name:  EvidyonClient
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
EvidyonClient::EvidyonClient() : myStateMachine(VCS_STARTUP), window_font_(25) {
  myMainWindow = NULL;
  ZeroMemory(&d3d_params_, sizeof(d3d_params_));
  d3d_ = NULL;
  d3d_device_ = NULL;
  myNextRenderingTime = 0.0;
  enet_client_ = NULL;
  enet_server_ = NULL;
  ZeroMemory(&myCharacter, sizeof(myCharacter));
  myUsingSoftwareDeviceFlag = false;
  myUsingMixedDeviceFlag = false;
  myTargetFPS = 0.0;
  time_of_day_adjustment_ = 0.0;
  quest_array_ = NULL;
  number_of_quests_ = 0;
  images_ = NULL;
  number_of_images_ = 0;
  items_ = NULL;
  number_of_items_ = 0;
  spells_ = NULL;
  number_of_spells_ = 0;
  maps_ = NULL;
  number_of_maps_ = NULL;
  current_map_ = NULL;
  current_map_index_ = Evidyon::World::INVALID_MAP_INDEX;
  zones_ = NULL;
  number_of_zones_ = 0;
}



//-----------------------------------------------------------------------------
// Name:  execute
// Desc:  Runs the client application
//-----------------------------------------------------------------------------
void EvidyonClient::execute() {
  Client::ClientFacade facade(this);

  // After startup, go to to the "connect" state
  myStateMachine.queueState(VCS_CONNECT);

  // Execute until we run out of states
  while(myStateMachine.advance()) {
    run(myStateMachine.currentState());
  }
}



//-----------------------------------------------------------------------------
// Name:  execute
// Desc:  Runs the client application
//-----------------------------------------------------------------------------
void EvidyonClient::handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  bool returnResult = false;
  myKeyboardInputSource.handleWindowsMessage(hWnd, uMsg, wParam, lParam, &returnResult);
  myMouseInputSource.handleWindowsMessage(hWnd, uMsg, wParam, lParam, &returnResult);
}



//-----------------------------------------------------------------------------
// Name:  cleanup
// Desc:  Shuts down all of the program components
//-----------------------------------------------------------------------------
void EvidyonClient::cleanup() {

  // Get rid of all of the resources
  freeResources();

  // Close the window and shut down
  if (myMainWindow) {
      CloseWindow(myMainWindow);
      DestroyWindow(myMainWindow);
      myMainWindow = NULL;
  }

  // Get rid of the server connection, if it still exists
  if (APP_WARNING(enet_server_ != NULL)("Server was still connected when program closed")) {
      enet_peer_reset(enet_server_);
      enet_server_ = NULL;
  }

  // Get rid of Direct3D
  SAFE_RELEASE(d3d_device_);
  SAFE_RELEASE(d3d_);

  // Close the window and shut down
  if (myMainWindow)
  {
      DestroyWindow(myMainWindow);
      myMainWindow = NULL;
  }

  // Reset variables
  ZeroMemory(&d3d_params_, sizeof(d3d_params_));

  // Get rid of the client object
  if (enet_client_ != NULL)
  {
      enet_host_destroy(enet_client_);
      enet_client_ = NULL;
  }

  // Close the enet library
  enet_deinitialize();

#ifdef ENABLE_ADMIN_CONSOLE
  // Release the admin console
  admin_console_.destroy();
#endif
}



//-----------------------------------------------------------------------------
// Name:  windowsMessagePump
// Desc:  Executes windows messages and returns when the next frame is to be run
//-----------------------------------------------------------------------------
bool EvidyonClient::windowsMessagePump()
{
    // Allow Windows to have some processing time
    //Sleep(0);

#ifdef ENABLE_ADMIN_CONSOLE
    { // Update the admin console
      using namespace Evidyon::Network::Packet::Client;
      Evidyon::AdminConsole::Command command = AdminConsole::COMMAND_INVALID;
      std::string parameters;
      if (admin_console_.update(&command, &parameters)) {
        switch (command) {
        case AdminConsole::COMMAND_DEBUG:
          DEBUG_INFO("debug> %s", parameters.c_str());
          break;
        case AdminConsole::COMMAND_EXIT:
          return false;
        case AdminConsole::COMMAND_ACTORS:
          //for (GlobalActorManager::ActorMap::const_iterator i = global_actor_manager_.allActorsBegin();
          //     i != global_actor_manager_.allActorsEnd(); ++i) {
          //  ActorInstance* actor = i->second;
          //  char line[256];
          //  sprintf_s(line,
          //            "%s = %i (%.02f, %.02f)",
          //            actor->getName(),
          //            actor->getID(),
          //            actor->getX(),
          //            actor->getY());
          //  admin_console_.print(line);
          //}
          admin_console_.print("===== Actors =====================");
          break;
        case AdminConsole::COMMAND_SQLQUERY: {
          enet_peer_send(enet_server_, 0, Encode::admin_sqlQuery(parameters));
          } break;

        case AdminConsole::COMMAND_CREATEITEM: {
          for (int i = 0; i < number_of_items_; ++i) {
            if (0 == _stricmp(parameters.c_str(),
                              items_[i].name)) {
              enet_peer_send(enet_server_, 0, Encode::admin_createItem(i));
              break;
            }
          }
          } break;

        case AdminConsole::COMMAND_CHARACTERS: {
          std::string query = "select character_id, name from 'characters' where account_id=" + parameters;
          enet_peer_send(enet_server_, 0, Encode::admin_sqlQuery(query));
          } break;

        case AdminConsole::COMMAND_INVENTORY: {
          unsigned int character_id;
          sscanf_s(parameters.c_str(), "%lu", &character_id);
          char query[256];
          sprintf_s(query, 256, "select * from 'items' where owner_character_id=%lu", character_id);
          enet_peer_send(enet_server_, 0, Encode::admin_sqlQuery(query));
          } break;

        case AdminConsole::COMMAND_BACKUP: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_BACKUP));
          } break;

        case AdminConsole::COMMAND_TURNOFF: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_TURNOFF));
          } break;

        case AdminConsole::COMMAND_ONLINE: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_ONLINE));
          } break;

        case AdminConsole::COMMAND_COMMIT: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_COMMIT));
          } break;

        case AdminConsole::COMMAND_RESPAWNALL: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_RESPAWNALL));
          } break;

        case AdminConsole::COMMAND_UNITTEST_DUPEITEM: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_UNITTEST_DUPEITEM));
          } break;

        case AdminConsole::COMMAND_RESOURCES: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_RESOURCES));
          } break;

        case AdminConsole::COMMAND_RESETATTRIBUTES: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_RESETATTRIBUTES));
          } break;

        case AdminConsole::COMMAND_QUERYCHARACTERSTATISTICS: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_QUERYCHARACTERSTATISTICS));
          } break;

        case AdminConsole::COMMAND_STRESSTESTS: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_STRESSTESTS));
          } break;

        case AdminConsole::COMMAND_GODMODE: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_GODMODE));
          } break;

        case AdminConsole::COMMAND_REPACKITEMS: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_REPACKITEMS));
          } break;

        case AdminConsole::COMMAND_UNITTEST_DROPONDEATH: {
          enet_peer_send(enet_server_, 0, Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_UNITTEST_DROPONDEATH));
          } break;


        case AdminConsole::COMMAND_SETALIGNMENT: {
          int alignment_value = atoi(parameters.c_str());
          sendPacket(Evidyon::Network::Packet::NetMsg_Admin_SetAlignment::writePacket(
            ENET_PACKET_FLAG_RELIABLE, alignment_value));
          } break;

        case AdminConsole::COMMAND_RESETALLPKS: {
          sendPacket(Encode::admin_simpleCommand(ADMINSIMPLECOMMAND_RESETALLPKS));
          } break;
        }
      }
    }
#endif

    // Run the message loop until it's empty
    MSG msg; 
    ZeroMemory(&msg, sizeof(msg));
    while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // If we get a quit message, exit!
        if (APP_INFO(msg.message == WM_QUIT)("Quit message received")) return false;

        // Clear out the message structure
        ZeroMemory(&msg, sizeof(msg));
    }

    bool mainWindowExists = FALSE != IsWindow(myMainWindow);
    if (!mainWindowExists)
        DEBUG_INFO("Main window no longer exists");

    // Handle the next frame as long as the window still exists
    return mainWindowExists;
}




//-----------------------------------------------------------------------------
// Name:  allowFrameToBeRendered
// Desc:  Executes windows messages and returns when the next frame is to be run
//-----------------------------------------------------------------------------
bool EvidyonClient::allowFrameToBeRendered(double frameTime)
{
    double timeBeyondNextRender = frameTime - myNextRenderingTime;
    if (timeBeyondNextRender > 0.0)
    {
        static double lastRenderTime = frameTime - 1.0/myTargetFPS;
        double CURRENT_PERIOD = (frameTime - lastRenderTime);
        lastRenderTime = frameTime;

        float scaler = (1.0/myTargetFPS) / CURRENT_PERIOD;
        scaler = min(1.1f, scaler);
        scaler = max(0.5f, scaler);
        myConfiguredFramePeriod *= scaler;
        myNextRenderingTime = myNextRenderingTime + myConfiguredFramePeriod;
        float timeUntilNextRender = myNextRenderingTime - frameTime;

        // Don't allow the frame-rate to drop below 10 FPS
        if (timeUntilNextRender > 1.0f / 10.0f) myNextRenderingTime = frameTime + 1.0f / 10.0f;

        //static int frames = 0;
        //++frames;
        //static double lastZeroTime = frameTime;
        //if (frames == 60)
        //{
        //    DEBUG_INFO("%.02f FPS (1/configured period:  %.02f)", frames / (frameTime - lastZeroTime), 1.0f/myConfiguredFramePeriod);
        //    frames = 0;
        //    lastZeroTime = frameTime;
        //}

        //myNextRenderingTime = frameTime + myConfiguredFramePeriod;
       // myNextRenderingTime += myConfiguredFramePeriod;

        // Make sure the mouse is in the correct location
        updateMouseRenderingLocation();


        return true;
    }
    else
        return false;
}


//-----------------------------------------------------------------------------
void EvidyonClient::updateMouseRenderingLocation() {
  GUIPoint mouseLocation = myMouseInputSource.getMousePosition();
  mouseLocation = gui_primary_layer_.getPrimaryCanvas()->getArea()->constrain(mouseLocation);
  mouse_position_canvas_.alignXLeft(mouseLocation.x);
  mouse_position_canvas_.alignYTop(mouseLocation.y);
}

//-----------------------------------------------------------------------------
void EvidyonClient::setMouseCursorNormal() {
  mouse_cursor_canvas_.alignXLeft(0);
  mouse_cursor_canvas_.alignYTop(0);
  mouse_cursor_.setSourceRegion(&EvidyonGUIDefs::MOUSE_CURSOR_NORMAL, EvidyonGUIDefs::TEXTURE_SIZE);
}



//-----------------------------------------------------------------------------
void EvidyonClient::setMouseCursorActive() {
  mouse_cursor_canvas_.alignXLeft(-4);
  mouse_cursor_canvas_.alignYTop(-4);
  mouse_cursor_.setSourceRegion(&EvidyonGUIDefs::MOUSE_CURSOR_ACTIVE, EvidyonGUIDefs::TEXTURE_SIZE);
}







//----[  getCamera  ]----------------------------------------------------------
IsometricCamera* EvidyonClient::getCamera() {
  return &camera_;
}



//----[  getClientActor  ]-----------------------------------------------------
Actor::Client::ClientActorInstance* EvidyonClient::getClientActor() {
  return actor_manager_.getClientActor();
}




//----[  getGeosids  ]---------------------------------------------------------
const GeosidArray* EvidyonClient::getGeosids() {
  return &geosids_;
}




//----[  getCurrentMap  ]------------------------------------------------------
World::MapIndex EvidyonClient::getCurrentMap() const {
  return current_map_index_;
}





//----[  sendPacket  ]---------------------------------------------------------
void EvidyonClient::sendPacket(ENetPacket* packet) {
  if (packet) enet_peer_send(enet_server_, 0, packet);
}


//-----------------------------------------------------------------------------
void EvidyonClient::renderLoginBackground() {
  renderstate_manager_.resetState();
  gui_primary_layer_.reinitializeD3DDeviceState();

  GUIVertex* vertices;

  const float centers_sizes[] = {
    0.0f, 0.0f, 500.0f/1600.0f*d3d_params_.BackBufferWidth,
    400.0f/1600.0f*d3d_params_.BackBufferWidth,  400.0f/1200.0f*d3d_params_.BackBufferHeight, 1024.0f/1600.0f*d3d_params_.BackBufferWidth,
    600.0f/1600.0f*d3d_params_.BackBufferWidth,  300.0f/1200.0f*d3d_params_.BackBufferHeight, 800.0f/1600.0f*d3d_params_.BackBufferWidth,
    1000.0f/1600.0f*d3d_params_.BackBufferWidth, 700.0f/1200.0f*d3d_params_.BackBufferHeight, 1400.0f/1600.0f*d3d_params_.BackBufferWidth,
    -100.0f/1600.0f*d3d_params_.BackBufferWidth, 700.0f/1200.0f*d3d_params_.BackBufferHeight, 900.0f/1600.0f*d3d_params_.BackBufferWidth,
  };

  size_t quads = 5;
  gui_primary_layer_.allocateQuadListGeometry(&quads, &vertices);

  d3d_device_->SetTexture(0, getImage(Evidyon::Client::CLIENT_IMAGE_EVIDYON_SYMBOL));

  for (size_t q = 0; q < 5 && q < quads; ++q) {
    GUIQuad quad;
    quad.initialize(0.75f, D3DCOLOR_XRGB(255,255,255));
    quad.createRotated(centers_sizes[q*3+0],
                       centers_sizes[q*3+1],
                       centers_sizes[q*3+2],
                       GetTickCount()/(10000.0f * q * (q == 1 ? -1.0f : 1.0f)));
    quad.copyTo(vertices);
    vertices += 4;
  }

  gui_primary_layer_.flush();
}



void EvidyonClient::keepConnectionAlive() {

  // todo: check for d/c
  enet_host_service(enet_client_, NULL, 0);

}



//----[  getImage  ]--------------------------------------------------------
LPDIRECT3DTEXTURE9 EvidyonClient::getImage(Evidyon::Client::ClientImage image) {
  return images_[clientImage(image)];
}





//----[  beginScene  ]---------------------------------------------------------
bool EvidyonClient::beginScene() {
  assert(d3d_device_);
  if (FAILED(d3d_device_->BeginScene())) return false;
  renderstate_manager_.beginScene();
  visual_fx_renderer_.beginScene();
  scenery_renderer_.beginScene();
  animated_mesh_renderer_.beginScene();
  skinned_mesh_renderer_.beginScene();
  return true;
}



//----[  endScene  ]-----------------------------------------------------------
bool EvidyonClient::endScene(State current_state) {
  assert(d3d_device_);
  d3d_device_->EndScene();
  if (FAILED(d3d_device_->Present(NULL, NULL, NULL, NULL))) {
    myStateMachine.jumpState(VCS_LOST_D3DDEVICE);
    myStateMachine.queueState(current_state);
    return false;
  }
  return true;
}



//----[  endSceneScreenshot  ]-------------------------------------------------
bool EvidyonClient::endSceneScreenshot(State current_state,
                                       char* file_name) {
  assert(d3d_device_);
  d3d_device_->EndScene();

  char date[MAX_PATH];
  GetDateFormatA(LOCALE_USER_DEFAULT,
                 0,
                 NULL,
                 "yyyy-MM-dd",
                 date,
                 sizeof(date));
  char time[MAX_PATH];
  GetTimeFormatA(LOCALE_USER_DEFAULT,
                 0,
                 NULL,
                 "h-mm-ss tt",
                 time,
                 sizeof(time));

  std::string output_path;

  if (SUCCEEDED(SHGetFolderPath(NULL,
                                CSIDL_MYPICTURES,
                                NULL,
                                SHGFP_TYPE_CURRENT,
                                file_name))) {

    LPDIRECT3DSURFACE9 back_buffer_surface;
    HRESULT hr = d3d_device_->GetBackBuffer(0,
                                            0,
                                            D3DBACKBUFFER_TYPE_MONO,
                                            &back_buffer_surface);
    strcat_s(file_name, MAX_PATH, "\\Evidyon");
    CreateDirectory(file_name, NULL);
    if (SUCCEEDED(hr)) {
      char file_name_without_path[MAX_PATH];
      sprintf_s(file_name_without_path,
                sizeof(file_name_without_path),
                "\\Evidyon - %s at %s.bmp",
                date,
                time);
      output_path = "My Documents\\Evidyon";
      output_path += file_name_without_path;
      strcat_s(file_name,
               MAX_PATH,
               file_name_without_path);
      hr = D3DXSaveSurfaceToFile(file_name,
                                 D3DXIFF_BMP,
                                 back_buffer_surface,
                                 NULL,
                                 NULL);
      SAFE_RELEASE(back_buffer_surface);
      CONFIRM(SUCCEEDED(hr));
    }
  }

  // tidy up the path name
  strcpy_s(file_name, MAX_PATH, output_path.c_str());

  // Present the backbuffer contents to the display
  if (FAILED(d3d_device_->Present(NULL, NULL, NULL, NULL))) {
    myStateMachine.jumpState(VCS_LOST_D3DDEVICE);
    myStateMachine.queueState(current_state);
    return false;
  }

  return true;
}


//----[  clientImage  ]--------------------------------------------------------
Evidyon::Image::ImageIndex EvidyonClient::clientImage(Evidyon::Client::ClientImage image) {
  return client_data_.images[image];
}




//----[  clientSound  ]--------------------------------------------------------
Evidyon::Sound::SoundIndex EvidyonClient::clientSound(Evidyon::Client::ClientSound sound) {
  return client_data_.sounds[sound];
}




//----[  locationIsVisible  ]--------------------------------------------------
bool EvidyonClient::locationIsVisible(int x, int z) {
  //float cx, cz;
  //actor_manager_.getClientActor()->getPosition(&cx, &cz);
  //return ((x - cx) * (x - cx) + (z - cz) * (z - cz) < 8.0f * 8.0f);
  return true;
  //switch (map_renderer_.getVisibility(x, z)) {
  //  using namespace Evidyon::World;
  //  default:
  //    assert(false);
  //  case VISIBILITY_INVISIBLE:
  //  case VISIBILITY_HIDDEN:
  //  case VISIBILITY_OCCLUDED:
  //    return false;
  //  case VISIBILITY_NORMAL:
  //    return true;
  //}
}



//----[  navigability  ]-------------------------------------------------------
Evidyon::World::Navigability EvidyonClient::navigability(int x, int z) {
  assert(current_map_);
  return current_map_->getNavigability(x,z);
}



//----[  footstepSound  ]------------------------------------------------------
Evidyon::Sound::SoundIndex EvidyonClient::footstepSound(int x, int z) {
  World::Navigability n = navigability(x, z);
  return (n == World::NAVIGABILITY_SWIM || n == World::NAVIGABILITY_WADE)
         ? client_data_.water_footstep_sound
         : client_data_.land_footstep_sound;
}





//----[  DynamicArrayActorIDFunctionoid  ]-------------------------------------
class DynamicArrayActorIDFunctionoid
  : public DynamicArray<Evidyon::Actor::ActorID>::ElementFunctionoid {
public:
  virtual bool hasMoreElements() {
    return i != end;
  }
  virtual bool fill(size_t count, Evidyon::Actor::ActorID* element, bool* failed) {
    if (!element) return true;
    if (i == end) return false;
    *element = *i;
    ++i;
    return true;
  }
  Evidyon::Actor::Client::ClientActorManager::ActorIDSet::const_iterator i, end;
};


//----[  NetMsg_SlowSync  ]----------------------------------------------------
void EvidyonClient::NetMsg_SlowSync(double time, ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  static const int BUFFER_SIZE = 512/sizeof(Evidyon::Actor::ActorSlowSync);
  float time_since_last_sync;
  Evidyon::Actor::ActorSlowSync slow_sync[BUFFER_SIZE];
  size_t number_of_slow_syncs;
  NetMsg_SlowSync::readPacket(packet,
                              &time_since_last_sync,
                              &CopiedArray<Evidyon::Actor::ActorSlowSync>(
                                slow_sync,
                                BUFFER_SIZE,
                                &number_of_slow_syncs));

  if (time_since_last_sync > 0) {
    // Sync all of the actors.  The begin...end pair cause all nonclient
    // actors that were not updated to be erased.
    unsigned int counter;
    actor_manager_.beginSlowSync(&counter);
    for (size_t i = 0; i < number_of_slow_syncs; ++i) {
      actor_manager_.slowSync(time,
                              time_since_last_sync,
                              &slow_sync[i]);
    }
    actor_manager_.endSlowSync(counter);
  } else {
    // this is an overflow sync--don't erase anything, and
    // use the same slow sync counter as before
    time_since_last_sync = -time_since_last_sync;
    for (size_t i = 0; i < number_of_slow_syncs; ++i) {
      actor_manager_.slowSync(time,
                              time_since_last_sync,
                              &slow_sync[i]);
    }
  }

  { // Ask for all of the descriptions of actors that need them
    DynamicArrayActorIDFunctionoid functionoid;
    functionoid.i = actor_manager_.actorsNeedingDescriptionBegin();
    functionoid.end = actor_manager_.actorsNeedingDescriptionEnd();
    if (functionoid.i != functionoid.end) {
      do {
        sendPacket(NetMsg_AskActorFullDescriptions::writePacket(
          ENET_PACKET_FLAG_RELIABLE,
          &DynamicArray<Evidyon::Actor::ActorID>(&functionoid)));
      } while (functionoid.i != functionoid.end);
      actor_manager_.actorDescriptionsRequested();
    }
  }

  { // Ask for all of the actor names
    using namespace Evidyon::Actor::Client;
    ClientActorManager::ActorIDSet::const_iterator i, end;
    i = actor_manager_.actorsNeedingNameBegin();
    end = actor_manager_.actorsNeedingNameEnd();
    if (i != end) {
      do {
        sendPacket(NetMsg_AskActorName::writePacket(
          ENET_PACKET_FLAG_RELIABLE,
          *i));
      } while (++i != end);
      actor_manager_.actorNamesRequested();
    }
  }
}





//----[  NetMsg_Event_GenerateBasicProjectile  ]-------------------------------
void EvidyonClient::NetMsg_Event_GenerateBasicProjectile(double time,
                                                         ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;

  Event::EventID event_id;
  Event::EventIndex event_index;
  unsigned int packed_angle, packed_x, packed_z;
  NetMsg_Event_GenerateBasicProjectile::readPacket(
    packet,
    &event_id,
    &event_index,
    &packed_angle,
    &packed_x,
    &packed_z);

  // unpack
  float angle = (D3DX_PI * 2.0f / 256.0f) * packed_angle;
  float x = packed_x / 32.0f;
  float z = packed_z / 32.0f; // 32 = 1<<5

  //DEBUG_INFO("Projectile #%lu type %lu @ %.02f, %.02f going %.02f", event_id, event_index, x, z, angle);

  // come up with a target
  float target_x = cos(angle) + x;
  float target_z = sin(angle) + z;

  // create
  Binding::BindingPtr null_binding;
  null_binding.reset();
  event_manager_.generate(event_id,
                          event_index,
                          time,
                          1.0f, // TODO: intensity
                          x,
                          z,
                          null_binding,
                          target_x,
                          target_z,
                          NULL,
                          0,
                          false);
}



//----[  NetMsg_Event_Terminate  ]---------------------------------------------
void EvidyonClient::NetMsg_Event_Terminate(double time, ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  
  Event::EventID event_id;
  NetMsg_Event_Terminate::readPacket(
    packet,
    &event_id);

  event_manager_.terminate(time, event_id);
}


//----[  NetMsg_Event_GenerateTargetedSpecialFX_SrcToTgt  ]--------------------
void EvidyonClient::NetMsg_Event_GenerateTargetedSpecialFX_SrcToTgt(
    double time, ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Actor::Client;

  Event::EventID event_id;
  Event::EventIndex event_index;
  Actor::ActorID source_id, target_id;
  NetMsg_Event_GenerateTargetedSpecialFX_SrcToTgt::readPacket(
    packet,
    &event_id,
    &event_index,
    &source_id,
    &target_id);

  ClientActorInstance* source = actor_manager_.getActor(source_id);
  ClientActorInstance* target = actor_manager_.getActor(target_id);

  if (!source || !target) return;

  // create
  float source_x, source_z;
  source->getPosition(&source_x, &source_z);
  Binding::BindingPtr target_binding = target->getBinding();
  float target_x, target_z;
  target->getPosition(&target_x, &target_z);
  event_manager_.generate(event_id,
                          event_index,
                          time,
                          1.0f, // TODO: intensity
                          source_x,
                          source_z,
                          source->getBinding(),
                          target_x,
                          target_z,
                          &target_binding,
                          1,
                          source == actor_manager_.getClientActor() ||
                          target == actor_manager_.getClientActor());
}



//----[  NetMsg_Event_GenerateTargetedSpecialFX_TgtXZ  ]-----------------------
void EvidyonClient::NetMsg_Event_GenerateTargetedSpecialFX_TgtXZ(
    double time, ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Actor::Client;

  Event::EventID event_id;
  Event::EventIndex event_index;
  Actor::ActorID source_id;
  unsigned int packed_target_x, packed_target_z;
  NetMsg_Event_GenerateTargetedSpecialFX_TgtXZ::readPacket(
    packet,
    &event_id,
    &event_index,
    &source_id,
    &packed_target_x,
    &packed_target_z);

  ClientActorInstance* source = actor_manager_.getActor(source_id);

  if (!source) return;

  // create
  float source_x, source_z;
  source->getPosition(&source_x, &source_z);
  float target_x = packed_target_x / 32.0f;
  float target_z = packed_target_z / 32.0f; // 32 = 1<<5
  event_manager_.generate(event_id,
                          event_index,
                          time,
                          1.0f, // TODO: intensity
                          source_x,
                          source_z,
                          source->getBinding(),
                          target_x,
                          target_z,
                          NULL,
                          0,
                          source == actor_manager_.getClientActor());
}



//----[  NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZ  ]------------------
void EvidyonClient::NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZ(
    double time, ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Actor::Client;

  Event::EventID event_id;
  Event::EventIndex event_index;
  unsigned int packed_source_x, packed_source_z;
  unsigned int packed_target_x, packed_target_z;
  NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZ::readPacket(
    packet,
    &event_id,
    &event_index,
    &packed_source_x,
    &packed_source_z,
    &packed_target_x,
    &packed_target_z);

  // create
  Binding::BindingPtr null_source_binding;
  float source_x = packed_source_x / 32.0f;
  float source_z = packed_source_z / 32.0f;
  float target_x = packed_target_x / 32.0f;
  float target_z = packed_target_z / 32.0f; // 32 = 1<<5
  event_manager_.generate(event_id,
                          event_index,
                          time,
                          1.0f, // TODO: intensity
                          source_x,
                          source_z,
                          null_source_binding,
                          target_x,
                          target_z,
                          NULL,
                          0,
                          false);
}



//----[  NetMsg_Event_GenerateTargetedSpecialFX_TgtXZAOE  ]--------------------
void EvidyonClient::NetMsg_Event_GenerateTargetedSpecialFX_TgtXZAOE(
    double time, ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Actor::Client;
  static const int MAX_TARGETS = 16;

  Event::EventID event_id;
  Event::EventIndex event_index;
  Actor::ActorID target_ids[MAX_TARGETS];
  size_t number_of_targets;
  Actor::ActorID source_id;
  unsigned int packed_target_x, packed_target_z;
  NetMsg_Event_GenerateTargetedSpecialFX_TgtXZAOE::readPacket(
    packet,
    &event_id,
    &event_index,
    &source_id,
    &packed_target_x,
    &packed_target_z,
    &CopiedArray<Actor::ActorID>(target_ids, MAX_TARGETS, &number_of_targets));

  ClientActorInstance* source = actor_manager_.getActor(source_id);

  if (!source) return;

  // Copy targets for which we have existing actors
  size_t valid_targets = 0;
  Binding::BindingPtr target_bindings[MAX_TARGETS];
  bool involves_client = source == actor_manager_.getClientActor();
  for (size_t i = 0; i < number_of_targets; ++i) {
    ClientActorInstance* actor = actor_manager_.getActor(target_ids[i]);
    if (!actor) continue;
    involves_client = involves_client || actor == actor_manager_.getClientActor();
    target_bindings[valid_targets++] = actor->getBinding();
  }

  // create
  Binding::BindingPtr null_source_binding;
  float source_x, source_z;
  source->getPosition(&source_x, &source_z);
  float target_x = packed_target_x / 32.0f;
  float target_z = packed_target_z / 32.0f; // 32 = 1<<5
  event_manager_.generate(event_id,
                          event_index,
                          time,
                          1.0f, // TODO: intensity
                          source_x,
                          source_z,
                          source->getBinding(),
                          target_x,
                          target_z,
                          target_bindings,
                          valid_targets,
                          involves_client);
}






//----[  NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZAOE  ]---------------
void EvidyonClient::NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZAOE(
    double time, ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Actor::Client;
  static const int MAX_TARGETS = 16;

  Event::EventID event_id;
  Event::EventIndex event_index;
  Actor::ActorID target_ids[MAX_TARGETS];
  size_t number_of_targets;
  unsigned int packed_source_x, packed_source_z;
  unsigned int packed_target_x, packed_target_z;
  NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZAOE::readPacket(
    packet,
    &event_id,
    &event_index,
    &packed_source_x,
    &packed_source_z,
    &packed_target_x,
    &packed_target_z,
    &CopiedArray<Actor::ActorID>(target_ids, MAX_TARGETS, &number_of_targets));

  bool involves_client = false;

  // Copy targets for which we have existing actors
  size_t valid_targets = 0;
  Binding::BindingPtr target_bindings[MAX_TARGETS];
  for (size_t i = 0; i < number_of_targets; ++i) {
    ClientActorInstance* actor = actor_manager_.getActor(target_ids[i]);
    if (!actor) continue;
    involves_client = involves_client || actor == actor_manager_.getClientActor();
    target_bindings[valid_targets++] = actor->getBinding();
  }

  // create
  Binding::BindingPtr null_source_binding;
  float source_x = packed_source_x / 32.0f;
  float source_z = packed_source_z / 32.0f;
  float target_x = packed_target_x / 32.0f;
  float target_z = packed_target_z / 32.0f; // 32 = 1<<5
  event_manager_.generate(event_id,
                          event_index,
                          time,
                          1.0f, // TODO: intensity
                          source_x,
                          source_z,
                          null_source_binding,
                          target_x,
                          target_z,
                          target_bindings,
                          valid_targets,
                          involves_client);
}

