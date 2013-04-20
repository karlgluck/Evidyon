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
#ifndef __DCXWIN32DIALOGCONTROLLER_H__
#define __DCXWIN32DIALOGCONTROLLER_H__

namespace dcx {

    /**
     * Use this function as the dialog processing method parameter in functions like
     * DialogBoxParam and pass a pointer to a DialogController as the optional parameter.
     */
    INT_PTR CALLBACK dcxControlledDialogProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );


    /**
     * A class that derives from this one is passed information from a standard dialog-
     * processing routine.  Either use the built-in methods to create the dialog, or be sure
     * to pass dcxControlledDialogProc as the window processor, and a "this" pointer as the LPARAM.
     */
    class dcxWin32DialogController
    {
        // Allow the dialog function to call the private notification methods
        friend INT_PTR CALLBACK dcx::dcxControlledDialogProc(HWND,UINT,WPARAM,LPARAM);

        protected:

            /**
             * Runs the controlled dialog box.
             * @param hInstance Instance of the application (usually GetModuleHandle(NULL) will work)
             * @param pResource Resource template of the dialog (use MAKEINTRESOURCE(dialog_id))
             * @param hParent The window of which to make this dialog a child.  Windows in the parent
             *                chain cannot be activated until this dialog is closed.
             * @return Whatever is passed to the second parameter of EndDialog
             */
            INT_PTR beginDialog( HINSTANCE hInstance, LPCSTR pTemplate, HWND hParent );

            /**
             * Runs the controlled dialog box.
             * @param hInstance Instance of the application (usually GetModuleHandle(NULL) will work)
             * @param pTemplate The compiled dialog template to use; dcxWin32DialogTemplate builds one
             * @param hParent The window of which to make this dialog a child.  Windows in the parent
             *                chain cannot be activated until this dialog is closed.
             * @return Whatever is passed to the second parameter of EndDialog
             */
            INT_PTR beginDialog( HINSTANCE hInstance, LPCDLGTEMPLATE dlgTemplate, HWND hParent );

            /**
             * Creates the controlled dialog box.  This will instantiate the dialog box, but then the
             * calling procedure becomes responsible for its management.  This method will not try
             * to stop you from instantiating multiple windows using this single controller, because
             * sometimes this is a valid behaviour (creating tool windows, for example) but you must
             * make careful, consistant use of the hDlg parameter of the notification methods.
             * @param hInstance Instance of the application (usually GetModuleHandle(NULL) will work)
             * @param pTemplate The compiled dialog template to use; dcxWin32DialogTemplate builds one
             * @param hParent The window of which to make this dialog a child.  Windows in the parent
             *                chain cannot be activated until this dialog is closed.
             * @return The created dialog handle, or NULL
             */
            HWND createDialog( HINSTANCE hInstance, LPCSTR pTemplate, HWND hParent );

            /**
             * Creates a controlled dialog box from an in-memory dialog template.  This method
             * instantiates the dialog box, but the calling procedure becomes responsible for the
             * dialog's management and Window's message-pump.  This method will not try to stop
             * you from instantiating multiple windows using this single controller, because
             * sometimes this is a valid behaviour (creating tool windows, for example) but you must
             * make careful, consistant use of the hDlg parameter of the notification methods.
             * @param hInstance Instance of the application (usually GetModuleHandle(NULL) will work)
             * @param pResource Resource template of the dialog (use MAKEINTRESOURCE(dialog_id))
             * @param hParent The window of which to make this dialog a child.  Windows in the parent
             *                chain cannot be activated until this dialog is closed.
             * @return The created dialog handle, or NULL
             */
            HWND createDialog( HINSTANCE hInstance, LPCDLGTEMPLATE dlgTemplate, HWND hParent );


        protected:

            /**
             * If a menu item is selected, this method is invoked
             *   @param hDlg The dialog containing the menu
             *   @param hMenu The menu containing the selected item
             *   @param ItemID The ID of the item that was selected
             *   @param ItemIndex The index of the selected item
             */
            virtual INT_PTR dialogMenuNotification( HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex );

            /**
             * When something on the dialog is activated, this function is invoked
             *   @param hWnd Dialog handle
             *   @param hControl Control that spawned the notification
             *   @param ControlID ID of the control that created the notification
             *   @param Notification The event that occurred
             *   @return Return code
             */
            virtual INT_PTR dialogNotification( HWND hDlg, HWND hControl, UINT_PTR ControlID, UINT Notification );

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
            virtual INT_PTR dialogEvent( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
    };
}



#endif