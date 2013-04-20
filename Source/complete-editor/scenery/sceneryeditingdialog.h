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
#include "shared/scenery/tools/scenery.h"
#include "shared/evidyoncamera.h"
#include "shared/scenery/sceneryrenderer.h"
#include "shared/mesh/tools/mesh.h"
#include "shared/mesh/meshrenderer.h"
#include "shared/evidyond3dlightmanager.h"


namespace Evidyon {


  
//----[  SceneryEditingDialog  ]-----------------------------------------------
// This dialog lets the user:
//  - assign scenery texture subsets
//  - resize, rotate, translate scenery (randomly & in defined ways)
//  - transform source mesh and see effect on all existing scenery
// Format:
//  - start with an arbitrary list of scenery objects (specified by the user)
//  - sort scenery by their source mesh
//  - give each mesh its own column (5 tiles wide)
//  - give each scenery element its own row (5 tiles high)
//  - arrow keys navigate between instances
//  - context menu allows actions:
//      toggle between editing source mesh & editing scenery instance
//      change subset to which texture is being assigned
//      assign subset texture via dialog
//      duplicate this subset texture for all scenery of this type
//      assign scenery to regular Y-rotated intervals
//      add a new scenery element for a mesh
// scenery/mesh:
//  - change uniform scale by holding S and moving mouse
//  - change x/z scale & Y scale by holding D and moving mouse (horiz. is x/z, vert = y)
//  - change X/Z translation by holding T and moving mouse
//  - change Y translation by holding Y and moving mouse
//  - change Y rotation with R & mouse
// scenery only:
//    - change texture by holding X and moving mouse
//    - change the attached special fx's horizontal offset with S
//    - vertical offset with A
class SceneryEditingDialog : public dcx::dcxWin32DialogController,
                                    Tools::D3DWindow {
  struct SubsetOfSceneryBeingEdited {
    Texture::TextureIndex texture_index;
    Scenery::Tools::ScenerySubset* subset;
  };

  struct SceneryBeingEdited {
    float x, z;
    Scenery::SceneryIndex scenery_index;
    Scenery::Tools::Scenery* scenery;
    SubsetOfSceneryBeingEdited* subsets;
    D3DXVECTOR3 scaling, rotation, translation, visual_fx_offset;
    Scenery::SceneryInstance* instance;
  };

  struct SceneryWithSameSourceMesh {
    int column;
    Mesh::MeshIndex mesh_index;
    Mesh::Tools::Mesh* mesh;
    D3DXVECTOR3 scaling, rotation, translation;
    typedef std::vector<SceneryBeingEdited*> SceneryArray;
    SceneryArray mesh_scenery;
  };

public:
  SceneryEditingDialog();
  virtual ~SceneryEditingDialog();
  void destroy();

  // The functionoid is automatically deleted when this dialog exits as long as
  // begin() has successfully instantiated the dialog.
  bool begin(Evidyon::Tools::CompleteEditor* editor,
             dc::dcGenericResource::Array& scenery_to_edit);
  virtual HWND getWindowHandle();
  virtual bool render(Evidyon::Tools::CompleteEditor* editor,
                      LPDIRECT3DDEVICE9 d3d_device);

  // Writes the SRT and texture information into the scenery objects
  void saveChanges();

private:
  virtual INT_PTR dialogEvent(HWND hDlg,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam);

  size_t indexInMeshList(SceneryBeingEdited* scenery,
                         SceneryWithSameSourceMesh* mesh);

  // Rebuilds the scenery's transformation matrix based on its source mesh's
  // transformation and its own transformation.
  void updateSceneryInstance(SceneryWithSameSourceMesh* mesh,
                             SceneryBeingEdited* scenery);

  // Generates a new scenery instance of the given type.  It will
  // duplicate the properties of the first instance in the list,
  // if it exists.
  void createScenery(const std::string& name,
                     SceneryWithSameSourceMesh* mesh,
                     SceneryBeingEdited* source_scenery);

  void addSceneryOfMesh(SceneryWithSameSourceMesh* mesh,
                        Scenery::Tools::Scenery* scenery);

  // Updates which scenery instances are visible based on the camera's
  // current position.
  void updateVisibleScenery();

  // Shortcuts to obtaining these objects from the editor object
  Mesh::MeshRenderer* meshRenderer();
  Scenery::SceneryRenderer* sceneryRenderer();

private:
  // Main window handle
  HWND dialog_handle_;

  // Root editor object
  Evidyon::Tools::CompleteEditor* editor_;

  // The set of meshes that are being displayed
  SceneryWithSameSourceMesh* meshes_;
  size_t number_of_meshes_;

  // Screen coordinates where the mouse was clicked.  This is used to
  // calculate how much to modify whatever attribute is being changed.
  int mouse_down_x_, mouse_down_y_;

  // The object currently being edited
  bool editing_mesh_;
  SceneryWithSameSourceMesh* current_mesh_;
  SceneryBeingEdited* current_scenery_;
  SubsetOfSceneryBeingEdited* current_scenery_subset_;
  struct {
    struct {
      D3DXVECTOR3 scaling, rotation, translation;
    } mesh;
    struct {
      D3DXVECTOR3 scaling, rotation, translation;
      D3DXVECTOR3 visual_fx_offset;
    } scenery;
  } current_original_transforms_;
  Texture::TextureIndex current_original_texture_;

  // Current position in the map.  This is smoothed toward the mesh &
  // scenery objects being edited.
  D3DXVECTOR2 camera_position_;
  EvidyonCamera camera_;

  // Updated each time render() is called.
  double last_update_time_;
};



}

#endif