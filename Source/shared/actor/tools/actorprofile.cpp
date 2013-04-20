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
#include "actorprofile.h"
#include "shared/actor/actorprofile.h"
#include "shared/actor/actortemplate.h"


namespace Evidyon {
namespace Actor {
namespace Tools {



  
//----[  ActorProfile  ]-------------------------------------------------------
ActorProfile::ActorProfile() {
  member("Skinned Mesh", &skinned_mesh_);
  for (int i = 0; i < NUMBER_OF_CONSTANT_ACTOR_ANIMATIONS; ++i) {
    member(ConstantActorAnimationString(ConstantActorAnimation(i)),
           &constant_animations_[i]);
  }
  for (int i = 0; i < NUMBER_OF_ACTOR_SOUNDS; ++i) {
    member(std::string("Sound: ")+ActorSoundString(ActorSound(i)), &sounds_[i]);
  }
  for (int i = 0; i < NUMBER_OF_CONSTANT_ACTOR_ATTACHMENT_POINTS; ++i) {
    member(ConstantActorAttachmentPointString(
              ConstantActorAttachmentPoint(i)), &attachment_points_[i]);
    attachment_points_[i].setValue(Evidyon::SkinnedMesh::ATTACHMENTPOINT_BACK);
  }
  for (int i = 0; i < NUMBER_OF_ACTOR_PROFILE_SPECIALIZATION_TYPES; ++i) {
    member(ActorProfileSpecializationTypeString(
              ActorProfileSpecializationType(i)), &specializations_[i]);
  }
  member("Blood SpecialFX", &blood_fx_);
}




//----[  compile  ]------------------------------------------------------------
void ActorProfile::compile(Evidyon::Actor::ActorProfile* description) {
  description->skinned_mesh = skinned_mesh_.getReferencedResourceIndex();
  for (int i = 0; i < NUMBER_OF_CONSTANT_ACTOR_ANIMATIONS; ++i) {
    description->constant_animations[i]
      = constant_animations_[i].getReferencedResourceIndex();
  }
  for (int i = 0; i < NUMBER_OF_ACTOR_SOUNDS; ++i) {
    description->sounds[i] = sounds_[i].getReferencedResourceIndex();
  }
  for (int i = 0; i < NUMBER_OF_CONSTANT_ACTOR_ATTACHMENT_POINTS; ++i) {
    description->attachment_points[i] = attachment_points_[i].getValue();
  }
  for (int i = 0; i < NUMBER_OF_ACTOR_PROFILE_SPECIALIZATION_TYPES; ++i) {
    specializations_[i].compile(&description->specializations[i]);
  }
  description->blood_special_fx = blood_fx_.getReferencedResourceIndex();
}







//----[  compileDefaultTemplate  ]---------------------------------------------
void ActorProfile::compileDefaultTemplate(
    Evidyon::Actor::ActorTemplate* actor_template) {
  memset(actor_template, 0, sizeof(Evidyon::Actor::ActorTemplate));
  actor_template->template_type = STATIC_TEMPLATE;
  actor_template->interaction = ACTORINTERACTION_ATTACK; // default interaction
  actor_template->actor_profile = getIndex();
  actor_template->alignment = ALIGNMENT_NEUTRAL;
  actor_template->skinned_mesh_form
    = SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX;

  for (int i = 0; i < NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
    actor_template->equipped_items[i] = Scenery::INVALID_SCENERY_INDEX;
  }
}





//----[  getSkinnedMesh  ]-----------------------------------------------------
Evidyon::SkinnedMesh::Tools::SkinnedMesh* ActorProfile::getSkinnedMesh() {
  return skinned_mesh_.getReferencedResource();
}







//----[  setSkinnedMesh  ]-----------------------------------------------------
void ActorProfile::setSkinnedMesh(Evidyon::SkinnedMesh::Tools::SkinnedMesh* skinned_mesh) {
  for (int i = 0; i < NUMBER_OF_CONSTANT_ACTOR_ANIMATIONS; ++i) {
    constant_animations_[i].setGenericReferencedResource(NULL);
  }
  for (int i = 0; i < NUMBER_OF_ACTOR_PROFILE_SPECIALIZATION_TYPES; ++i) {
    specializations_[i].clearAnimationReferences();
  }
}




}
}
}

