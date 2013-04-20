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
#include "eventinstance_projectile.h"
#include "event_projectile.h"
#include <assert.h>
#include <math.h> // atan2f, sin, cos
#include "shared/binding/binding.h"
#include "shared/binding/locationbinding.h"
#include "common/null.h"
#include "shared/specialfx/specialfxmanager.h"

namespace Evidyon {
namespace Event {


//----[  EventInstance_Projectile  ]-------------------------------------------
EventInstance_Projectile::EventInstance_Projectile(
    double time,
    float source_x,
    float source_z,
    float target_x,
    float target_z,
    const Event_Projectile* data) {

  expiration_time_ = time + data->lifetime;
  data_ = data;
  x_ = source_x;
  z_ = source_z;

  float dx = target_x - source_x;
  float dz = target_z - source_z;
  float dd = sqrtf(dx*dx+dz*dz);

  dx_ = dx * data->speed / dd;
  dz_ = dz * data->speed / dd;

  angle_ = -atan2f(dz, dx);

  projectile_ = Binding::LocationBinding::New(x_, 0.0f, z_);
}




//----[  projectileBinding  ]--------------------------------------------------
Binding::BindingPtr EventInstance_Projectile::projectileBinding() {
  return projectile_;
}




//----[  setBindingsUsed  ]----------------------------------------------------
void EventInstance_Projectile::setBindingsUsed(
    Binding::BindingPtrSet* bindings) {
  using namespace Evidyon::Binding;
  assert(bindings->size() < MAX_BINDINGS);

  size_t index = 0;
  for (BindingPtrSet::iterator i = bindings->begin(); i != bindings->end(); ++i) {
    bindings_[index++] = *i;
  }
}




//----[  update  ]-------------------------------------------------------------
bool EventInstance_Projectile::update(double time, double time_since_last_update) {
  x_ += dx_ * time_since_last_update;
  z_ += dz_ * time_since_last_update;
  projectile_->set(x_, 0.0f, z_, angle_);
  if (time < expiration_time_) return false;
  terminate(time, NULL); // end without special effect
  return true; // expired
}




//----[  terminate  ]----------------------------------------------------------
void EventInstance_Projectile::terminate(
    double time,
    SpecialFX::SpecialFXManager* special_fx_manager) {
  if (special_fx_manager) {
    projectile_->setExpired();
    projectile_.reset();
    Binding::BindingPtr binding(
      std::tr1::static_pointer_cast<Binding::Binding,
                                    Binding::LocationBinding>(projectile_));
    special_fx_manager->generate(
      data_->destruction_special_fx,
      time,
      1.0f, // TODO: use initial intensity?
      x_,
      z_,
      binding,
      x_,
      z_,
      &binding,
      1,
      data_->destruction_special_fx_duration,
      NULL,
      false);
  }
  for (int i = 0; i < MAX_BINDINGS; ++i) {
    Binding::Binding* binding = bindings_[i].get();
    bindings_[i].reset();
    if (binding == NULL) continue;
    binding->setExpired();
  }
}


}
}

