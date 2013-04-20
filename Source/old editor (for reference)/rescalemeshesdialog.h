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

#ifndef __RESCALEMESHESDIALOG_H__
#define __RESCALEMESHESDIALOG_H__
#pragma once

#include <dcx/d3d>

/**
 * 
 */
class RescaleMeshesDialog : public dcx::dcxD3DDialogController
{
    public:

        /**
         * Initializes the class
         */
        RescaleMeshesDialog();

        /**
         * Runs the rescale-meshes dialog
         */
        int execute(const dc::dcGenericResource::Array* display);

        /**
         * Forces this dialog (and any of its children) to close
         */
        void end(INT_PTR result);


    protected:

        /**
         * If a menu item is selected, this method is invoked
         *   @param hDlg The dialog containing the menu
         *   @param hMenu The menu containing the selected item
         *   @param ItemID The ID of the item that was selected
         *   @param ItemIndex The index of the selected item
         */
        INT_PTR dialogMenuNotification(HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex);

        /**
         * When something on the dialog is activated, this function is invoked
         *   @param hWnd Dialog handle
         *   @param hControl Control that spawned the notification
         *   @param ControlID ID of the control that created the notification
         *   @param Notification The event that occurred
         *   @return Return code
         */
        INT_PTR dialogNotification(HWND hDlg, HWND hControl, WORD ControlID, WORD Notification);

        /**
         * All messages received by the dialog are passed through here.  The default
         * implementation of this method will call EndDialog with a value of "0"
         * when the window is closed.
         *   @param hWnd Dialog handle
         *   @param uMsg Message
         *   @param wParam Parameter
         *   @param lParam Parameter
         *   @return Return code
         */
        INT_PTR dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:

        dcxWin32Mouse myMouse;
        dcxWin32Keyboard myKeyboard;
};



#endif