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
#ifndef __SHARED_ACTOR_TOOLS_ACTORPROFILE_H__
#define __SHARED_ACTOR_TOOLS_ACTORPROFILE_H__
#pragma once

#include "shared/sound/tools/wavsound.h"
#include "shared/skinned-mesh/tools/skinnedmesh.h"
#include "shared/skinned-mesh/tools/skinnedmeshanimation.h"
#include "shared/actor/constantactoranimation.h"
#include "shared/actor/actorprofilespecializationtype.h"
#include "shared/actor/actorsound.h"
#include "actorprofilespecialization.h"
#include "shared/actor/constantactorattachmentpoint.h"
#include "shared/specialfx/tools/specialfx.h"


namespace Evidyon {
namespace Actor {
struct ActorProfile;
struct ActorTemplate;
}
}


namespace Evidyon {
namespace Actor {
namespace Tools {



//----[  ActorProfile  ]-------------------------------------------------------
class ActorProfile : public dc::dcResource<ActorProfile> {
public:
  static std::string staticTypeName() { return "ActorProfile"; }
public:
  ActorProfile();
  void compile(Evidyon::Actor::ActorProfile* description);

  // Writes this profile's index into the template, and fills out all fields
  // with default values.
  void compileDefaultTemplate(Evidyon::Actor::ActorTemplate* actor_template);

  // Obtains the selected skinned mesh for this actor profile.  This is the
  // mesh from which animations are selected.
  Evidyon::SkinnedMesh::Tools::SkinnedMesh* getSkinnedMesh();

  // Changes the mesh referenced by this profile.  All animations (including
  // those in the specializations) are cleared.
  void setSkinnedMesh(Evidyon::SkinnedMesh::Tools::SkinnedMesh* skinned_mesh);

private:
  Evidyon::Tools::SkinnedMeshReference skinned_mesh_;
  Evidyon::Tools::SkinnedMeshAnimationReference
    constant_animations_[NUMBER_OF_CONSTANT_ACTOR_ANIMATIONS];
  Evidyon::Tools::SoundReference sounds_[NUMBER_OF_ACTOR_SOUNDS];
  dc::dcEnum<SkinnedMesh::SkinnedMeshAttachmentPoint>
    attachment_points_[NUMBER_OF_CONSTANT_ACTOR_ATTACHMENT_POINTS];
  ActorProfileSpecialization
    specializations_[NUMBER_OF_ACTOR_PROFILE_SPECIALIZATION_TYPES];
  Evidyon::Tools::SpecialFXReference blood_fx_;
};

}
}
}

namespace Evidyon {
namespace Tools {

  
//----[  ActorProfileElement  ]------------------------------------------------
typedef dc::dcList<Evidyon::Actor::Tools::ActorProfile>::Element
  ActorProfileElement;


//----[  ActorProfileReference  ]----------------------------------------------
typedef dc::dcList<Evidyon::Actor::Tools::ActorProfile>::Reference
  ActorProfileReference;
}
}

#endif