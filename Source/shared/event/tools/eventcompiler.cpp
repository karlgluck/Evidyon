//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#include "eventcompiler.h"
#include "shared/event/event.h"
#include <assert.h>
#include "common/safemacros.h"


namespace Evidyon {
namespace Event {
namespace Tools {



  
//----[  EventCompiler  ]------------------------------------------------------
EventCompiler::EventCompiler() {
}




//----[  ~EventCompiler  ]-----------------------------------------------------
EventCompiler::~EventCompiler() {
  clear();
}




//----[  clear  ]--------------------------------------------------------------
void EventCompiler::clear() {
  for (EventList::iterator i = events_.begin();
        i != events_.end(); ++i) {
    Evidyon::Event::Event* _event = *i;
    SAFE_DELETE(_event);
  }
  events_.clear();
}



//----[  add  ]----------------------------------------------------------------
EventIndex EventCompiler::add(
    const Evidyon::Event::Event* description) {
  if (description->type == Evidyon::Event::Event::TARGETED_SPECIAL_FX &&
      description->targeted_special_fx.special_fx ==
      Evidyon::SpecialFX::INVALID_SPECIALFX_INDEX) {
    return INVALID_EVENT_INDEX;
  }
  if (description->type == Evidyon::Event::Event::PROJECTILE &&
      description->projectile.creation_special_fx ==
        Evidyon::SpecialFX::INVALID_SPECIALFX_INDEX &&
      description->projectile.intermediate_special_fx ==
        Evidyon::SpecialFX::INVALID_SPECIALFX_INDEX &&
      description->projectile.destruction_special_fx ==
        Evidyon::SpecialFX::INVALID_SPECIALFX_INDEX) {
    return INVALID_EVENT_INDEX;
  }
  Evidyon::Event::Event* internal_event = new Evidyon::Event::Event;
  memcpy(internal_event, description, sizeof(*internal_event));
  events_.push_back(internal_event);
  return events_.size() - 1;
}


//----[  add  ]----------------------------------------------------------------
EventIndex EventCompiler::add(
    const Evidyon::Event::Event_Projectile* description) {
  Evidyon::Event::Event unioned_event;
  unioned_event.type = Evidyon::Event::Event::PROJECTILE;
  memcpy(&unioned_event.projectile, description, sizeof(*description));
  return add(&unioned_event);
}




//----[  add  ]----------------------------------------------------------------
EventIndex EventCompiler::add(
    const Evidyon::Event::Event_TargetedSpecialFX* description) {
  Evidyon::Event::Event unioned_event;
  unioned_event.type = Evidyon::Event::Event::TARGETED_SPECIAL_FX;
  memcpy(&unioned_event.targeted_special_fx, description, sizeof(*description));
  return add(&unioned_event);
}




//----[  numberOfEvents  ]-----------------------------------------------------
size_t EventCompiler::numberOfEvents() const {
  return events_.size();
}




//----[  compileEvent  ]-------------------------------------------------------
void EventCompiler::compileEvent(size_t index,
                                 Evidyon::Event::Event* event_out) {
  memcpy(event_out, events_.at(index), sizeof(*event_out));
}



}
}
}

