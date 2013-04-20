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
#ifndef __SHARED_SPECIALFX_SPECIALFX_H__
#define __SHARED_SPECIALFX_SPECIALFX_H__
#pragma once


#include "specialfx_soundandvisualfx.h"
#include "specialfx_soundand3visualfx.h"
#include "specialfx_soundvisualfxandemitter.h"


namespace Evidyon {
namespace SpecialFX {

//----[  SpecialFX  ]----------------------------------------------------------
struct SpecialFX {
  enum Type {
    SOUND_AND_VISUAL_FX,
    SOUND_AND_3VISUAL_FX,
    SOUND_VISUAL_FX_AND_EMITTER,
  } type;
  union {
    SpecialFX_SoundAndVisualFX sound_and_visual_fx;
    SpecialFX_SoundAnd3VisualFX sound_and_3visual_fx;
    SpecialFX_SoundVisualFXAndEmitter sound_visual_fx_and_emitter;
  };
};

}
}

#endif