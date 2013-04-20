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
#include "mapmaskcolorselectordialog.h"
#include "common/safemacros.h"



namespace Evidyon {


MaskColorSelectorDialogFunctionoid::~MaskColorSelectorDialogFunctionoid() {}



static const int CLIENT_SIZE = 1024;


//----[  MapMaskColorSelectorDialog  ]-----------------------------------------
MapMaskColorSelectorDialog::~MapMaskColorSelectorDialog() {
  SAFE_RELEASE(mask_texture_);
}

  
//----[  begin  ]--------------------------------------------------------------
bool MapMaskColorSelectorDialog::begin(
    HWND owner,
    Evidyon::Tools::CompleteEditor* editor,
    World::Tools::Map* map,
    MapMaskElement* map_mask,
    MaskColorSelectorDialogFunctionoid* allocated_functionoid) {

  mask_texture_ = NULL;
  if (!map_mask->loadSourceFileTexture(editor->getD3DDevice(),
                                       &mask_texture_)) {
    MessageBox(editor->topWindow(),
               "Warning: This mask's source file is invalid",
               "Mask Reference Error",
               MB_OK);
    return false;
  }

  editor_ = editor;
  map_ = map;
  map_mask_ = map_mask;
  allocated_functionoid_ = allocated_functionoid;
  dcx::dcxWin32DialogTemplate dialog;
  dialog.setTitle("Select Map Mask Color");
  BYTE buffer[256];
  dialog.setStyleAsPrompt();
  LPCDLGTEMPLATE dlg_template = dialog.compile(buffer, sizeof(buffer));
  dialog_handle_ = createDialog(GetModuleHandle(NULL),
                                dlg_template,
                                owner);

  // Make the window larger than our texture--we'll use the extra space to
  // show the selected color
  dcx::dcxWin32SetWindowClientSize(dialog_handle_,
                                   CLIENT_SIZE + 32*2,
                                   CLIENT_SIZE + 32*2);
  return editor->addD3DWindow(this);
}


//----[  render  ]-------------------------------------------------------------
bool MapMaskColorSelectorDialog::render(
    Evidyon::Tools::CompleteEditor* editor,
    LPDIRECT3DDEVICE9 d3d_device) {

  if (SUCCEEDED(d3d_device->BeginScene())) {
    d3d_device->Clear(0,
                      NULL,
                      D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                      current_color_,
                      1.0f,
                      0);
    editor->getRenderStateManager()->resetState();
    d3d_device->SetTexture(0, mask_texture_);
    d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
    d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
    d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    struct Vertex {
      FLOAT x, y, z, rhw; // The transformed position for the vertex
      FLOAT u, v;         // The texture coordinates
    } vertices[4] = {
      { 32, 32, 0.0f, 0.5f, 0.0f, 0.0f },
      { float(32+CLIENT_SIZE), 32.0f, 0.0f, 0.5f, 1.0f, 0.0f },
      { float(32+CLIENT_SIZE), float(32+CLIENT_SIZE), 0.0f, 0.5f, 1.0f, 1.0f },
      { 32.0f, float(32+CLIENT_SIZE), 0.0f, 0.5f, 0.0f, 1.0f },
    };
    d3d_device->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);
    d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
    d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    d3d_device->EndScene();
  }
  return true;
}




//----[  dialogEvent  ]------------------------------------------------------
INT_PTR MapMaskColorSelectorDialog::dialogEvent(HWND hDlg,
                                                UINT uMsg,
                                                WPARAM wParam,
                                                LPARAM lParam) {
  if (uMsg == WM_CLOSE) {
    editor_->removeD3DWindow(this);
    EndDialog(hDlg, 0);
    DestroyWindow(hDlg); 
    SetWindowLong(hDlg, GWL_USERDATA, NULL);
    delete this;
  } else if (uMsg == WM_MOUSEMOVE) {
    POINTS pt = MAKEPOINTS(lParam);
    D3DCOLOR color = 0;
    if (pt.x >= 32 && pt.y >= 32) {
      int x = pt.x - 32;
      int y = pt.y - 32;
      if (x < CLIENT_SIZE && y < CLIENT_SIZE) {
        D3DLOCKED_RECT lr;
        D3DSURFACE_DESC desc;
        mask_texture_->GetLevelDesc(0, &desc);
        int tex_x = x / float(CLIENT_SIZE) * desc.Width;
        int tex_y = y / float(CLIENT_SIZE) * desc.Height;
        RECT rc = { tex_x, tex_y, tex_x+1, tex_y+1 };
        if (SUCCEEDED(mask_texture_->LockRect(
              0,
              &lr,
              &rc,
              D3DLOCK_READONLY))) {
          color = 0xFF000000 | (*((D3DCOLOR*)lr.pBits));
          mask_texture_->UnlockRect(0);
          const Evidyon::World::Tools::MapMaskColorData* named_color
            = map_mask_->colorData(color);
          char str[256];
          if (named_color) {
            sprintf_s(str,
                      256,
                      "Select Map Mask Color - Mouse Over \"%s\"", 
                      named_color->getName().c_str());
          } else {
            sprintf_s(str,
                      256,
                      "Select Map Mask Color - Mouse Over Unnamed Color %lu,%lu,%lu", 
                      0xFF&(color >> 16),
                      0xFF&(color >> 8),
                      0xFF&(color >> 0));
          }
          SetWindowText(hDlg, str);
        }
      }
    }
    if (color != 0) {
      current_color_ = color;
    } else {
      current_color_
        = D3DCOLOR_XRGB(rand()%255,rand()%255,rand()%255); // scramble
      SetWindowText(hDlg, "Select Map Mask Color");
    }
  } else if (uMsg == WM_LBUTTONUP) {
    //if (current_color_ >> 24 != 0) { // make sure alpha is valid
      POINTS pt = MAKEPOINTS(lParam);
      D3DCOLOR color = 0;
      int x = pt.x - 32;
      int y = pt.y - 32;
      D3DSURFACE_DESC desc;
      mask_texture_->GetLevelDesc(0, &desc);
      bool close_dialog = false;
      int tex_x = x / float(CLIENT_SIZE) * desc.Width;
      int tex_y = y / float(CLIENT_SIZE) * desc.Height;
      allocated_functionoid_->execute(this,
                                      map_,
                                      map_mask_,
                                      current_color_,
                                      &close_dialog,
                                      tex_x,
                                      tex_y);
      if (close_dialog) {
        delete allocated_functionoid_;
        editor_->removeD3DWindow(this);
        EndDialog(hDlg, 0);
        DestroyWindow(hDlg); 
        SetWindowLong(hDlg, GWL_USERDATA, NULL);
        delete this;
      }
    //}
  } else if (uMsg == WM_PAINT) {
    editor_->renderD3DWindow(this);
  }
  return 0;
}



}