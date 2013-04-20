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
#include "sceneryeditingdialog.h"
#include "common/safemacros.h"
#include <assert.h>


namespace Evidyon {

  
//----[  SceneryEditingDialog  ]-----------------------------------------------
SceneryEditingDialog::SceneryEditingDialog() : camera_position_(0.0f, 0.0f) {
  dialog_handle_ = NULL;
  editor_ = NULL;
  meshes_ = NULL;
  number_of_meshes_ = NULL;
  mouse_down_x_ = -9999;
  mouse_down_y_ = -9999;
  editing_mesh_ = false;
  current_mesh_ = NULL;
  current_scenery_ = NULL;
  current_scenery_subset_ = NULL;
  last_update_time_ = 0.0;
}


//----[  ~SceneryEditingDialog  ]----------------------------------------------
SceneryEditingDialog::~SceneryEditingDialog() {
  destroy();
}



//----[  destroy  ]------------------------------------------------------------
void SceneryEditingDialog::destroy() {
  if (meshes_) {
    Scenery::SceneryRenderer* scenery_renderer = sceneryRenderer();
    for (size_t i = 0; i < number_of_meshes_; ++i) {
      SceneryWithSameSourceMesh::SceneryArray* list = &meshes_[i].mesh_scenery;
      for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin();
           i != list->end(); ++i) {
        SceneryBeingEdited* scenery_being_edited = *i;
        SAFE_DELETE_ARRAY(scenery_being_edited->subsets);
        if (scenery_renderer && scenery_being_edited->instance) {
          scenery_renderer->release(scenery_being_edited->instance);
        }
      }
    }
    SAFE_DELETE_ARRAY(meshes_);
  }
}

  
//----[  begin  ]--------------------------------------------------------------
bool SceneryEditingDialog::begin(
    Evidyon::Tools::CompleteEditor* editor,
    dc::dcGenericResource::Array& scenery_to_edit) {
  editor_ = editor;
  editor->acquireTextures(false);
  editor->acquireScenery();

  // Get the set of meshes referenced by this scenery
  typedef std::set<Mesh::Tools::Mesh*> MeshesSet;
  typedef std::multimap<Mesh::Tools::Mesh*, Scenery::Tools::Scenery*> MeshToSceneryMap;
  MeshesSet meshes;
  MeshToSceneryMap scenery_map;
  for (dc::dcGenericResource::Array::iterator i = scenery_to_edit.begin();
      i != scenery_to_edit.end(); ++i) {
    dc::dcList<Scenery::Tools::Scenery>::Element* scenery
          = (dc::dcList<Scenery::Tools::Scenery>::Element*)(*i);
    dc::dcTable<Mesh::Tools::Mesh>::Element* mesh_interface =
      scenery->getReferencedMeshTableElement();
    if (mesh_interface) {
      Mesh::Tools::Mesh* mesh = mesh_interface->getImplementation();
      if (mesh) {
        meshes.insert(mesh);
        scenery_map.insert(MeshToSceneryMap::value_type(mesh, scenery));
      }
    }
  }

  if (meshes.empty() || scenery_map.empty()) return false;

  meshes_ = new SceneryWithSameSourceMesh[meshes.size()];
  number_of_meshes_ = meshes.size();

  size_t index = 0;
  for (MeshesSet::iterator i = meshes.begin(); i != meshes.end(); ++i, ++index) {
    SceneryWithSameSourceMesh* mesh_group = &meshes_[index];
    mesh_group->column = index;
    Mesh::Tools::Mesh* mesh = *i;
    mesh_group->mesh = mesh;
    mesh_group->mesh_index = mesh->getOwner()->getIndex();

    // The mesh's SRT is actually integrated in the geometry itself, so just
    // set these values to identity.
    mesh_group->scaling.x = 1.0f;
    mesh_group->scaling.y = 1.0f;
    mesh_group->scaling.z = 1.0f;
    mesh_group->rotation.x = 0.0f;
    mesh_group->rotation.y = 0.0f;
    mesh_group->rotation.z = 0.0f;
    mesh_group->translation.x = 0.0f;
    mesh_group->translation.y = 0.0f;
    mesh_group->translation.z = 0.0f;

    MeshToSceneryMap::iterator lo = scenery_map.lower_bound(mesh);
    MeshToSceneryMap::iterator hi = scenery_map.upper_bound(mesh);
    for (MeshToSceneryMap::iterator j = lo; j != hi; ++j) {
      addSceneryOfMesh(mesh_group, j->second);
    }
  }

  // set up for editing the first one
  current_mesh_ = &meshes_[0];
  current_scenery_ = current_mesh_ ->mesh_scenery.front();
  current_scenery_subset_ = &current_scenery_->subsets[0];


  dcx::dcxWin32DialogTemplate dialog;
  dialog.setStyleAsResizeableAppWindow();
  dialog.setTitle("Scenery Editor");
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
HWND SceneryEditingDialog::getWindowHandle() {
  return dialog_handle_;
}





//----[  render  ]-------------------------------------------------------------
bool SceneryEditingDialog::render(Evidyon::Tools::CompleteEditor* editor,
                                  LPDIRECT3DDEVICE9 d3d_device) {

  // Slide to the current object
  D3DXVECTOR2 delta, target(current_scenery_->x, current_scenery_->z);
  D3DXVec2Subtract(&delta,
                   &target,
                   &camera_position_);
  if (D3DXVec2LengthSq(&delta) > 0.01f) {
    D3DXVec2Lerp(&camera_position_,
                 &camera_position_,
                 &target,
                 0.1f);
    updateVisibleScenery();
  } else {
    camera_position_ = target;
  }

  Evidyon::VisualFX::VisualFXRenderer* visual_fx_renderer
    = editor_->getVisualFXRenderer();

  // Advance all special effects
  double time = dcx::dcxWin32Clock::getAccurateSystemTime();
  visual_fx_renderer->update(time, time - last_update_time_);
  last_update_time_ = time;

  if (SUCCEEDED(d3d_device->BeginScene())) {
    d3d_device->Clear(0,
                      NULL,
                      D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                      D3DCOLOR_XRGB(0,0,0),
                      1.0f,
                      0);
    d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);

    camera_.setPosition(camera_position_.x, camera_position_.y);
    camera_.initialize3DRendering(d3d_device);
    EvidyonD3DLightManager* light_manager
      = editor_->getLightManager();
    light_manager->setDaylightAmbience();
    light_manager->beginScene(camera_position_.x, camera_position_.y);

    EvidyonD3DRenderStateManager* renderstate_manager
      = editor_->getRenderStateManager();
    Scenery::SceneryRenderer* scenery_renderer
      = editor_->getSceneryRenderer();
    renderstate_manager->beginScene();
    scenery_renderer->beginScene();
    visual_fx_renderer->beginScene();

    // Render all of the objects in the world
    size_t number_of_textures = editor->getTextureTable()->getMembers().size();
    for (Texture::TextureIndex texture = 0;
                               texture < number_of_textures;
                             ++texture) {
      scenery_renderer->render(texture, renderstate_manager);
      visual_fx_renderer->render(texture);
    }

    // Draw the ground plane
    if (GetAsyncKeyState(VK_TAB) == 0) {
      d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
      d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
      d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
      d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
      d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
      d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
      d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
      d3d_device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
      d3d_device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
      d3d_device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
      d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
      renderstate_manager->setWorldMatrix(0, NULL);
      float x_left = camera_position_.x - 20.0f;
      float x_right = camera_position_.x + 20.0f;
      float y_top = camera_position_.y - 20.0f;
      float y_bottom = camera_position_.y + 20.0f;
      RECT visibility_area = {
        int(x_left),
        int(y_top),
        int(x_right),
        int(y_bottom)
      };
      struct Vertex {
        FLOAT x, y, z;
        D3DCOLOR diffuse;
      };
      for (int y = visibility_area.top; y < visibility_area.bottom; ++y) {
        for (int x = visibility_area.left; x < visibility_area.right; ++x) {
          static const D3DCOLOR wire_color = D3DCOLOR_ARGB(255,255,255,255);
          Vertex vertices[4] = {
            { x*1.0f, 0.0f, y*1.0f, wire_color },
            { float(x*1.0f+1.0f), 0.0f, y*1.0f, wire_color },
            { float(x*1.0f+1.0f), 0.0f, (y+1)*1.0f, wire_color },
            { x*1.0f, 0.0f, (y+1)*1.0f, wire_color },
          };
          d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
        }
      }
      d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
      static const D3DCOLOR color = D3DCOLOR_ARGB(128, 128,64,0);
      Vertex vertices[4] = {
        { x_left, 0.0f, y_top, color },
        { float(x_right), 0.0f, y_top, color },
        { float(x_right), 0.0f, y_bottom, color },
        { x_left, 0.0f, y_bottom, color },
      };
      d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));


    }

    d3d_device->EndScene();
  }
  return true;
}





