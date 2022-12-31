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

