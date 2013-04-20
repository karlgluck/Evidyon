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
#ifndef __SERVER_AVATAR_AVATARSTATS_H__
#define __SERVER_AVATAR_AVATARSTATS_H__
#pragma once

#include "shared/magic/buffmask.h"
#include "shared/actor/actorprofileindex.h"
#include "shared/actor/actorequipmentslot.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"
#include "common/number.h"
#include "shared/time/timer.h"
#include "shared/scenery/sceneryindex.h"
#include "shared/actor/actorprofilespecializationtype.h"
#include "shared/avatar/spellavailabilitymask.h"


namespace Evidyon {
namespace Server {


//----[  AvatarStats  ]--------------------------------------------------------
// This structure holds the sum of all of the other avatar-stat-affecting
// fields, such as from items, buffs, enchantments and the avatar itself.
struct AvatarStats {
  Number hp, mp;
  Number max_hp, max_mp;
  Number movement_speed;
  Number melee_min_damage, melee_max_damage;
  Time::Milliseconds melee_effect_delay, melee_repeat_delay;
  Number hp_regen, mp_regen; // can be positive or negative
  Number hp_steal, mp_steal; // works only for 1st hit of attack
  Number armor;

  // Misc. data
  bool can_speak;
  bool can_cast_spells;
  bool can_melee_attack;
  bool hidden_from_monsters;
  bool flying, levitating;
  Evidyon::Actor::ActorProfileIndex actor_profile;
  Evidyon::SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form;
  Evidyon::Magic::BuffMask buff_icons;
  Actor::ActorProfileSpecializationType specialization;
  Scenery::SceneryIndex equipped[Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS];
  Avatar::SpellAvailabilityMask spell_availability;

  // When the enchantments have decaying properties, this timer is set so that
  // the decayed values get reincorporated here.
  Time::Timer<Time::RESOLUTION_500_MS> recalculate_timer;

  // Ensures that all calculated members are filled with junk that is obviously
  // incorrect.  This does not affect 'hp' or 'mp'.  The recalculate timer
  // is triggered.
  void invalidateCalculatedMembers();
};




}
}

#endif