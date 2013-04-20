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
#include "specialfxmanager.h"
#include "shared/specialfx/specialfx.h"
#include "shared/specialfx/specialfxbinding.h"
#include "shared/specialfx/specialfxemitter.h"
#include "shared/visualfx/visualfxrenderer.h"
#include "shared/sound/soundmanager.h"
#include "shared/specialfx/specialfxbindingsource.h"
#include "common/safemacros.h"
#include "shared/specialfx/specialfxemitterinstance.h"


namespace Evidyon {
namespace SpecialFX {




  
//----[  SpecialFXManager  ]---------------------------------------------------
SpecialFXManager::SpecialFXManager() {
  zero();
}




//----[  ~SpecialFXManager  ]--------------------------------------------------
SpecialFXManager::~SpecialFXManager() {
  destroy();
}




//----[  create  ]-------------------------------------------------------------
void SpecialFXManager::create(VisualFX::VisualFXRenderer* visual_fx_renderer,
                              Sound::SoundManager* sound_manager) {
  visual_fx_renderer_ = visual_fx_renderer;
  sound_manager_ = sound_manager;
}




//----[  destroy  ]------------------------------------------------------------
void SpecialFXManager::destroy() {
  clearSpecialFX();
  clearBindings();
  clearEmitters();
  zero();
}




//----[  initNumberOfSpecialFX  ]----------------------------------------------
bool SpecialFXManager::initNumberOfSpecialFX(size_t count) {
  special_fx_ = new SpecialFX[count];
  assert(special_fx_);
  number_of_special_fx_ = count;
  return special_fx_ != NULL;
}




//----[  initSpecialFX  ]------------------------------------------------------
void SpecialFXManager::initSpecialFX(size_t index, const SpecialFX* special_fx) {
  assert(index < number_of_special_fx_);
  memcpy(&special_fx_[index], special_fx, sizeof(*special_fx));
}




//----[  clearSpecialFX  ]-----------------------------------------------------
void SpecialFXManager::clearSpecialFX() {
  SAFE_DELETE_ARRAY(special_fx_);
  number_of_special_fx_ = 0;
}




//----[  initNumberOfBindings  ]-----------------------------------------------
bool SpecialFXManager::initNumberOfBindings(size_t count) {
  bindings_ = new SpecialFXBinding[count];
  assert(bindings_);
  number_of_bindings_ = count;
  return bindings_ != NULL;

}




//----[  initBinding  ]--------------------------------------------------------
void SpecialFXManager::initBinding(size_t index, const SpecialFXBinding* binding) {
  assert(index < number_of_bindings_);
  memcpy(&bindings_[index], binding, sizeof(*binding));
}




//----[  clearBindings  ]------------------------------------------------------
void SpecialFXManager::clearBindings() {
  SAFE_DELETE_ARRAY(bindings_);
  number_of_bindings_ = 0;
}



//----[  initNumberOfEmitters  ]-----------------------------------------------
bool SpecialFXManager::initNumberOfEmitters(size_t count) {
  emitters_ = new SpecialFXEmitter[count];
  assert(emitters_);
  number_of_emitters_ = count;
  return emitters_ != NULL;

}




//----[  initEmitter  ]--------------------------------------------------------
void SpecialFXManager::initEmitter(size_t index, const SpecialFXEmitter* emitter) {
  assert(index < number_of_emitters_);
  memcpy(&emitters_[index], emitter, sizeof(*emitter));
}




//----[  clearEmitters  ]------------------------------------------------------
void SpecialFXManager::clearEmitters() {
  SAFE_DELETE_ARRAY(emitters_);
  number_of_emitters_ = 0;
}






//----[  generate  ]-----------------------------------------------------------
void SpecialFXManager::generate(SpecialFXIndex special_fx,
                                double time,
                                float intensity,
                                float source_x,
                                float source_z,
                                Binding::BindingPtr source,
                                float target_x,
                                float target_z,
                                Binding::BindingPtr* targets,
                                size_t number_of_targets,
                                double expiration,
                                Binding::BindingPtrSet* bindings_used,
                                bool involves_client) {
  assert(visual_fx_renderer_);
  if (special_fx == INVALID_SPECIALFX_INDEX) return;

  SpecialFXBindingSource binding_source(bindings_, number_of_bindings_);
  binding_source.set(
    expiration < 0.0 ? -1.0 : (time + expiration),
    source_x,
    source_z,
    source,
    target_x,
    target_z,
    bindings_used);

  number_of_targets = number_of_targets > 1 ? number_of_targets : 1;

  if (source.get()) source->update(time, 0.0);

  for (size_t i = 0; i < number_of_targets; ++i) {
    if (targets)  {
      binding_source.setTargetObject(targets[i]);
      if (targets[i].get()) targets[i]->update(time, 0.0);
    }
    acquire(&binding_source,
            special_fx,
            intensity,
            time,
            expiration,
            i == 0,
            involves_client);    // only enable sound for the first target
  }
}






//----[  update  ]-------------------------------------------------------------
void SpecialFXManager::update(double time, double time_since_last_update) {
  for (BindingPtrList::iterator i = binding_instances_.begin();
                                i != binding_instances_.end();
                            /*++i in loop*/) {
    Binding::BindingPtr binding = *i;
    binding->update(time, time_since_last_update);
    if (binding->hasExpired()) {
      binding.reset();
      i->reset();
      BindingPtrList::iterator next = i; ++next;
      binding_instances_.erase(i);
      i = next;
    } else {
      ++i;
    }
  }
  for (EmitterList::iterator i = emitter_instances_.begin();
                             i != emitter_instances_.end();
                            /*++i in loop*/) {
    SpecialFXEmitterInstance* emitter = *i;
    if (!emitter->update(time, time_since_last_update, this)) {
      SAFE_DELETE(emitter);
      EmitterList::iterator next = i; ++next;
      emitter_instances_.erase(i);
      i = next;
    } else {
      ++i;
    }
  }
}



//----[  eraseTimedFX  ]-------------------------------------------------------
void SpecialFXManager::eraseTimedFX() {
  update(3.402823466e+38F, 0.0f);
  update(3.402823466e+38F, 0.0f);
}





//----[  specialFX  ]----------------------------------------------------------
const SpecialFX* SpecialFXManager::specialFX(SpecialFXIndex special_fx) {
  assert(special_fx < number_of_special_fx_);
  if (special_fx >= number_of_special_fx_) return NULL;
  return &special_fx_[special_fx];
}



//----[  emitter  ]------------------------------------------------------------
const SpecialFXEmitter* SpecialFXManager::emitter(
    SpecialFXEmitterIndex index) {
  assert(index < number_of_emitters_);
  if (index >= number_of_emitters_) return NULL;
  return &emitters_[index];
}


//----[  zero  ]---------------------------------------------------------------
void SpecialFXManager::zero() {
  visual_fx_renderer_ = NULL;
  sound_manager_ = NULL;
  special_fx_ = NULL;
  number_of_special_fx_ = 0;
  bindings_ = NULL;
  number_of_bindings_ = 0;
  emitters_ = NULL;
  number_of_emitters_ = 0;
}



//----[  acquire  ]------------------------------------------------------------
void SpecialFXManager::acquire(SpecialFXBindingSource* binding_source,
                               SpecialFXIndex special_fx,
                               float intensity,
                               double time,
                               double expiration,
                               bool play_sound,
                               bool involves_client) {

  const SpecialFX* special_fx_data = specialFX(special_fx);
  if (!special_fx_data) return;

  switch (special_fx_data->type) {
    case SpecialFX::SOUND_AND_VISUAL_FX: {
      const SpecialFX_SoundAndVisualFX* params
        = &special_fx_data->sound_and_visual_fx;
      if (play_sound) {
        sound_manager_->playSound(involves_client ? Sound::SOUNDPRIORITY_SELF
                                                  : Sound::SOUNDPRIORITY_NEARBY,
                                  binding_source->getSourceX(),
                                  binding_source->getSourceZ(), 
                                  params->sound);
      }
      Binding::BindingPtr source_binding(
        binding_source->createBinding(params->visual_fx_source));
      Binding::BindingPtr target_binding(
        binding_source->createBinding(params->visual_fx_target));
      visual_fx_renderer_->acquireVisualFX(
        params->visual_fx,
        intensity,
        source_binding,
        target_binding);
      binding_instances_.push_back(source_binding);
      binding_instances_.push_back(target_binding);
    } break;

    case SpecialFX::SOUND_AND_3VISUAL_FX: {
      const SpecialFX_SoundAnd3VisualFX* params
        = &special_fx_data->sound_and_3visual_fx;
      if (play_sound) {
        sound_manager_->playSound(involves_client ? Sound::SOUNDPRIORITY_SELF
                                                  : Sound::SOUNDPRIORITY_NEARBY,
                                  binding_source->getSourceX(),
                                  binding_source->getSourceZ(),
                                  params->sound);
      }
      Binding::BindingPtr source_binding(
        binding_source->createBinding(params->visual_fx_source));
      Binding::BindingPtr target_binding(
        binding_source->createBinding(params->visual_fx_target));
      for (int i = 0; i < 3; ++i) {
        if (params->visual_fx[i] ==
            VisualFX::INVALID_VISUALFX_INDEX) {
          continue;
        }
        visual_fx_renderer_->acquireVisualFX(
          params->visual_fx[i],
          intensity,
          source_binding,
          target_binding);
      }
      binding_instances_.push_back(source_binding);
      binding_instances_.push_back(target_binding);
    } break;

    case SpecialFX::SOUND_VISUAL_FX_AND_EMITTER: {
      const SpecialFX_SoundVisualFXAndEmitter* params
        = &special_fx_data->sound_visual_fx_and_emitter;
      if (play_sound) {
        sound_manager_->playSound(involves_client ? Sound::SOUNDPRIORITY_SELF
                                                  : Sound::SOUNDPRIORITY_NEARBY,
                                  binding_source->getSourceX(),
                                  binding_source->getSourceZ(),
                                  params->sound);
      }
      Binding::BindingPtr source_binding(
        binding_source->createBinding(params->visual_fx_source));
      Binding::BindingPtr target_binding(
        binding_source->createBinding(params->visual_fx_target));
      visual_fx_renderer_->acquireVisualFX(
        params->visual_fx,
        intensity,
        source_binding,
        target_binding);
      binding_instances_.push_back(source_binding);
      binding_instances_.push_back(target_binding);
      const SpecialFXEmitter* emitter_data
        = emitter(params->emitter);
      if (emitter_data) {
        SpecialFXEmitterInstance* emitter
          = SpecialFXEmitterInstance::New(
              time,
              expiration,
              intensity,
              emitter_data,
              binding_source);
        if (emitter) {
          emitter_instances_.push_back(emitter);
        }
      }
    } break;
  }
}


}
}

