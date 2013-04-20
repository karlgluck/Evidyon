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
#include "specialfxcompiler.h"
#include "specialfxbinding.h"
#include "common/safemacros.h"
#include "shared/specialfx/specialfxbinding.h"
#include "shared/specialfx/specialfxbinding_offset.h"
#include "shared/specialfx/specialfxbinding_oscillation.h"
#include "shared/specialfx/specialfxbinding_orbit.h"
#include "shared/specialfx/specialfx.h"
#include "shared/specialfx/specialfx_soundandvisualfx.h"
#include "shared/specialfx/specialfx_soundand3visualfx.h"
#include "shared/specialfx/specialfxemitter.h"
#include "shared/specialfx/specialfxemitter_interval.h"
#include <assert.h>


namespace Evidyon {
namespace SpecialFX {
namespace Tools {



  
//----[  SpecialFXCompiler  ]--------------------------------------------------
SpecialFXCompiler::SpecialFXCompiler(size_t number_of_root_special_fx) {
  number_of_root_special_fx_ = number_of_root_special_fx;
  number_of_derived_special_fx_ = 0;
}


//----[  ~SpecialFXCompiler  ]-------------------------------------------------
SpecialFXCompiler::~SpecialFXCompiler() {
  clear();
}




//----[  add  ]----------------------------------------------------------------
SpecialFXBindingIndex SpecialFXCompiler::add(
    const Evidyon::SpecialFX::SpecialFXBinding* binding) {
  Evidyon::SpecialFX::SpecialFXBinding* internal_binding = new Evidyon::SpecialFX::SpecialFXBinding;
  memcpy(internal_binding, binding, sizeof(*internal_binding));
  bindings_.push_back(internal_binding);
  return bindings_.size() - 1;
}


  
//----[  add  ]----------------------------------------------------------------
SpecialFXBindingIndex SpecialFXCompiler::add(
    const Evidyon::SpecialFX::SpecialFXBinding_Offset* binding) {
  Evidyon::SpecialFX::SpecialFXBinding unioned_binding;
  unioned_binding.type = Evidyon::SpecialFX::SpecialFXBinding::OFFSET;
  memcpy(&unioned_binding.offset, binding, sizeof(*binding));
  return add(&unioned_binding);
}




//----[  add  ]----------------------------------------------------------------
SpecialFXBindingIndex SpecialFXCompiler::add(
    const Evidyon::SpecialFX::SpecialFXBinding_Orbit* binding) {
  Evidyon::SpecialFX::SpecialFXBinding unioned_binding;
  unioned_binding.type = Evidyon::SpecialFX::SpecialFXBinding::ORBIT;
  memcpy(&unioned_binding.offset, binding, sizeof(*binding));
  return add(&unioned_binding);
}




//----[  add  ]----------------------------------------------------------------
SpecialFXBindingIndex SpecialFXCompiler::add(
    const Evidyon::SpecialFX::SpecialFXBinding_Oscillation* binding) {
  Evidyon::SpecialFX::SpecialFXBinding unioned_binding;
  unioned_binding.type = Evidyon::SpecialFX::SpecialFXBinding::OSCILLATION;
  memcpy(&unioned_binding.offset, binding, sizeof(*binding));
  return add(&unioned_binding);
}




//----[  add  ]----------------------------------------------------------------
SpecialFXBindingIndex SpecialFXCompiler::add(
    const Evidyon::SpecialFX::SpecialFXBinding_Slider* binding) {
  Evidyon::SpecialFX::SpecialFXBinding unioned_binding;
  unioned_binding.type = Evidyon::SpecialFX::SpecialFXBinding::SLIDER;
  memcpy(&unioned_binding.slider, binding, sizeof(*binding));
  return add(&unioned_binding);
}





//----[  add  ]----------------------------------------------------------------
Evidyon::SpecialFX::SpecialFXIndex SpecialFXCompiler::add(
    Evidyon::SpecialFX::SpecialFXIndex index,
    const Evidyon::SpecialFX::SpecialFX* special_fx) {
  Evidyon::SpecialFX::SpecialFX* internal_special_fx = new Evidyon::SpecialFX::SpecialFX;
  memcpy(internal_special_fx, special_fx, sizeof(*special_fx));
  if (index == INVALID_SPECIALFX_INDEX) {
    index = number_of_root_special_fx_ + number_of_derived_special_fx_++;
  } else {
    assert(index < number_of_root_special_fx_);
  }
  SpecialFXMap::_Pairib 
    result = special_fx_.insert(SpecialFXMap::value_type(index,internal_special_fx));
  assert(result.second);
  return index;
}



//----[  add  ]----------------------------------------------------------------
Evidyon::SpecialFX::SpecialFXIndex SpecialFXCompiler::add(
    Evidyon::SpecialFX::SpecialFXIndex index,
    const Evidyon::SpecialFX::SpecialFX_SoundAndVisualFX* special_fx) {
  Evidyon::SpecialFX::SpecialFX unioned;
  unioned.type = Evidyon::SpecialFX::SpecialFX::SOUND_AND_VISUAL_FX;
  memcpy(&unioned.sound_and_visual_fx, special_fx, sizeof(*special_fx));
  return add(index, &unioned);
}



//----[  add  ]----------------------------------------------------------------
Evidyon::SpecialFX::SpecialFXIndex SpecialFXCompiler::add(
    Evidyon::SpecialFX::SpecialFXIndex index,
    const Evidyon::SpecialFX::SpecialFX_SoundAnd3VisualFX* special_fx) {
  Evidyon::SpecialFX::SpecialFX unioned;
  unioned.type = Evidyon::SpecialFX::SpecialFX::SOUND_AND_3VISUAL_FX;
  memcpy(&unioned.sound_and_3visual_fx, special_fx, sizeof(*special_fx));
  return add(index, &unioned);
}



//----[  add  ]----------------------------------------------------------------
Evidyon::SpecialFX::SpecialFXIndex SpecialFXCompiler::add(
    Evidyon::SpecialFX::SpecialFXIndex index,
    const Evidyon::SpecialFX::SpecialFX_SoundVisualFXAndEmitter* special_fx) {
  Evidyon::SpecialFX::SpecialFX unioned;
  unioned.type = Evidyon::SpecialFX::SpecialFX::SOUND_VISUAL_FX_AND_EMITTER;
  memcpy(&unioned.sound_visual_fx_and_emitter, special_fx, sizeof(*special_fx));
  return add(index, &unioned);
}






//----[  add  ]----------------------------------------------------------------
Evidyon::SpecialFX::SpecialFXEmitterIndex SpecialFXCompiler::add(
    const Evidyon::SpecialFX::SpecialFXEmitter* emitter) {
  Evidyon::SpecialFX::SpecialFXEmitter* internal_emitter = new Evidyon::SpecialFX::SpecialFXEmitter;
  memcpy(internal_emitter, emitter, sizeof(*emitter));
  emitters_.push_back(internal_emitter);
  return emitters_.size() - 1;
}



//----[  add  ]----------------------------------------------------------------
Evidyon::SpecialFX::SpecialFXEmitterIndex SpecialFXCompiler::add(
    const Evidyon::SpecialFX::SpecialFXEmitter_Interval* emitter) {
  Evidyon::SpecialFX::SpecialFXEmitter unioned;
  unioned.type = Evidyon::SpecialFX::SpecialFXEmitter::INTERVAL;
  memcpy(&unioned.interval, emitter, sizeof(*emitter));
  return add(&unioned);
}


//----[  clear  ]--------------------------------------------------------------
void SpecialFXCompiler::clear() {
  for (SpecialFXMap::iterator i = special_fx_.begin();
        i != special_fx_.end(); ++i) {
    Evidyon::SpecialFX::SpecialFX* special_fx = i->second;
    SAFE_DELETE(special_fx);
  }
  special_fx_.clear();
  for (SpecialFXBindingList::iterator i = bindings_.begin();
        i != bindings_.end(); ++i) {
    Evidyon::SpecialFX::SpecialFXBinding* binding = *i;
    SAFE_DELETE(binding);
  }
  bindings_.clear();
  for (SpecialFXEmitterList::iterator i = emitters_.begin();
        i != emitters_.end(); ++i) {
    Evidyon::SpecialFX::SpecialFXEmitter* emitter = *i;
    SAFE_DELETE(emitter);
  }
}







//----[  numberOfSpecialFX  ]--------------------------------------------------
size_t SpecialFXCompiler::numberOfSpecialFX() const {
  return special_fx_.size();
}





//----[  compileSpecialFX  ]---------------------------------------------------
void SpecialFXCompiler::compileSpecialFX(size_t index,
                                         Evidyon::SpecialFX::SpecialFX* special_fx) {
  memcpy(special_fx, special_fx_.find(index)->second, sizeof(*special_fx));
}




//----[  numberOfBindings  ]---------------------------------------------------
size_t SpecialFXCompiler::numberOfBindings() const {
  return bindings_.size();
}




//----[  compile  ]------------------------------------------------------------
void SpecialFXCompiler::compileBinding(size_t index,
                                       Evidyon::SpecialFX::SpecialFXBinding* binding) {
  memcpy(binding, bindings_.at(index), sizeof(*binding));
}




//----[  numberOfEmitters  ]---------------------------------------------------
size_t SpecialFXCompiler::numberOfEmitters() const {
  return emitters_.size();
}




//----[  compileEmitter  ]-----------------------------------------------------
void SpecialFXCompiler::compileEmitter(size_t index,
                                       Evidyon::SpecialFX::SpecialFXEmitter* emitter) {
  memcpy(emitter, emitters_.at(index), sizeof(*emitter));
}


}
}
}

