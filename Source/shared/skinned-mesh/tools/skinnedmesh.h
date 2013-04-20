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
#ifndef __TOOLS_SKINNEDMESH_H__
#define __TOOLS_SKINNEDMESH_H__
#pragma once

#include "shared/skinned-mesh/skinnedmeshattachmentpoint.h"
#include <dc/list>
#include "skinnedmeshform.h"
#include "skinnedmeshattachmentpointoffset.h"

namespace Evidyon {
namespace SkinnedMesh {
struct SkinnedMesh;

namespace Tools {

  
//----[  SkinnedMesh  ]---------------------------------------------------
// Holds a list of meshes that all have the same bone structure, and therefore
// share animations, attachment points and other information.
class SkinnedMesh : public dc::dcResource<SkinnedMesh> {
  typedef dc::dcList<SkinnedMeshForm> FormsList;
  typedef dc::dcList<SkinnedMeshAnimation> AnimationsList;
public:
  static std::string staticTypeName() { return "SkinnedMesh"; }
public:
  SkinnedMesh();
  bool compile(Evidyon::SkinnedMesh::SkinnedMesh* description);
  const dc::dcGenericResource::Array& forms() { return forms_.getMembers(); }
  const dc::dcGenericResource::Array& animations() { return animations_.getMembers(); }
  SkinnedMeshAttachmentPointOffset* getAttachmentPoint(
    SkinnedMeshAttachmentPoint point);

private:
  FormsList forms_;
  AnimationsList animations_;
  SkinnedMeshAttachmentPointOffset
    attachment_point_offsets_[NUMBER_OF_ATTACHMENT_POINTS];
  //VisualFX::VisualFXReference visual_fx_; //make this specialfx
};

}
}
}

#include <dc/list>

namespace Evidyon {
namespace Tools {

//----[  SkinnedMeshReference  ]-----------------------------------------------
typedef dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMesh>::Reference
  SkinnedMeshReference;

//----[  SkinnedMeshElement  ]-------------------------------------------------
typedef dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMesh>::Element
  SkinnedMeshElement;

}
}

#endif