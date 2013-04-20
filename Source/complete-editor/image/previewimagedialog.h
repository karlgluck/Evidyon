//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __PREVIEWIMAGEDIALOG_H__
#define __PREVIEWIMAGEDIALOG_H__
#pragma once


#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"

namespace Evidyon {



//----[  PreviewImageDialog  ]-------------------------------------------------
class PreviewImageDialog : public dcx::dcxWin32DialogController, Evidyon::Tools::D3DWindow {
public:
  bool begin(Evidyon::Tools::CompleteEditor* editor,
             Evidyon::Image::Tools::Image* image);
  HWND getWindowHandle() { return dialog_handle_; }
  bool render(Evidyon::Tools::CompleteEditor* editor,
              LPDIRECT3DDEVICE9 d3d_device);
private:
  virtual INT_PTR dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
  HWND dialog_handle_;
  Evidyon::Tools::CompleteEditor* editor_;
  Evidyon::Image::Tools::Image* image_;
  D3DSURFACE_DESC desc;
};


}

#endif