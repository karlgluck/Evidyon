//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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

