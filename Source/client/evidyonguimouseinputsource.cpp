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
#include "stdafx.h"


#define WM_MOUSEWHEEL       0x020A
#define WHEEL_DELTA         120



EvidyonGUIMouseInputSource::EvidyonGUIMouseInputSource() {
}




//-----------------------------------------------------------------------------
// Name: handleWindowsMessage
// Desc: Processes Windows messages
//-----------------------------------------------------------------------------
LRESULT EvidyonGUIMouseInputSource::handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult)
{
    // By default, we don't need to return a result
    *returnResult = false;

    // Handle the message
    switch(uMsg)
    {
        case WM_MOUSEMOVE:
        {
            main_window = hWnd;

            // Get the point structure
            POINTS pt = MAKEPOINTS(lParam);
            GUIPoint guiPoint = { pt.x, pt.y };

            // Update the state
            mouseMovedTo(guiPoint);

        } break;

        case WM_LBUTTONDOWN:   mouseButtonAction(GUIMOUSE_DOWN); SetCapture(hWnd); break;
        case WM_LBUTTONUP:     mouseButtonAction(GUIMOUSE_CLICK_SINGLE); 
                               mouseButtonAction(GUIMOUSE_UP); ReleaseCapture(); break;
        case WM_LBUTTONDBLCLK: mouseButtonAction(GUIMOUSE_CLICK_DOUBLE); break;

        case WM_RBUTTONDOWN:   mouseButtonAction(GUIMOUSE_ALT_DOWN); SetCapture(hWnd); break;
        case WM_RBUTTONUP:     mouseButtonAction(GUIMOUSE_CLICK_ALT_SINGLE); 
                               mouseButtonAction(GUIMOUSE_ALT_UP); ReleaseCapture(); break;
        case WM_RBUTTONDBLCLK: mouseButtonAction(GUIMOUSE_CLICK_ALT_DOUBLE); break;

        case WM_MOUSEWHEEL:    mouseScroll(-(float)(((int)wParam)) / (float)(1000.0f * WHEEL_DELTA)); break;
    }

    // Success
    return S_OK;
}