//----[  saveChanges  ]--------------------------------------------------------
void SceneryEditingDialog::saveChanges() {
  if (meshes_) {
    D3DXVECTOR3 *s, *r, *t;
    for (size_t i = 0; i < number_of_meshes_; ++i) {
      SceneryWithSameSourceMesh* mesh = &meshes_[i];
      s = &mesh->scaling;
      r = &mesh->rotation;
      t = &mesh->translation;
      mesh->mesh->combineSRT(s->x, s->y, s->z,
                             r->x, r->y, r->z,
                             t->x, t->y, t->z);
      size_t subsets = meshRenderer()->getNumberOfMeshSubsets(mesh->mesh_index);
      SceneryWithSameSourceMesh::SceneryArray* list = &mesh->mesh_scenery;
      for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin();
           i != list->end(); ++i) {
        SceneryBeingEdited* scenery_being_edited = *i;
        const dc::dcGenericResource::Array& textures = editor_->getTextures();
        Scenery::Tools::Scenery* scenery = scenery_being_edited->scenery;
        s = &scenery_being_edited->scaling;
        r = &scenery_being_edited->rotation;
        t = &scenery_being_edited->translation;
        scenery->setSRT(s->x, s->y, s->z,
                        r->x, r->y, r->z,
                        t->x, t->y, t->z);
        t = &scenery_being_edited->visual_fx_offset;
        scenery->setVisualFXOffset(t->x, t->y, t->z);
        for (size_t j = 0; j < subsets; ++j) {
          Texture::TextureIndex texture = scenery_being_edited->subsets[j].texture_index;
          dc::dcGenericResource* texture_resource
            = (texture >= textures.size()) ? NULL : textures.at(texture);
          scenery->getSubset(j)->setReferencedTexture(texture_resource);
        }
      }
    }
  }
}




