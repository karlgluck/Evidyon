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
#ifndef __ANIMATEDMESHUTILS_H__
#define __ANIMATEDMESHUTILS_H__
#pragma once



#include <d3d9.h>

struct UnseenSkinnedMesh;
struct UnseenSkeletalAnimation;


namespace Evidyon {
namespace AnimatedMesh {
struct AnimatedMeshData;
struct RenderableAnimatedMesh;
struct AnimatedMeshAnimation;




//----[  AnimatedMeshUtil_InterpretUnseenSkinnedMesh  ]------------------------
// This method takes the (older) UnseenSkinnedMesh type and creates a new set
// of mesh data based on it.
bool AnimatedMeshUtil_InterpretUnseenSkinnedMesh(
    const UnseenSkinnedMesh* input_source_mesh,
    AnimatedMeshData* output_animated_mesh_data);



//----[  AnimatedMeshUtil_DeallocateAnimatedMeshData  ]------------------------
void AnimatedMeshUtil_DeallocateAnimatedMeshData(
  AnimatedMeshData* animated_mesh_data);


//----[  AnimatedMeshUtil_CreateRenderableAnimatedMesh  ]----------------------
// Translates source animated mesh data into an animated mesh that can be
// rendered with the given device.
bool AnimatedMeshUtil_CreateRenderableAnimatedMesh(
    LPDIRECT3DDEVICE9 d3d_device,
    const AnimatedMeshData* animated_mesh_data,
    RenderableAnimatedMesh* renderable_animated_mesh);

//----[  AnimatedMeshUtil_DeallocateRenderableAnimatedMesh  ]------------------
void AnimatedMeshUtil_DeallocateRenderableAnimatedMesh(
  RenderableAnimatedMesh* renderable_animated_mesh);



//----[  AnimatedMeshUtil_InterpretUnseenSkeletalAnimation  ]------------------
// Turns the deprecated UnseenSkeletalAnimation type into the usable
// AnimatedMeshAnimation.  In the future, the .X mesh compiler will be
// rewritten to generate this type directly.
bool AnimatedMeshUtil_InterpretUnseenSkeletalAnimation(
  const UnseenSkeletalAnimation* input_source_animation,
  AnimatedMeshAnimation* output_animation);

//----[  AnimatedMeshUtil_DeallocateRenderableAnimatedMesh  ]------------------
void AnimatedMeshUtil_DeallocateAnimatedMeshAnimation(
  AnimatedMeshAnimation* animation);



//----[  AnimatedMeshUtil_Serialize  ]-----------------------------------------
size_t AnimatedMeshUtil_Serialize(
  AnimatedMeshAnimation* animation_in,
  char* buffer,
  const char* buffer_end);


//----[  AnimatedMeshUtil_Deserialize  ]---------------------------------------
bool AnimatedMeshUtil_Deserialize(
  char* buffer,
  const char* buffer_end,
  AnimatedMeshAnimation* animation_out);




//----[  Vertex0Weights  ]-----------------------------------------------------
struct Vertex0Weights {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
};



//----[  Vertex2Weights  ]-----------------------------------------------------
struct Vertex2Weights {
  float x, y, z;
  float weights[1];
  DWORD matrix_indices;
  float nx, ny, nz;
  float u, v;
};



//----[  Vertex3Weights  ]-----------------------------------------------------
struct Vertex3Weights {
  float x, y, z;
  float weights[2];
  DWORD matrix_indices;
  float nx, ny, nz;
  float u, v;
};



//----[  Vertex4Weights  ]-----------------------------------------------------
struct Vertex4Weights {
  float x, y, z;
  float weights[3];
  DWORD matrix_indices;
  float nx, ny, nz;
  float u, v;
};




}
}

#endif