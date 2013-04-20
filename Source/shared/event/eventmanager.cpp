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
#include "eventmanager.h"
#include "eventinstance.h"
#include "event.h"
#include "shared/specialfx/specialfxmanager.h"
#include "common/null.h"
#include <assert.h>
#include "common/safemacros.h"
#include "eventinstance_projectile.h"
#include "eventinstance_targetedspecialfx.h"

namespace Evidyon {
namespace Event {



  
//----[  generateClientEventID  ]----------------------------------------------
Evidyon::Event::EventID EventManager::generateClientEventID() {
  static Evidyon::Event::EventID next_event_id;
  // high bit always set for client events; server always has high bit
  // cleared.
  return 0x80000000 | (next_event_id++);
}

  
//----[  EventManager  ]-------------------------------------------------------
EventManager::EventManager() {
  special_fx_manager_ = NULL;
  events_ = NULL;
}



//----[  ~EventManager  ]------------------------------------------------------
EventManager::~EventManager() {
  destroy();
}


//----[  create  ]-------------------------------------------------------------
void EventManager::create(SpecialFX::SpecialFXManager* special_fx_manager) {
  special_fx_manager_ = special_fx_manager;
}



//----[  destroy  ]------------------------------------------------------------
void EventManager::destroy() {
  clearEvents();
  special_fx_manager_ = NULL;
}




//----[  initNumberOfEvents  ]-------------------------------------------------
bool EventManager::initNumberOfEvents(size_t count) {
  events_ = new Event[count];
  assert(events_);
  number_of_events_ = count;
  return events_ != NULL;
}




//----[  initEvent  ]----------------------------------------------------------
void EventManager::initEvent(size_t index, const Event* _event) {
  assert(index < number_of_events_);
  memcpy(&events_[index], _event, sizeof(*_event));
}



//----[  clearEvents  ]--------------------------------------------------------
void EventManager::clearEvents() {
  clearEventInstances();
  SAFE_DELETE_ARRAY(events_);
  number_of_events_ = 0;
}



//----[  generate  ]-----------------------------------------------------------
void EventManager::generate(EventID event_id,
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
                            bool involves_client) {
  assert(event_id != Evidyon::Event::INVALID_EVENT_ID);
  Event* event_object = getEvent(event_index);
  if (!event_object) return;

  switch (event_object->type) {
    case Event::PROJECTILE: {
      Binding::BindingPtrSet bindings_used;
      Event_Projectile* data
        = &event_object->projectile;
      EventInstance_Projectile* instance
        = new EventInstance_Projectile(time,
                                       source_x,
                                       source_z,
                                       target_x,
                                       target_z,
                                       data);
      special_fx_manager_->generate(
        data->creation_special_fx,
        time,
        intensity,
        source_x,
        source_z,
        source,
        target_x,
        target_z,
        &instance->projectileBinding(),
        1,
        data->lifetime,
        &bindings_used,
        involves_client);
      instance->setBindingsUsed(&bindings_used);
      instances_.insert(EventInstances::value_type(event_id, instance));
    } break;

    case Event::TARGETED_SPECIAL_FX: {
      Binding::BindingPtrSet bindings_used;
      Event_TargetedSpecialFX* data = &event_object->targeted_special_fx;
      EventInstance_TargetedSpecialFX* instance
        = new EventInstance_TargetedSpecialFX(time, data);
      special_fx_manager_->generate(
        data->special_fx,
        time,
        intensity,
        source_x,
        source_z,
        source,
        target_x,
        target_z,
        targets,
        number_of_targets,
        data->duration,
        &bindings_used,
        involves_client);
      instance->setBindingsUsed(&bindings_used);
      instances_.insert(EventInstances::value_type(event_id, instance));
    } break;

    default:
      assert(false && "invalid event object type");
      break;
  }
}




//----[  terminate  ]----------------------------------------------------------
void EventManager::terminate(double time, EventID event_id) {
  EventInstance* instance = getInstance(event_id);
  if (instance) {
    instance->terminate(time, special_fx_manager_);
    SAFE_DELETE(instance);
    instances_.erase(event_id);
  }
}



//----[  isActive  ]-----------------------------------------------------------
bool EventManager::isActive(EventID event_id) const {
  EventInstances::const_iterator i =
    (event_id == Evidyon::Event::INVALID_EVENT_ID)
      ? instances_.end()
      : instances_.find(event_id);
  return i == instances_.end() ? false : true;
}


//----[  clearEventInstances  ]------------------------------------------------
void EventManager::clearEventInstances() {
  for (EventInstances::iterator i = instances_.begin(); i != instances_. end(); ++i) {
    EventInstance* instance = i->second;
    instance->terminate(0.0, NULL);
    SAFE_DELETE(instance);
  }
  instances_.clear();
}



//----[  update  ]-------------------------------------------------------------
void EventManager::update(double time, double time_since_last_update) {
  for (EventInstances::iterator i = instances_.begin();
          i != instances_. end();) {
    EventInstance* instance = i->second;
    if (instance->update(time, time_since_last_update)) {
      SAFE_DELETE(instance);
      EventInstances::iterator next = i; ++next;
      instances_.erase(i);
      i = next;
    } else {
      ++i;
    }
  }
}




//----[  getEvent  ]-----------------------------------------------------------
Event* EventManager::getEvent(EventIndex event_index) {
  if (event_index == Evidyon::Event::INVALID_EVENT_INDEX) return NULL;
  assert(event_index < number_of_events_);
  return event_index < number_of_events_ ? &events_[event_index] : NULL;
}




//----[  getInstance  ]--------------------------------------------------------
EventInstance* EventManager::getInstance(EventID event_id) {
  EventInstances::iterator i =
    (event_id == Evidyon::Event::INVALID_EVENT_ID)
      ? instances_.end()
      : instances_.find(event_id);
  return i == instances_.end() ? NULL : i->second;
}


}
}

