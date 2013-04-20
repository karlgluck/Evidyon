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
#ifndef __SHARED_EVENT_EVENTMANAGER_H__
#define __SHARED_EVENT_EVENTMANAGER_H__
#pragma once

#include "eventid.h"
#include "eventindex.h"
#include <map>
#include "shared/binding/bindingptr.h"

namespace Evidyon {
namespace SpecialFX {
class SpecialFXManager;
}
namespace Event {
struct Event;
class EventInstance;
}
}


namespace Evidyon {
namespace Event {

//----[  EventManager  ]-----------------------------------------------------
class EventManager {
  typedef std::map<EventID, EventInstance*> EventInstances;

public:
  // Create a unique ID for use locally on the client.  This method will not
  // generate IDs that conflict with those created by the server.
  static Evidyon::Event::EventID generateClientEventID();

public:
  EventManager();
  ~EventManager();

  void create(SpecialFX::SpecialFXManager* special_fx_manager);
  void destroy();

  // Fills the manager with event types
  bool initNumberOfEvents(size_t count);
  void initEvent(size_t index, const Event* _event);
  void clearEvents();

  // Begins a new event
  void generate(EventID event_id,
                EventIndex event_index,
                double time,
                float intensity,
                float source_x,
                float source_z,
                Binding::BindingPtr source,
                float target_x,
                float target_z,
                Binding::BindingPtr* targets,
                size_t number_of_targets,
                bool involves_client);

  // Terminate is only called when the server sends a event fx termination
  // message to the client.  This occurs only when a event fx ends in an
  // unpredictable way.  For example, no termination message is sent when
  // a projectile expires without hitting anything or when a magic effect
  // ends normally.  However, if someone cancels the effect (takes down a
  // portal) or something else causes it to end (projectile hits something,
  // variable-length enchantment times out or is dispelled) the terminate
  // message is sent.
  void terminate(double time, EventID event_id);

  // Returns 'true' if an event with the given ID number exists
  bool isActive(EventID event_id) const;

  // Erase all active events.  Do this before changing maps and before
  // leaving the world.
  void clearEventInstances();

  // Advances the state of the events
  void update(double time, double time_since_last_update);

private:
  Event* getEvent(EventIndex event_index);
  EventInstance* getInstance(EventID event_id);

private:
  SpecialFX::SpecialFXManager* special_fx_manager_;
  Event* events_;
  size_t number_of_events_;
  EventInstances instances_;
};

}
}

#endif