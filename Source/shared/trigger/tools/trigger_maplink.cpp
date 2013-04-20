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
#include "trigger_maplink.h"
#include "shared/trigger/trigger.h"


namespace Evidyon {
namespace Trigger {
namespace Tools {

dc::dcClass<Trigger_MapLink>::Implements<Trigger> Trigger_MapLink::implements_trigger_;




//----[  Trigger_MapLink  ]----------------------------------------------------
Trigger_MapLink::Trigger_MapLink() {
  member("Destination Map", &destination_map_);
  member("x", &x_);
  member("y", &y_);
}



//----[  compile  ]------------------------------------------------------------
bool Trigger_MapLink::compile(Evidyon::Trigger::Trigger* trigger) {
  trigger->type = Evidyon::Trigger::Trigger::MAP_LINK;
  trigger->map_link.destination_map = destination_map_.getReferencedResourceIndex();
  trigger->map_link.x = x_.getValue() + 0.5;
  trigger->map_link.y = y_.getValue() + 0.5;
  return trigger->map_link.destination_map != Evidyon::World::INVALID_MAP_INDEX;
}


}
}
}

