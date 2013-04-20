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
#ifndef __MASKCOLORSELECTORDIALOG_H__
#define __MASKCOLORSELECTORDIALOG_H__
#pragma once


#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"

namespace Evidyon {


class MapMaskColorSelectorDialog;
class MaskColorSelectorDialogFunctionoid;


//----[  MapMaskColorSelectorDialog  ]-----------------------------------------
class MapMaskColorSelectorDialog : public dcx::dcxWin32DialogController,
                                          Evidyon::Tools::D3DWindow {
public:
  typedef dc::dcList<World::Tools::MapMask>::Element MapMaskElement;
public:
  ~MapMaskColorSelectorDialog();

  // The functionoid is automatically deleted when this dialog exits as long as
  // begin() has successfully instantiated the dialog.
  bool begin(HWND owner,
             Evidyon::Tools::CompleteEditor* editor,
             World::Tools::Map* map,
             MapMaskElement* map_mask,
             MaskColorSelectorDialogFunctionoid* allocated_functionoid);
  HWND getWindowHandle() { return dialog_handle_; }
  bool render(Evidyon::Tools::CompleteEditor* editor,
              LPDIRECT3DDEVICE9 d3d_device);

private:
  virtual INT_PTR dialogEvent(HWND hDlg,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam);

private:
  HWND dialog_handle_;
  Evidyon::Tools::CompleteEditor* editor_;
  LPDIRECT3DTEXTURE9 mask_texture_;
  World::Tools::Map* map_;
  MapMaskElement* map_mask_;
  D3DCOLOR current_color_;
  MaskColorSelectorDialogFunctionoid* allocated_functionoid_;

};

  
//----[  MaskColorSelectorDialogFunctionoid  ]---------------------------------
class MaskColorSelectorDialogFunctionoid {
public:
  virtual ~MaskColorSelectorDialogFunctionoid() = 0;
  virtual void execute(
      MapMaskColorSelectorDialog* dialog,
      World::Tools::Map* map,
      MapMaskColorSelectorDialog::MapMaskElement* selected_mask,
      D3DCOLOR selected_color,
      bool* close_dialog,
      int texture_mouse_x,
      int texture_mouse_y) = 0;
};


}

#endif