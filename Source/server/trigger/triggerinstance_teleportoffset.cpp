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
#include "triggerinstance_teleportoffset.h"
#include "server/actor.h"
#include <dc/debug>

namespace Evidyon {
namespace Trigger {
namespace Server {



  
//----[  TriggerInstance_MapLink  ]--------------------------------------------
TriggerInstance_TeleportOffset::TriggerInstance_TeleportOffset(
    const Evidyon::Trigger::Trigger_TeleportOffset* data) {
  data_ = *data; //  memcpy(&data_, data, sizeof(data_));
}




//----[  onEnter  ]------------------------------------------------------------
void TriggerInstance_TeleportOffset::onEnter(ActorInstance* actor) {
  float x, z;
  actor->getPosition(&x, &z);
  actor->teleportTo(x + data_.added_x, z + data_.added_z);
  DEBUG_INFO("Teleport To:  %f %f", x + data_.added_x, z + data_.added_z);
}



}
}
}

