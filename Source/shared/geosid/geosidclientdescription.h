//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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