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
#ifndef __SERVER_TRIGGER_TRIGGERINSTANCE_TELEPORTOFFSET_H__
#define __SERVER_TRIGGER_TRIGGERINSTANCE_TELEPORTOFFSET_H__
#pragma once

#include "triggerinstance.h"
#include "shared/trigger/trigger_teleportoffset.h"

namespace Evidyon {
namespace Trigger {
namespace Server {


  
//----[  TriggerInstance_TeleportOffset  ]-------------------------------------
class TriggerInstance_TeleportOffset : public TriggerInstance {
public:
  TriggerInstance_TeleportOffset(
    const Evidyon::Trigger::Trigger_TeleportOffset* data);

public:
  virtual void onEnter(ActorInstance* actor);

private:
  Evidyon::Trigger::Trigger_TeleportOffset data_;
};


}
}
}

#endif