//----[  dialogEvent  ]--------------------------------------------------------
INT_PTR SceneryEditingDialog::dialogEvent(HWND hDlg,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
  if (uMsg == WM_CLOSE) {
    int selection = dcx::dcxYesNoCancelDialog(
      hDlg,
      "Save Changes?",
      "Would you like to save the changes to this scenery?");
    if (selection == IDCANCEL) return 0;
    if (selection == IDYES) saveChanges();
    editor_->removeD3DWindow(this);
    EndDialog(hDlg, 0);
    DestroyWindow(hDlg); 
    SetWindowLong(hDlg, GWL_USERDATA, NULL);
    delete this;
  } else if (uMsg == WM_PAINT) {
    editor_->renderD3DWindow(this);
  } else if (uMsg == WM_CONTEXTMENU) {
    POINTS clicked_at = MAKEPOINTS(lParam);
    MENUITEMINFO menu_item;

    enum {
      EDIT_SOURCE_MESH = 1000,
      EDIT_SCENERY,
      CREATE_NEW_SCENERY,
      EDIT_SCENERY_SUBSET, // must go at end, generates +i for each scenery
    };
    char string_buffer[512];

    HMENU hMenu = CreatePopupMenu();
    const std::string& mesh_name = current_mesh_->mesh->getOwner()->getName();
    sprintf_s(string_buffer,
              512,
              "Edit Mesh '%s'",
              mesh_name.c_str());
    dcx::dcxWin32InitMenuItem(&menu_item, string_buffer, false, EDIT_SOURCE_MESH);
    menu_item.fState = editing_mesh_ ? MFS_CHECKED : 0;
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    HMENU hScenerySubsetMenu = CreateMenu();
    size_t number_of_subsets
      = meshRenderer()->getNumberOfMeshSubsets(current_mesh_->mesh_index);
    for (size_t i = 0; i < number_of_subsets; ++i) {
      Evidyon::Scenery::Tools::ScenerySubset* subset
        = current_scenery_->subsets[i].subset;
      sprintf_s(string_buffer,
                512,
                "Subset %s",
                subset->getName().c_str());
      dcx::dcxWin32InitMenuItem(&menu_item, subset->getName().c_str(),
                                false,
                                EDIT_SCENERY_SUBSET+i);
      menu_item.fState = (current_scenery_subset_->subset==subset) ? MFS_CHECKED : 0;
      InsertMenuItem(hScenerySubsetMenu, 0xFFFFFFFF, TRUE, &menu_item);
    }
    sprintf_s(string_buffer,
              512,
              "Edit Scenery '%s'",
              current_scenery_->scenery->getName().c_str());
    dcx::dcxWin32InitMenuItem(&menu_item,
                              string_buffer,
                              hScenerySubsetMenu);
    menu_item.fState = editing_mesh_ ? 0 : MFS_CHECKED;
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItemSeperator(&menu_item);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item,
                              "Create Scenery",
                              false, CREATE_NEW_SCENERY);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);

    SetForegroundWindow(hDlg);
    PostMessage(hDlg, WM_NULL, 0, 0);
    int selected = (int)TrackPopupMenu(hMenu,
                                       TPM_RETURNCMD|TPM_LEFTBUTTON,
                                       clicked_at.x,
                                       clicked_at.y,
                                       0,
                                       (HWND)(hDlg),
                                       NULL);
    DestroyMenu(hScenerySubsetMenu);
    DestroyMenu(hMenu);

    switch (selected) {
      case 0: break; // dialog was canceled
      case EDIT_SOURCE_MESH:    editing_mesh_ = true; break;
      case EDIT_SCENERY:        editing_mesh_ = false; break;
      case CREATE_NEW_SCENERY: {
        int number_of_new_instances = 1;
        std::string name;
        if (!dcx::dcxWin32PromptNumericInput(
            hDlg,
            "Create Duplicate Scenery",
            "How many new instances would you like to make?",
            &number_of_new_instances,
            true)) {
          number_of_new_instances = 0;
        }
        if (number_of_new_instances <= 0 ||
            !dcx::dcxWin32PromptTextInput(
            hDlg,
            "Create Duplicate Scenery",
            number_of_new_instances == 1 ?
            "Enter a name for this new scenery object, or "\
            "leave blank to generate one automatically:"
            :
            "Enter a prefix for these scenery objects (this is optional):",
            &name,
            false, false, false, false)) {
           number_of_new_instances = 0;
        }
        if (number_of_new_instances == 1) {
          if (name.empty()) {
            char generated_name[512];
            sprintf_s(generated_name,
                      512,
                      "%s #%lu", 
                      mesh_name.c_str(),
                      current_mesh_->mesh_scenery.size() + 1);
            name = generated_name;
          }
          createScenery(name, current_mesh_, current_scenery_);
        } else {
          while (number_of_new_instances-- > 0) {
            char generated_name[512];
            sprintf_s(generated_name,
                      512,
                      "%s%s #%lu", 
                      name.c_str(),
                      mesh_name.c_str(),
                      current_mesh_->mesh_scenery.size() + 1);
            createScenery(generated_name, current_mesh_, current_scenery_);
          }
        }
      } break;
      default:
        selected -= EDIT_SCENERY_SUBSET;
        if (selected < number_of_subsets) {
          editing_mesh_ = false;
          current_scenery_subset_ = &current_scenery_->subsets[selected];
        }
        break;
    }

  } else if (uMsg == WM_SIZE) {
    camera_.rebuildTopdownCamera(LOWORD(lParam),HIWORD(lParam));
  } else if (uMsg == WM_KEYDOWN) {
    switch (wParam) {
      case VK_LEFT: {
        int column = current_mesh_->column;
        if (column > 0) {
          current_mesh_ = &meshes_[column - 1];
          current_scenery_ = current_mesh_->mesh_scenery.front();
          current_scenery_subset_ = current_scenery_->subsets;
        }
      } break;

      case VK_RIGHT: {
        int column = current_mesh_->column;
        if (column + 1 < number_of_meshes_) {
          current_mesh_ = &meshes_[column + 1];
          current_scenery_ = current_mesh_->mesh_scenery.front();
          current_scenery_subset_ = current_scenery_->subsets;
        }
      } break;

      case VK_UP: {
        size_t index = indexInMeshList(current_scenery_, current_mesh_);
        if (index > 0) {
          --index;
          current_scenery_ = current_mesh_->mesh_scenery.at(index);
          current_scenery_subset_ = current_scenery_->subsets;
        }
      } break;

      case VK_DOWN: {
        size_t index = 1 + indexInMeshList(current_scenery_, current_mesh_);
        if (index < current_mesh_->mesh_scenery.size()) {
          current_scenery_ = current_mesh_->mesh_scenery.at(index);
          current_scenery_subset_ = current_scenery_->subsets;
        }
      } break;

      case 'Z': {
        current_mesh_->rotation = current_original_transforms_.mesh.rotation;
        current_mesh_->scaling = current_original_transforms_.mesh.scaling;
        current_mesh_->translation = current_original_transforms_.mesh.translation;
        current_scenery_->rotation = current_original_transforms_.scenery.rotation;
        current_scenery_->scaling = current_original_transforms_.scenery.scaling;
        current_scenery_->translation = current_original_transforms_.scenery.translation;
        current_scenery_->visual_fx_offset = current_original_transforms_.scenery.visual_fx_offset;
        SceneryWithSameSourceMesh::SceneryArray* list = &current_mesh_->mesh_scenery;
        for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin();
             i != list->end(); ++i) {
          updateSceneryInstance(current_mesh_, *i);
        }
      } break;
    }
  } else if (uMsg == WM_LBUTTONDOWN) {
    POINTS pts = MAKEPOINTS(lParam);
    mouse_down_x_ = pts.x;
    mouse_down_y_ = pts.y;
    current_original_transforms_.mesh.rotation = current_mesh_->rotation;
    current_original_transforms_.mesh.scaling  = current_mesh_->scaling;
    current_original_transforms_.mesh.translation = current_mesh_->translation;
    current_original_transforms_.scenery.rotation = current_scenery_->rotation;
    current_original_transforms_.scenery.scaling  = current_scenery_->scaling;
    current_original_transforms_.scenery.translation = current_scenery_->translation;
    current_original_transforms_.scenery.visual_fx_offset = current_scenery_->visual_fx_offset;
    current_original_texture_ = current_scenery_subset_->texture_index;
  } else if (uMsg == WM_MOUSEMOVE) {
    POINTS pts = MAKEPOINTS(lParam);
    if ((wParam & MK_LBUTTON) == MK_LBUTTON) {
      if (GetAsyncKeyState('T') != 0) { // X&Z position
        D3DXVECTOR3 offset((pts.x - mouse_down_x_) / 100.0, 0.0f,
                           (pts.y - mouse_down_y_) / 100.0);
        if (editing_mesh_) {
          current_mesh_->translation = current_original_transforms_.mesh.translation + offset;
          SceneryWithSameSourceMesh::SceneryArray* list = &current_mesh_->mesh_scenery;
          for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin();
               i != list->end(); ++i) {
            updateSceneryInstance(current_mesh_, *i);
          }
        } else {
          current_scenery_->translation
            = current_original_transforms_.scenery.translation + offset;
          updateSceneryInstance(current_mesh_, current_scenery_);
        }
      } else if (GetAsyncKeyState('Y') != 0) { // vertical position
        D3DXVECTOR3 offset(0.0f, -(pts.y - mouse_down_y_) / 100.0, 0.0f);
        if (editing_mesh_) {
          current_mesh_->translation
            = current_original_transforms_.mesh.translation + offset;
          SceneryWithSameSourceMesh::SceneryArray* list = &current_mesh_->mesh_scenery;
          for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin();
               i != list->end(); ++i) {
            updateSceneryInstance(current_mesh_, *i);
          }
        } else {
          current_scenery_->translation
            = current_original_transforms_.scenery.translation + offset;
          updateSceneryInstance(current_mesh_, current_scenery_);
        }
      } else if (GetAsyncKeyState('S')) { // uniform scale
        int center_x = 1024/2;
        int center_y = 768/2;
        float sdx = (mouse_down_x_ - center_x) * 1.0;
        float sdy = (mouse_down_y_ - center_y) * 1.0;
        float dx = (pts.x - center_x) * 1.0;
        float dy = (pts.y - center_y) * 1.0;
        float s = sqrtf(dx*dx+dy*dy)/sqrtf(sdx*sdx+sdy*sdy);

        if (editing_mesh_) {
          current_mesh_->scaling
            = s * current_original_transforms_.mesh.scaling;
          SceneryWithSameSourceMesh::SceneryArray* list = &current_mesh_->mesh_scenery;
          for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin();
               i != list->end(); ++i) {
            updateSceneryInstance(current_mesh_, *i);
          }
        } else {
          current_scenery_->scaling
            = s * current_original_transforms_.scenery.scaling;
          updateSceneryInstance(current_mesh_, current_scenery_);
        }
      } else if (GetAsyncKeyState('D')) { // XZ + Y scale
        int center_x = 1024/2;
        int center_y = 768/2;
        float sdx = (mouse_down_x_ - center_x) * 1.0;
        float sdy = (mouse_down_y_ - center_y) * 1.0;
        float dx = (pts.x - center_x) * 1.0;
        float dy = (pts.y - center_y) * 1.0;
        float scale_xz = dx*dx / (sdx*sdx);
        float scale_y = dy*dy / (sdy*sdy);

        if (editing_mesh_) {
          current_mesh_->scaling.x
            = scale_xz * current_original_transforms_.mesh.scaling.x;
          current_mesh_->scaling.y
            = scale_y * current_original_transforms_.mesh.scaling.y;
          current_mesh_->scaling.z
            = scale_xz * current_original_transforms_.mesh.scaling.z;
          SceneryWithSameSourceMesh::SceneryArray* list = &current_mesh_->mesh_scenery;
          for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin();
               i != list->end(); ++i) {
            updateSceneryInstance(current_mesh_, *i);
          }
        } else {
          current_scenery_->scaling.x
            = scale_xz * current_original_transforms_.scenery.scaling.x;
          current_scenery_->scaling.y
            = scale_y * current_original_transforms_.scenery.scaling.y;
          current_scenery_->scaling.z
            = scale_xz * current_original_transforms_.scenery.scaling.z;
          updateSceneryInstance(current_mesh_, current_scenery_);
        }
      } else if (GetAsyncKeyState('R')) { // rotate about Y
        float center_x = 1024/2.0f;
        float center_y = 768/2.0f;
        float rotate_amount
          = atan2f(mouse_down_y_-center_y, mouse_down_x_-center_x)
          - atan2f(pts.y-center_y,pts.x-center_x);
        //float rotate_amount = -(pts.x - mouse_down_x_) / 300.0 * D3DX_PI/2.0f;
        if (editing_mesh_) {
          current_mesh_->rotation.y
            = rotate_amount + current_original_transforms_.mesh.rotation.y;
          if ((wParam & MK_SHIFT) == MK_SHIFT) { // constrain!
            current_mesh_->rotation.y -=
              fmodf(current_mesh_->rotation.y,D3DX_PI/4.0f);
          }
          SceneryWithSameSourceMesh::SceneryArray* list = &current_mesh_->mesh_scenery;
          for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin();
               i != list->end(); ++i) {
            updateSceneryInstance(current_mesh_, *i);
          }
        } else {
          current_scenery_->rotation.y
            = rotate_amount + current_original_transforms_.scenery.rotation.y;
          if ((wParam & MK_CONTROL) == MK_CONTROL) { // constrain!
            current_scenery_->rotation.y -=
              fmodf(current_scenery_->rotation.y,D3DX_PI/4.0f);
          }
          updateSceneryInstance(current_mesh_, current_scenery_);
        }
      } else if (GetAsyncKeyState('X')) { // change texture
        if (!editing_mesh_) {
          // divide by the tile pixel size
          int dx = (pts.x - mouse_down_x_) / 55;
          int dy = (pts.y - mouse_down_y_) / 49;
          int added_tex = dy * 10 + dx;
          size_t total_textures = editor_->getTextures().size();
          current_scenery_subset_->texture_index
            = (total_textures * 99 // some large mult. to make the % positive
               + added_tex
               + current_original_texture_) % total_textures;
          updateSceneryInstance(current_mesh_, current_scenery_);
        }
      } else if (GetAsyncKeyState('Q')) { // move special fx planar (X/Z)
        if (!editing_mesh_) {
          D3DXVECTOR3 offset((pts.x - mouse_down_x_) / 100.0, 0.0f,
                             (pts.y - mouse_down_y_) / 100.0);
          current_scenery_->visual_fx_offset
            = current_original_transforms_.scenery.visual_fx_offset + offset;
          updateSceneryInstance(current_mesh_, current_scenery_);
        }
      } else if (GetAsyncKeyState('W')) { // move special fx vertically (Y)
        if (!editing_mesh_) {
          D3DXVECTOR3 offset(0.0f, -(pts.y - mouse_down_y_) / 100.0, 0.0f);
          current_scenery_->visual_fx_offset
            = current_original_transforms_.scenery.visual_fx_offset + offset;
          updateSceneryInstance(current_mesh_, current_scenery_);
        }
      }
    }
  }

  return 0;
}




