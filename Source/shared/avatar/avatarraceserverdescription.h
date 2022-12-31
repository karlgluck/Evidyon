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
#ifndef __SHARED_AVATAR_AVATARRACESERVERDESCRIPTION_H__
#define __SHARED_AVATAR_AVATARRACESERVERDESCRIPTION_H__
#pragma once


#include "avatarattribute.h"
#include "avatargender.h"
#include "avatarrace.h"
#include "shared/actor/actortemplateindex.h"
#include "shared/actor/actorprofileindex.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"
#include "shared/scenery/sceneryindex.h"

namespace Evidyon {
namespace Avatar {



  
//----[  AvatarRaceServerDescription  ]----------------------------------------
struct AvatarRaceServerDescription {
  int attribute_level_1_maxes[NUMBER_OF_AVATAR_ATTRIBUTES];

  struct GenderData {
    Evidyon::Actor::ActorTemplateIndex actor_template;
    Evidyon::Actor::ActorProfileIndex actor_profile;
    Evidyon::SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form;
    Evidyon::Scenery::SceneryIndex hair_style[AVATAR_RACE_HAIR_STYLES_PER_GENDER];
  } gender_data[NUMBER_OF_AVATAR_GENDERS];
};

}
}

#endif