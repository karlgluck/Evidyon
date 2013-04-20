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
#ifndef __SHARED_TRIGGER_TRIGGER_GEOSIDACTIVATE_H__
#define __SHARED_TRIGGER_TRIGGER_GEOSIDACTIVATE_H__
#pragma once

#include "shared/geosid/geosidindex.h"

namespace Evidyon {
namespace Trigger {


  
//----[  Trigger_GeosidActivate  ]---------------------------------------------
// This trigger type is placed automatically by a geosid in the 9 squares
// centered on its location.  When a player steps on one of these squares,
// their default action with the geosid will be performed:
//  - if the geosid has opened a portal the player will be
//      transported to that portal's location
//  - for wild geosids, a non-owning guild member will attack that geosid
struct Trigger_GeosidActivate {
  Geosid::GeosidIndex geosid;
};

}
}

#endif