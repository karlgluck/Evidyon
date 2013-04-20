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
#include "specialfxemitterinstance_interval.h"
#include "specialfxemitter_interval.h"
#include "specialfxmanager.h"
#include <d3dx9math.h>
#include "specialfxbindingsource.h"
#include "shared/binding/binding.h"
#include "shared/binding/noupdatewrapperbinding.h"


namespace Evidyon {
namespace SpecialFX {



  
//----[  SpecialFXEmitterInstance_Interval  ]----------------------------------
SpecialFXEmitterInstance_Interval::SpecialFXEmitterInstance_Interval(
    double time,
    double generate_expiration_time,
    float intensity,
    const SpecialFXEmitter_Interval* data) {
  data_ = data;
  number_left_to_emit_ = data->maximum_number_to_emit;
  emitter_expiration_time_ = time + data->lifetime;
  generate_expiration_time_ = generate_expiration_time;
  next_emit_time_ = 0.0;
  intensity_ = intensity;
}



//----[  initialize  ]---------------------------------------------------------
bool SpecialFXEmitterInstance_Interval::initialize(SpecialFXBindingSource* source) {
  emitter_source_ = source->createBinding(data_->source_binding);
  emitter_target_ = source->createBinding(data_->target_binding);
  no_update_emitter_source_
    = Binding::NoUpdateWrapperBinding::New(emitter_source_);
  no_update_emitter_target_
    = Binding::NoUpdateWrapperBinding::New(emitter_target_);
  return emitter_source_.get() != NULL && emitter_target_.get() != NULL;
}


//----[  update  ]-------------------------------------------------------------
bool SpecialFXEmitterInstance_Interval::update(
    double time,
    double time_since_last_update,
    SpecialFXManager* special_fx_manager) {

  if (time > emitter_expiration_time_ || number_left_to_emit_ <= 0) {
    return false;
  }

  emitter_source_->update(time, time_since_last_update);
  emitter_target_->update(time, time_since_last_update);

  if (next_emit_time_ < time) {
    next_emit_time_ = time + data_->emit_period;
    D3DXVECTOR3 source(0.0f, 0.0f, 0.0f);
    D3DXVec3TransformCoord(&source, &source, emitter_source_->getTransform());
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVec3TransformCoord(&target, &target, emitter_target_->getTransform());
    special_fx_manager->generate(
      data_->emitted_special_fx_index,
      time,
      intensity_,
      source.x,
      source.z,
      no_update_emitter_source_,
      target.x,
      target.z,
      &no_update_emitter_target_,
      1,
      generate_expiration_time_,
      NULL,
      false);
    return (--number_left_to_emit_) > 0;
  }

  return true;
}


}
}


