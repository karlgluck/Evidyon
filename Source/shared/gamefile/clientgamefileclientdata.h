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
#ifndef __COMPLETE_EDITOR_GAMEFILE_CLIENTGAMEFILECLIENTDATA_H__
#define __COMPLETE_EDITOR_GAMEFILE_CLIENTGAMEFILECLIENTDATA_H__
#pragma once


#include "shared/image/imageindex.h"
#include "shared/sound/soundindex.h"
#include "shared/specialfx/specialfxindex.h"
#include "shared/client/clientimage.h"
#include "shared/client/clientsound.h"
#include "shared/client/clientspecialfx.h"

namespace Evidyon {
namespace GameFile {



//----[  ClientGameFileClientData  ]-------------------------------------------
struct ClientGameFileClientData {
  Evidyon::Image::ImageIndex
    images[Evidyon::Client::NUMBER_OF_CLIENT_IMAGES];
  Evidyon::Sound::SoundIndex
    sounds[Evidyon::Client::NUMBER_OF_CLIENT_SOUNDS];
  Evidyon::SpecialFX::SpecialFXIndex
    special_fx[Evidyon::Client::NUMBER_OF_CLIENT_SPECIALFX];
  Evidyon::Sound::SoundIndex land_footstep_sound, water_footstep_sound;
};


}
}

#endif