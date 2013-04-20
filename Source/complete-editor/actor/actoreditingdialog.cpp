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
#include "actoreditingdialog.h"
#include "common/safemacros.h"
#include <assert.h>



namespace Evidyon {



//----[  SyncMethodString  ]---------------------------------------------------
const char* ActorEditingDialog::SyncMethodString(SyncMethod value) {
  switch (value) {
    case SYNCMETHOD_FASTSYNC_CURSOR:    return "Cursor - Fast";
    case SYNCMETHOD_SLOWSYNC_CURSOR:    return "Cursor - Slow";
    case SYNCMETHOD_FASTSYNC_CONSTANTSPEED:   return "Constant - Fast";
    case SYNCMETHOD_SLOWSYNC_CONSTANTSPEED:   return "Constant - Slow";
    default: return NULL;
  }
}




//----[  RightClickActionString  ]---------------------------------------------
const char* ActorEditingDialog::RightClickActionString(RightClickAction value) {
  if (value == ALTACTION_TELEPORT_ONCE) return "Teleport Once";
  if (value == ALTACTION_TELEPORT_CONSTANT) return "Teleport Constantly";
  if (value >= ALTACTION_DOACTORACTION_LOW &&
      value <= ALTACTION_DOACTORACTION_HIGH) {
    return Actor::ActorActionString(
      Actor::ActorAction(value - ALTACTION_DOACTORACTION_LOW));
  }
  return NULL;
}






//----[  ActorEditingDialog  ]-------------------------------------------------
ActorEditingDialog::ActorEditingDialog()
    : location_(0.0f, 0.0f), destination_(0.0f, 0.0f) {
  dialog_handle_ = NULL;
  editor_ = NULL;
  instance_ = NULL;
  sync_method_ = SYNCMETHOD_FASTSYNC_CONSTANTSPEED;
  flags_.union_value = 0;
  last_update_time_ = 0.0;
  last_sync_time_ = 0.0;
  right_click_action_ = ALTACTION_DOACTORACTION_LOW;
  right_button_is_down_ = false;
  current_actor_profile_ = 0;
  current_actor_form_ = 0;
}





//----[  ~ActorEditingDialog  ]------------------------------------------------
ActorEditingDialog::~ActorEditingDialog() {
  destroy();
}




//----[  destroy  ]------------------------------------------------------------
void ActorEditingDialog::destroy() {
  if (instance_) {
    actorRenderer()->release(instance_);
    instance_ = NULL;
  }
}




//----[  begin  ]--------------------------------------------------------------
bool ActorEditingDialog::begin(Tools::CompleteEditor* editor,
                               dc::dcGenericResource::Array& actor_profiles) {
  actor_profiles_ = new ActorProfileBeingEdited[actor_profiles.size()];
  assert(actor_profiles_);
  if (!actor_profiles_) return false;
  number_of_actor_profiles_ = actor_profiles.size();
  int index = 0;
  dc::dcGenericResource::Array::const_iterator iterator;
  for (iterator = actor_profiles.begin();
       iterator != actor_profiles.end(); ++iterator, ++index) {
    Actor::Tools::ActorProfile* actor_profile
      = (Actor::Tools::ActorProfile*)(*iterator);
    SkinnedMesh::Tools::SkinnedMesh* skinned_mesh
      = actor_profile->getSkinnedMesh();
    assert(skinned_mesh);
    if (!skinned_mesh) { destroy(); return false; }

    actor_profiles_[index].number_of_forms = skinned_mesh->forms().size();
    actor_profiles_[index].profile = actor_profile;
    actor_profiles_[index].profile_index = actor_profile->getIndex();
  }

  editor_ = editor;
  editor->acquireVisualFX(true);
  editor->acquireAnimatedMeshes();
  editor->acquireAnimatedMeshAnimations();
  editor->acquireSkinnedMeshes();
  editor->acquireActorProfiles();
  editor->acquireSounds();

  dcx::dcxWin32DialogTemplate dialog;
  dialog.setStyleAsResizeableAppWindow();
  dialog.setTitle("Actor Editor");
  BYTE buffer[256];
  LPCDLGTEMPLATE dlg_template = dialog.compile(buffer, sizeof(buffer));
  dialog_handle_ = createDialog(GetModuleHandle(NULL),
                                dlg_template,
                                editor->topWindow());
  dcx::dcxWin32SetWindowClientSize(dialog_handle_, 1024, 768);
  camera_.rebuildTopdownCamera(1024, 768);
  return editor->addD3DWindow(this);
}




//----[  getWindowHandle  ]----------------------------------------------------
HWND ActorEditingDialog::getWindowHandle() {
  return dialog_handle_;
}




//----[  render  ]-------------------------------------------------------------
bool ActorEditingDialog::render(Tools::CompleteEditor* editor,
                                LPDIRECT3DDEVICE9 d3d_device) {
  if (!instance_) {
    assert(current_actor_profile_ < number_of_actor_profiles_);
    Actor::ActorTemplate actor_template;
    actor_profiles_[current_actor_profile_].profile
      ->compileDefaultTemplate(&actor_template);
    instance_ = actorRenderer()->acquire(
      &actor_template,
      actor_profiles_[current_actor_profile_].profile_index,
      current_actor_form_,
      flags_.state == Actor::ActorSyncFlags::MOVEMENT_DEAD);
    assert(instance_);
    actorRenderer()->updateEquipment(instance_, NULL, true);
    instance_->skinned_mesh_instance->hidden = false;
  }

  if (right_click_action_
      == (ALTACTION_DOACTORACTION_LOW + Actor::ACTORACTION_DIE)) {
    if (right_button_is_down_) {
      flags_.state = Actor::ActorSyncFlags::MOVEMENT_DEAD;
    }
  }
  if (right_button_is_down_ &&
      right_click_action_ == ALTACTION_TELEPORT_CONSTANT) {
    flags_.state = Actor::ActorSyncFlags::MOVEMENT_TELEPORT;
  } else {
    flags_.state = Actor::ActorSyncFlags::MOVEMENT_WALK;
  }

  double time = dcx::dcxWin32Clock::getAccurateSystemTime();
  double time_since_last_update = time - last_update_time_;
  last_update_time_ = time;

  // Determine the length of time between syncs
  double sync_delay = 0.0;
  if (sync_method_ == SYNCMETHOD_FASTSYNC_CURSOR ||
      sync_method_ == SYNCMETHOD_FASTSYNC_CONSTANTSPEED) {
    sync_delay = 1.0 / 10.0;
  } else if (sync_method_ == SYNCMETHOD_SLOWSYNC_CURSOR ||
             sync_method_ == SYNCMETHOD_SLOWSYNC_CONSTANTSPEED) {
    sync_delay = 1.0 / 3.0;
  } else {
    assert(false);
  }

  // Update the sync flags and the location of the actor
  if (sync_method_ == SYNCMETHOD_FASTSYNC_CURSOR ||
      sync_method_ == SYNCMETHOD_SLOWSYNC_CURSOR) {
    D3DXVECTOR2 delta;
    D3DXVec2Subtract(&delta, &destination_, &location_);
    if (D3DXVec2LengthSq(&delta) > 0.01f) {
      flags_.packAngle(atan2f(delta.y, delta.x));
      flags_.state = Evidyon::Actor::ActorSyncFlags::MOVEMENT_WALK;
    } else {
      flags_.state =
        flags_.state == Evidyon::Actor::ActorSyncFlags::MOVEMENT_TELEPORT
        ? Evidyon::Actor::ActorSyncFlags::MOVEMENT_TELEPORT
        : Evidyon::Actor::ActorSyncFlags::MOVEMENT_IDLE;
    }
    location_ = destination_;
  } else { // constant speed
    D3DXVECTOR2 delta;
    D3DXVec2Subtract(&delta, &destination_, &location_);
    if (D3DXVec2LengthSq(&delta) > 0.01f) {
      float angle = atan2f(delta.y, delta.x);
      location_.x += 4.0f * cosf(angle) * time_since_last_update;
      location_.y += 4.0f * sinf(angle) * time_since_last_update;
      flags_.packAngle(angle);
      flags_.state = Evidyon::Actor::ActorSyncFlags::MOVEMENT_WALK;
    } else {
      flags_.state =
        flags_.state == Evidyon::Actor::ActorSyncFlags::MOVEMENT_TELEPORT
        ? Evidyon::Actor::ActorSyncFlags::MOVEMENT_TELEPORT
        : Evidyon::Actor::ActorSyncFlags::MOVEMENT_IDLE;
    }
  }

  // Sync the actor when the timer expires
  double time_since_last_sync = time - last_sync_time_;
  if (time_since_last_sync >= sync_delay) {
    last_sync_time_ = time;
    actorRenderer()->sync(instance_,
                          time_since_last_sync,
                          time,
                          location_.x,
                          location_.y,
                          flags_);
    flags_.blood = false;
    flags_.state = Actor::ActorSyncFlags::MOVEMENT_IDLE;
  }

  VisualFX::VisualFXRenderer* visual_fx_renderer
    = editor_->getVisualFXRenderer();
  Scenery::SceneryRenderer* scenery_renderer
    = editor_->getSceneryRenderer();

  // Update everything
  actorRenderer()->updateInstance(instance_,
                                  time,
                                  time_since_last_update,
                                  this);
  visual_fx_renderer->update(time, time_since_last_update);

  if (SUCCEEDED(d3d_device->BeginScene())) {
    d3d_device->Clear(0,
                      NULL,
                      D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                      D3DCOLOR_XRGB(0,0,0),
                      1.0f,
                      0);
    d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    camera_.setPosition(0.0f, 0.0f);
    camera_.initialize3DRendering(d3d_device);
    EvidyonD3DLightManager* light_manager
      = editor_->getLightManager();
    light_manager->setDaylightAmbience();
    light_manager->beginScene(0.0f, 0.0f); // camera x,z

    EvidyonD3DRenderStateManager* renderstate_manager
      = editor_->getRenderStateManager();
    AnimatedMesh::AnimatedMeshRenderer* animated_mesh_renderer
      = editor_->getAnimatedMeshRenderer();
    SkinnedMesh::SkinnedMeshRenderer* skinned_mesh_renderer
      = editor_->getSkinnedMeshRenderer();

    renderstate_manager->beginScene();
    animated_mesh_renderer->beginScene();
    skinned_mesh_renderer->beginScene();
    visual_fx_renderer->beginScene();
    scenery_renderer->beginScene();

    // Render all of the objects in the world
    size_t number_of_textures = editor->getTextureTable()->getMembers().size();
    for (Texture::TextureIndex texture = 0;
                               texture < number_of_textures;
                             ++texture) {
      scenery_renderer->render(texture, renderstate_manager);
      visual_fx_renderer->render(texture);
      skinned_mesh_renderer->render(texture, renderstate_manager);
    }

    // Draw the ground plane
    if (GetAsyncKeyState(VK_TAB) == 0) {
      editor_->renderTranslucentGroundPlane(0.0f, 0.0f);
    }

    d3d_device->EndScene();
  }

  return true;
}




//----[  dialogEvent  ]--------------------------------------------------------
INT_PTR ActorEditingDialog::dialogEvent(HWND hDlg,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam) {
 if (uMsg == WM_CLOSE) {
    editor_->removeD3DWindow(this);
    EndDialog(hDlg, 0);
    DestroyWindow(hDlg); 
    SetWindowLong(hDlg, GWL_USERDATA, NULL);
    delete this;
  } else if (uMsg == WM_PAINT) {
    editor_->renderD3DWindow(this);
  } else if (uMsg == WM_LBUTTONDOWN ||
             (uMsg == WM_MOUSEMOVE && (wParam&MK_LBUTTON))) {
    POINTS pts = MAKEPOINTS(lParam);
    float x, z;
    camera_.getWorldLocation(
      pts.x,
      pts.y,
      &x,
      &z,
      NULL);
    destination_.x = x;
    destination_.y = z;
  } else if (uMsg == WM_RBUTTONDOWN) {
    right_button_is_down_ = true;
    if (right_click_action_ >= ALTACTION_DOACTORACTION_LOW &&
        right_click_action_ < ALTACTION_DOACTORACTION_HIGH) {
      //flags_.action
      //  = Actor::ActorAction(right_click_action_-ALTACTION_DOACTORACTION_LOW);
    } else {
      switch (right_click_action_) {
      case ALTACTION_TELEPORT_ONCE:
      case ALTACTION_TELEPORT_CONSTANT:
        flags_.state = Evidyon::Actor::ActorSyncFlags::MOVEMENT_TELEPORT;
         break;
      default:
        assert(false);
      }
    }
  } else if (uMsg == WM_RBUTTONUP) {
    right_button_is_down_ = false;
    flags_.state = Actor::ActorSyncFlags::MOVEMENT_IDLE;
  } else if (uMsg == WM_MBUTTONUP ||
             ((uMsg == WM_KEYUP) && (wParam == VK_SPACE))) {
    enum {
     SELECT_SYNC_METHOD = 1000,
     SELECT_RIGHT_CLICK_ACTION = 2000,
    };
    POINTS clicked_at_points = MAKEPOINTS(lParam);
    POINT clicked_at = {clicked_at_points.x, clicked_at_points.y};
    ClientToScreen(hDlg, &clicked_at);
    MENUITEMINFO menu_item;

    HMENU hMenu = CreatePopupMenu();
    for (int i = 0; i < NUMBER_OF_SYNC_METHODS; ++i) {
      dcx::dcxWin32InitMenuItem(&menu_item, SyncMethodString(SyncMethod(i)),
                                false, SELECT_SYNC_METHOD + i);
      menu_item.fState = (i == sync_method_) ? MFS_CHECKED : 0;
      InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    }
    dcx::dcxWin32InitMenuItemSeperator(&menu_item);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    for (int i = 0; i < NUMBER_OF_RIGHT_CLICK_ACTIONS; ++i) {
      dcx::dcxWin32InitMenuItem(
        &menu_item,
        RightClickActionString(RightClickAction(i)),
        false,
        SELECT_RIGHT_CLICK_ACTION + i);
      menu_item.fState = (i == sync_method_) ? MFS_CHECKED : 0;
      InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    }
    SetForegroundWindow(hDlg);
    PostMessage(hDlg, WM_NULL, 0, 0);
    int selected = (int)TrackPopupMenu(hMenu,
                                       TPM_RETURNCMD|TPM_LEFTBUTTON,
                                       clicked_at.x,
                                       clicked_at.y,
                                       0,
                                       (HWND)(hDlg),
                                       NULL);
    DestroyMenu(hMenu);
    if (selected >= SELECT_SYNC_METHOD &&
        selected < SELECT_RIGHT_CLICK_ACTION) {
      sync_method_ = SyncMethod(selected - SELECT_SYNC_METHOD);
    } else if (selected >= SELECT_RIGHT_CLICK_ACTION) {
      right_click_action_
        = RightClickAction(selected - SELECT_RIGHT_CLICK_ACTION);
    }
  }
  return 0;
}




//----[  actorRenderer  ]------------------------------------------------------
Actor::ActorRenderer* ActorEditingDialog::actorRenderer() {
  return editor_->getActorRenderer();
}



//----[  skinnedMeshRenderer  ]------------------------------------------------
SkinnedMesh::SkinnedMeshRenderer*
ActorEditingDialog::skinnedMeshRenderer() {
  return editor_->getSkinnedMeshRenderer();
}

}