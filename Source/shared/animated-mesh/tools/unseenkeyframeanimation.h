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
#ifndef __UNSEENKEYFRAMEANIMATION_H__
#define __UNSEENKEYFRAMEANIMATION_H__
#pragma once




#include <dc/filename>
#include "shared/animated-mesh/unseenskinnedmesh.h"

namespace Evidyon {
namespace AnimatedMesh {
struct AnimatedMeshAnimation;
}
}

/**
 * This class references a file containing a keyframe animation ready to be applied to a mesh
 */
class UnseenKeyframeAnimation : public dc::dcResource<UnseenKeyframeAnimation> {
public:
  UnseenKeyframeAnimation();
  ~UnseenKeyframeAnimation();


public:

  // Loads the referenced animation from the source file
  bool loadAnimation(bool force_reload,
                     UnseenSkeletalAnimation** animation);
  bool loadAnimatedMeshAnimation(
    bool force_reload,
    Evidyon::AnimatedMesh::AnimatedMeshAnimation* animated_mesh_animation);

  // Frees the internal animation file, if it exists
  void unloadAnimation();

  // Gets the source file name for this skeletal animation
  dc::dcFileName* getFileName();


public:

  /**
   * Gets the string name of this resource type
   * @return Class name
   */
  static std::string staticTypeName();


protected:

  /// The file from which to load the animation
  dc::dcFileName mySourceFile;

private:

  // In-memory animation
  UnseenSkeletalAnimation* loaded_animation_;
};

#include <dc/list>

namespace Evidyon {
namespace Tools {
typedef dc::dcList<UnseenKeyframeAnimation>::Element SkeletalAnimationElement;
typedef dc::dcList<UnseenKeyframeAnimation>::Reference SkeletalAnimationReference;
}
}


#endif