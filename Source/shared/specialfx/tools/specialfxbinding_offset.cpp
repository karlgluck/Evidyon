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
#include "specialfxbinding_offset.h"
#include "shared/specialfx/specialfxbinding_offset.h"
#include "shared/specialfx/tools/specialfxcompiler.h"

namespace Evidyon {
namespace SpecialFX {
namespace Tools {

dc::dcClass<SpecialFXBinding_Offset>::Implements<SpecialFXBinding>
  SpecialFXBinding_Offset::implementsSpecialFXBinding;



//----[  SpecialFXBinding_Offset  ]--------------------------------------------
SpecialFXBinding_Offset::SpecialFXBinding_Offset() {
  member("Input Binding", &input_binding_);
  member("Offset", &offset_);
}



//----[  compile  ]------------------------------------------------------------
SpecialFXBindingIndex SpecialFXBinding_Offset::compile(
    SpecialFXCompiler* compiler) {
  Evidyon::SpecialFX::SpecialFXBinding_Offset binding;

  SpecialFXBinding* input_binding_object = input_binding_.getImplementation();
  Evidyon::SpecialFX::SpecialFXBindingIndex input_binding
    = input_binding_object
      ? input_binding_object->compile(compiler)
      : Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  if (input_binding == Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX) {
    return Evidyon::SpecialFX::INVALID_SPECIALFX_BINDING_INDEX;
  }

  binding.input_binding = input_binding;
  offset_.getTransform((D3DXMATRIX*)&binding.offset);

  return compiler->add(&binding);
}



}
}
}

