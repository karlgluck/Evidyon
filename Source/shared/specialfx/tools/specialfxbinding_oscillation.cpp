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
#include "specialfxbinding_oscillation.h"
#include "shared/specialfx/tools/specialfxcompiler.h"
#include "shared/specialfx/specialfxbinding_oscillation.h"
#include <d3dx9math.h>

namespace Evidyon {
namespace SpecialFX {
namespace Tools {

dc::dcClass<SpecialFXBinding_Oscillation>::Implements<SpecialFXBinding>
  SpecialFXBinding_Oscillation::implementsSpecialFXBinding;

//----[  SpecialFXBinding_Oscillation  ]---------------------------------------
SpecialFXBinding_Oscillation::SpecialFXBinding_Oscillation() {
  member("Input Binding", &input_binding_);
  member("Horizontal Amplitude", &amplitude_horizontal_);
  member("Horizontal Speed", &speed_horizontal_);
  member("Horizontal Phase Offset (degrees)", &phase_offset_horizontal_);
  member("Vertical Amplitude", &amplitude_vertical_);
  member("Vertical Speed", &speed_vertical_);
  member("Vertical Phase Offset (degrees)", &phase_offset_vertical_);
}



//----[  compile  ]------------------------------------------------------------
SpecialFXBindingIndex SpecialFXBinding_Oscillation::compile(
      SpecialFXCompiler* compiler) {
    Evidyon::SpecialFX::SpecialFXBinding_Oscillation binding;

  SpecialFXBinding* input_binding_object = input_binding_.getImplementation();
  Evidyon::SpecialFX::SpecialFXBindingIndex input_binding
    = input_binding_object
      ? input_binding_object->compile(compiler)
      : Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  if (input_binding == Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX) {
    return Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  }

  binding.input_binding = input_binding;
  binding.amplitude_horizontal = amplitude_horizontal_.getValue();
  binding.speed_horizontal = D3DXToRadian(speed_horizontal_.getValue());
  binding.phase_offset_horizontal = D3DXToRadian(phase_offset_horizontal_.getValue());
  binding.amplitude_vertical = amplitude_vertical_.getValue();
  binding.speed_vertical = D3DXToRadian(speed_vertical_.getValue());
  binding.phase_offset_vertical = D3DXToRadian(phase_offset_vertical_.getValue());

  return compiler->add(&binding);
}



}
}
}

