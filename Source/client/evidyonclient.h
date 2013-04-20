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
#ifndef __CLIENT_EVIDYONCLIENT_H__
#define __CLIENT_EVIDYONCLIENT_H__
#pragma once


#include <hash_map>
#include "adminconsole.h"
#include "evidyonguisounds.h"



#include "shared/evidyond3drenderstatemanager.h"
#include "shared/visualfx/visualfxrenderer.h"
#include "shared/mesh/meshrenderer.h"
#include "shared/scenery/sceneryrenderer.h"
#include "shared/evidyond3dlightmanager.h"
#include "shared/sound/soundmanager.h"
#include "shared/animated-mesh/animatedmeshrenderer.h"
#include "shared/skinned-mesh/skinnedmeshrenderer.h"
#include "shared/actor/actorrenderer.h"
#include "shared/specialfx/specialfxmanager.h"
#include "shared/event/eventmanager.h"

#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarraceclientdescription.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarclassclientdescription.h"
#include "shared/avatar/avatargender.h"
#include "shared/gamefile/clientgamefileclientdata.h"
#include "shared/quest/quest-client.h"

#include "shared/client/clientdata.h"
#include "shared/world/maprenderer.h"
#include "shared/world/maprendererdatasource.h"
#include "shared/world/mapindex.h"

#include "clientmapdata.h"

#include "actor/clientactormanager.h"

#include "shared/geosid/geosidclientdescription.h"

#include "common/kg/kg.h"


using namespace Evidyon;

namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
namespace Scenery {
struct SceneryInstance;
}
namespace Zone {
struct Zone;
}
namespace Spell {
struct SpellClientDescription;
}
}

typedef kg::AllocatedArray<Evidyon::Geosid::GeosidClientDescription> GeosidArray;

/**
 * Contains all of the programming for the Evidyon client application
 */
class EvidyonClient
  : public Evidyon::Client::ClientData {

  // This declaration holds the different states that the application can be in.
  enum State {
    VCS_STARTUP,
    VCS_CONNECT,
    VCS_ACCOUNT_LOGIN,
    VCS_CREATE_ACCOUNT,
    VCS_LOGGED_INTO_ACCOUNT,
    VCS_CREATE_CHARACTER,
    VCS_WORLD_MAIN,
    VCS_DISCONNECT,
    VCS_NO_CONNECTION,
    VCS_SHUTDOWN,
    VCS_CREDITS,

    VCS_LOST_D3DDEVICE,

    VCS_DEBUGSTATESANDBOX,
  };

  const char* stateName(State state);
  void run(State state);


public:
  EvidyonClient();
  void execute();
  void handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


protected:

  /**
   * Shuts down all of the program components and erases allocated memory when the program
   * is about to close.
   */
  void cleanup();


// Program methods
protected:

  /**
   * Executes a robust algorithm to create the Direct3D device.  Implemented in
   * the source file acquiregraphics.cpp
   */
  bool createD3DDevice(bool windowed, unsigned int requiredNumberOfBlendMatrices);

  /**
   * Executes windows messages and returns when the next frame is to be run.  This
   * method will return false if the application should exit.
   */
  bool windowsMessagePump();

  // Sets up subsystems
  bool acquireSystem();
  bool acquireNetwork();
  bool acquireGraphics();
  bool acquireInput();
  bool acquireResources();

  // Unloads the resource file
  void freeResources();

  // Polls the network connection to make sure we stay connected
  void keepConnectionAlive();

// Program states
protected:
  void stateStartup();
  void stateConnect();
  void stateAccountLogin();
  void stateCreateAccount();
  void stateLoggedIntoAccount();
  void stateCreateCharacter();
  void stateWorldMain();
  void stateDisconnect();
  void stateNoConnection();
  void stateShutdown();
  void stateCredits();
  void stateLostD3DDevice();

// Debugging states
  void debugStateSandbox();


protected:

  /// This state machine object is used to control transitions between execution methods
  dcxStateMachine<State> myStateMachine;

  /// Client settings loaded from the hard drive
  dcxIniFileReader config_;

public:
  /// Mouse-input controller
  EvidyonGUIMouseInputSource myMouseInputSource;


  // Sends a packet to the server
  void sendPacket(ENetPacket* packet);


private:
  /// Keyboard-input controller
  EvidyonGUIKeyboardInputSource myKeyboardInputSource;

  // Components of the graphics solution
  HWND myMainWindow;
  D3DPRESENT_PARAMETERS d3d_params_;
  bool myUsingMixedDeviceFlag;
  bool myUsingSoftwareDeviceFlag;
  LPDIRECT3D9 d3d_;
  LPDIRECT3DDEVICE9 d3d_device_;
  double myTargetFPS;
  double myNextRenderingTime;   // The next time at which to render a frame; limits the client's frame rate
  double myConfiguredFramePeriod; // Config-file based delay time for rendering

  /// Network components
  ENetHost* enet_client_;
  ENetPeer* enet_server_;

  /// Various stuff used in the GUI
  GUIFont window_font_;
  GUICanvas mouse_position_canvas_;
  GUICanvas mouse_cursor_canvas_;
  GUIPictureElement mouse_cursor_;
  GUILayerBasic gui_primary_layer_;


  void renderLoginBackground();
  void updateMouseRenderingLocation();  // called automatically by allowFrameToBeRendered
  void setMouseCursorNormal();
  void setMouseCursorActive();

public:
  void getMouseWorldLocation(float* x, float* z) { GUIPoint p = myMouseInputSource.getMousePosition(); camera_.getWorldLocation(p.x, p.y, x, z); }
  IsometricCamera* getCamera();
  Actor::Client::ClientActorInstance* getClientActor();
  const GeosidArray* getGeosids();
  World::MapIndex getCurrentMap() const;

protected:

  /**
   * If this method returns 'true', the calling method should render a frame.  This
   * limits the rate at which frames are drawn to the screen
   */
  bool allowFrameToBeRendered(double frameTime);

  /// The camera used for rendering the world
  IsometricCamera camera_;



/// Manage actors in the world
protected:
  /// Information about this player
  struct
  {
    bool male;
    int race_index;
    int class_index;
    float currentHPPercent, currentMPPercent, currentEXPPercent;
    int attributes[5];
    ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE];
    int inventory_update_indices_[AVATAR_INVENTORY_SIZE];
    int highest_inventory_update_index_;
    int currentHP, maxHP;
    int currentMP, maxMP;
    Avatar::SpellAvailabilityMask spell_availability;
    char name[Avatar::MAX_AVATAR_NAME_LENGTH];
    unsigned int money;
  } myCharacter;

  struct {
    Evidyon::Avatar::AvatarGender gender;
    Evidyon::Avatar::AvatarRace race;
    Evidyon::Avatar::AvatarClass class_;
  } avatar_;

  double time_of_day_adjustment_;

