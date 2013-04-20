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
#ifndef __ANIMATEDMESHDATA_H__
#define __ANIMATEDMESHDATA_H__
#pragma once



#include "animatedgeometryvertex.h"
#include "animatedgeometryindex.h"
#include <d3dx9math.h>            // for D3DXMATRIXA16


namespace Evidyon {
namespace AnimatedMesh {

//----[  AnimatedMeshData  ]---------------------------------------------------
// This is the source information from which the animated mesh renderer creates
// an animated mesh.  The renderer chops up the texture groups depending on
// the hardware's maximum number of total blend matrices (usually between 18
// and 35, or 255 in software mode).
struct AnimatedMeshData {

  struct Frame {
    D3DXMATRIXA16 offset;
    unsigned int parent_frame_index;
  };

  Frame* frames;
  size_t number_of_frames;

  struct Bone {
    unsigned int frame_index;
    D3DXMATRIXA16 inverse_offset;
  };

  Bone* bones;
  size_t number_of_bones;

  // A texture group contains all of the geometry in the mesh that should be
  // rendered with a single texture.  All vertices have the same number of
  // influences.  If a vertex does not have this number of influences, the
  // last influence matrix index is repeated with a 0.0f weight.
  struct TextureGroup {
    AnimatedGeometryVertex* vertices;
    size_t number_of_vertices;
    AnimatedGeometryIndex* indices;
    size_t number_of_indices;
  };

  TextureGroup* texture_groups;
  size_t number_of_texture_groups;
};





}
}

#endif