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
#include "specialfxemitter_interval.h"
#include "shared/specialfx/tools/specialfxcompiler.h"
#include "shared/specialfx/specialfxemitter_interval.h"

namespace Evidyon {
namespace SpecialFX {
namespace Tools {

dc::dcClass<SpecialFXEmitter_Interval>::Implements<SpecialFXEmitter>
  SpecialFXEmitter_Interval::implementsSpecialFXEmitter;

//----[  SpecialFXEmitter_Interval  ]---------------------------------------
SpecialFXEmitter_Interval::SpecialFXEmitter_Interval() {
  member("SpecialFX", &special_fx_);
  member("Lifetime", &lifetime_);
  member("Emit Period", &emit_period_);
  member("Maximum Number to Emit", &maximum_number_to_emit_);
  member("Source Binding", &source_binding_);
  member("Target Binding", &target_binding_);
}



//----[  compile  ]------------------------------------------------------------
SpecialFXEmitterIndex SpecialFXEmitter_Interval::compile(
      SpecialFXCompiler* compiler) {
    Evidyon::SpecialFX::SpecialFXEmitter_Interval emitter;

  SpecialFXBinding* source_binding_object = source_binding_.getImplementation();
  Evidyon::SpecialFX::SpecialFXEmitterIndex source_binding
    = source_binding_object
      ? source_binding_object->compile(compiler)
      : Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  if (source_binding == Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX) {
    return Evidyon::SpecialFX::INVALID_SPECIALFX_EMITTER_INDEX;
  }

  SpecialFXBinding* target_binding_object = target_binding_.getImplementation();
  Evidyon::SpecialFX::SpecialFXEmitterIndex target_binding
    = target_binding_object
      ? target_binding_object->compile(compiler)
      : Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  if (target_binding == Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX) {
    return Evidyon::SpecialFX::INVALID_SPECIALFX_EMITTER_INDEX;
  }

  SpecialFX* special_fx_object = special_fx_.getImplementation();
  Evidyon::SpecialFX::SpecialFXEmitterIndex special_fx
    = special_fx_object
      ? special_fx_object->compile(Evidyon::SpecialFX::INVALID_SPECIALFX_INDEX,
                                   compiler)
      : Evidyon::SpecialFX::INVALID_SPECIALFX_INDEX;
  if (special_fx == Evidyon::SpecialFX::INVALID_SPECIALFX_INDEX) {
    return Evidyon::SpecialFX::INVALID_SPECIALFX_EMITTER_INDEX;
  }

  emitter.emitted_special_fx_index = special_fx;
  emitter.lifetime = lifetime_.getValue();
  emitter.emit_period = emit_period_.getValue();
  emitter.source_binding = source_binding;
  emitter.target_binding = target_binding;
  emitter.maximum_number_to_emit = maximum_number_to_emit_.getValue();

  return compiler->add(&emitter);
}



}
}
}


