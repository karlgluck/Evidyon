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
#ifndef __COMPLETE_EDITOR_ACTOR_ACTOREDITINGDIALOG_H__
#define __COMPLETE_EDITOR_ACTOR_ACTOREDITINGDIALOG_H__
#pragma once


#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "shared/evidyoncamera.h"
#include "shared/evidyond3dlightmanager.h"
#include "shared/actor/actoraction.h"
#include "shared/actor/actorrenderer.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"
#include "shared/client/clientdata.h"

namespace Evidyon {
namespace Actor {
namespace Tools {
class ActorProfile;
}
}
}

namespace Evidyon {

//----[  ActorEditingDialog  ]-------------------------------------------------
// This class adds a default template for each actor type to the renderer.
class ActorEditingDialog : public dcx::dcxWin32DialogController,
                                  Tools::D3DWindow,
                                  Client::ClientData {
  enum SyncMethod {
    SYNCMETHOD_FASTSYNC_CURSOR,
    SYNCMETHOD_SLOWSYNC_CURSOR,
    SYNCMETHOD_FASTSYNC_CONSTANTSPEED,
    SYNCMETHOD_SLOWSYNC_CONSTANTSPEED,
    NUMBER_OF_SYNC_METHODS,
  };

  static const char* SyncMethodString(SyncMethod value);

  enum RightClickAction {
    ALTACTION_TELEPORT_ONCE,
    ALTACTION_TELEPORT_CONSTANT,
    ALTACTION_DOACTORACTION_LOW,
    ALTACTION_DOACTORACTION_HIGH = ALTACTION_DOACTORACTION_LOW
                                  + Evidyon::Actor::NUMBER_OF_ACTOR_ACTIONS,
    NUMBER_OF_RIGHT_CLICK_ACTIONS
  };

  static const char* RightClickActionString(RightClickAction value);


  struct ActorProfileBeingEdited {
    Evidyon::Actor::ActorProfileIndex profile_index;
    Evidyon::Actor::Tools::ActorProfile* profile;
    unsigned int number_of_forms; // pulled from the skinned mesh
  };

public:
  ActorEditingDialog();
  ~ActorEditingDialog();
  void destroy();
  bool begin(Evidyon::Tools::CompleteEditor* editor,
             dc::dcGenericResource::Array& actor_profiles);
  virtual HWND getWindowHandle();
  virtual bool render(Evidyon::Tools::CompleteEditor* editor,
                      LPDIRECT3DDEVICE9 d3d_device);

private:
  virtual INT_PTR dialogEvent(HWND hDlg,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam);

  Evidyon::Actor::ActorRenderer* actorRenderer();
  Evidyon::SkinnedMesh::SkinnedMeshRenderer* skinnedMeshRenderer();


public:
  //// Gets the index of a defined image type
  //virtual Image::ImageIndex clientImage(Client::ClientImage image);

  //// Obtains the sound from its definition
  //virtual Evidyon::Sound::SoundIndex clientSound(Client::ClientSound sound);

  //// Determines whether or not the given location is visible on the current
  //// map, based on the user's location.  This interfaces with the map renderer
  //// at some level, but this abstraction allows that to happen somewhere else.
  //virtual bool locationIsVisible(int x, int z);

  //// Obtains the navigability of the location on the current map.
  //virtual Evidyon::World::Navigability navigability(int x, int z);

  //// Obtains the sound that an actor at (x,y) should make when it puts a foot
  //// down.  This allows walking in water or swimming to make a different sound
  //// from being on land.
  ////return movement_sounds_[(navigability(x,y))]; // wading, swimming, walking, ...
  //virtual Evidyon::Sound::SoundIndex footstepSound(int x, int z);

private:

  // Main window handle
  HWND dialog_handle_;

  // Root editor object
  Tools::CompleteEditor* editor_;

  // The instance being controlled
  Actor::RenderableActorInstance* instance_;

  // The camera is locked at the origin
  EvidyonCamera camera_;

  // How the mouse is generating updates for the actor
  SyncMethod sync_method_;

  // The instance's true location in the world
  D3DXVECTOR2 location_;

  // Instance's target location
  D3DXVECTOR2 destination_;

  // Most recent set of flags
  Actor::ActorSyncFlags flags_;

  // Updated each time render() is called.
  double last_update_time_;
  double last_sync_time_;

  // What to do when the right mouse button is pressed.  If this action is
  // 'die', the action is sent as long as the button is held down;
  // otherwise, the action is sent once when it the button is pushed.
  RightClickAction right_click_action_;
  bool right_button_is_down_; // set 1 loop after RMB is pressed

  ActorProfileBeingEdited* actor_profiles_;
  size_t number_of_actor_profiles_;

  size_t current_actor_profile_;
  SkinnedMesh::SkinnedMeshFormIndex current_actor_form_;
};

}

#endif