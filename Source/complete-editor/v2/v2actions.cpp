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
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"




//-----------------------------------------------------------------------------
// Name:  
// Desc:  
//-----------------------------------------------------------------------------
template <typename T, typename Y> int AddNewTtoTableY(dc::dcGenericResource* resource, void* data)
{
    // Get the table type
    dc::dcTable<Y>* table = (dcTable<Y>*)resource;

    // Ask the user for a name
    std::string name = "";
    if (dcx::dcxWin32PromptTextInput(NULL,  "Resource Name?", "Please enter a name for this resource:", &name))
    {
        // Create the resource
        dc::dcGenericResource* image = table->create<T>(name);

        // Sort the table.  TODO: make this not necessary?
        table->sort();

        // Success!
        return 1;
    }

    return 0;
}

