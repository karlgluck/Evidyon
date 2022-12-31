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

