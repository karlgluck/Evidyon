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
#ifndef __SPECIALFXEDITINGDIALOG_H__
#define __SPECIALFXEDITINGDIALOG_H__
#pragma once


#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "shared/evidyoncamera.h"
#include "shared/evidyond3dlightmanager.h"
#include "shared/binding/bindingptr.h"


namespace Evidyon {



class SpecialFXEditingDialogBinding;

  
//----[  SpecialFXEditingDialog  ]----------------------------------------------
// In this dialog: click to generate a visual effect of the current
// type.  It exists as long as the mouse is held down.  The source coordinate
// is the center of the screen, and the target coordinate is the mouse.
// TODO: use keys to cycle through the different types of FX
//       context menu allows select effect to display, create new effect
//       based on current effect, generate brand new effect, edit current effect
class SpecialFXEditingDialog : public dcx::dcxWin32DialogController,
                                     Tools::D3DWindow {
  typedef std::tr1::shared_ptr<SpecialFXEditingDialogBinding>
    SpecialFXEditingDialogBindingPtr;

public:
  SpecialFXEditingDialog();
  ~SpecialFXEditingDialog();
  void destroy();

  // The functionoid is automatically deleted when this dialog exits as long as
  // begin() has successfully instantiated the dialog.
  bool begin(Evidyon::Tools::CompleteEditor* editor,
             const dc::dcGenericResource::Array& special_fx_to_edit);
  virtual HWND getWindowHandle();
  virtual bool render(Evidyon::Tools::CompleteEditor* editor,
                      LPDIRECT3DDEVICE9 d3d_device);

private:
  virtual INT_PTR dialogEvent(HWND hDlg,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam);

private:

  // Rewrites the window's title based on the effect currently being displayed
  void updateWindowTitle();

private:

  // Main window handle
  HWND dialog_handle_;

  // Root editor object
  Evidyon::Tools::CompleteEditor* editor_;

  // Current position in the map.  This is smoothed toward the mesh &
  // scenery objects being edited.
  D3DXVECTOR2 camera_position_;
  EvidyonCamera camera_;

  // The index of the edit effect to create when the mouse is clicked
  size_t current_fx_index_;

  typedef std::vector<dc::dcTable<SpecialFX::Tools::SpecialFX>::Element*> SpecialFXList;
  SpecialFXList special_fx_to_edit_;

  // Effect bindings
  SpecialFXEditingDialogBindingPtr source_;
  SpecialFXEditingDialogBindingPtr target_;

  // Time the particle effects were last updated
  double last_update_time_;

  // Manages ambient light and lighting effects
  Evidyon::EvidyonD3DLightManager light_manager_;

  // Currently selected effect intensity
  float intensity_;
};



}


#endif