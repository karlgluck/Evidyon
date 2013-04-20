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
#include "specialfxbindinginstance.h"
#include "specialfxbindinginstance_offset.h"
#include "specialfxbindinginstance_orbit.h"
#include "specialfxbindinginstance_oscillation.h"
#include "specialfxbindinginstance_slider.h"
#include "specialfxbinding.h"
#include <assert.h>
#include "common/safemacros.h"

namespace Evidyon {
namespace SpecialFX {

  
//----[  New  ]----------------------------------------------------------------
Evidyon::Binding::BindingPtr SpecialFXBindingInstance::New(
    const SpecialFXBinding* data,
    SpecialFXBindingSource* source) {

  SpecialFXBindingInstance* instance = NULL;

  switch (data->type) {
  default:
    assert(false);
    {
      Evidyon::Binding::BindingPtr ptr(instance);
      return ptr;
    }
  case SpecialFXBinding::OFFSET: {
      instance = new SpecialFXBindingInstance_Offset(&data->offset);
    } break;
  case SpecialFXBinding::ORBIT: {
      instance = new SpecialFXBindingInstance_Orbit(&data->orbit);
    } break;
  case SpecialFXBinding::OSCILLATION: {
      instance
        = new SpecialFXBindingInstance_Oscillation(&data->oscillation);
    } break;
  case SpecialFXBinding::SLIDER: {
      instance = new SpecialFXBindingInstance_Slider(&data->slider);
    } break;
  }

  if (!instance->initialize(source)) {
    SAFE_DELETE(instance);
  }

  Evidyon::Binding::BindingPtr binding_ptr(instance);
  return binding_ptr;
}

SpecialFXBindingInstance::~SpecialFXBindingInstance() {
}


}
}

