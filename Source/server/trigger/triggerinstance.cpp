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
#include "triggerinstance.h"
#include "triggerinstance_maplink.h"
#include "triggerinstance_teleportoffset.h"
#include "triggerinstance_geosidactivate.h"
#include "shared/trigger/trigger.h"
#include <assert.h>
#include "common/null.h"


namespace Evidyon {
namespace Trigger {
namespace Server {


  
//----[  CreateInstance  ]-----------------------------------------------------
TriggerInstance* TriggerInstance::CreateInstance(
    const Evidyon::Trigger::Trigger* data) {
  switch (data->type) {
  using namespace Evidyon::Trigger;
  default: assert(false); return NULL;
  case Trigger::MAP_LINK:
    return new TriggerInstance_MapLink(&data->map_link);
  case Trigger::TELEPORT_OFFSET:
    return new TriggerInstance_TeleportOffset(&data->teleport_offset);
  case Trigger::GEOSID_ACTIVATE:
    return new TriggerInstance_GeosidActivate(&data->geosid_activate);
  }
}




//----[  ~TriggerInstance  ]---------------------------------------------------
TriggerInstance::~TriggerInstance() {
}


//----[  onEnter  ]------------------------------------------------------------
void TriggerInstance::onEnter(ActorInstance* actor) {
}



}
}
}

