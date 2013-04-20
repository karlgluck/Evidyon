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