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
#ifndef __SHARED_MAGIC_MAGIC_MUTATE_H__
#define __SHARED_MAGIC_MAGIC_MUTATE_H__
#pragma once

#include "shared/actor/actorprofileindex.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"

namespace Evidyon {
namespace Magic {

//----[  Magic_Mutate  ]-------------------------------------------------------
// Changes the avatar into something else.
//  TODO: for mutate to fireball, there is no way to set aura damage / sec.
struct Magic_Mutate {
  bool can_speak;
  bool can_cast_spells;
  bool can_melee_attack;
  bool can_be_seen_by_monsters;
  bool flying, levitating;
  float movement_speed_added;
  float attack_delay_multiplier;
  int added_armor;
  float duration;

  // ? take target's profile & form if avatar and < level
  int copy_target_avatar_level;
  Actor::ActorProfileIndex actor_profile;
  SkinnedMesh::SkinnedMeshFormIndex mesh_form;
};

}
}

#endif