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
#include "visualfxeditingdialog.h"
#include "common/safemacros.h"
#include <assert.h>
#include "shared/binding/binding.h"


namespace Evidyon {

class VisualFXEditingDialogBinding : public Evidyon::Binding::Binding {
public:
  ~VisualFXEditingDialogBinding(){}
  void setLocation(float x, float y, float z) {
    D3DXMatrixTranslation(&transform_, x, y, z);
  }

  virtual void update(double time, double time_since_last_update) {
  }

  virtual bool visible() {
    return true;
  }

  virtual const D3DXMATRIX* getTransform() const {
    return &transform_;
  }
private:
  D3DXMATRIXA16 transform_;
};

  
//----[  VisualFXEditingDialog  ]----------------------------------------------
VisualFXEditingDialog::VisualFXEditingDialog() : camera_position_(0.0f, 0.0f) {
  dialog_handle_ = NULL;
  editor_ = NULL;
  source_.reset();
  target_.reset();
  intensity_ = 0.0f;
}




//----[  VisualFXEditingDialog  ]----------------------------------------------
VisualFXEditingDialog::~VisualFXEditingDialog() {
  destroy();
}





//----[  destroy  ]------------------------------------------------------------
void VisualFXEditingDialog::destroy() {
  visualFXRenderer()->clearVisualFXInstances();
  source_.reset();
  target_.reset();
  light_manager_.destroy();
}






//----[  begin  ]--------------------------------------------------------------
bool VisualFXEditingDialog::begin(
    Evidyon::Tools::CompleteEditor* editor,
    const dc::dcGenericResource::Array& visual_fx_to_edit) {
  editor_ = editor;
  editor->acquireVisualFX();

  if (!light_manager_.create(editor->getD3DDevice())) return false;


  if (visual_fx_to_edit.empty()) return false;
  dc::dcGenericResource::Array::const_iterator i = visual_fx_to_edit.begin();
  for (; i != visual_fx_to_edit.end(); ++i) {
    visual_fx_to_edit_.push_back(
      (dc::dcTable<VisualFX::Tools::VisualFX>::Element*)(*i));
  }

  current_fx_index_ = 0;

  source_.reset();
  last_update_time_ = GetTickCount() / 1000.0;


  dcx::dcxWin32DialogTemplate dialog;
  dialog.setStyleAsResizeableAppWindow();
  BYTE buffer[256];
  LPCDLGTEMPLATE dlg_template = dialog.compile(buffer, sizeof(buffer));
  dialog_handle_ = createDialog(GetModuleHandle(NULL),
                                dlg_template,
                                editor->topWindow());
  updateWindowTitle();
  dcx::dcxWin32SetWindowClientSize(dialog_handle_, 1024, 768);
  camera_.rebuildTopdownCamera(1024, 768);
  return editor->addD3DWindow(this);
}





//----[  getWindowHandle  ]----------------------------------------------------
HWND VisualFXEditingDialog::getWindowHandle() {
  return dialog_handle_;
}





//----[  render  ]-------------------------------------------------------------
bool VisualFXEditingDialog::render(Evidyon::Tools::CompleteEditor* editor,
                                   LPDIRECT3DDEVICE9 d3d_device) {

  // Slide to the current object
  D3DXVECTOR2 delta, target(0.5f, 0.5f);
  D3DXVec2Subtract(&delta,
                   &target,
                   &camera_position_);
  if (D3DXVec2LengthSq(&delta) > 0.01f) {
    D3DXVec2Lerp(&camera_position_,
                 &camera_position_,
                 &target,
                 0.1f);
  } else {
    camera_position_ = target;
  }

  Evidyon::VisualFX::VisualFXRenderer* visual_fx_renderer
    = visualFXRenderer();

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
    light_manager_.setDaylightAmbience();
    light_manager_.beginScene(camera_position_.x, camera_position_.y);

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
      static const D3DCOLOR color = D3DCOLOR_ARGB(128, 32, 32, 32);
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
INT_PTR VisualFXEditingDialog::dialogEvent(HWND hDlg,
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
    //editor_->renderD3DWindow(this);
  } else if (uMsg == WM_SIZE) {
    camera_.rebuildTopdownCamera(LOWORD(lParam),HIWORD(lParam));
  } else if (uMsg == WM_LBUTTONDOWN) {
    VisualFXEditingDialogBindingPtr new_target(new VisualFXEditingDialogBinding());
    target_.swap(new_target);
    POINTS pt = MAKEPOINTS(lParam);
    float x, z;
    camera_.getWorldLocation(
      pt.x,
      pt.y,
      &x,
      &z,
      NULL);
    target_->setLocation(x, 0.0f, z);
    VisualFXEditingDialogBindingPtr new_source(new VisualFXEditingDialogBinding());
    source_.swap(new_source);
    source_->setLocation(0.0f, 0.0f, 0.0f);
    visualFXRenderer()->acquireVisualFX(
      visual_fx_to_edit_.at(current_fx_index_)->getIndex(),
      intensity_,
      source_,
      target_);
  } else if (uMsg == WM_MOUSEMOVE) {
    if (target_) {
      POINTS pt = MAKEPOINTS(lParam);
      float x, z;
      camera_.getWorldLocation(
        pt.x,
        pt.y,
        &x,
        &z,
      NULL);
      target_->setLocation(x, 0.0f, z);
    }
  } else if (uMsg == WM_LBUTTONUP) {
    if (target_) {
      target_->setExpired();
      target_.reset();
    }
    if (source_) {
      source_->setExpired();
      source_.reset();
    }
  } else if (uMsg == WM_KEYDOWN) {
    switch (wParam) {
      case VK_UP:
      case VK_LEFT:
        if (current_fx_index_ == 0) {
          current_fx_index_ = visual_fx_to_edit_.size() - 1;
        } else {
          --current_fx_index_;
        }
        updateWindowTitle();
        break;

      case VK_RIGHT:
      case VK_DOWN:
        ++current_fx_index_;
        if (current_fx_index_ >= visual_fx_to_edit_.size()) {
          current_fx_index_ = 0;
        }
        updateWindowTitle();
        break;

      case '0':
        if (intensity_ > 0.5f) {
          intensity_ = 0.0f;
        } else {
          intensity_ = 1.0f;
        }
        updateWindowTitle();
        break;

    default:
      {
        if (wParam >= '1' && wParam <= '9') {
          intensity_ = (wParam - '1' + 1) * (1.0f / 10.0f);
        updateWindowTitle();
        }
      }
    }
  }

  return 0;
}


//----[  visualFXRenderer  ]---------------------------------------------------
VisualFX::VisualFXRenderer* VisualFXEditingDialog::visualFXRenderer() {
  return editor_->getVisualFXRenderer();
}




//----[  updateWindowTitle  ]--------------------------------------------------
void VisualFXEditingDialog::updateWindowTitle() {
  char window_title[512];
  sprintf_s(window_title,
            512,
            "VisualFX Editor - %.01f%% intensity - %s",
            intensity_ * 100.0f,
            editor_->getVisualFX().at(
              visual_fx_to_edit_.at(current_fx_index_)->getIndex())
                ->getName().c_str());
  SetWindowText(dialog_handle_, window_title);
}


}