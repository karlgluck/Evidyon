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
#ifndef __SHARED_GEOSID_GEOSIDCLIENTDESCRIPTION_H__
#define __SHARED_GEOSID_GEOSIDCLIENTDESCRIPTION_H__
#pragma once

#include "shared/geosid/geonite.h"
#include "shared/geosid/geosidhashid.h"
#include "shared/geosid/geosidindex.h" 
#include "shared/world/mapindex.h"
#include "shared/magic/magicindex.h"
#include "common/number.h"
#include "shared/geosid/geosidtype.h"
#include "shared/event/eventindex.h"

namespace Evidyon {
namespace Geosid {



//----[  GeosidClientDescription  ]--------------------------------------------
struct GeosidClientDescription {

  Geosid::GeosidIndex index;

  // every geosid has a unique name
  static const int NAME_LENGTH = 32;
  char name[NAME_LENGTH];

  // The type defines what can be done at this geosid.  Item sacrifices can
  // only be made to white (town) geosids.
  GeosidType type;

  // Where in the world this geosid appears
  World::MapIndex map;
  float x, z;
  Event::EventIndex change_owner_event_index;
};



}
}

#endif