//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#ifndef __COMPLETE_EDITOR_GAMEFILE_CLIENTGAMEFILETEXTUREDATA_H__
#define __COMPLETE_EDITOR_GAMEFILE_CLIENTGAMEFILETEXTUREDATA_H__
#pragma once


#include "shared/texture/texturedescription.h"

namespace Evidyon {
namespace GameFile {


#pragma pack(push, 1)
//----[  ClientGameFileTextureData  ]------------------------------------------
struct ClientGameFileTextureData {
  static const int MAXIMUM_BLEND_TYPES = 1<<4;
  static const int MAXIMUM_ANIMATION_TYPES = 1<<2;
  unsigned image_index : 16;
  unsigned blend_type : 4;
  unsigned flip_horizontal : 1;
  unsigned flip_vertical : 1;
  unsigned animation_type : 2;
  Evidyon::Texture::TextureDescription::Animation animation;
};
#pragma pack(pop)

}
}


#endif