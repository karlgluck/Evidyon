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