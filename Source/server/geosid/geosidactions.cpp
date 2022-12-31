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

