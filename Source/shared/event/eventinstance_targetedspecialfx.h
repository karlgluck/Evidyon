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