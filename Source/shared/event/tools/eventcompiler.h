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