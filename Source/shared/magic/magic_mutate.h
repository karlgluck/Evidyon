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