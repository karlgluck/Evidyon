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
#ifndef __SKINNEDMESHINSTANCE_H__
#define __SKINNEDMESHINSTANCE_H__
#pragma once



#include <d3dx9math.h>
#include "skinnedmeshattachmentpoint.h"

namespace Evidyon {
namespace SkinnedMesh {
struct SkinnedMesh;
struct SkinnedMeshForm;
struct SkinnedMeshAnimation;



//----[  SkinnedMeshInstance  ]------------------------------------------------
struct SkinnedMeshInstance {
  struct Animation {
    double start_time;  // animation-local reference time
    const SkinnedMeshAnimation* animation;
    float weight;
    float speed;        // calculated value
  };

  // Whether or not to draw this mesh instance
  bool hidden;

  // Source data on which this instance is based
  const SkinnedMesh* skinned_mesh;
  const SkinnedMeshForm* skinned_mesh_form;

  // Animations that are currently being applied to this mesh instance.  The
  // base animations are blended smoothly from the primary to the secondary.
  // Action animations trump base animations completely for the bones that
  // they modify.
  Animation primary_base_animation;
  Animation secondary_base_animation;
  Animation action_animation;

  // All the bones in the current mesh state.  This array is allocated and
  // managed by the renderer.
  D3DXMATRIXA16* bone_matrices;

  // These matrices are recalculated automatically when the animation state
  // is updated.
  D3DXMATRIXA16 attachment_point_matrices[NUMBER_OF_ATTACHMENT_POINTS];
};



}
}

#endif