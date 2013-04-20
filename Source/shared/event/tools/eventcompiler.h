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
#ifndef __SHARED_EVENT_TOOLS_EVENTCOMPILER_H__
#define __SHARED_EVENT_TOOLS_EVENTCOMPILER_H__
#pragma once

#include <vector>
#include "shared/event/eventindex.h"

namespace Evidyon {
namespace Event {
struct Event;
struct Event_Projectile;
struct Event_TargetedSpecialFX;
}
}

namespace Evidyon {
namespace Event {
namespace Tools {


  
//----[  EventCompiler  ]------------------------------------------------------
// Sources of events include:
//  items - they can create projectiles or special effects for when they hit
//  magic - all magic effects are events
class EventCompiler {
  typedef std::vector<Evidyon::Event::Event*> EventList;
public:
  EventCompiler();
  ~EventCompiler();
  void clear();

  EventIndex add(const Evidyon::Event::Event* description);
  EventIndex add(const Evidyon::Event::Event_Projectile* description);
  EventIndex add(const Evidyon::Event::Event_TargetedSpecialFX* description);
  
  size_t numberOfEvents() const;
  void compileEvent(size_t index, Evidyon::Event::Event* event_out);
  
private:
  EventList events_;
};

}
}
}

#endif