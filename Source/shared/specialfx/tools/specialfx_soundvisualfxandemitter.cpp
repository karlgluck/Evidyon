//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008 - 2013 Karl Gluck                                       //
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
#include "specialfx_soundvisualfxandemitter.h"
#include "shared/specialfx/specialfx_soundvisualfxandemitter.h"
#include "shared/specialfx/tools/specialfxcompiler.h"


namespace Evidyon {
namespace SpecialFX {
namespace Tools {


dc::dcClass<SpecialFX_SoundVisualFXAndEmitter>::Implements<SpecialFX>
  SpecialFX_SoundVisualFXAndEmitter::implementsSpecialFX;

//----[  SpecialFX_SoundVisualFXAndEmitter  ]-----------------------------------------
SpecialFX_SoundVisualFXAndEmitter::SpecialFX_SoundVisualFXAndEmitter() {
  member("Sound", &sound_);
  member("VisualFX", &visual_fx_);
  member("VisualFX Source Binding", &visual_fx_source_binding_);
  member("VisualFX Target Binding", &visual_fx_target_binding_);
  member("Emitter", &emitter_);
}



//----[  compile  ]------------------------------------------------------------
SpecialFXIndex SpecialFX_SoundVisualFXAndEmitter::compile(
    SpecialFXIndex index,
    SpecialFXCompiler* compiler) {

  Evidyon::SpecialFX::SpecialFX_SoundVisualFXAndEmitter description;
  description.sound
    = sound_.getReferencedResourceIndex();
  description.visual_fx
    = visual_fx_.getReferencedResourceIndex();


  SpecialFXBinding* source_binding
    = visual_fx_source_binding_.getImplementation();
  SpecialFXBinding* target_binding
    = visual_fx_target_binding_.getImplementation();

  description.visual_fx_source
    = source_binding ? source_binding->compile(compiler)
                     : SPECIALFX_BINDING_SOURCE_OBJECT;
  description.visual_fx_target
    = target_binding ? target_binding->compile(compiler)
                     : SPECIALFX_BINDING_TARGET_OBJECT;

  SpecialFXEmitter* emitter = emitter_.getImplementation();
  description.emitter
    = emitter ? emitter->compile(compiler)
              : INVALID_SPECIALFX_EMITTER_INDEX;

  return compiler->add(index, &description);
}


}
}
}

