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
#include "eventinstance_targetedspecialfx.h"
#include "event_targetedspecialfx.h"
#include <assert.h>
#include "shared/binding/binding.h"
#include "common/null.h"

namespace Evidyon {
namespace Event {


  
//----[  EventInstance_TargetedSpecialFX  ]------------------------------------
EventInstance_TargetedSpecialFX::EventInstance_TargetedSpecialFX(
    double time,
    const Event_TargetedSpecialFX* data) {
  if (data->duration < 0.0) {
    expiration_time_ = -1.0;
  } else {
    expiration_time_ = time + data->duration;
  }
}




//----[  setBindingsUsed  ]----------------------------------------------------
void EventInstance_TargetedSpecialFX::setBindingsUsed(
    Binding::BindingPtrSet* bindings) {
  using namespace Evidyon::Binding;
  assert(bindings->size() < MAX_BINDINGS);

  size_t index = 0;
  for (BindingPtrSet::iterator i = bindings->begin(); i != bindings->end(); ++i) {
    bindings_[index++] = *i;
  }
}




//----[  update  ]-------------------------------------------------------------
bool EventInstance_TargetedSpecialFX::update(
    double time,
    double time_since_last_update) {
  if ((expiration_time_ > 0.0) && time > expiration_time_) {
    terminate(0.0, NULL);
    return true;
  } else {
    return false;
  }
}




//----[  terminate  ]----------------------------------------------------------
void EventInstance_TargetedSpecialFX::terminate(
    double time,
    SpecialFX::SpecialFXManager* special_fx_manager) {
  for (int i = 0; i < MAX_BINDINGS; ++i) {
    Binding::Binding* binding = bindings_[i].get();
    bindings_[i].reset();
    if (binding == NULL) continue;
    binding->setExpired();
  }
}


}
}

