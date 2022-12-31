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
#ifndef __SHARED_SPECIALFX_TOOLS_SPECIALFX_SOUNDAND3VISUALFX_H__
#define __SHARED_SPECIALFX_TOOLS_SPECIALFX_SOUNDAND3VISUALFX_H__
#pragma once

#include "specialfx.h"
#include "shared/sound/tools/wavsound.h"
#include "shared/visualfx/tools/visualfx.h"
#include "specialfxbinding.h"



namespace Evidyon {
namespace SpecialFX {
namespace Tools {

//----[  SpecialFX_SoundAnd3VisualFX  ]----------------------------------------
class SpecialFX_SoundAnd3VisualFX : public dc::dcResource<SpecialFX_SoundAnd3VisualFX, SpecialFX> {
  static dc::dcClass<SpecialFX_SoundAnd3VisualFX>::Implements<SpecialFX> implementsSpecialFX;
public:
  static std::string staticTypeName() { return "SpecialFX_SoundAnd3VisualFX"; }
public:
  SpecialFX_SoundAnd3VisualFX();
  virtual SpecialFXIndex compile(SpecialFXIndex defined_index,
                                 SpecialFXCompiler* compiler);
private:
  Evidyon::Tools::SoundReference sound_;
  Evidyon::Tools::VisualFXReference visual_fx_[3];
  SpecialFXBindingInterface source_binding_;
  SpecialFXBindingInterface target_binding_;
};

}
}
}

#endif