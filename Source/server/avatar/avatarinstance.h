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
#ifndef __SERVER_AVATAR_AVATARINSTANCE_H__
#define __SERVER_AVATAR_AVATARINSTANCE_H__
#pragma once

#include "shared/currencytype.h"
#include "shared/time/timer.h"
#include "shared/avatar/avatarlimits.h"
#include "shared/avatar/avatargender.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/avatar/avatarattribute.h"
#include "shared/avatar/spellavailabilitymask.h"
#include "shared/actor/actorprofileindex.h"
#include "shared/actor/actorprofilespecializationtype.h"
#include "shared/actor/actorequipmentslot.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"
#include "shared/scenery/sceneryindex.h"
#include "common/number.h"
#include "server/guild/guildid.h"


namespace Evidyon {
namespace Projectile {
struct Projectile;
}
}

namespace Evidyon {
namespace Server {



//----[  AvatarInstance  ]-----------------------------------------------------
struct AvatarInstance {
  char name[Avatar::MAX_AVATAR_NAME_LENGTH];
  Avatar::AvatarGender gender;
  Avatar::AvatarRace race;
  Avatar::AvatarClass class_;
  unsigned int total_monsters_killed;
  Avatar::AvatarExperience experience_this_level;
  Avatar::AvatarExperience experience_to_next_level;
  Avatar::AvatarExperience penalty_experience;
  int attribute_values[Avatar::NUMBER_OF_AVATAR_ATTRIBUTES];
  int available_attribute_points;

  // The guild of which this avatar is a member
  Server::GuildID guild;

  // How the avatar looks without any armor on
  Evidyon::Actor::ActorProfileIndex actor_profile;
  Evidyon::SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form;

  CurrencyType money;

  int kill_count_at_last_pk;
  unsigned int last_pk_hour;
  int alignment_value;

  // Uses the avatar's stats, level and the provided parameters to fill in
  // the internal max hp, max mp, movement speed, action delay, regeneration
  // rates and ability effect values.  The equipment values are all reset.
  // This method does nothing with spells.
  void calculateBaseStats();

  // these are unmodified base values calculated from the player's stats
  Avatar::SpellAvailabilityMask spell_availability;
  Avatar::AvatarLevel level;
  Number max_hp, max_mp;
  Number movement_speed;
  Number hp_regen, mp_regen;
  
  // These numbers are calculated from the stat-based values to reflect
  // the bonuses gained from guild-owned geosids.  Equipment values are
  // based on these.
  Number guild_max_hp, guild_max_mp, guild_xp_multiplier;

  // These values are calculated based on the equipped items and the avatar's
  // core attributes.
  struct {
    Number weapon_min_damage, weapon_max_damage;
    Number max_hp, max_mp;
    Time::Milliseconds melee_effect_delay;  // how long after swinging does the attack hit
    Time::Milliseconds melee_repeat_delay; // how long after swinging to swing again
    const Evidyon::Projectile::Projectile* projectile; // only ranged weapons have this
    Number melee_follow_distance, melee_attack_distance;
    Number movement_speed;
    Number defense;
    Avatar::SpellAvailabilityMask spell_availability;
    Number hp_steal, mp_steal;
    Number hp_regen, mp_regen;
    Evidyon::Actor::ActorProfileIndex actor_profile;
    Evidyon::Actor::ActorProfileSpecializationType specialization;
    Evidyon::SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form;
    Scenery::SceneryIndex equipped[Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS];
  } equipment;
};


}
}

#endif