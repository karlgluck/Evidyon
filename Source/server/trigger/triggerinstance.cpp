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

