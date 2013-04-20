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
#ifndef __EVIDYONTOOLDIALOG_H__
#define __EVIDYONTOOLDIALOG_H__
#pragma once


namespace dc {
class dcGenericResource;
}

#include <dcx/dcxwin32dialogcontroller.h>

namespace Evidyon {
namespace Tools {


class EvidyonTool;
class EvidyonToolDialogListElement;

  
//----[  EvidyonToolDialog  ]--------------------------------------------------
class EvidyonToolDialog  : public dcx::dcxWin32DialogController {
public:

  // Initializes this dialog
  EvidyonToolDialog(EvidyonTool* owner);

  // Loads, activates and displays this dialog using the provided resource as
  // the root of the dialog tree.  The root resource is not itself displayed
  // in the tree.  This is a blocking method call.
  // If the dialog's parent window is specified, that owner window will not be
  // accessible until this one closes.
  // If a menu is not needed, set its resource ID to 0.
  bool execute(dc::dcGenericResource* root,
               HWND parent_window,
               WORD menu_resource_id);

  // Loads the dialog, but returns after it has been created.  This is the
  // asynchronous way of handling the dialog, so be sure to run a message-pump!
  bool begin(dc::dcGenericResource* root,
             HWND parent_window,
             WORD menu_resource_id);

  // Forces this dialog (and any of its children) to close
  void end(INT_PTR result);

  // Clears the list-box and re-adds the root members of the box
  void synchronizeDisplay();

  // Obtains the dialog's window handle (if it is currently executing)
  HWND getWindowHandle() const;

  // Obtains the root tool object
  EvidyonTool* getTool() const;


protected:

  // These methods override declarations in dcxWin32DialogController to
  // implement the functionality of this dialog.
  virtual INT_PTR dialogMenuNotification(HWND hDlg,
                                         HMENU hMenu,
                                         WORD ItemID,
                                         WORD ItemIndex);
  virtual INT_PTR dialogNotification(HWND hDlg,
                                     HWND hControl,
                                     UINT_PTR ControlID,
                                     UINT Notification);
  virtual INT_PTR dialogEvent(HWND hDlg,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam);


protected:

  // Called for WM_INITDIALOG
  INT_PTR dialogInitDialog(HWND hDlg);

  // Called for WM_CLOSE
  INT_PTR dialogClose();

  // Called for WM_MEASUREITEM
  INT_PTR dialogMeasureItem(LPMEASUREITEMSTRUCT measureItemData);

  // Called for WM_DRAWITEM
  INT_PTR dialogDrawItem(LPDRAWITEMSTRUCT drawItemData);

  // Called for WM_DELETEITEM
  INT_PTR dialogDeleteItem(LPDELETEITEMSTRUCT deleteItemData);

  // Called for WM_CONTEXTMENU
  INT_PTR dialogContextMenu(HWND hWindow, POINTS point);

  // Called for WM_SIZE
  INT_PTR dialogSize(WORD width, WORD height);


protected:

  // Removes children of an object from the list in this editor
  void recursiveRemoveChildren(EvidyonToolDialogListElement* element,
                               int BaseIndex);

private:

  // Writes this dialog's template
  LPCDLGTEMPLATE compileDialogTemplate(WORD menu_resource_id,
                                       BYTE* buffer,
                                       size_t buffer_size);


private:

  // The tool to notify of events
  EvidyonTool* owner_;

  // This dialog's window handle
  HWND window_;

  // The root resource that this dialog displays
  dc::dcGenericResource* root_resource_;
};

}
}



#endif