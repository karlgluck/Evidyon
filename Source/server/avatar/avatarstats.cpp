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

