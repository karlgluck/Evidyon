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

// Desc:    Entry point to the Project Volucris resource editor, which is a project built using
//          the DirectCraft (dc) and DirectCraft Extensions (dcx) libraries.

#include "../common/vld-10/vld.h"
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include <dc/dcresourcetrader.h>

#include <dcx/inifilereader>
#include "commctrl.h"




//------------------------------------------------------------------------------------------------
// Name:  WinMain
// Desc:  Entry point for the application
//------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    { // initialize the path
      char buffer[MAX_PATH];
      GetCurrentDirectory(MAX_PATH, buffer);
      dc::dcFileName::setCurrentPath(buffer);
    }

    INITCOMMONCONTROLSEX InitCtrlEx;
    InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCtrlEx.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&InitCtrlEx);

    if (true)
    {
        std::string s;
        dc::dcResourceTrader::printAllResourceTypes(&s);
        OutputDebugString(s.c_str());
    }

    // Execute the editor application
    return globalEditor()->Run();
}
