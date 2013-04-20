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
#include "image.h"
#include "common/safemacros.h"
#include "common/null.h"

namespace Evidyon {
namespace Image {
namespace Tools {



  
//----[  Image  ]--------------------------------------------------------------
Image::Image() {
  member("Is Terrain Texture", &terrain_texture_);
  member("Is Wall Texture", &wall_texture_);
  loaded_d3d_texture_ = NULL;
}




//----[  loadD3DTexture  ]-----------------------------------------------------
bool Image::loadD3DTexture(LPDIRECT3DDEVICE9 d3d_device, bool force_reload) {
  if (loaded_d3d_texture_ && !force_reload) return true;
  unloadD3DTexture();
  return obtainD3DTexture(d3d_device, &loaded_d3d_texture_);
}




//----[  unloadD3DTexture  ]---------------------------------------------------
void Image::unloadD3DTexture() {
  SAFE_RELEASE(loaded_d3d_texture_);
}




//----[  getD3DTexture  ]------------------------------------------------------
LPDIRECT3DTEXTURE9 Image::getD3DTexture() {
  return loaded_d3d_texture_;
}


//----[  compareTo  ]----------------------------------------------------------
int Image::compareTo(const dc::dcGenericResource* other) const {
  if (!other || getTypeID() != other->getTypeID()) {
     return dc::dcGenericResource::compareTo(other);
  } else {
    bool this_is_wall = wall_texture_.getValue();
    bool this_is_terrain = terrain_texture_.getValue() || this_is_wall;
    const Image* other_image = ((const Image*)other);
    bool other_is_wall = other_image->wall_texture_.getValue();
    bool other_is_terrain = other_is_wall || other_image->terrain_texture_.getValue();
    if (this_is_terrain && !other_is_terrain) return +1;
    if (!this_is_terrain && other_is_terrain) return -1;
    if (this_is_wall && !other_is_wall) return +1;
    if (!this_is_wall && other_is_wall) return -1;
    return 0;
  }
}



}
}
}