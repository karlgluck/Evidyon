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
#ifndef __SHARED_SPECIALFX_SPECIALFXMANAGER_H__
#define __SHARED_SPECIALFX_SPECIALFXMANAGER_H__
#pragma once


#include "shared/binding/bindingptr.h"
#include "shared/binding/bindingptrset.h"
#include "shared/specialfx/specialfxindex.h"
#include "shared/specialfx/specialfxemitterindex.h"
#include <list>

namespace Evidyon {
namespace VisualFX {
class VisualFXRenderer;
}
namespace Sound {
class SoundManager;
}
namespace SpecialFX {
struct SpecialFX;
struct SpecialFXBinding;
struct SpecialFXEmitter;
class SpecialFXBindingSource;
class SpecialFXEmitterInstance;
}
}

namespace Evidyon {
namespace SpecialFX {

//----[  SpecialFXManager  ]---------------------------------------------------
class SpecialFXManager {
  typedef std::list<Binding::BindingPtr> BindingPtrList;
  typedef std::list<SpecialFXEmitterInstance*> EmitterList;

public:
  SpecialFXManager();
  ~SpecialFXManager();

  void create(VisualFX::VisualFXRenderer* visual_fx_renderer,
              Sound::SoundManager* sound_manager);
  void destroy();

  // Fills the manager with special fx types
  bool initNumberOfSpecialFX(size_t count);
  void initSpecialFX(size_t index, const SpecialFX* special_fx);
  void clearSpecialFX();

  // Fills the manager with binding types
  bool initNumberOfBindings(size_t count);
  void initBinding(size_t index, const SpecialFXBinding* binding);
  void clearBindings();

  // Fills the manager with emitter types
  bool initNumberOfEmitters(size_t count);
  void initEmitter(size_t index, const SpecialFXEmitter* emitter);
  void clearEmitters();

  // Instantiates a new special effect.  The effect is controlled entirely
  // through timers and bindings.
  // If the expiration time is less than 0, it is ignored.  A positive
  // expiration time ensures that all effects will expire after 
  // update() is called with a 'time' parameter greater than that value.
  // 'bindings_used' returns all bindings that the manager attached to a
  // special fx object.  Setting 'bindings_used' to NULL is valid.
  void generate(SpecialFXIndex special_fx,
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
                bool involves_client); // if true, sound is highest priority

  // Should be called before the visual FX are updated, but after the actors,
  // scenery and map are updated.
  void update(double time, double time_since_last_update);

  // Removes all effects and generators that are based on internal timers
  // by running a series of updates far in the future, causing them all to
  // expire and be removed.
  void eraseTimedFX();


private:

  // Gets the special fx at the given index.  If the index is invalid,
  // this method returns NULL.
  const SpecialFX* specialFX(SpecialFXIndex special_fx);

  // Gets the emitter at the given index.  If the index is invalid,
  // this method returns NULL.
  const SpecialFXEmitter* emitter(SpecialFXEmitterIndex index);

  // Reset all class members
  void zero();

  // Generates a special effect.  Some special effects may cause this
  // to be called recursively.
  // play_sound is only set for the first target of an AOE, so that
  // a multitarget spell doesn't get its sound multiplied.
  void acquire(SpecialFXBindingSource* binding_source,
               SpecialFXIndex special_fx,
               float intensity,
               double time,
               double expiration,
               bool play_sound,
               bool involves_client);

private:

  // Manager objects used to create effects
  VisualFX::VisualFXRenderer* visual_fx_renderer_;
  Sound::SoundManager* sound_manager_;

  // Different types of effects that can be generated
  SpecialFX* special_fx_;
  size_t number_of_special_fx_;

  // Contains the ways the special FX can be bound
  SpecialFXBinding* bindings_;
  size_t number_of_bindings_;

  // Contains the types of emitters
  SpecialFXEmitter* emitters_;
  size_t number_of_emitters_;

  // Expired bindings are removed from this list during update()
  BindingPtrList binding_instances_;

  // Expired emitters are removed from this list during update()
  EmitterList emitter_instances_;
};

}
}

#endif