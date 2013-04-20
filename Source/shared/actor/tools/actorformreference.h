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