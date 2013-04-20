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
#include "zone.h"
#include "shared/zone/zone.h"


namespace Evidyon {
namespace Zone {
namespace Tools {



  
//----[  Zone  ]---------------------------------------------------------------
Zone::Zone() {
  member("Name", &name_);
  member("Background Sound", &background_sound_);
  member("Rules", &rules_);
  member("Chaos?", &chaos_);
}



//----[  compile  ]------------------------------------------------------------
void Zone::compile(Evidyon::Zone::Zone* zone) {
  strncpy_s(zone->name,
            Evidyon::Zone::Zone::NAME_LENGTH,
            name_.getValue().c_str(),
            Evidyon::Zone::Zone::NAME_LENGTH-1);
  zone->background_sound = background_sound_.getReferencedResourceIndex();
  zone->rules = rules_.getValue();
  zone->chaos = chaos_.getValue();
}



//----[  getZoneRules  ]-------------------------------------------------------
ZoneRules Zone::getZoneRules() const {
  return rules_.getValue();
}

//----[  isChaosZone  ]--------------------------------------------------------
bool Zone::isChaosZone() const {
  return chaos_.getValue();
}


}
}
}

