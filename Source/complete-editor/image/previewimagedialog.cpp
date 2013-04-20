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
#include "previewimagedialog.h"


namespace Evidyon {


  
//----[  begin  ]--------------------------------------------------------------
bool PreviewImageDialog::begin(Evidyon::Tools::CompleteEditor* editor,
                               Evidyon::Image::Tools::Image* image) {
  if (!image->loadD3DTexture(editor->getD3DDevice(), true)) return false;
  LPDIRECT3DTEXTURE9 texture = image->getD3DTexture();
  texture->GetLevelDesc(0, &desc);
  editor_ = editor;
  image_ = image;
  dcx::dcxWin32DialogTemplate dialog;
  dialog.setStyleAsResizeableAppWindow();
  dialog.setTitle("Image Preview");
  BYTE buffer[256];
  LPCDLGTEMPLATE dlg_template = dialog.compile(buffer, sizeof(buffer));
  dialog_handle_ = createDialog(GetModuleHandle(NULL),
                                dlg_template,
                                editor->topWindow());
  dcx::dcxWin32SetWindowClientSize(dialog_handle_, desc.Width, desc.Height);
  return editor->addD3DWindow(this);
}

//----[  render  ]-------------------------------------------------------------
bool PreviewImageDialog::render(Evidyon::Tools::CompleteEditor* editor,
                                LPDIRECT3DDEVICE9 d3d_device) {
  if (SUCCEEDED(d3d_device->BeginScene())) {
    d3d_device->Clear(0,
                      NULL,
                      D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                      D3DCOLOR_XRGB(255,0,0),
                      1.0f,
                      0);
    editor->getRenderStateManager()->resetState();
    d3d_device->SetTexture(0, image_->getD3DTexture());
    struct Vertex {
      FLOAT x, y, z, rhw; // The transformed position for the vertex
      FLOAT u, v;         // The coordinates
    } vertices[4] = {
      { 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f },
      { float(desc.Width), 0.0f, 0.0f, 0.5f, 1.0f, 0.0f },
      { float(desc.Width), float(desc.Height), 0.0f, 0.5f, 1.0f, 1.0f },
      { 0.0f, float(desc.Height), 0.0f, 0.5f, 0.0f, 1.0f },
    };
    d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    d3d_device->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);
    d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
    d3d_device->EndScene();
  }
  return true;
}




//----[  dialogEvent  ]------------------------------------------------------
INT_PTR PreviewImageDialog::dialogEvent(HWND hDlg,
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
  }
  return 0;
}



}