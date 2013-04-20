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
#ifndef __ACTORPROFILE_H__
#define __ACTORPROFILE_H__
#pragma once



#include "shared/skinned-mesh/skinnedmeshindex.h"
#include "shared/skinned-mesh/skinnedmeshanimationindex.h"
#include "shared/skinned-mesh/skinnedmeshattachmentpoint.h"
#include "constantactoranimation.h"
#include "shared/sound/soundindex.h"
#include "actorsound.h"
#include "constantactorattachmentpoint.h"
#include "actorprofilespecialization.h"
#include "actorprofilespecializationtype.h"
#include "shared/specialfx/specialfxindex.h"


namespace Evidyon {
namespace Actor {



//----[  ActorProfile  ]-------------------------------------------------------
struct ActorProfile {

  // A profile just contains the skinned mesh this actor uses.  The actual
  // form of the mesh is determined by either armor that the actor equips or
  // the actor template's default form.
  Evidyon::SkinnedMesh::SkinnedMeshIndex skinned_mesh;
  Evidyon::SkinnedMesh::SkinnedMeshAnimationIndex
    constant_animations[NUMBER_OF_CONSTANT_ACTOR_ANIMATIONS];

  // Actor sounds
  Evidyon::Sound::SoundIndex sounds[NUMBER_OF_ACTOR_SOUNDS];

  // Constant attachment points--ones that don't change based on what the
  // player has equipped.  For example, 
  SkinnedMesh::SkinnedMeshAttachmentPoint
    attachment_points[NUMBER_OF_CONSTANT_ACTOR_ATTACHMENT_POINTS];

  // These profiles are dependent on the actor's primary equipped weapon or
  // other unique features and modify how the actor behaves.
  ActorProfileSpecialization
    specializations[NUMBER_OF_ACTOR_PROFILE_SPECIALIZATION_TYPES];

  // Effect played when the actor gets hit
  SpecialFX::SpecialFXIndex blood_special_fx;
};





}
}

#endif