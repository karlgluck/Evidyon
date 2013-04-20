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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../dcx/dcxwin32dialogcontroller.h"


using namespace dcx;

// todo:  using SetWindowLong is not recommended, although it works, because 64-bit addresses
//        cannot be stored here.


//-----------------------------------------------------------------------------
// Name: dcxControlledDialogProc
// Desc: Passes messages through to a DialogController
//-----------------------------------------------------------------------------
INT_PTR CALLBACK dcx::dcxControlledDialogProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    // If this dialog is just being created, we have to save the class interface
    if( uMsg == WM_INITDIALOG )
    {
        // Save the parameter as a pointer
        SetWindowLongPtr( hDlg, GWL_USERDATA, (LONG_PTR)lParam );

        // Make sure this dialog's menu (if it has one) sends WM_MENUCOMMAND messages
        HMENU hMenu = GetMenu( hDlg );
        if( hMenu )
        {
            MENUINFO menuInfo;
            menuInfo.cbSize = sizeof(MENUINFO);
            menuInfo.fMask = MIM_STYLE;
            GetMenuInfo( hMenu, &menuInfo );
            menuInfo.dwStyle |= MNS_NOTIFYBYPOS;
            SetMenuInfo( hMenu, &menuInfo );
        }
    }

    // Get the interface pointer
    dcxWin32DialogController* pController = (dcxWin32DialogController*)GetWindowLongPtr( hDlg, GWL_USERDATA );

    // If no controller exists yet...don't do anything!
    if( !pController ) return 0;

    // If this is a notification message, pass it via a different interface
    if( uMsg == WM_COMMAND )
        return pController->dialogNotification( hDlg, (HWND)lParam, LOWORD(wParam), HIWORD(wParam) );
    else if( uMsg == WM_NOTIFY )
        return pController->dialogNotification( hDlg, ((NMHDR*)lParam)->hwndFrom, (int)wParam, ((NMHDR*)lParam)->code );
    else if( uMsg == WM_MENUCOMMAND )
        //return pController->dialogMenuNotification( hDlg, (HMENU)lParam, LOWORD(wParam), HIWORD(wParam) );
        // todo: this is ugly and not the way this is supposed to work....
        return pController->dialogMenuNotification( hDlg, (HMENU)lParam, GetMenuItemID( (HMENU)lParam, LOWORD(wParam) ), LOWORD(wParam) );
    else
        return pController->dialogEvent( hDlg, uMsg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Name: beginDialog
// Desc: Runs the controlled dialog box
//-----------------------------------------------------------------------------
INT_PTR dcxWin32DialogController::beginDialog( HINSTANCE hInstance, LPCSTR pTemplate, HWND hParent )
{
    return DialogBoxParam( hInstance, pTemplate, hParent, dcxControlledDialogProc, (LPARAM)this );
}


//-----------------------------------------------------------------------------
// Name: beginDialog
// Desc: Runs the controlled dialog box
//-----------------------------------------------------------------------------
INT_PTR dcxWin32DialogController::beginDialog( HINSTANCE hInstance, LPCDLGTEMPLATE dlgTemplate, HWND hParent )
{
    return DialogBoxIndirectParam( hInstance, dlgTemplate, hParent, dcxControlledDialogProc, (LPARAM)this );
}


//-----------------------------------------------------------------------------
// Name: beginDialog
// Desc: Creates the controlled dialog box and returns its handle
//-----------------------------------------------------------------------------
HWND dcxWin32DialogController::createDialog( HINSTANCE hInstance, LPCSTR pTemplate, HWND hParent )
{
    return CreateDialogParam( hInstance, pTemplate, hParent, dcxControlledDialogProc, (LPARAM)this );
}



//-----------------------------------------------------------------------------
// Name: createDialog
// Desc: Creates a controlled dialog box from an in-memory dialog template
//-----------------------------------------------------------------------------
HWND dcxWin32DialogController::createDialog( HINSTANCE hInstance, LPCDLGTEMPLATE dlgTemplate, HWND hParent )
{
    return CreateDialogIndirectParam( hInstance, dlgTemplate, hParent, dcxControlledDialogProc, (LPARAM)this );
}



//-----------------------------------------------------------------------------
// Name: dialogMenuNotification
// Desc: If a menu item is selected, this method is invoked
//-----------------------------------------------------------------------------
INT_PTR dcxWin32DialogController::dialogMenuNotification( HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex )
{
    return 0;
}


//-----------------------------------------------------------------------------
// Name: dialogNotification
// Desc: Default implementation
//-----------------------------------------------------------------------------
INT_PTR dcxWin32DialogController::dialogNotification( HWND hDlg, HWND hControl, UINT_PTR ControlID, UINT Notification )
{
    return 0;
}

//-----------------------------------------------------------------------------
// Name: dialogEvent
// Desc: Default implementation
//-----------------------------------------------------------------------------
INT_PTR dcxWin32DialogController::dialogEvent( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( uMsg == WM_CLOSE ) { EndDialog( hDlg, 0 ); DestroyWindow( hDlg ); }
    return 0;
}