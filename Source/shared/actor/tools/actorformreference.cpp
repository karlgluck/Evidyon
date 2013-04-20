//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008 - 2013 Karl Gluck                                       //
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
#include "actorformreference.h"


namespace Evidyon {
namespace Actor {
namespace Tools {




//----[  ActorFormReference  ]-------------------------------------------------
ActorFormReference::ActorFormReference() {
  member("Actor Profile", &actor_profile_);
  member("Skinned Mesh Form", &skinned_mesh_form_);
}


  
//----[  compile  ]------------------------------------------------------------
void ActorFormReference::compile(
    Actor::ActorProfileIndex* actor_profile,
    Evidyon::SkinnedMesh::SkinnedMeshFormIndex* form) {
  *actor_profile = actor_profile_.getReferencedResourceIndex();
  *form = skinned_mesh_form_.getReferencedResourceIndex();
}


//----[  compile  ]------------------------------------------------------------
void ActorFormReference::compile(
    Evidyon::Tools::ActorProfileElement** actor_profile,
    Evidyon::SkinnedMesh::SkinnedMeshFormIndex* form) {
  *actor_profile = actor_profile_.getReferencedResource();
  *form = skinned_mesh_form_.getReferencedResourceIndex();
}


//----[  compile  ]------------------------------------------------------------
void ActorFormReference::compile(
    Evidyon::Tools::ActorProfileElement** actor_profile,
    Evidyon::Tools::SkinnedMeshFormElement** form) {
  *actor_profile = actor_profile_.getReferencedResource();
  *form = skinned_mesh_form_.getReferencedResource();
}



//----[  isValid  ]------------------------------------------------------------
bool ActorFormReference::isValid() {
  if (actor_profile_.isMissingReference() ||
      skinned_mesh_form_.isMissingReference()) return false;
  //dc::dcGenericResource* form
  //  = skinned_mesh_form_.getGenericReferencedResource();
  //if (!form || form->getOwner()->getOwner()  // element->list->owner
  //             != actor_profile_.getGenericReferencedResource()) return false;

  return true;
}


}
}
}