//----[  indexInMeshList  ]----------------------------------------------------
size_t SceneryEditingDialog::indexInMeshList(
    SceneryBeingEdited* scenery,
    SceneryWithSameSourceMesh* mesh) {  
  assert(scenery);
  assert(mesh);
  size_t index = 0;
  for (SceneryWithSameSourceMesh::SceneryArray::iterator i = mesh->mesh_scenery.begin();
       i != mesh->mesh_scenery.end(); ++i, ++index) {
    if (*i == scenery) return index;
  }
  assert(false);
  return -1;
}






//----[  updateSceneryInstance  ]----------------------------------------------
void SceneryEditingDialog::updateSceneryInstance(
    SceneryWithSameSourceMesh* mesh,
    SceneryBeingEdited* scenery) {
  D3DXMATRIXA16 s, r, t, mesh_matrix, scenery_matrix;

  // Build the mesh's transform
  D3DXMatrixScaling(&s,
                    mesh->scaling.x,
                    mesh->scaling.y,
                    mesh->scaling.z);
  D3DXMatrixRotationYawPitchRoll(&r,
                                 mesh->rotation.y,
                                 mesh->rotation.x,
                                 mesh->rotation.z);
  D3DXMatrixTranslation(&t,
                        mesh->translation.x,
                        mesh->translation.y,
                        mesh->translation.z);
  D3DXMatrixMultiply(&mesh_matrix, &s, &r);
  D3DXMatrixMultiply(&mesh_matrix, &mesh_matrix, &t);

  // Build the scenery's transform
  D3DXMatrixScaling(&s,
                    scenery->scaling.x,
                    scenery->scaling.y,
                    scenery->scaling.z);
  D3DXMatrixRotationYawPitchRoll(&r,
                                 scenery->rotation.y,
                                 scenery->rotation.x,
                                 scenery->rotation.z);
  D3DXMatrixTranslation(&t,
                        scenery->translation.x,
                        scenery->translation.y,
                        scenery->translation.z);
  D3DXMatrixMultiply(&scenery_matrix, &s, &r);
  D3DXMatrixMultiply(&scenery_matrix, &scenery_matrix, &t);

  // Build the positioning offset
  D3DXMatrixTranslation(&t,
                        scenery->x,
                        0.0f,
                        scenery->z);

  // Combine the matrices
  D3DXMATRIXA16 combined;
  D3DXMatrixMultiply(&combined, &mesh_matrix, &scenery_matrix);
  D3DXMatrixMultiply(&scenery->instance->combined_transform,
                     &combined,
                     &t);
  scenery->instance->updateBoundVisualFX(
    scenery->visual_fx_offset.x,
    scenery->visual_fx_offset.y,
    scenery->visual_fx_offset.z);

  // Set the textures
  size_t number_of_subsets
    = meshRenderer()->getNumberOfMeshSubsets(mesh->mesh_index);
  for (size_t i = 0; i < number_of_subsets; ++i) {
    sceneryRenderer()->replaceScenerySubsetTexture(
      scenery->scenery_index,
      i,
      scenery->subsets[i].texture_index);
  }

}







