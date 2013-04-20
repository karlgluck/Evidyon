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
#ifndef __SHARED_ACTOR_TOOLS_ACTORPROFILESPECIALIZATION_H__
#define __SHARED_ACTOR_TOOLS_ACTORPROFILESPECIALIZATION_H__
#pragma once

#include "shared/skinned-mesh/skinnedmeshattachmentpoint.h"
#include "shared/actor/specializedactorattachmentpoint.h"
#include "shared/actor/specializedactoranimation.h"
#include "shared/skinned-mesh/tools/skinnedmeshanimation.h"
#include <dc/enum>


namespace Evidyon {
namespace Actor {
struct ActorProfileSpecialization;
namespace Tools {
class ActorProfile;
}
}
namespace SkinnedMesh {
namespace Tools {
class SkinnedMesh;
}
}
}


namespace Evidyon {
namespace Actor {
namespace Tools {


  
  
//----[  ActorProfileSpecialization  ]-----------------------------------------
class ActorProfileSpecialization
    : public dc::dcResource<ActorProfileSpecialization> {
public:
  static std::string staticTypeName() { return "ActorProfileSpecialization"; }
public:
  ActorProfileSpecialization();
  void compile(Evidyon::Actor::ActorProfileSpecialization* description);

  // Every specialization should be owned by an actor profile.  This method
  // obtains the owner, asserting that it is of the correct type.
  ActorProfile* getOwnerActorProfile();

  // This method obtains the skinned mesh object from which to select
  // the animations in this profile specialization.  If the owner profile has
  // not selected a skinned mesh, this method returns NULL.
  virtual Evidyon::SkinnedMesh::Tools::SkinnedMesh* getSkinnedMesh();

  // Resets all of the animations in this specialization.  Called when the owner
  // profile changes its skinned mesh reference.
  void clearAnimationReferences();

private:
  dc::dcEnum<SkinnedMesh::SkinnedMeshAttachmentPoint>
    attachment_points_[NUMBER_OF_SPECIALIZED_ACTOR_ATTACHMENT_POINTS];
  Evidyon::Tools::SkinnedMeshAnimationReference
    animations_[NUMBER_OF_SPECIALIZED_ACTOR_ANIMATIONS];
};

}
}
}

#endif