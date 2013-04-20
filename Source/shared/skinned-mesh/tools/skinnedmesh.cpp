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
#include "skinnedmesh.h"
#include "shared/skinned-mesh/skinnedmesh.h"
#include "shared/skinned-mesh/skinnedmeshform.h"
#include "shared/skinned-mesh/skinnedmeshanimation.h"
#include <assert.h>


namespace Evidyon {
namespace SkinnedMesh {
namespace Tools {


  
//----[  SkinnedMesh  ]--------------------------------------------------------
SkinnedMesh::SkinnedMesh() {
  member("Forms", &forms_);
  member("Animations", &animations_);
  for (int i = 0; i < int(NUMBER_OF_ATTACHMENT_POINTS); ++i) {
    member(SkinnedMeshAttachmentPointString(SkinnedMeshAttachmentPoint(i)),
           &attachment_point_offsets_[i]);
  }
}



//----[  compile  ]------------------------------------------------------------
bool SkinnedMesh::compile(Evidyon::SkinnedMesh::SkinnedMesh* description) {
  { // compile forms
    const dc::dcGenericResource::Array& forms = forms_.getMembers();
    description->number_of_forms = forms.size();
    description->forms
      = new Evidyon::SkinnedMesh::SkinnedMeshForm[forms.size()];
    size_t index = 0;
    for (dc::dcGenericResource::Array::const_iterator i = forms.begin();
         i != forms.end(); ++i, ++index) {
      SkinnedMeshForm* element = (SkinnedMeshForm*)(*i);
      element->compile(&description->forms[index]);
    }
  }

  { // compile animations
    const dc::dcGenericResource::Array& animations = animations_.getMembers();
    description->number_of_animations = animations.size();
    description->animations
      = new Evidyon::SkinnedMesh::SkinnedMeshAnimation[animations.size()];
    size_t index = 0;
    for (dc::dcGenericResource::Array::const_iterator i = animations.begin();
         i != animations.end(); ++i, ++index) {
      SkinnedMeshAnimation* element = (SkinnedMeshAnimation*)(*i);
      element->compile(&description->animations[index]);
    }
  }

  for (int i = 0; i < int(NUMBER_OF_ATTACHMENT_POINTS); ++i) {
    attachment_point_offsets_[i].compile(
      description->attachment_point_offsets + i);
  }

  return true;
}



//----[  getAttachmentPoint  ]-------------------------------------------------
SkinnedMeshAttachmentPointOffset* SkinnedMesh::getAttachmentPoint(
    SkinnedMeshAttachmentPoint point) {
  assert(point < NUMBER_OF_ATTACHMENT_POINTS);
  assert(point >= 0);
  return &attachment_point_offsets_[point];
}

}
}
}