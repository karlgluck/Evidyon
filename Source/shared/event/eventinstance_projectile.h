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
#ifndef __SHARED_EVENT_EVENTINSTANCE_PROJECTILE_H__
#define __SHARED_EVENT_EVENTINSTANCE_PROJECTILE_H__
#pragma once

#include "eventinstance.h"
#include "shared/binding/bindingptr.h"
#include "shared/binding/bindingptrset.h"
#include "shared/binding/locationbindingptr.h"

namespace Evidyon {
namespace Event {
struct Event_Projectile;
}
namespace SpecialFX {
class SpecialFXManager;
}
}


namespace Evidyon {
namespace Event {

//----[  EventInstance_Projectile  ]-------------------------------------------
class EventInstance_Projectile : public EventInstance {
  static const int MAX_BINDINGS = 16;
public:
  EventInstance_Projectile(double time,
                           float source_x,
                           float source_z,
                           float target_x,
                           float target_z,
                           const Event_Projectile* data);

  Binding::BindingPtr projectileBinding();
  void setBindingsUsed(Binding::BindingPtrSet* bindings);

  virtual bool update(double time, double time_since_last_update);
  virtual void terminate(double time, SpecialFX::SpecialFXManager* special_fx_manager);

private:
  double expiration_time_;
  const Event_Projectile* data_;
  float x_, z_, angle_, dx_, dz_;
  Binding::LocationBindingPtr projectile_;
  Binding::BindingPtr bindings_[MAX_BINDINGS];
};

}
}

#endif