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
#ifndef __SERVER_TRIGGER_TRIGGERINSTANCE_H__
#define __SERVER_TRIGGER_TRIGGERINSTANCE_H__
#pragma once


namespace Evidyon {
class ActorInstance;
namespace Trigger {
struct Trigger;
}
}

namespace Evidyon {
namespace Trigger {
namespace Server {


  
//----[  TriggerInstance  ]----------------------------------------------------
class TriggerInstance {
public:
  static TriggerInstance* CreateInstance(
    const Evidyon::Trigger::Trigger* data);

public:

  virtual ~TriggerInstance() = 0;

  // This method is called whenever an actor enters a location to which this
  // trigger is bound.
  virtual void onEnter(ActorInstance* actor);

private:
};

}
}
}

#endif