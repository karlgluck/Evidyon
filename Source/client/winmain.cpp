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
#include "shared/evidyon_version.h"

/// Application reference
EvidyonClient* g_Client = NULL;

#include <direct.h>



//----[  MainWindowMsgProc  ]--------------------------------------------------
LRESULT WINAPI MainWindowMsgProc(HWND hWnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam)
{

    // Pass messages to the
    g_Client->handleWindowsMessage(hWnd, uMsg, wParam, lParam);

    switch(uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}





//----[  WinMain  ]------------------------------------------------------------
// This is where it all begins!
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT) {
  srand(GetTickCount());

  // Run the application
  g_Client = new EvidyonClient();
  g_Client->execute();
  SAFE_DELETE(g_Client);

  // Display any errors
  DEBUG_ATEXIT();

  // Exit
  return 0;
}