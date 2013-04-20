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