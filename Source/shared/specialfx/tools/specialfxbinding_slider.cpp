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
#include "specialfxbinding_slider.h"
#include "shared/specialfx/specialfxbinding_slider.h"
#include "shared/specialfx/tools/specialfxcompiler.h"

namespace Evidyon {
namespace SpecialFX {
namespace Tools {

dc::dcClass<SpecialFXBinding_Slider>::Implements<SpecialFXBinding>
  SpecialFXBinding_Slider::implementsSpecialFXBinding;

//----[  SpecialFXBinding_Slider  ]---------------------------------------
SpecialFXBinding_Slider::SpecialFXBinding_Slider() {
  member("Source Binding", &source_binding_);
  member("Target Binding", &target_binding_);
  member("Arrival Time", &arrival_time_);
}



//----[  compile  ]------------------------------------------------------------
SpecialFXBindingIndex SpecialFXBinding_Slider::compile(
      SpecialFXCompiler* compiler) {
    Evidyon::SpecialFX::SpecialFXBinding_Slider binding;

  SpecialFXBinding* source_binding_object = source_binding_.getImplementation();
  Evidyon::SpecialFX::SpecialFXBindingIndex source_binding
    = source_binding_object
      ? source_binding_object->compile(compiler)
      : Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  if (source_binding == Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX) {
    return Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  }

  SpecialFXBinding* target_binding_object = target_binding_.getImplementation();
  Evidyon::SpecialFX::SpecialFXBindingIndex target_binding
    = target_binding_object
      ? target_binding_object->compile(compiler)
      : Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  if (target_binding == Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX) {
    return Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  }

  binding.source_binding = source_binding;
  binding.target_binding = target_binding;
  binding.increment = 1.0f / arrival_time_.getValue();

  return compiler->add(&binding);
}



}
}
}

