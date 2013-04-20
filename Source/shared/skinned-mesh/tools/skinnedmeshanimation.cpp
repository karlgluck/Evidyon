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
#include "skinnedmeshanimation.h"
#include "shared/skinned-mesh/skinnedmeshanimation.h"


namespace Evidyon {
namespace SkinnedMesh {
namespace Tools {



  
//----[  SkinnedMeshAnimation  ]-----------------------------------------------
SkinnedMeshAnimation::SkinnedMeshAnimation() {
  member("Skeletal Animation", &skeletal_animation_);
  member("Speed Factor", &speed_factor_);
  member("Speed Offset", &speed_offset_);
  member("Length in Seconds", &length_in_seconds_);
  length_in_seconds_.setValue(1.0f);
  speed_factor_.setValue(1.0f);
}



//----[  compile  ]------------------------------------------------------------
void SkinnedMeshAnimation::compile(
    Evidyon::SkinnedMesh::SkinnedMeshAnimation* description) const {
  description->animation = skeletal_animation_.getReferencedResourceIndex();
  description->length_in_seconds = length_in_seconds_.getValue();
  description->speed_factor = speed_factor_.getValue();
  description->speed_offset = speed_offset_.getValue();
}



//----[  getAnimation  ]-------------------------------------------------------
UnseenSkeletalAnimation* SkinnedMeshAnimation::getAnimation() {
  UnseenSkeletalAnimation* skeletal_animation = NULL;
  UnseenKeyframeAnimation* keyframe_animation
    = skeletal_animation_.getReferencedResource();
  if (keyframe_animation == NULL) return NULL;
  if (keyframe_animation->loadAnimation(false, &skeletal_animation)) {
    return skeletal_animation;
  }
  return NULL;
}




//----[  calculateAnimationSpeed  ]--------------------------------------------
float SkinnedMeshAnimation::calculateAnimationSpeed(
    float source_speed_variable) const {
  return speed_factor_.getValue() * source_speed_variable
          + speed_offset_.getValue();
}




//----[  setSkeletalAnimationReference  ]--------------------------------------
void SkinnedMeshAnimation::setSkeletalAnimationReference(
    Evidyon::Tools::SkeletalAnimationElement* skeletal_animation) {
  skeletal_animation_.setReferencedResource(skeletal_animation);
}




}
}
}