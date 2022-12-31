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
#ifndef __LOCATIONVISUALDATA_H__
#define __LOCATIONVISUALDATA_H__
#pragma once



#include "shared/texture/textureindex.h"
#include "shared/scenery/sceneryindex.h"
#include "shared/world/walltype.h"
#include "shared/skinned-mesh/skinnedmeshindex.h"


namespace Evidyon {
namespace World {

  
//----[  LocationVisualData  ]-------------------------------------------------
// the default location values are set in map.cpp
struct LocationVisualData {
  float terrain_height;
  Texture::TextureIndex terrain_texture;
  int terrain_texture_rotation;

  Texture::TextureIndex fluid_texture;

  Texture::TextureIndex wall_texture;
  WallType wall_type;
  bool occluding; // Whether or not this tile hides the ones behind it

  // The kind of scenery to render here
  Scenery::SceneryIndex scenery;

  // Which skinned mesh to render.  Form and animation 0 are always used.
  SkinnedMesh::SkinnedMeshIndex skinned_mesh;
  //SkinnedMesh::SkinnedMeshForm form;
  //SkinnedMesh::SkinnedMeshAnimationIndex animation;
};


}
}

#endif