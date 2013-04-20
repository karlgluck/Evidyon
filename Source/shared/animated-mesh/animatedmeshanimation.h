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
#ifndef __ANIMATEDMESHANIMATION_H__
#define __ANIMATEDMESHANIMATION_H__
#pragma once



namespace Evidyon {
namespace AnimatedMesh {

//----[  AnimatedMeshAnimation  ]----------------------------------------------
struct AnimatedMeshAnimation {
  // How many frames are in the animated mesh to which this animation can be
  // applied.  This value is checked by the renderer to validate the animation.
  size_t number_of_total_frames;

  // How many of the total frames are actually modified by this animation.
  // This allows animations to only apply themselves to portions of a skeleton,
  // which makes action animations much easier.
  size_t number_of_animated_frames;

  // This is an array of length 'number_of_animated_frames' that translates the
  // animation's frame index to its corresponding animated mesh frame index.
  // Not all frames are necessairily animated.  An entry with no corresponding
  // transform is listed as INVALID_FRAME_TRANSFORM_INDEX in this table.
  unsigned int* animated_frame_to_frame_table;
  static const unsigned int INVALID_FRAME_TRANSFORM_INDEX = -1;

  // The total number of unique poses within this animation.  Each key must be
  // equally spaced in the animation.
  size_t number_of_keys;

  // This structure is stored per-frame, per-key and holds the frame state in
  // the key's pose.
  struct AnimatedFrameTransform {
    D3DXVECTOR3 translation;
    D3DXQUATERNION rotation;
    D3DXVECTOR3 scaling;
  };

  //// A key is stored for each pose in the animation
  //struct Key {
  //  AnimatedFrameTransform* animated_frame_transforms;
  //};
  //// List of poses in this animation
  //Key* keys;

  // All of the frame transforms for all of the keys are allocated in one
  // contiguous block of memory; each key's frame transforms start at
  // key_index * number_of_animated_frames within this array.
  AnimatedFrameTransform* animated_frame_transforms;
};

}
}

#endif