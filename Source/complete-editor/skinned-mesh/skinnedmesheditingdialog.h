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
#ifndef __SCENERYEDITINGDIALOG_H__
#define __SCENERYEDITINGDIALOG_H__
#pragma once


#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "shared/skinned-mesh/skinnedmeshattachmentpoint.h"
#include "shared/skinned-mesh/tools/skinnedmeshform.h"
#include "shared/skinned-mesh/tools/skinnedmeshattachmentpointoffset.h"
#include "shared/skinned-mesh/tools/skinnedmesh.h"
#include "shared/skinned-mesh/tools/skinnedmeshanimation.h"
#include "shared/evidyoncamera.h"
#include "shared/evidyond3dlightmanager.h"


namespace Evidyon {
namespace Scenery {
struct SceneryInstance;
}
}


namespace Evidyon {




  
//----[  SkinnedMeshEditingDialog  ]-------------------------------------------
// Double Right-Click Menu:
//    Walk - Makes actor walk toward mouse when clicked, or back to source
//           location when released.  User selects the animation to perform.
//    Run - Same as walk, but moves at least 2x as fast
//    Set Right-Click Animation - Pick a non-base animation to execute when
//                                the user right-clicks
// S + Click = Modify Scaling
// R + Click = Modify Root Rotation
class SkinnedMeshEditingDialog : public dcx::dcxWin32DialogController,
                                        Tools::D3DWindow {
  enum EditAction {
    EDIT_VERTICAL_OFFSET,
    EDIT_HORIZONTAL_OFFSET,
    EDIT_UNIFORM_SCALING,
    EDIT_HORIZ_VERT_SCALING,
    EDIT_ANIMATION_SPEED,
    EDIT_RUN_ACTIONANIMATION,
    EDIT_X_ROTATION,
    EDIT_Y_ROTATION,
    EDIT_Z_ROTATION,
    NUMBER_OF_EDIT_ACTIONS,
  };

  static const char* EditActionString(EditAction action);

  struct SkinnedMeshFormBeingEdited {
    SkinnedMesh::Tools::SkinnedMeshForm* form;
    SkinnedMesh::SkinnedMeshFormIndex form_index;
    SkinnedMesh::SkinnedMeshInstance* instance;
    float x, z; // "home" location
    float current_x, current_z, current_angle; // current location
    D3DXVECTOR3 scaling, rotation, translation;
  };

  struct SkinnedMeshAnimationBeingEdited {
    SkinnedMesh::Tools::SkinnedMeshAnimation* animation;
    SkinnedMesh::SkinnedMeshAnimationIndex animation_index;
    float speed_factor;
  };

  struct SkinnedMeshAttachmentPointOffsetBeingEdited {
    SkinnedMesh::Tools::SkinnedMeshAttachmentPointOffset*
      attachment_point_offset;
    unsigned int source_bone;
    D3DXVECTOR3 scaling, rotation, translation;
  };

  struct SkinnedMeshBeingEdited {
    int column;
    SkinnedMesh::Tools::SkinnedMesh* skinned_mesh;
    typedef std::vector<SkinnedMeshFormBeingEdited*> FormArray;
    FormArray forms;
    typedef std::vector<SkinnedMeshAnimationBeingEdited*> AnimationArray;
    AnimationArray animations;
    SkinnedMesh::SkinnedMeshIndex skinned_mesh_index;
    SkinnedMeshAttachmentPointOffsetBeingEdited
      attachment_points[SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS];
    D3DXVECTOR3 scaling, rotation, translation; // combined into each form
  };

public:
  SkinnedMeshEditingDialog();
  virtual ~SkinnedMeshEditingDialog();
  void destroy();

  bool begin(Evidyon::Tools::CompleteEditor* editor,
             dc::dcGenericResource::Array& skinned_meshes_to_edit);
  virtual HWND getWindowHandle();
  virtual bool render(Evidyon::Tools::CompleteEditor* editor,
                      LPDIRECT3DDEVICE9 d3d_device);

private:
  virtual INT_PTR dialogEvent(HWND hDlg,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam);

  // Updates which scenery instances are visible based on the camera's
  // current position.
  void updateVisibleInstances();

  SkinnedMesh::SkinnedMeshRenderer* skinnedMeshRenderer();

  HMENU createAnimationsSubmenu(int base_id);
  HMENU createAttachmentPointsSubmenu(int base_id);

  size_t indexInFormsList(SkinnedMeshFormBeingEdited* form,
                          SkinnedMeshBeingEdited* skinned_mesh) const;
  size_t indexInAttachmentPointsList(
    SkinnedMeshBeingEdited* skinned_mesh,
     SkinnedMeshAttachmentPointOffsetBeingEdited* point) const;
  void updateSkinnedMeshForm(SkinnedMeshBeingEdited* skinned_mesh,
                             SkinnedMeshFormBeingEdited* form);
  void updateSkinnedMeshAttachmentPoint(
    SkinnedMeshBeingEdited* skinned_mesh,
    SkinnedMeshAttachmentPointOffsetBeingEdited* form);

  // Writes all changes to the meshes
  void saveChanges();
  void resetForm();


  void promptUserForAttachmentPointScenery(
    SkinnedMesh::SkinnedMeshAttachmentPoint attachment_point);

private:

  // Main window handle
  HWND dialog_handle_;

  // Root editor object
  Evidyon::Tools::CompleteEditor* editor_;

  // The set of skinned meshes that are being displayed
  SkinnedMeshBeingEdited* skinned_meshes_;
  size_t number_of_skinned_meshes_;

  // Screen coordinates where the mouse was clicked.  This is used to
  // calculate how much to modify whatever attribute is being changed.
  int mouse_down_x_, mouse_down_y_;

  // Rate at which the currently selected mesh moves toward the mouse
  float last_mouse_down_world_x_, last_mouse_down_world_z_;
  float mouse_world_x_, mouse_world_z_;
  float moving_rate_;

  // Configured by user pressing number keys
  float selected_moving_rate_;

  // Active editing parameters
  SkinnedMeshBeingEdited* current_skinned_mesh_;
  SkinnedMeshFormBeingEdited* current_form_;
  SkinnedMeshAttachmentPointOffsetBeingEdited* current_attachment_point_;

  // Executed with a base speed of the rate at which the mesh slides
  // toward the mouse location.
  SkinnedMeshAnimationBeingEdited* current_base_animation_;

  // This animation is run when the user right-clicks
  SkinnedMeshAnimationBeingEdited* current_action_animation_;

  EditAction edit_action_;

  //// The object currently being edited
  struct {
    struct {
      D3DXVECTOR3 scaling, rotation, translation;
    } attachment_point;
    struct {
      D3DXVECTOR3 scaling, rotation, translation;
    } form;
    struct {
      float speed_factor;
    } animation;
  } current_original_transforms_;
  bool editing_attachment_point_;
  //Texture::TextureIndex current_original_texture_;

  // Current position in the map.  This is smoothed toward the mesh &
  // scenery objects being edited.
  D3DXVECTOR2 camera_position_;
  EvidyonCamera camera_;

  // Updated each time render() is called.
  double last_update_time_;

  // Items attached to the various points on this skinned mesh for
  // previewing their transforms.
  Scenery::SceneryInstance*
    attachment_point_items_[SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS];
};



}

#endif