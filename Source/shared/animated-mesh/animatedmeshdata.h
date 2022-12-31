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