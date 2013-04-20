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
#ifndef __TOOLS_SKINNEDMESHANIMATION_H__
#define __TOOLS_SKINNEDMESHANIMATION_H__
#pragma once

// todo: change to SkeletalAnimation.h
#include "shared/animated-mesh/tools/unseenkeyframeanimation.h"
#include <dc/float>
#include <dc/boolean>
#include "shared/skinned-mesh/skinnedmeshanimation.h"


namespace Evidyon {
namespace SkinnedMesh {
namespace Tools {



  
//----[  SkinnedMeshAnimation  ]-----------------------------------------------
class SkinnedMeshAnimation : public dc::dcResource<SkinnedMeshAnimation> {
public:
  static std::string staticTypeName() { return "SkinnedMeshAnimation"; }
public:
  SkinnedMeshAnimation();

  // Fills out the animation's description
  void compile(Evidyon::SkinnedMesh::SkinnedMeshAnimation* description) const;

  // Returns source_speed_variable * speed_factor_ + speed_offset_
  float calculateAnimationSpeed(float source_speed_variable) const;

  // Changes the referenced animation
  void setSkeletalAnimationReference(
    Evidyon::Tools::SkeletalAnimationElement* skeletal_animation);

  dc::dcFloat* getSpeedFactor() { return &speed_factor_; }

private:
  UnseenSkeletalAnimation* getAnimation();

private:

  // This is the source animation that is going to be configured for use with
  // the owning skinned mesh.
  Evidyon::Tools::SkeletalAnimationReference skeletal_animation_;

  // The factor is multiplied by whatever speed variable controls this
  // animation, whereas the offset is just added.  The speed variable is
  // logically defined based on what the animation is configured to represent
  // in the actor: for the walking animation, it is the speed of movement; for
  // attacking, it's the speed of the attack and so on.
  dc::dcFloat speed_factor_;
  dc::dcFloat speed_offset_;

  // How long to make this animation
  dc::dcFloat length_in_seconds_;
};


}
}
}

#include <dc/list>

namespace Evidyon {
namespace Tools {


  
//----[  SkinnedMeshAnimationReference  ]--------------------------------------
typedef dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshAnimation>::Reference
  SkinnedMeshAnimationReference;


}
}


#endif