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
#include "actorprofilespecialization.h"
#include "shared/actor/actorprofilespecialization.h"
#include <assert.h>
#include "actorprofile.h"


namespace Evidyon {
namespace Actor {
namespace Tools {





//----[  ActorProfileSpecialization  ]-----------------------------------------
ActorProfileSpecialization::ActorProfileSpecialization() {
  for (int i = 0; i < NUMBER_OF_SPECIALIZED_ACTOR_ATTACHMENT_POINTS; ++i) {
    member(SpecializedActorAttachmentPointString(
              SpecializedActorAttachmentPoint(i)), &attachment_points_[i]);
  }
  for (int i = 0; i < NUMBER_OF_SPECIALIZED_ACTOR_ANIMATIONS; ++i) {
    member(SpecializedActorAnimationString(SpecializedActorAnimation(i)),
           &animations_[i]);
  }
}





//----[  compile  ]------------------------------------------------------------
void ActorProfileSpecialization::compile(
    Evidyon::Actor::ActorProfileSpecialization* description) {
  for (int i = 0; i < NUMBER_OF_SPECIALIZED_ACTOR_ATTACHMENT_POINTS; ++i) {
    description->attachment_points[i] 
      = attachment_points_[i].getValue();
  }
  for (int i = 0; i < NUMBER_OF_SPECIALIZED_ACTOR_ANIMATIONS; ++i) {
    description->animations[i]
      = animations_[i].getReferencedResourceIndex();
  }

  // copy primary to alternates if they don't exist
  if (description->animations[ANIMATION_ATTACK_2]
        == SkinnedMesh::INVALID_SKINNED_MESH_ANIMATION_INDEX) {
    description->animations[ANIMATION_ATTACK_2]
      = description->animations[ANIMATION_ATTACK_1];
  }
}




//----[  getOwnerActorProfile  ]-----------------------------------------------
ActorProfile* ActorProfileSpecialization::getOwnerActorProfile() {
//  assert(getOwner()->getTypeID() ==
//         dc::dcClass<ActorProfile>::staticResourceManager()->uniqueID());
  return (ActorProfile*)(getOwner());
}




//----[  getSkinnedMesh  ]-----------------------------------------------------
Evidyon::SkinnedMesh::Tools::SkinnedMesh*
    ActorProfileSpecialization::getSkinnedMesh() {
  ActorProfile* owner_profile = getOwnerActorProfile();
  return owner_profile->getSkinnedMesh();
}



//----[  clearAnimationReferences  ]-------------------------------------------
void ActorProfileSpecialization::clearAnimationReferences() {
  for (int i = 0; i < NUMBER_OF_SPECIALIZED_ACTOR_ANIMATIONS; ++i) {
    animations_[i].setGenericReferencedResource(NULL);
  }
}

}
}
}