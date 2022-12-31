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
#include "avatarstats.h"


namespace Evidyon {
namespace Server {


//----[  invalidateCalculatedMembers  ]----------------------------------------
void AvatarStats::invalidateCalculatedMembers() {
  max_hp = -1;
  max_mp = -1;
  movement_speed = -1;
  melee_min_damage = -1;
  melee_max_damage = -1;
  melee_effect_delay = -1;
  melee_repeat_delay = -1;
  hp_regen = 0;
  mp_regen = 0;
  hp_steal = -1;
  mp_steal = -1;
  armor = -1;
  can_speak = false;
  can_cast_spells = false;
  can_melee_attack = false;
  hidden_from_monsters = false;
  flying = false;
  levitating = false;
  actor_profile = Evidyon::Actor::INVALID_ACTOR_PROFILE_INDEX;
  skinned_mesh_form = Evidyon::SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX;
  buff_icons.value = 0xFFFFFFFF;
  specialization = Evidyon::Actor::INVALID_ACTOR_PROFILE_SPECIALIZATION;
  memset(equipped,
         -1,
         sizeof(Scenery::SceneryIndex) * Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS);
  spell_availability.zero();
  recalculate_timer.trigger();
}


}
}

