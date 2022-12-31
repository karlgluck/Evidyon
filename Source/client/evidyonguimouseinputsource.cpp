//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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

