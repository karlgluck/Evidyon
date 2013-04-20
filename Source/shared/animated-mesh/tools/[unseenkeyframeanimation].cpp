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
#include "unseenkeyframeanimation.h"
#include <dc/filestream>
#include "common/safemacros.h"
#include "shared/animated-mesh/animatedmeshutils.h"
#include <assert.h>


//-----------------------------------------------------------------------------
// Name: UnseenKeyframeAnimation
// Desc: Initializes this resource
//-----------------------------------------------------------------------------
UnseenKeyframeAnimation::UnseenKeyframeAnimation() {
  member("Source File", &mySourceFile);
  loaded_animation_ = NULL;
}


//-----------------------------------------------------------------------------
// Name: UnseenKeyframeAnimation
// Desc: Initializes this resource
//-----------------------------------------------------------------------------
UnseenKeyframeAnimation::~UnseenKeyframeAnimation() {
  unloadAnimation();
}


//-----------------------------------------------------------------------------
// Name: loadAnimation
// Desc: Loads the referenced animation from the source file
//-----------------------------------------------------------------------------
bool UnseenKeyframeAnimation::loadAnimation(bool force_reload,
                                            UnseenSkeletalAnimation** animation) {
  assert(!"the UKA format has changed and this method should not be called any more");
  return false;

  //if (!force_reload && loaded_animation_) {
  //  *animation = loaded_animation_;
  //  return true;
  //} else {
  //  unloadAnimation();
  //  loaded_animation_ = new UnseenSkeletalAnimation;
  //  memset(loaded_animation_, 0, sizeof(UnseenSkeletalAnimation));
  //  *animation = loaded_animation_;
  //}

  //// Open the source stream
  //dc::dcFileStream fs;
  //if (APP_ERROR(!fs.open(mySourceFile.getValue().c_str(), 0))("Couldn't open source keyframe animation file %s", mySourceFile.getValue().c_str()))
  //    return false;

  //// Load the mesh
  //bool failed = APP_ERROR(!loadUnseenSkeletalAnimation(&fs, loaded_animation_))("Invalid Unseen keyframe animation file %s", mySourceFile.getValue().c_str());

  //// Close the stream
  //fs.close();

  //// Return the result
  //return !failed;
}


//----[  loadAnimatedMeshAnimation  ]------------------------------------------
bool UnseenKeyframeAnimation::loadAnimatedMeshAnimation(
    bool force_reload,
    Evidyon::AnimatedMesh::AnimatedMeshAnimation* animated_mesh_animation) {

  // Open the source stream
  dc::dcFileStream fs;
  if (APP_ERROR(!fs.open(mySourceFile.getValue().c_str(), 0))("Couldn't open source keyframe animation file %s", mySourceFile.getValue().c_str()))
      return false;

  size_t bytes = fs.size();
  char* temp = new char[bytes];
  fs.read(temp, bytes);
  bool succeeded
    = AnimatedMeshUtil_Deserialize(temp, temp + bytes, animated_mesh_animation);

  delete[] temp;
  return succeeded;

  //UnseenSkeletalAnimation* unseen_animation;
  //if (!loadAnimation(false, &unseen_animation)) return false;
  //if (!AnimatedMeshUtil_InterpretUnseenSkeletalAnimation(
  //      unseen_animation,
  //      animated_mesh_animation)) {
  //  return false;
  //}
  //return true;
}



//----[  unloadAnimation  ]----------------------------------------------------
void UnseenKeyframeAnimation::unloadAnimation() {
  if (loaded_animation_) {
    deallocateUnseenSkeletalAnimation(loaded_animation_);
    SAFE_DELETE(loaded_animation_);
  }
}


//-----------------------------------------------------------------------------
// Name: getFileName
// Desc: Gets the source file name for this skeletal animation
//       @return Pointer to the internal file name object
//-----------------------------------------------------------------------------
dc::dcFileName* UnseenKeyframeAnimation::getFileName() {
    return &mySourceFile;
}



//-----------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Gets the string name of this resource type
//       @return Class name
//-----------------------------------------------------------------------------
std::string UnseenKeyframeAnimation::staticTypeName()
{
    return "UnseenKeyframeAnimation";
}

