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
#include "aimeleeattack.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "../shared/actor/evidyon_actor.h"


AIMeleeAttack::AIMeleeAttack() {
  member("Range", &range_);
  member("Attack", &attack_);
  member("Min % Target HP Damage Per Second", &percent_dps_min_);
  member("Max % Target HP Damage Per Second", &percent_dps_max_);
}

void AIMeleeAttack::compile(const LevelAttributes* intended_level_attributes,
                            int intended_level,
                            float time_between_attacks,
                            const float attack_durations[Evidyon::ACTORATTACK_COUNT],
                            Evidyon::AIMeleeAttackDescription* description) const {
  description->range = range_.getValue();
  description->action = static_cast<Evidyon::ActorAction>(static_cast<int>(Evidyon::ACTORACTION_ATTACK1) + attack_.getValue());

  float duration = attack_durations[attack_.getValue()];
  float repeat_time = time_between_attacks + duration;

  int dps_min = percent_dps_min_.getValue() / 100.0f * intended_level_attributes->getAverageHP();
  int dps_max = percent_dps_max_.getValue() / 100.0f * intended_level_attributes->getAverageHP();

  float min_damage = dps_min * repeat_time;
  float max_damage = dps_max * repeat_time;
  int random_damage = (int)(max_damage - min_damage);

  // additional damage per second has been disabled
  float additional_damage_per_level = 0.0f; // repeat_time * additional_dps_per_level_.getValue();

  float base_damage = min_damage - intended_level * additional_damage_per_level;
  if (base_damage < 1.0f) {
    base_damage = 1.0f;
    additional_damage_per_level = (min_damage - 1) / intended_level;

    // if the slope is too shallow, prevent integer roundoff error
    if (additional_damage_per_level < 1.0f) {
      additional_damage_per_level = 0.0f;
      base_damage = min_damage;
    }
  }

  description->duration = duration;
  description->base_damage = base_damage > 1.0f ? ((int)base_damage) : 1;
  description->random_additional_damage = random_damage > 0 ? random_damage : 0;
  description->added_damage_per_level = (int)additional_damage_per_level;
}

std::string AIMeleeAttack::staticTypeName() {
  return "AIMeleeAttack";
}

