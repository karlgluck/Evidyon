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
#ifndef __ACTORTYPE_H__
#define __ACTORTYPE_H__
#pragma once


#include <dcx/vector3>
#include <dc/float>
#include <dc/list>
#include <dc/map>
#include <dc/integer>
#include "stdafx.h"
#include "unseenskinnedmeshresource.h"
#include "unseenkeyframeanimation.h"
#include "colorkey.h"
#include "image.h"
#include "texture.h"
#include "wavsound.h"
#include "skinnedmeshattachmentpoint.h"
#include "../shared/actor/evidyon_actor.h"

/**
 * This class is used to represent an actor in the world; actors are representations of
 * dynamic objects.
*/
class ActorType : public dc::dcResource<ActorType> {
public:

  /**
   * Initializes this resource
   */
  ActorType();

  dcx::dcxVector3<dc::dcFloat>* getScalingFactor() { return &myScalingFactor; }
  dcx::dcxVector3<dc::dcFloat>* getRotation() { return &myRotation; }
  dcx::dcxVector3<dc::dcFloat>* getTranslation() { return &myTranslation; }
  dc::dcFloat* getRadialSize() { return &myRadialSize; }
  float getRadialSizeValue() const { return myRadialSize.getValue(); }
  float getHeightValue() const { return height_.getValue(); }
  dc::dcList<UnseenSkinnedMeshResource>::Reference* getSkinnedMesh() { return &mySkinnedMesh; }
  dc::dcMap<dc::dcInteger,dcTable<Texture>::Reference>* getMeshTextures() { return &myMeshTextures; }
  dc::dcList<UnseenKeyframeAnimation>::Element* getActionAnimation(ActorAnimation action) { return myActionAnimations[action].getReferencedResource(); }

  dc::dcFloat* getSpeedOffset(ActorAnimation action) { return &myActionAnimationSpeedOffsets[action]; }
  dc::dcFloat* getSpeedFactor(ActorAnimation action) { return &myActionAnimationSpeedFactors[action]; }

  SkinnedMeshAttachmentPoint* getSkinnedMeshAttachmentPoint(int point) { return &myAttachmentPoints[point]; }


public:

  /**
   * Writes this actor type's information into the output description structure
   */
  bool compileForClient(ClientActorTypeDescription* description) const;

  /**
   * Shows a dialog that lets the user visually set the scale of this actor
   */
  void runEditDialog();

public:

  /**
   * Gets the string name of this resource type
   * @return Class name
   */
  static std::string staticTypeName();


protected:

  // The source skinned mesh to use for this actor
  dc::dcList<UnseenSkinnedMeshResource>::Reference mySkinnedMesh;

  // Generates a transformation matrix to change this actor's mesh
  dcx::dcxVector3<dc::dcFloat> myScalingFactor;
  dcx::dcxVector3<dc::dcFloat> myRotation;
  dcx::dcxVector3<dc::dcFloat> myTranslation;

  // The radius of the circle that this actor takes up
  dc::dcFloat myRadialSize;

  dc::dcFloat height_; // This actor's vertical height

  // Maps action types to animations that show that action
  dc::dcList<UnseenKeyframeAnimation>::Reference myActionAnimations[ACTORANIMATION_COUNT];

  // Defines a speed offset from the file
  dc::dcFloat myActionAnimationSpeedOffsets[ACTORANIMATION_COUNT];

  // Defines how this animation scales its speed as the action to which it is
  // attached changes its speed.
  dc::dcFloat myActionAnimationSpeedFactors[ACTORANIMATION_COUNT];

  // The map designating which texture to use to render each skinned mesh texture subset
  dc::dcMap<dcInteger,dcTable<Texture>::Reference> myMeshTextures;

  // Different sounds an actor makes
  dc::dcList<WAVSound>::Reference myActorSounds[ACTORSOUND_COUNT];

  // The attachment points for items
  SkinnedMeshAttachmentPoint myAttachmentPoints[ACTORATTACHMENTPOINT_COUNT];
};



#endif