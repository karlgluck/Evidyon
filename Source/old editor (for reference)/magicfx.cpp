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
#include "magicfx.h"
#include "../shared/compiledspecialfx.h"


MagicFX::MagicFX() {
  member("Visual FX - 1", &visual_fx_[0]);
  member("Visual FX - 2", &visual_fx_[1]);
  member("Visual FX - 3", &visual_fx_[2]);
  member("Casting Sound", &casting_sound_);
}


void MagicFX::compileForClient(Evidyon::CompiledMagicFXParams* compiled) const {
  compiled->sound_fx_index = casting_sound_.getReferencedResourceIndex();
  for (int i = 0; i < Evidyon::CompiledMagicFXParams::MAX_VISUAL_FX; ++i) {
    const dc::dcTable<VisualFX>::Element* element = visual_fx_[i].getReferencedResource();
    compiled->visual_fx_indices[i] =
        element == NULL ? 0xFFFFFFFF : element->getIndex();
  }
}


std::string MagicFX::staticTypeName() {
  return "MagicFX";
}