protected:
  typedef std::multimap<std::pair<int,int>,Scenery::SceneryInstance*> ItemsOnMapTable;
  ItemsOnMapTable myItemsOnMapTable;

  EvidyonGUISounds gui_sounds_;

  Evidyon::Quests::Client::Quest* quest_array_;
  size_t number_of_quests_;

  Evidyon::Spell::SpellClientDescription* spells_;
  size_t number_of_spells_;

#ifdef ENABLE_ADMIN_CONSOLE
  AdminConsole admin_console_;
#endif

  LPDIRECT3DTEXTURE9 getImage(Evidyon::Client::ClientImage image);

  Evidyon::GameFile::ClientGameFileClientData client_data_;
  Evidyon::Avatar::AvatarRaceClientDescription
    races_[Evidyon::Avatar::NUMBER_OF_AVATAR_RACES];
  Evidyon::Avatar::AvatarClassClientDescription
    classes_[Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES];

  LPDIRECT3DTEXTURE9* images_;
  size_t number_of_images_;

  // todo: kg::AllocatedArray
  Evidyon::Item::ItemClientDescription* items_;
  size_t number_of_items_;

  // todo: kg::AllocatedArray
  Evidyon::Client::ClientMapData* maps_;
  size_t number_of_maps_;

  // todo: kg::AllocatedArray
  Evidyon::World::MapIndex current_map_index_;
  Evidyon::Client::ClientMapData* current_map_;

  // todo: kg::AllocatedArray
  Evidyon::Zone::Zone* zones_;
  size_t number_of_zones_;

  GeosidArray geosids_;

  // Managers that visualize various parts of the data
  EvidyonD3DRenderStateManager renderstate_manager_;
  Mesh::MeshRenderer mesh_renderer_;
  Evidyon::EvidyonD3DLightManager light_manager_;
  VisualFX::VisualFXRenderer visual_fx_renderer_;
  Scenery::SceneryRenderer scenery_renderer_;
  Sound::SoundManager sound_manager_;
  AnimatedMesh::AnimatedMeshRenderer animated_mesh_renderer_;
  SkinnedMesh::SkinnedMeshRenderer skinned_mesh_renderer_;
  Actor::ActorRenderer actor_renderer_;
  Evidyon::World::MapRenderer map_renderer_;
  Evidyon::Actor::Client::ClientActorManager actor_manager_;
  SpecialFX::SpecialFXManager special_fx_manager_;
  Event::EventManager event_manager_;

private:
  bool beginScene();

  // If the return value is 'false', return from the current state.  This
  // allows automatic processing of lost-device and other problems.
  bool endScene(State current_state);

  // Same as endScene, but takes a screenshot and saves it to a file.  The
  // name of the file is returned in 'file_name', which must be MAX_PATH
  // characters long.
  bool endSceneScreenshot(State current_state,
                          char* file_name);


// implement ClientDataSource
public:
  virtual Evidyon::Image::ImageIndex clientImage(Evidyon::Client::ClientImage image);
  virtual Evidyon::Sound::SoundIndex clientSound(Evidyon::Client::ClientSound sound);
  virtual bool locationIsVisible(int x, int z);
  virtual Evidyon::World::Navigability navigability(int x, int z);
  virtual Evidyon::Sound::SoundIndex footstepSound(int x, int z);

private:
  void NetMsg_SlowSync(double time, ENetPacket* packet);
  void NetMsg_Event_GenerateBasicProjectile(double time, ENetPacket* packet);
  void NetMsg_Event_Terminate(double time, ENetPacket* packet);
  void NetMsg_Event_GenerateTargetedSpecialFX_SrcToTgt(double time, ENetPacket* packet);
  void NetMsg_Event_GenerateTargetedSpecialFX_TgtXZ(double time, ENetPacket* packet);
  void NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZ(double time, ENetPacket* packet);
  void NetMsg_Event_GenerateTargetedSpecialFX_TgtXZAOE(double time, ENetPacket* packet);
  void NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZAOE(double time, ENetPacket* packet);
};



#endif