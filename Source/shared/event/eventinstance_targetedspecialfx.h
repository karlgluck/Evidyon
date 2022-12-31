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
#ifndef __SHARED_EVENT_EVENTINSTANCE_TARGETEDSPECIALFX_H__
#define __SHARED_EVENT_EVENTINSTANCE_TARGETEDSPECIALFX_H__
#pragma once

#include "eventinstance.h"
#include "shared/binding/bindingptr.h"
#include "shared/binding/bindingptrset.h"

namespace Evidyon {
namespace Event {
struct Event_TargetedSpecialFX;
}
namespace SpecialFX {
class SpecialFXManager;
}
}


namespace Evidyon {
namespace Event {

//----[  EventInstance_TargetedSpecialFX  ]------------------------------------
class EventInstance_TargetedSpecialFX : public EventInstance {
  static const int MAX_BINDINGS = 16;
public:
  EventInstance_TargetedSpecialFX(double time,
                                  const Event_TargetedSpecialFX* data);

  void setBindingsUsed(Binding::BindingPtrSet* bindings);

  virtual bool update(double time, double time_since_last_update);
  virtual void terminate(double time,
                         SpecialFX::SpecialFXManager* special_fx_manager);

private:
  // sentinel: < 0 = don't expire (must be terminated)
  double expiration_time_;
  Binding::BindingPtr bindings_[MAX_BINDINGS]; 
};

}
}

#endif