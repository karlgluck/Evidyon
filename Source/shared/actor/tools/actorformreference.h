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
#ifndef __SHARED_ACTOR_TOOLS_ACTORFORMREFERENCE_H__
#define __SHARED_ACTOR_TOOLS_ACTORFORMREFERENCE_H__
#pragma once

#include "actorprofile.h"
#include "shared/skinned-mesh/tools/skinnedmeshform.h"
#include "shared/actor/actorprofileindex.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"

namespace Evidyon {
namespace Actor {
namespace Tools {



//----[  ActorFormReference  ]-------------------------------------------------
class ActorFormReference : public dc::dcResource<ActorFormReference> {
public:
  static std::string staticTypeName() { return "ActorFormReference"; }
public:
  ActorFormReference();
  void compile(Actor::ActorProfileIndex* actor_profile,
               Evidyon::SkinnedMesh::SkinnedMeshFormIndex* form);
  void compile(Evidyon::Tools::ActorProfileElement** actor_profile,
               Evidyon::SkinnedMesh::SkinnedMeshFormIndex* form);
  void compile(Evidyon::Tools::ActorProfileElement** actor_profile,
               Evidyon::Tools::SkinnedMeshFormElement** form);

  Evidyon::Tools::ActorProfileReference* getActorProfile() { return &actor_profile_; }
  Evidyon::Tools::SkinnedMeshFormReference* getForm() { return &skinned_mesh_form_; }
  bool isValid();
private:
  Evidyon::Tools::ActorProfileReference actor_profile_;
  Evidyon::Tools::SkinnedMeshFormReference skinned_mesh_form_;
};


}
}
}

#endif