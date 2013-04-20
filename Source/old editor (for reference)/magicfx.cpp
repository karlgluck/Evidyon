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
