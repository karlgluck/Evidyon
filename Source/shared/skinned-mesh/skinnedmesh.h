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
#ifndef __SKINNEDMESH_H__
#define __SKINNEDMESH_H__
#pragma once



#include "skinnedmeshattachmentpoint.h" // for NUMBER_OF_ATTACHMENT_POINTS
#include "skinnedmeshattachmentpointoffset.h"


namespace Evidyon {
namespace SkinnedMesh {
struct SkinnedMeshForm;
struct SkinnedMeshAnimation;


//----[  SkinnedMesh  ]--------------------------------------------------------
struct SkinnedMesh {

  // These are all of the different animated meshes that this skinned mesh type
  // can control using the same set of attachment points and animations.
  SkinnedMeshForm* forms;
  size_t number_of_forms;

  // Configured animations for the mesh
  SkinnedMeshAnimation* animations;
  size_t number_of_animations;

  SkinnedMeshAttachmentPointOffset
    attachment_point_offsets[NUMBER_OF_ATTACHMENT_POINTS];
};



}
}

#endif