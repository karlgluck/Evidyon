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
#ifndef __SHARED_SPECIALFX_SPECIALFXEMITTERINSTANCE_INTERVAL_H__
#define __SHARED_SPECIALFX_SPECIALFXEMITTERINSTANCE_INTERVAL_H__
#pragma once


#include "specialfxemitterinstance.h"
#include "shared/binding/bindingptr.h"

namespace Evidyon {
namespace SpecialFX {
struct SpecialFXEmitter_Interval;
}
}

namespace Evidyon {
namespace SpecialFX {

//----[  SpecialFXEmitterInstance_Interval  ]----------------------------------
class SpecialFXEmitterInstance_Interval : public SpecialFXEmitterInstance {
public:
  SpecialFXEmitterInstance_Interval(double time,
                                    double generate_expiration_time,
                                    float intensity,
                                    const SpecialFXEmitter_Interval* data);
  virtual bool initialize(SpecialFXBindingSource* source);
  virtual bool update(double time,
                      double time_since_last_update,
                      SpecialFXManager* special_fx_manager);

private:

  // Source configuration
  const SpecialFXEmitter_Interval* data_;

  // The time at which the emitter automatically shuts off.  Ignored if < 0
  double emitter_expiration_time_;

  // all emitted effects must expire at this time
  double generate_expiration_time_;

  // Next time at which to emit an effect, based on the period of the
  // source data.
  double next_emit_time_;

  // Source intensity parameter from the original generate command used to
  // derive the sub-effects.
  float intensity_;

  // Decremented each period; triggers expiration if <= 0
  int number_left_to_emit_;

  // These bindings are updated by update()
  Binding::BindingPtr emitter_source_;
  Binding::BindingPtr emitter_target_;
  // These bindings wrap the source and target, but don't propagate the update()
  // calls.  These provide both objects and locations to the generated effect
  // (locations are the result of transforming the origin and taking the x/z
  // components)
  Binding::BindingPtr no_update_emitter_source_;
  Binding::BindingPtr no_update_emitter_target_;
};

}
}

#endif