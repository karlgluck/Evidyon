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
#ifndef __RENDERABLEANIMATEDMESH_H__
#define __RENDERABLEANIMATEDMESH_H__
#pragma once



#include <d3d9.h>
#include <d3dx9math.h>

namespace Evidyon {
namespace AnimatedMesh {


/*

Possible performance optimizations:
- instead of using 4 matrix influences for every bone, break into influence
  groups.  this is not done now because it adds yet another set of arrays and
  the complexity and added batchiness may not provide much of a performance
  boost.
- combine VBs and IBs for all of a texture group's subsets.  nothing would
  change but that they were all 1 buffer (well, indices would be offset...)
  then each subset would just have rendering parameters & matrices in it.  this
  doesn't reduce the complexity of the mesh at all, but it does reduces the
  overall number of VB/IB changes especially in situations where many meshes
  of the same type using different, sequential textures are being rendered:
  in the current method the geometry buffers would change 16 times for
  8 textures of 2 subsets; this method would have 1 geometry buffer switch

*/


//----[  RenderableAnimatedMesh  ]---------------------------------------------
// Created and stored internally by the animated mesh renderer.  Created from
// source AnimatedMeshData.
struct RenderableAnimatedMesh {

  struct Frame {
    D3DXMATRIXA16 offset;
    size_t parent_frame_index;
  };

  Frame* frames;
  size_t number_of_frames;

  struct Bone {
    size_t frame_index;
    D3DXMATRIXA16 inverse_offset;
  };

  Bone* bones;
  size_t number_of_bones;

  // At least 1 subset is created for each texture group.  If the number of
  // bones used by vertices in the texture group is greater than the current
  // hardware's maximum supported amount, multiple subsets are created such
  // that each can fit correctly in the renderer.
  struct TextureGroupSubset {

    // Geometry data to be rendered
    LPDIRECT3DVERTEXBUFFER9 vb;
    LPDIRECT3DINDEXBUFFER9 ib;

    // Rendering parameters
    unsigned int number_of_vertices;
    unsigned int number_of_triangles;

    // Indices of the source bones to use when rendering this data.  The
    // renderer sets world matrix 0 to be bone_matrices[bone_indices[0]],
    // world matrix 1 to be bone[bone_indices[1]], etc. up to the number of
    // bones in this subset.
    unsigned int* matrix_bone_indices;
    size_t number_of_matrices;
  };

  // Lists the subsets for this texture group
  struct TextureGroup {
    TextureGroupSubset* subsets;
    size_t number_of_subsets;
  };

  // Specifies, for each texture group, the list of subsets in that group.
  TextureGroup* texture_groups;
  size_t number_of_texture_groups;
};




}
}

#endif