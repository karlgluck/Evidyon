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
#include "shared/world/tools/mapmask.h"
#include "shared/world/tools/mapareareference.h"
#include "shared/world/tools/maplocationreference.h"
#include "shared/world/tools/map.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"

#include <assert.h>

#include "shared/geosid/tools/geosid.h"




//----[  Geosid  ]-------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Geosid::Tools::Geosid)
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Geosid::GeosidType>)
  ENUM_ACTIONS(Evidyon::Geosid::GeosidType),
END_DEFINING_ACTIONS()





//----[  dc::dcList<Geosid>  ]-------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Geosid::Tools::Geosid>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add...", &DirectCraftActions::GenerateCollectionElement<Evidyon::Tools::GeosidList> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Geosid::Tools::Geosid>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Geosid::Tools::Geosid>::Reference)
  LIST_REFERENCE_ACTIONS(Evidyon::Geosid::Tools::Geosid),
END_DEFINING_ACTIONS()

