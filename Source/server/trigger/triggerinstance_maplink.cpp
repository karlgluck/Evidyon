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
#include "triggerinstance_maplink.h"
#include "server/actor.h"
#include "server/globalworldmanager.h"


namespace Evidyon {
namespace Trigger {
namespace Server {



  
//----[  TriggerInstance_MapLink  ]--------------------------------------------
TriggerInstance_MapLink::TriggerInstance_MapLink(
    const Evidyon::Trigger::Trigger_MapLink* data) {
  memcpy(&data_, data, sizeof(data_));
}




//----[  onEnter  ]------------------------------------------------------------
void TriggerInstance_MapLink::onEnter(ActorInstance* actor) {
  actor->changeMap(
    GlobalWorldManager::singleton()->getMapFromIndex(data_.destination_map),
    data_.x + 0.5f,
    data_.y + 0.5f);
}


}
}
}

