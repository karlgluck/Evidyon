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
#include "skinnedmesheditingdialog.h"
#include "common/safemacros.h"
#include <assert.h>
#include "common/randf.h"



namespace Evidyon {



  
//----[  EditActionString  ]---------------------------------------------------
const char* SkinnedMeshEditingDialog::EditActionString(EditAction action) {
  switch (action) {
    case EDIT_VERTICAL_OFFSET: return "Change Vertical Offset";
    case EDIT_HORIZONTAL_OFFSET: return "Change Horizontal Offset";
    case EDIT_UNIFORM_SCALING: return "Edit Uniform Scaling";
    case EDIT_HORIZ_VERT_SCALING: return "Edit Horizontal/Vertical Scaling";
    case EDIT_ANIMATION_SPEED: return "Scale Animation Speed";
    case EDIT_RUN_ACTIONANIMATION: return "Run Action Animation";
    case EDIT_X_ROTATION: return "Edit X Rotation";
    case EDIT_Y_ROTATION: return "Edit Y Rotation";
    case EDIT_Z_ROTATION: return "Edit Z Rotation";
    default:
      assert(false);
      return NULL;
  }
}
  
//----[  SkinnedMeshEditingDialog  ]-------------------------------------------
SkinnedMeshEditingDialog::SkinnedMeshEditingDialog()
    : camera_position_(0.0f, 0.0f) {
  dialog_handle_ = NULL;
  editor_ = NULL;
  mouse_down_x_ = -9999;
  mouse_down_y_ = -9999;
  mouse_world_x_ = 0.0f;
  mouse_world_z_ = 0.0f;
  last_update_time_ = 0.0;
  current_form_ = NULL;
  moving_rate_ = 0.0f;
  current_base_animation_ = NULL;
  current_action_animation_ = NULL;
  current_attachment_point_ = NULL;
  last_update_time_ = 0.0;
  editing_attachment_point_ = false;
  edit_action_ = EDIT_RUN_ACTIONANIMATION;
  selected_moving_rate_ = 1.0f;
  memset(attachment_point_items_, 0, sizeof(attachment_point_items_));
}




//----[  SkinnedMeshEditingDialog  ]-------------------------------------------
SkinnedMeshEditingDialog::~SkinnedMeshEditingDialog() {
  destroy();
}



//----[  destroy  ]------------------------------------------------------------
void SkinnedMeshEditingDialog::destroy() {
  Scenery::SceneryRenderer* renderer = editor_->getSceneryRenderer();
  for (int i = 0; i < SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS; ++i) {
    Scenery::SceneryInstance* instance = attachment_point_items_[i];
    if (instance) renderer->release(instance);
    attachment_point_items_[i] = NULL;
  }
  if (skinned_meshes_) {
    for (size_t i = 0; i < number_of_skinned_meshes_; ++i) {
      SkinnedMeshBeingEdited::FormArray::iterator form_iter
        = skinned_meshes_[i].forms.begin();
      for (; form_iter != skinned_meshes_[i].forms.end(); ++form_iter) {
        SkinnedMeshFormBeingEdited* form_being_edited
          = (SkinnedMeshFormBeingEdited*)(*form_iter);
        skinnedMeshRenderer()->release(form_being_edited->instance);
        SAFE_DELETE(form_being_edited);
      }
      SkinnedMeshBeingEdited::AnimationArray::iterator anim_iter
        = skinned_meshes_[i].animations.begin();
      for (; anim_iter != skinned_meshes_[i].animations.end(); ++anim_iter) {
        SkinnedMeshAnimationBeingEdited* animation_being_edited
          = (SkinnedMeshAnimationBeingEdited*)(*anim_iter);
        SAFE_DELETE(animation_being_edited);
      }
    }
    SAFE_DELETE_ARRAY(skinned_meshes_);
    number_of_skinned_meshes_ = 0;
  }
  current_form_ = NULL;
}

  
//----[  begin  ]--------------------------------------------------------------
bool SkinnedMeshEditingDialog::begin(
    Evidyon::Tools::CompleteEditor* editor,
    dc::dcGenericResource::Array& skinned_meshes_to_edit) {
  editor_ = editor;
  editor->acquireScenery(true); // acquires visualfx/meshes/textures
  editor->acquireAnimatedMeshes();
  editor->acquireAnimatedMeshAnimations();
  editor->acquireSkinnedMeshes();

  size_t number_of_skinned_meshes = skinned_meshes_to_edit.size();
  skinned_meshes_ = new SkinnedMeshBeingEdited[number_of_skinned_meshes];
  number_of_skinned_meshes_ = number_of_skinned_meshes;
  int column = 0;
  size_t index = 0;
  for (dc::dcGenericResource::Array::iterator i = skinned_meshes_to_edit.begin();
      i != skinned_meshes_to_edit.end(); ++i, ++column, ++index) {
    SkinnedMeshBeingEdited* skinned_mesh = &skinned_meshes_[index];
    skinned_mesh->skinned_mesh = (SkinnedMesh::Tools::SkinnedMesh*)(*i);
    skinned_mesh->column = column;
    SkinnedMesh::SkinnedMeshIndex skinned_mesh_index
      = skinned_mesh->skinned_mesh->getIndex();
    skinned_mesh->skinned_mesh_index = skinned_mesh_index;
    skinned_mesh->rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    skinned_mesh->translation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    skinned_mesh->scaling = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

    // Get all attachment points
    for (int i = 0; i < SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS; ++i) {
      SkinnedMeshAttachmentPointOffsetBeingEdited* offset
        = &skinned_mesh->attachment_points[i];
      if (!current_attachment_point_) {
        current_attachment_point_ = offset;
      }
      offset->attachment_point_offset
        = skinned_mesh->skinned_mesh->getAttachmentPoint(
            SkinnedMesh::SkinnedMeshAttachmentPoint(i));
      offset->source_bone
        = offset->attachment_point_offset->getBoneIndex();
      skinned_mesh
        ->attachment_points[i].attachment_point_offset
          ->offset()
            ->getSRTRadians(&offset->scaling.x,
                            &offset->scaling.y,
                            &offset->scaling.z,
                            &offset->rotation.x,
                            &offset->rotation.y,
                            &offset->rotation.z,
                            &offset->translation.x,
                            &offset->translation.y,
                            &offset->translation.z);
    }

    // Add all forms
    const dc::dcGenericResource::Array& forms
      = skinned_mesh->skinned_mesh->forms();
    int form_index = 0;
    for (dc::dcGenericResource::Array::const_iterator forms_iter = forms.begin();
        forms_iter != forms.end(); ++forms_iter, ++form_index) {
      SkinnedMeshFormBeingEdited* form_being_edited
        = new SkinnedMeshFormBeingEdited;
      form_being_edited->x = column * 5.0f;
      form_being_edited->z = form_index * 5.0f;
      form_being_edited->current_x = form_being_edited->x;
      form_being_edited->current_z = form_being_edited->z;
      form_being_edited->current_angle = randfAngleRadians();
      SkinnedMesh::Tools::SkinnedMeshForm* form
        = (SkinnedMesh::Tools::SkinnedMeshForm*)(*forms_iter);
      form_being_edited->form = form;
      form_being_edited->form_index = form_index;
      form->getTransform()->getSRTRadians(
        &form_being_edited->scaling.x,
        &form_being_edited->scaling.y,
        &form_being_edited->scaling.z,
        &form_being_edited->rotation.x,
        &form_being_edited->rotation.y,
        &form_being_edited->rotation.z,
        &form_being_edited->translation.x,
        &form_being_edited->translation.y,
        &form_being_edited->translation.z);
      form_being_edited->instance
        = skinnedMeshRenderer()->acquire(skinned_mesh_index, form_index);
      skinnedMeshRenderer()->changeBaseAnimation(
        0,
        0.0,
        1.0f,
        form_being_edited->instance);

      if (!current_form_) {
        current_skinned_mesh_ = skinned_mesh;
        current_form_ = form_being_edited;
      }
      skinned_mesh->forms.push_back(form_being_edited);
    }

    // Add all animations
    const dc::dcGenericResource::Array& animations
      = skinned_mesh->skinned_mesh->animations();
    int animation_index = 0;
    dc::dcGenericResource::Array::const_iterator anims_iter;
    for (anims_iter = animations.begin();
         anims_iter != animations.end(); ++anims_iter, ++animation_index) {
      SkinnedMeshAnimationBeingEdited* animation_being_edited
        = new SkinnedMeshAnimationBeingEdited;
      animation_being_edited->animation
        = (SkinnedMesh::Tools::SkinnedMeshAnimation*)(*anims_iter);
      animation_being_edited->animation_index = animation_index;
      animation_being_edited->speed_factor
        = animation_being_edited->animation->getSpeedFactor()->getValue();
      skinned_mesh->animations.push_back(animation_being_edited);

      if (!current_base_animation_) {
        assert(animation_index == 0);
        current_base_animation_ = animation_being_edited;
      }
    }
  }

  dcx::dcxWin32DialogTemplate dialog;
  dialog.setStyleAsResizeableAppWindow();
  dialog.setTitle("Skinned Mesh Editor");
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
HWND SkinnedMeshEditingDialog::getWindowHandle() {
  return dialog_handle_;
}





//----[  render  ]-------------------------------------------------------------
bool SkinnedMeshEditingDialog::render(Evidyon::Tools::CompleteEditor* editor,
                                  LPDIRECT3DDEVICE9 d3d_device) {
  assert(current_form_);
  if (!current_form_) return false;

  { // Slide to the current object
    D3DXVECTOR2 delta, target(current_form_->x, current_form_->z);
    D3DXVec2Subtract(&delta,
                     &target,
                     &camera_position_);
    if (D3DXVec2LengthSq(&delta) > 0.01f) {
      D3DXVec2Lerp(&camera_position_,
                   &camera_position_,
                   &target,
                   0.1f);
      updateVisibleInstances();
    } else {
      camera_position_ = target;
    }
  }

  Evidyon::VisualFX::VisualFXRenderer* visual_fx_renderer
    = editor_->getVisualFXRenderer();

  double time = dcx::dcxWin32Clock::getAccurateSystemTime();
  double time_since_last_update = time - last_update_time_;
  visual_fx_renderer->update(time, time - last_update_time_);
  last_update_time_ = time;

  // Position the current mesh
  {
    if (moving_rate_ > 0.0f) {
      D3DXVECTOR2 delta(cosf(-current_form_->current_angle),
                        sinf(-current_form_->current_angle));
      float movement_amount = moving_rate_ * time_since_last_update;
      delta *= movement_amount;
      current_form_->current_x += delta.x;
      current_form_->current_z += delta.y;
    }
  }


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
    light_manager->setNightAmbience();
    light_manager->beginScene(camera_position_.x, camera_position_.y);

    EvidyonD3DRenderStateManager* renderstate_manager
      = editor_->getRenderStateManager();
    AnimatedMesh::AnimatedMeshRenderer* animated_mesh_renderer
      = editor_->getAnimatedMeshRenderer();
    SkinnedMesh::SkinnedMeshRenderer* skinned_mesh_renderer
      = editor_->getSkinnedMeshRenderer();
    Scenery::SceneryRenderer* scenery_renderer
      = editor_->getSceneryRenderer();

    // Update all of the skinned meshes
    for (size_t i = 0; i < number_of_skinned_meshes_; ++i) {
      SkinnedMeshBeingEdited::FormArray::iterator form_iter
        = skinned_meshes_[i].forms.begin();
      for (; form_iter != skinned_meshes_[i].forms.end(); ++form_iter) {
        SkinnedMeshFormBeingEdited* form_being_edited
          = (SkinnedMeshFormBeingEdited*)(*form_iter);

        D3DXMATRIXA16 root_transform;
        D3DXMatrixTranslation(&root_transform,
                              form_being_edited->current_x,
                              0.0f,
                              form_being_edited->current_z);
        D3DXMATRIXA16 rotate_y;
        D3DXMatrixRotationY(&rotate_y, form_being_edited->current_angle);
        D3DXMatrixMultiply(&root_transform, &rotate_y, &root_transform);
        skinned_mesh_renderer->updateAnimation(
          time,
          &root_transform,
          form_being_edited->instance);
      }
    }

    // update attachment points
    for (int i = 0; i < SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS; ++i) {
      Scenery::SceneryInstance* instance = attachment_point_items_[i];
      if (instance == NULL) continue;
      instance->setTransform(
        &current_form_->instance->attachment_point_matrices[i]);
    }

    renderstate_manager->beginScene();
    animated_mesh_renderer->beginScene();
    skinned_mesh_renderer->beginScene();
    scenery_renderer->beginScene();
    visual_fx_renderer->beginScene();

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
      renderstate_manager->resetState();
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





//----[  dialogEvent  ]--------------------------------------------------------
INT_PTR SkinnedMeshEditingDialog::dialogEvent(HWND hDlg,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
  if (uMsg == WM_CLOSE) {
    int selection = dcx::dcxYesNoCancelDialog(
      hDlg,
      "Save Changes?",
      "Would you like to save the changes to these skinned meshes?");
    if (selection == IDCANCEL) return 0;
    if (selection == IDYES) saveChanges();
    editor_->removeD3DWindow(this);
    EndDialog(hDlg, 0);
    DestroyWindow(hDlg); 
    SetWindowLong(hDlg, GWL_USERDATA, NULL);
    delete this;
  } else if (uMsg == WM_PAINT) {
    editor_->renderD3DWindow(this);
  } else if (uMsg == WM_KEYDOWN) {
    switch (wParam) {
      case VK_LEFT: {
        int column = current_skinned_mesh_->column;
        if (column > 0) {
          resetForm();
          current_skinned_mesh_ = &skinned_meshes_[column - 1];
          current_form_ = current_skinned_mesh_->forms.front();
          current_base_animation_ = current_skinned_mesh_->animations.front();
          current_action_animation_ = current_base_animation_;
          current_attachment_point_
            = &current_skinned_mesh_->attachment_points[0];
        }
      } break;

      case VK_RIGHT: {
        int column = current_skinned_mesh_->column;
        if (column + 1 < number_of_skinned_meshes_) {
          resetForm();
          current_skinned_mesh_ = &skinned_meshes_[column + 1];
          current_form_ = current_skinned_mesh_->forms.front();
          current_base_animation_ = current_skinned_mesh_->animations.front();
          current_action_animation_ = current_base_animation_;
          current_attachment_point_
            = &current_skinned_mesh_->attachment_points[0];
        }
      } break;

      case VK_UP: {
        size_t index = indexInFormsList(current_form_, current_skinned_mesh_);
        if (index > 0) {
          resetForm();
          --index;
          current_form_ = current_skinned_mesh_->forms.at(index);
        }
      } break;

      case VK_DOWN: {
        size_t index = indexInFormsList(current_form_, current_skinned_mesh_);
        if (index + 1 < current_skinned_mesh_->forms.size()) {
          resetForm();
          ++index;
          current_form_ = current_skinned_mesh_->forms.at(index);
        }
      } break;

      case 'Z': { // undo
        resetForm();
        current_attachment_point_->rotation = current_original_transforms_.attachment_point.rotation;
        current_attachment_point_->scaling = current_original_transforms_.attachment_point.scaling;
        current_attachment_point_->translation = current_original_transforms_.attachment_point.translation;
        current_form_->rotation = current_original_transforms_.form.rotation;
        current_form_->scaling = current_original_transforms_.form.scaling;
        current_form_->translation = current_original_transforms_.form.translation;
      } break;

      default:
        switch (wParam - '0') {
          case 1: selected_moving_rate_ = 0.5f; break;
          case 2: selected_moving_rate_ = 1.0f; break;
          case 3: selected_moving_rate_ = 1.5f; break;
          case 4: selected_moving_rate_ = 2.0f; break;
          case 5: selected_moving_rate_ = 2.5f; break;
          case 6: selected_moving_rate_ = 3.0f; break;
          case 7: selected_moving_rate_ = 4.0f; break;
          case 8: selected_moving_rate_ = 5.0f; break;
          case 9: selected_moving_rate_ = 6.0f; break;
          case 0: selected_moving_rate_ = 10.0f; break;
        }
    }
  } else if (uMsg == WM_LBUTTONDOWN) {
    if ((wParam & MK_RBUTTON) == MK_RBUTTON) {
      moving_rate_ = selected_moving_rate_;
    } else {
      POINTS pts = MAKEPOINTS(lParam);
      mouse_down_x_ = pts.x;
      mouse_down_y_ = pts.y;
      current_original_transforms_.attachment_point.rotation = current_attachment_point_->rotation;
      current_original_transforms_.attachment_point.scaling  = current_attachment_point_->scaling;
      current_original_transforms_.attachment_point.translation = current_attachment_point_->translation;
      current_original_transforms_.form.rotation = current_form_->rotation;
      current_original_transforms_.form.scaling  = current_form_->scaling;
      current_original_transforms_.form.translation = current_form_->translation;
      current_original_transforms_.animation.speed_factor
        = current_base_animation_ == NULL ? 0.0f : current_base_animation_->speed_factor;
    }
  } else if (uMsg == WM_MOUSEMOVE) {
    POINTS pts = MAKEPOINTS(lParam);
    if ((wParam & MK_RBUTTON) == MK_RBUTTON) {
      float x, z;
      camera_.getWorldLocation(
        pts.x,
        pts.y,
        &x,
        &z,
        NULL);
      D3DXVECTOR2 delta, target(x, z),
                  position(current_form_->current_x,
                           current_form_->current_z);
      D3DXVec2Subtract(&delta,
                       &target,
                       &position);
      float angle = atan2f(delta.y, delta.x);
      current_form_->current_angle = -angle;
    } else if ((wParam & MK_LBUTTON) == MK_LBUTTON) {
      switch (edit_action_) {
        case EDIT_VERTICAL_OFFSET: {
          D3DXVECTOR3 offset(0.0f, -(pts.y - mouse_down_y_) / 100.0, 0.0f);
          if (editing_attachment_point_) {
            current_attachment_point_->translation
              = current_original_transforms_.attachment_point.translation + offset;
            updateSkinnedMeshAttachmentPoint(current_skinned_mesh_,
                                             current_attachment_point_);
          } else {
            current_form_->translation
              = current_original_transforms_.form.translation + offset;
            updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
          }
        } break;
        case EDIT_HORIZONTAL_OFFSET: {
          D3DXVECTOR3 offset((pts.x - mouse_down_x_) / 100.0, 0.0f,
                             (pts.y - mouse_down_y_) / 100.0);
          if (editing_attachment_point_) {
            current_attachment_point_->translation
              = current_original_transforms_.attachment_point.translation + offset;
            updateSkinnedMeshAttachmentPoint(current_skinned_mesh_,
                                             current_attachment_point_);
          } else {
            current_form_->translation
              = current_original_transforms_.form.translation + offset;
            updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
          }
        } break;
        case EDIT_UNIFORM_SCALING: {
          int center_x = 1024/2;
          int center_y = 768/2;
          float sdx = (mouse_down_x_ - center_x) * 1.0;
          float sdy = (mouse_down_y_ - center_y) * 1.0;
          float dx = (pts.x - center_x) * 1.0;
          float dy = (pts.y - center_y) * 1.0;
          float s = sqrtf(dx*dx+dy*dy)/sqrtf(sdx*sdx+sdy*sdy);

          if (editing_attachment_point_) {
            current_attachment_point_->scaling
              = s * current_original_transforms_.attachment_point.scaling;
            updateSkinnedMeshAttachmentPoint(current_skinned_mesh_,
                                             current_attachment_point_);
          } else {
            current_form_->scaling
              = s * current_original_transforms_.form.scaling;
            updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
          }
        } break;
        case EDIT_HORIZ_VERT_SCALING: {
          int center_x = 1024/2;
          int center_y = 768/2;
          float sdx = (mouse_down_x_ - center_x) * 1.0;
          float sdy = (mouse_down_y_ - center_y) * 1.0;
          float dx = (pts.x - center_x) * 1.0;
          float dy = (pts.y - center_y) * 1.0;
          float scale_xz = dx*dx / (sdx*sdx);
          float scale_y = dy*dy / (sdy*sdy);

          if (editing_attachment_point_) {
            current_attachment_point_->scaling.x
              = scale_xz * current_original_transforms_.attachment_point.scaling.x;
            current_attachment_point_->scaling.z
              = scale_xz * current_original_transforms_.attachment_point.scaling.z;
            current_attachment_point_->scaling.y
              = scale_y * current_original_transforms_.attachment_point.scaling.y;
            updateSkinnedMeshAttachmentPoint(current_skinned_mesh_,
                                             current_attachment_point_);
          } else {
            current_form_->scaling.x
              = scale_xz * current_original_transforms_.form.scaling.x;
            current_form_->scaling.y
              = scale_y * current_original_transforms_.form.scaling.y;
            current_form_->scaling.z
              = scale_xz * current_original_transforms_.form.scaling.z;
            updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
          }
        } break;
        case EDIT_ANIMATION_SPEED: {
          if (!current_base_animation_) break;
          int center_x = 1024/2;
          int center_y = 768/2;
          float sdx = (mouse_down_x_ - center_x) * 1.0;
          float sdy = (mouse_down_y_ - center_y) * 1.0;
          float dx = (pts.x - center_x) * 1.0;
          float dy = (pts.y - center_y) * 1.0;
          float s = sqrtf(dx*dx+dy*dy)/sqrtf(sdx*sdx+sdy*sdy);
          current_base_animation_->speed_factor
            = current_original_transforms_.animation.speed_factor * s;
          skinnedMeshRenderer()->changeSkinnedMeshAnimationSpeed(
            current_skinned_mesh_->skinned_mesh_index,
            current_base_animation_->animation_index,
            current_base_animation_->speed_factor);
        } break;

        case EDIT_RUN_ACTIONANIMATION: {
          if (current_action_animation_) {
            skinnedMeshRenderer()->runActionAnimation(
              current_action_animation_->animation_index,
              dcx::dcxWin32Clock::getAccurateSystemTime(),
              1.0f,
              current_form_->instance);
          }
        } break;



        //----[  EDIT_Y_ROTATION  ]--------------------------------------------
        case EDIT_Y_ROTATION: { // rotate about Y
          float center_x = 1024/2.0f;
          float center_y = 768/2.0f;
          float rotate_amount
            = atan2f(mouse_down_y_-center_y, mouse_down_x_-center_x)
            - atan2f(pts.y-center_y,pts.x-center_x);
          //float rotate_amount = -(pts.x - mouse_down_x_) / 300.0 * D3DX_PI/2.0f;
          if (editing_attachment_point_) {
            current_attachment_point_->rotation.y
              = rotate_amount + current_original_transforms_.attachment_point.rotation.y;
            if ((wParam & MK_SHIFT) == MK_SHIFT) { // constrain!
              current_attachment_point_->rotation.y -=
                fmodf(current_attachment_point_->rotation.y,D3DX_PI/4.0f);
            }
            updateSkinnedMeshAttachmentPoint(current_skinned_mesh_,
                                             current_attachment_point_);
          } else {
            current_form_->rotation.y
              = rotate_amount + current_original_transforms_.form.rotation.y;
            if ((wParam & MK_CONTROL) == MK_CONTROL) { // constrain!
              current_form_->rotation.y -=
                fmodf(current_form_->rotation.y,D3DX_PI/4.0f);
            }
            updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
          }
        } break;
      



        //----[  EDIT_X_ROTATION  ]--------------------------------------------
        case EDIT_X_ROTATION: { // rotate about X
          float center_x = 1024/2.0f;
          float center_y = 768/2.0f;
          float rotate_amount
            = atan2f(mouse_down_y_-center_y, mouse_down_x_-center_x)
            - atan2f(pts.y-center_y,pts.x-center_x);
          //float rotate_amount = -(pts.x - mouse_down_x_) / 300.0 * D3DX_PI/2.0f;
          if (editing_attachment_point_) {
            current_attachment_point_->rotation.x
              = rotate_amount + current_original_transforms_.attachment_point.rotation.x;
            if ((wParam & MK_SHIFT) == MK_SHIFT) { // constrain!
              current_attachment_point_->rotation.x -=
                fmodf(current_attachment_point_->rotation.x,D3DX_PI/4.0f);
            }
            updateSkinnedMeshAttachmentPoint(current_skinned_mesh_,
                                             current_attachment_point_);
          } else {
            current_form_->rotation.x
              = rotate_amount + current_original_transforms_.form.rotation.x;
            if ((wParam & MK_CONTROL) == MK_CONTROL) { // constrain!
              current_form_->rotation.x -=
                fmodf(current_form_->rotation.x,D3DX_PI/4.0f);
            }
            updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
          }
        } break;



        //----[  EDIT_Z_ROTATION  ]--------------------------------------------
        case EDIT_Z_ROTATION: { // rotate about Z
          float center_x = 1024/2.0f;
          float center_y = 768/2.0f;
          float rotate_amount
            = atan2f(mouse_down_y_-center_y, mouse_down_x_-center_x)
            - atan2f(pts.y-center_y,pts.x-center_x);
          if (editing_attachment_point_) {
            current_attachment_point_->rotation.z
              = rotate_amount + current_original_transforms_.attachment_point.rotation.z;
            if ((wParam & MK_SHIFT) == MK_SHIFT) { // constrain!
              current_attachment_point_->rotation.z -=
                fmodf(current_attachment_point_->rotation.z,D3DX_PI/4.0f);
            }
            updateSkinnedMeshAttachmentPoint(current_skinned_mesh_,
                                             current_attachment_point_);
          } else {
            current_form_->rotation.z
              = rotate_amount + current_original_transforms_.form.rotation.z;
            if ((wParam & MK_CONTROL) == MK_CONTROL) { // constrain!
              current_form_->rotation.z -=
                fmodf(current_form_->rotation.z,D3DX_PI/4.0f);
            }
            updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
          }
        } break;
          break;
      }/*else if (GetAsyncKeyState('R')) { // rotate about Y
        float center_x = 1024/2.0f;
        float center_y = 768/2.0f;
        float rotate_amount
          = atan2f(mouse_down_y_-center_y, mouse_down_x_-center_x)
          - atan2f(pts.y-center_y,pts.x-center_x);
        //float rotate_amount = -(pts.x - mouse_down_x_) / 300.0 * D3DX_PI/2.0f;
        if (editing_attachment_point_) {
          current_skinned_mesh_->rotation.y
            = rotate_amount + current_original_transforms_.skinned_mesh.rotation.y;
          if ((wParam & MK_SHIFT) == MK_SHIFT) { // constrain!
            current_skinned_mesh_->rotation.y -=
              fmodf(current_skinned_mesh_->rotation.y,D3DX_PI/4.0f);
          }
          SkinnedMeshBeingEdited::FormArray* list = &current_skinned_mesh_->forms;
          for (SkinnedMeshBeingEdited::FormArray::iterator i = list->begin();
               i != list->end(); ++i) {
            updateSkinnedMeshForm(current_skinned_mesh_, *i);
          }
        } else {
          current_form_->rotation.y
            = rotate_amount + current_original_transforms_.form.rotation.y;
          if ((wParam & MK_CONTROL) == MK_CONTROL) { // constrain!
            current_form_->rotation.y -=
              fmodf(current_form_->rotation.y,D3DX_PI/4.0f);
          }
          updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
        }
      } else if (GetAsyncKeyState('X')) { // change texture
        if (!editing_attachment_point_) {
          // divide by the tile pixel size
          int dx = (pts.x - mouse_down_x_) / 55;
          int dy = (pts.y - mouse_down_y_) / 49;
          int added_tex = dy * 10 + dx;
          size_t total_textures = editor_->getTextures().size();
          current_form_subset_->texture_index
            = (total_textures * 99 // some large mult. to make the % positive
               + added_tex
               + current_original_texture_) % total_textures;
          updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
        }
      } else if (GetAsyncKeyState('Q')) { // move special fx planar (X/Z)
        if (!editing_attachment_point_) {
          D3DXVECTOR3 offset((pts.x - mouse_down_x_) / 100.0, 0.0f,
                             (pts.y - mouse_down_y_) / 100.0);
          current_form_->visual_fx_offset
            = current_original_transforms_.form.visual_fx_offset + offset;
          updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
        }
      } else if (GetAsyncKeyState('W')) { // move special fx vertically (Y)
        if (!editing_attachment_point_) {
          D3DXVECTOR3 offset(0.0f, -(pts.y - mouse_down_y_) / 100.0, 0.0f);
          current_form_->visual_fx_offset
            = current_original_transforms_.form.visual_fx_offset + offset;
          updateSkinnedMeshForm(current_skinned_mesh_, current_form_);
        }
      }*/
    }
  } else if (uMsg == WM_RBUTTONUP) {
    moving_rate_ = 0.0f;
    if (current_base_animation_) {
      skinnedMeshRenderer()->changeBaseAnimation(
        current_base_animation_->animation_index,
        0.0,
        0.0f,
        current_form_->instance);
    }
  } else if (uMsg == WM_RBUTTONDOWN) {
    if((wParam & MK_LBUTTON) == MK_LBUTTON) {

      resetForm();
      current_attachment_point_->rotation = current_original_transforms_.attachment_point.rotation;
      current_attachment_point_->scaling = current_original_transforms_.attachment_point.scaling;
      current_attachment_point_->translation = current_original_transforms_.attachment_point.translation;
      current_form_->rotation = current_original_transforms_.form.rotation;
      current_form_->scaling = current_original_transforms_.form.scaling;
      current_form_->translation = current_original_transforms_.form.translation;

    } else {
      if (current_base_animation_) {
        skinnedMeshRenderer()->changeBaseAnimation(
          current_base_animation_->animation_index,
          dcx::dcxWin32Clock::getAccurateSystemTime(),
          selected_moving_rate_,
          current_form_->instance);
      }
    }
  } else if (uMsg == WM_RBUTTONDBLCLK) {
    enum {
     SELECT_EDIT_ACTION = 500,
     SELECT_BASE_ANIMATION = 1000,
     SELECT_ACTION_ANIMATION = 2000,
     SELECT_ATTACHMENT_POINT_ITEM = 3000,
     EDIT_ATTACHMENT_POINT = 4000,
     EDIT_FORM = 5001,
     RESET_POSITION = 5002,
     UNDO_CHANGES = 5003,
    };
    POINTS clicked_at_points = MAKEPOINTS(lParam);
    POINT clicked_at = {clicked_at_points.x, clicked_at_points.y};
    ClientToScreen(hDlg, &clicked_at);
    MENUITEMINFO menu_item;

    HMENU hEditSubmenu = CreateMenu();
    for (int i = 0; i < int(NUMBER_OF_EDIT_ACTIONS); ++i) {
      dcx::dcxWin32InitMenuItem(&menu_item,
                                EditActionString(EditAction(i)),
                                false,
                                SELECT_EDIT_ACTION + i);
      menu_item.fState = (i == edit_action_) ? MFS_CHECKED : 0;
      InsertMenuItem(hEditSubmenu, 0xFFFFFFFF, TRUE, &menu_item);
    }

    HMENU hMenu = CreatePopupMenu();
    HMENU hBaseSubmenu = createAnimationsSubmenu(SELECT_BASE_ANIMATION);
    HMENU hActionSubmenu = createAnimationsSubmenu(SELECT_ACTION_ANIMATION);
    HMENU hAttachSubmenu
      = createAttachmentPointsSubmenu(SELECT_ATTACHMENT_POINT_ITEM);
    HMENU hAttachmentsEditSubmenu
      = createAttachmentPointsSubmenu(EDIT_ATTACHMENT_POINT);

    dcx::dcxWin32InitMenuItem(&menu_item, "Left-Click Action", hEditSubmenu);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItemSeperator(&menu_item);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Set Base Animation", hBaseSubmenu);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Set Action Animation", hActionSubmenu);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItemSeperator(&menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Attach Item", hAttachSubmenu);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItemSeperator(&menu_item);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Edit Attachment Point", hAttachmentsEditSubmenu);
    menu_item.fState = editing_attachment_point_ ? MFS_CHECKED : 0;
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Edit Form", false, EDIT_FORM);
    menu_item.fState = editing_attachment_point_ ? 0 : MFS_CHECKED;
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItemSeperator(&menu_item);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Reset Position", false, RESET_POSITION);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Undo Last Changes", false, UNDO_CHANGES);
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
    DestroyMenu(hBaseSubmenu);
    DestroyMenu(hActionSubmenu);
    DestroyMenu(hAttachSubmenu);
    DestroyMenu(hAttachmentsEditSubmenu);
    DestroyMenu(hMenu);

    // Check to see whether or not the user picked an item
    if (selected >= SELECT_EDIT_ACTION &&
        selected < SELECT_BASE_ANIMATION) {
      edit_action_ = EditAction(selected - SELECT_EDIT_ACTION);
    } else if (selected >= SELECT_BASE_ANIMATION &&
        selected < SELECT_ACTION_ANIMATION) {
      current_base_animation_
        = current_skinned_mesh_
            ->animations.at(selected - SELECT_BASE_ANIMATION);
    } else if (selected >= SELECT_ACTION_ANIMATION &&
               selected < SELECT_ATTACHMENT_POINT_ITEM) {
      current_action_animation_
        = current_skinned_mesh_
            ->animations.at(selected - SELECT_ACTION_ANIMATION);
    } else if (selected >= SELECT_ATTACHMENT_POINT_ITEM &&
               selected < EDIT_ATTACHMENT_POINT) {
      promptUserForAttachmentPointScenery(
        SkinnedMesh::SkinnedMeshAttachmentPoint(
          selected - SELECT_ATTACHMENT_POINT_ITEM));
    } else if (selected >= EDIT_ATTACHMENT_POINT &&
               selected < EDIT_FORM) {
      editing_attachment_point_ = true;
      current_attachment_point_
        = &current_skinned_mesh_->attachment_points[selected - EDIT_ATTACHMENT_POINT];
    } else {
      switch (selected) {
        case EDIT_FORM:             editing_attachment_point_ = false; break;
        case RESET_POSITION:    resetForm(); break;
        case UNDO_CHANGES:
          resetForm();
          current_attachment_point_->rotation = current_original_transforms_.attachment_point.rotation;
          current_attachment_point_->scaling = current_original_transforms_.attachment_point.scaling;
          current_attachment_point_->translation = current_original_transforms_.attachment_point.translation;
          current_form_->rotation = current_original_transforms_.form.rotation;
          current_form_->scaling = current_original_transforms_.form.scaling;
          current_form_->translation = current_original_transforms_.form.translation;
          break;
      }
    }
  }
  return 0;
}




//----[  updateVisibleInstances  ]---------------------------------------------
void SkinnedMeshEditingDialog::updateVisibleInstances() {

}



//----[  skinnedMeshRenderer  ]------------------------------------------------
SkinnedMesh::SkinnedMeshRenderer*
SkinnedMeshEditingDialog::skinnedMeshRenderer() {
  return editor_->getSkinnedMeshRenderer();
}




//----[  createAnimationsSubmenu  ]--------------------------------------------
HMENU SkinnedMeshEditingDialog::createAnimationsSubmenu(int base_id) {
  HMENU hSubmenu = CreateMenu();

  const SkinnedMeshBeingEdited::AnimationArray& animations
    = current_skinned_mesh_->animations;
  size_t index = 0;
  for (SkinnedMeshBeingEdited::AnimationArray::const_iterator i = animations.begin();
        i != animations.end(); ++i, ++index) {
    MENUITEMINFO menu_item;
    dcx::dcxWin32InitMenuItem(&menu_item,
                              (*i)->animation->getName().c_str(),
                              false,
                              base_id + index);
    InsertMenuItem(hSubmenu, 0xFFFFFFFF, TRUE, &menu_item);
  }

  return hSubmenu;
}



//----[  createAttachmentPointsSubmenu  ]--------------------------------------
HMENU SkinnedMeshEditingDialog::createAttachmentPointsSubmenu(int base_id) {
  HMENU hSubmenu = CreateMenu();
  for (int i = 0; i < SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS; ++i) {
    MENUITEMINFO menu_item;
    dcx::dcxWin32InitMenuItem(
      &menu_item,
      SkinnedMesh::SkinnedMeshAttachmentPointString(
        SkinnedMesh::SkinnedMeshAttachmentPoint(i)),
      false,
      base_id + i);
    InsertMenuItem(hSubmenu, 0xFFFFFFFF, TRUE, &menu_item);
  }

  return hSubmenu;
}


//----[  indexInFormsList  ]---------------------------------------------------
size_t SkinnedMeshEditingDialog::indexInFormsList(
    SkinnedMeshFormBeingEdited* form,
    SkinnedMeshBeingEdited* skinned_mesh) const {
  assert(form);
  assert(skinned_mesh);
  size_t index = 0;
  for (SkinnedMeshBeingEdited::FormArray::const_iterator i = skinned_mesh->forms.begin();
       i != skinned_mesh->forms.end(); ++i, ++index) {
    if (*i == form) return index;
  }
  assert(false);
  return -1;
}



//----[  indexInAttachmentPointsList  ]----------------------------------------
size_t SkinnedMeshEditingDialog::indexInAttachmentPointsList(
    SkinnedMeshBeingEdited* skinned_mesh,
    SkinnedMeshAttachmentPointOffsetBeingEdited* point) const {
  assert(skinned_mesh);
  assert(point);
  size_t offset = point - skinned_mesh->attachment_points;
  assert(offset < SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS);
  if (offset >= SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS) offset = 0;
  return offset;
}



//----[  updateSkinnedMeshForm  ]----------------------------------------------
void SkinnedMeshEditingDialog::updateSkinnedMeshForm(
    SkinnedMeshBeingEdited* skinned_mesh,
    SkinnedMeshFormBeingEdited* form) {
  D3DXVECTOR3 rotation = skinned_mesh->rotation + form->rotation;
  D3DXVECTOR3 translation = skinned_mesh->translation + form->translation;
  D3DXVECTOR3 scaling(skinned_mesh->scaling.x * form->scaling.x,
                      skinned_mesh->scaling.y * form->scaling.y,
                      skinned_mesh->scaling.z * form->scaling.z);

  D3DXMATRIXA16 s, r, t;
  D3DXMatrixScaling(&s, scaling.x, scaling.y, scaling.z);
  D3DXMatrixTranslation(&t, translation.x, translation.y, translation.z);
  D3DXMatrixRotationYawPitchRoll(&r, rotation.y, rotation.x, rotation.z);
  D3DXMATRIXA16 c;
  D3DXMatrixMultiply(&c, &s, &r);
  D3DXMatrixMultiply(&c, &c, &t);

  Evidyon::SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form_index
    =  indexInFormsList(form, skinned_mesh);
  assert(skinned_mesh_form_index < skinned_mesh->forms.size());
  skinnedMeshRenderer()->changeSkinnedMeshFormTransform(
    skinned_mesh->skinned_mesh_index,
    skinned_mesh_form_index,
    &c);
}



//----[  updateSkinnedMeshAttachmentPoint  ]-----------------------------------
void SkinnedMeshEditingDialog::updateSkinnedMeshAttachmentPoint(
    SkinnedMeshBeingEdited* skinned_mesh,
    SkinnedMeshAttachmentPointOffsetBeingEdited* attachment_point) {
  assert(skinned_mesh);
  assert(attachment_point);
  assert(attachment_point - skinned_mesh->attachment_points <
           SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS);
  D3DXVECTOR3 rotation = attachment_point->rotation;
  D3DXVECTOR3 translation = attachment_point->translation;
  D3DXVECTOR3 scaling(attachment_point->scaling.x,
                      attachment_point->scaling.y,
                      attachment_point->scaling.z);

  D3DXMATRIXA16 s, r, t;
  D3DXMatrixScaling(&s, scaling.x, scaling.y, scaling.z);
  D3DXMatrixTranslation(&t, translation.x, translation.y, translation.z);
  D3DXMatrixRotationYawPitchRoll(&r, rotation.y, rotation.x, rotation.z);
  D3DXMATRIXA16 c;
  D3DXMatrixMultiply(&c, &s, &r);
  D3DXMatrixMultiply(&c, &c, &t);

  skinnedMeshRenderer()->changeSkinnedMeshAttachmentPoint(
    skinned_mesh->skinned_mesh_index,
    SkinnedMesh::SkinnedMeshAttachmentPoint(
      indexInAttachmentPointsList(skinned_mesh, attachment_point)),
    attachment_point->source_bone,
    &c);
}



//----[  saveChanges  ]--------------------------------------------------------
void SkinnedMeshEditingDialog::saveChanges() {
  if (skinned_meshes_) {
    D3DXVECTOR3 mesh_s, mesh_r, mesh_t;
    D3DXVECTOR3 s, r, t;
    for (size_t i = 0; i < number_of_skinned_meshes_; ++i) {
      SkinnedMeshBeingEdited* skinned_mesh = &skinned_meshes_[i];
      mesh_s = skinned_mesh->scaling;
      mesh_r = skinned_mesh->rotation;
      mesh_t = skinned_mesh->translation;
      {
        SkinnedMeshBeingEdited::FormArray* list = &skinned_mesh->forms;
        for (SkinnedMeshBeingEdited::FormArray::iterator i = list->begin();
             i != list->end(); ++i) {
          SkinnedMeshFormBeingEdited* form = *i;
          form->form->getTransform()->setSRTRadians(
            form->scaling.x * mesh_s.x,
            form->scaling.y * mesh_s.y,
            form->scaling.z * mesh_s.z,
            form->rotation.x + mesh_r.x,
            form->rotation.y + mesh_r.y,
            form->rotation.z + mesh_r.z,
            form->translation.x + mesh_t.x,
            form->translation.y + mesh_t.y,
            form->translation.z + mesh_t.z);
        }
      }
      for (int i = 0; i < SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS; ++i) {
        SkinnedMeshAttachmentPointOffsetBeingEdited* offset
          = &skinned_mesh->attachment_points[i];
        offset->attachment_point_offset->offset()->setSRTRadians(
          offset->scaling.x,
          offset->scaling.y,
          offset->scaling.z,
          offset->rotation.x,
          offset->rotation.y,
          offset->rotation.z,
          offset->translation.x,
          offset->translation.y,
          offset->translation.z);
        offset->attachment_point_offset->setBoneIndex(offset->source_bone);
      }
      {
        SkinnedMeshBeingEdited::AnimationArray* list
          = &skinned_mesh->animations;
        for (SkinnedMeshBeingEdited::AnimationArray::iterator i
              = list->begin(); i != list->end(); ++i) {
          SkinnedMeshAnimationBeingEdited* anim = *i;
          anim->animation->getSpeedFactor()->setValue(
            anim->speed_factor);
        }
      }
    }
  }
}




//----[  resetForm  ]----------------------------------------------------------
void SkinnedMeshEditingDialog::resetForm() {
  assert(current_form_);
  current_form_->current_x = current_form_->x;
  current_form_->current_z = current_form_->z;
  current_form_->current_angle = 0.0f;
}



//----[  promptUserForAttachmentPointScenery  ]--------------------------------
void SkinnedMeshEditingDialog::promptUserForAttachmentPointScenery(
    SkinnedMesh::SkinnedMeshAttachmentPoint attachment_point) {
  char prompt[512];
  sprintf_s(prompt, 512, "Pick the scenery to attach to %s",
    SkinnedMesh::SkinnedMeshAttachmentPointString(attachment_point));

  std::list<std::string> scenery_names;
  const dc::dcGenericResource::Array& scenery = editor_->getScenery();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = scenery.begin(); i != scenery.end(); ++i, ++index) {
    scenery_names.push_back((*i)->getName());
  }

  int selected = -1;
  if (!dcx::dcxWin32PromptForSelection(editor_->topWindow(),
                                       "Select Scenery",
                                       prompt,
                                       &scenery_names,
                                       &selected,
                                       false)) {
    return;
  }

  // Add this element
  Scenery::SceneryRenderer* renderer = editor_->getSceneryRenderer();
  Scenery::SceneryInstance* instance
    = attachment_point_items_[attachment_point];
  if (instance) renderer->release(instance);
  instance = renderer->acquire((Scenery::SceneryIndex)selected);
  attachment_point_items_[attachment_point] = instance;
  instance->hidden = false;
  current_attachment_point_
    = &current_skinned_mesh_->attachment_points[attachment_point];
}

}