//----[  createScenery  ]------------------------------------------------------
void SceneryEditingDialog::createScenery(const std::string& name,
                                         SceneryWithSameSourceMesh* mesh,
                                         SceneryBeingEdited* source_scenery) {
  Scenery::Tools::Scenery* scenery
    = (Scenery::Tools::Scenery*)editor_->getSceneryList()->generateElement(name);
  scenery->copy(source_scenery->scenery);
  Scenery::Scenery description;
  scenery->compile(meshRenderer(), &description, NULL);
  sceneryRenderer()->addScenery(scenery->getIndex(), description);
  addSceneryOfMesh(mesh, scenery);
}




//----[  addSceneryOfMesh  ]---------------------------------------------------
void SceneryEditingDialog::addSceneryOfMesh(SceneryWithSameSourceMesh* mesh,
                                            Scenery::Tools::Scenery* scenery) {
  size_t number_of_subsets
    = meshRenderer()->getNumberOfMeshSubsets(mesh->mesh_index);
  SceneryBeingEdited* scenery_being_edited = new SceneryBeingEdited();
  mesh->mesh_scenery.push_back(scenery_being_edited);
  scenery->getSRT(&scenery_being_edited->scaling.x,
                  &scenery_being_edited->scaling.y,
                  &scenery_being_edited->scaling.z,
                  &scenery_being_edited->rotation.x,
                  &scenery_being_edited->rotation.y,
                  &scenery_being_edited->rotation.z,
                  &scenery_being_edited->translation.x,
                  &scenery_being_edited->translation.y,
                  &scenery_being_edited->translation.z);
  scenery->getVisualFXOffset(&scenery_being_edited->visual_fx_offset.x,
                             &scenery_being_edited->visual_fx_offset.y,
                             &scenery_being_edited->visual_fx_offset.z);
  scenery_being_edited->scenery = scenery;
  scenery_being_edited->scenery_index = scenery->getIndex();
  scenery_being_edited->subsets
    = new SubsetOfSceneryBeingEdited[number_of_subsets];
  for (size_t i = 0; i < number_of_subsets; ++i) {
    Scenery::Tools::ScenerySubset* subset = scenery->getSubset(i);
    assert(subset);
    scenery_being_edited->subsets[i].texture_index
      = subset->getTextureIndex();
    scenery_being_edited->subsets[i].subset = subset;
  }
  scenery_being_edited->x = 0.5f + mesh->column * 5.0f;
  scenery_being_edited->z = 0.5f
    + indexInMeshList(scenery_being_edited, mesh) * 5.0f;

  scenery_being_edited->instance
    = sceneryRenderer()->acquire(scenery_being_edited->scenery_index);

  updateSceneryInstance(mesh, scenery_being_edited);
}




//----[  updateVisibleScenery  ]-----------------------------------------------
void SceneryEditingDialog::updateVisibleScenery() {
  int min_x = (int)(camera_position_.x - 10.0f);
  int max_x = (int)(camera_position_.x + 10.0f);
  for (size_t i = 0; i < number_of_meshes_; ++i) {
    int column = meshes_[i].column;
    assert(column == i); // just check, this should be true
    bool hidden = (column < min_x && column > max_x);
    SceneryWithSameSourceMesh::SceneryArray* list = &meshes_[i].mesh_scenery;
    for (SceneryWithSameSourceMesh::SceneryArray::iterator i = list->begin(); i != list->end(); ++i) {
      SceneryBeingEdited* scenery_being_edited = *i;
      if (scenery_being_edited->instance) {
        scenery_being_edited->instance->hidden = hidden;
      }
    }
  }
}





//----[  meshRenderer  ]-------------------------------------------------------
Mesh::MeshRenderer* SceneryEditingDialog::meshRenderer() {
  return editor_->getMeshRenderer();
}




//----[  sceneryRenderer  ]----------------------------------------------------
Scenery::SceneryRenderer* SceneryEditingDialog::sceneryRenderer() {
  return editor_->getSceneryRenderer();
}



}
