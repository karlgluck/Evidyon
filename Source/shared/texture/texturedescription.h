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