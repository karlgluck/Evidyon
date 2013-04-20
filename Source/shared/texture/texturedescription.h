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
#ifndef __SHARED_TEXTURE_TEXTUREDESCRIPTION_H__
#define __SHARED_TEXTURE_TEXTUREDESCRIPTION_H__
#pragma once



#include <d3d9.h> // for LPDIRECT3DTEXTURE9

namespace Evidyon {
namespace Texture {

//----[  TextureDescription  ]-------------------------------------------------
struct TextureDescription {

  // The image (d3d texture) to associate with this texture.  Multiple
  // textures can reference the same resource.
  LPDIRECT3DTEXTURE9 d3d_texture;

  // How to blend this
  enum BlendType {
    DISABLE_BLENDING,
    ALPHATESTONLY,
    LIGHTEN,
    DARKEN,
    ALPHABLEND,
    NUMBER_OF_BLEND_TYPES,
    INVALID_BLEND_TYPE,
  } blend_type;

  // Transforms the texture coordinates across either the X or Y axis
  bool flip_horizontal, flip_vertical;

  // What kind of texture animation to create
  enum AnimationType {
    FRAME_SEQUENCE,
    SLIDING,
    CIRCLING,
    STATIC,
    NUMBER_OF_ANIMATION_TYPES,
  } animation_type;

  union Animation {
    struct {
      int frames_wide, frames_high;
      float speed;
    } frame_sequence;

    struct {
      float dx, dy;
    } sliding;

    struct {
      float speed;
      float radius;
    } circling;
  } animation;
};

}
}

#endif