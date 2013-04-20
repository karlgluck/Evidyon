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
#ifndef __SHARED_SPECIALFX_SPECIALFXBINDINGCOMPILER_H__
#define __SHARED_SPECIALFX_SPECIALFXBINDINGCOMPILER_H__
#pragma once

#include <map>
#include <vector>
#include "shared/specialfx/specialfxindex.h"
#include "shared/specialfx/specialfxbindingindex.h"
#include "shared/specialfx/specialfxemitterindex.h"

namespace Evidyon {
namespace SpecialFX {
struct SpecialFXBinding;
struct SpecialFXBinding_Offset;
struct SpecialFXBinding_Orbit;
struct SpecialFXBinding_Oscillation;
struct SpecialFXBinding_Slider;
struct SpecialFX;
struct SpecialFX_SoundAnd3VisualFX;
struct SpecialFX_SoundAndVisualFX;
struct SpecialFX_SoundVisualFXAndEmitter;
struct SpecialFXEmitter;
struct SpecialFXEmitter_Interval;
}
}

namespace Evidyon {
namespace SpecialFX {
namespace Tools {


//----[  SpecialFXCompiler  ]-------------------------------------------
class SpecialFXCompiler {
  // SpecialFX must be listed in a map because they are an ordered
  // resource, but the compiler allows dynamic entries to be allocated.
  // These entries are added to the end of the list with a unique key that
  // is 
  typedef std::map<size_t, Evidyon::SpecialFX::SpecialFX*> SpecialFXMap;
  typedef std::vector<Evidyon::SpecialFX::SpecialFXBinding*> SpecialFXBindingList;
  typedef std::vector<Evidyon::SpecialFX::SpecialFXEmitter*> SpecialFXEmitterList;
public:
  SpecialFXCompiler(size_t number_of_root_special_fx);
  ~SpecialFXCompiler();
  void clear();
  SpecialFXBindingIndex add(const Evidyon::SpecialFX::SpecialFXBinding* binding);
  SpecialFXBindingIndex add(const Evidyon::SpecialFX::SpecialFXBinding_Offset* binding);
  SpecialFXBindingIndex add(const Evidyon::SpecialFX::SpecialFXBinding_Orbit* binding);
  SpecialFXBindingIndex add(const Evidyon::SpecialFX::SpecialFXBinding_Oscillation* binding);
  SpecialFXBindingIndex add(const Evidyon::SpecialFX::SpecialFXBinding_Slider* binding);
  SpecialFXIndex add(SpecialFXIndex target_index, const Evidyon::SpecialFX::SpecialFX* special_fx);
  SpecialFXIndex add(SpecialFXIndex target_index, const Evidyon::SpecialFX::SpecialFX_SoundAndVisualFX* special_fx);
  SpecialFXIndex add(SpecialFXIndex target_index, const Evidyon::SpecialFX::SpecialFX_SoundAnd3VisualFX* special_fx);
  SpecialFXIndex add(SpecialFXIndex target_index, const Evidyon::SpecialFX::SpecialFX_SoundVisualFXAndEmitter* special_fx);
  SpecialFXEmitterIndex add(const Evidyon::SpecialFX::SpecialFXEmitter* emitter);
  SpecialFXEmitterIndex add(const Evidyon::SpecialFX::SpecialFXEmitter_Interval* emitter);
  size_t numberOfSpecialFX() const;
  void compileSpecialFX(size_t index, Evidyon::SpecialFX::SpecialFX* special_fx);
  size_t numberOfBindings() const;
  void compileBinding(size_t index, SpecialFXBinding* binding);
  size_t numberOfEmitters() const;
  void compileEmitter(size_t index, SpecialFXEmitter* emitter);

private:
  size_t number_of_root_special_fx_;
  size_t number_of_derived_special_fx_; // incremented to create unique keys
  SpecialFXMap special_fx_;
  SpecialFXBindingList bindings_;
  SpecialFXEmitterList emitters_;
};


}
}
}

#endif