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
#include "lifeformai_projectilemonster.h"
#include "shared/actor/actortemplate.h"
#include "shared/actor/tools/actortemplatecompiler.h"
#include "shared/lifeformai/lifeformai.h"
#include "shared/actor/actorprofile.h"
#include "shared/event/tools/eventcompiler.h"
#include "shared/event/event_targetedspecialfx.h"
#include <assert.h>


namespace Evidyon {
namespace LifeformAI {
namespace Tools {

dc::dcClass<LifeformAI_ProjectileMonster>::Implements<LifeformAI>
  LifeformAI_ProjectileMonster::implements_LifeformAI;




//----[  LifeformAI_ProjectileMonster  ]--------------------------------------
LifeformAI_ProjectileMonster::LifeformAI_ProjectileMonster() {
  member("Displayed Name", &name_);
  member("Actor Form", &actor_form_);
  for (int i = 0; i < Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
    member(std::string("Equipped Item - ") +
           Actor::ActorEquipmentSlotString(Actor::ActorEquipmentSlot(i)),
           &equipment_[i]);
  }
  member("Actor Profile Specialization", &actor_profile_specialization_);
  member("Attack Duration (ms)", &attack_duration_);
  member("Time Between Attacks (ms)", &time_between_attacks_);
  member("HP Modifier", &hp_modifier_);
  member("DPS Modifier", &dps_modifier_);
  member("XP Modifier", &xp_modifier_);
  member("Speed", &speed_);
  member("Treasure", &treasure_);
  member("Treasure Level", &treasure_level_);
  member("Size (radius)", &size_);
  member("Spawn FX", &spawn_fx_);
  member("Spawn FX Duration (ms)", &spawn_fx_duration_);
  member("% into spawn FX to spawn", &percent_into_fx_to_spawn_);
  member("Is Magic Projectile?", &is_magic_projectile_);
  member("Projectile", &projectile_);

  actor_profile_specialization_.setValue(Actor::SPECIALIZE_UNARMED);
  attack_duration_.setValue(1000);
  time_between_attacks_.setValue(1000);
  speed_.setValue(3.0f);
  size_.setValue(1.0f);
  spawn_fx_duration_.setValue(500);
  percent_into_fx_to_spawn_.setValue(75.0);
}




//----[  compile  ]------------------------------------------------------------
bool LifeformAI_ProjectileMonster::compile(
    Evidyon::Treasure::Tools::TreasureCompiler* treasure_compiler,
  Evidyon::Event::Tools::EventCompiler* event_compiler,
    Actor::Tools::ActorTemplateCompiler* actor_template_compiler,
    Evidyon::LifeformAI::LifeformAI* description) {
  Evidyon::Tools::ActorProfileElement* actor_profile;
  Evidyon::SkinnedMesh::SkinnedMeshFormIndex form;
  actor_form_.compile(&actor_profile, &form);
  if (!actor_profile ||
      form == Evidyon::SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX) {
    return false;
  }

  // Compile the template for this lifeform
  Actor::ActorTemplate actor_template;
  actor_profile->compileDefaultTemplate(&actor_template);
  actor_template.alignment
    = Actor::ALIGNMENT_EVIL;
  actor_template.skinned_mesh_form = form;
  strncpy_s(actor_template.name,
            Actor::ActorTemplate::MAX_NAME_LENGTH,
            name_.getValue().c_str(),
            Actor::ActorTemplate::MAX_NAME_LENGTH-1);
  actor_template.template_type = Actor::STATIC_TEMPLATE;
  for (int i = 0; i < Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
    actor_template.equipped_items[i]
      = equipment_[i].getReferencedResourceIndex();
  }
  actor_template.profile_specialization
    = actor_profile_specialization_.getValue();
  Actor::ActorTemplateIndex actor_template_index
    = actor_template_compiler->addTemplate(&actor_template);
  assert(actor_template_index != Actor::INVALID_ACTOR_TEMPLATE_INDEX);

  // Write the general lifeform description data
  Event::Event_TargetedSpecialFX event_description;
  event_description.special_fx = spawn_fx_.getReferencedResourceIndex();
  if (event_description.special_fx != SpecialFX::INVALID_SPECIALFX_INDEX) {
    event_description.duration  // this value is in seconds, not milliseconds
      = double(spawn_fx_duration_.getValue() / 1000.0);
    description->spawn_event = event_compiler->add(&event_description);
  } else {
    description->spawn_event = Event::INVALID_EVENT_INDEX;
  }
  description->spawn_delay
    = spawn_fx_duration_.getValue()
      * percent_into_fx_to_spawn_.getValue() / 100.0;
  description->type
    = Evidyon::LifeformAI::LifeformAI::LIFEFORMAI_PROJECTILEMONSTER;

  // Write the type-specific lifeform description
  Evidyon::LifeformAI::LifeformAI_ProjectileMonster* my_description
    = &description->projectile_monster;
  my_description->actor_template_index = actor_template_index;
  Time::Milliseconds attack_period
    = time_between_attacks_.getValue() + attack_duration_.getValue();
  my_description->attack_duration = attack_duration_.getValue();
  my_description->attack_period = attack_period;
  my_description->speed = speed_.getValue();
  my_description->size = size_.getValue();
  my_description->despawn_event = description->spawn_event;
  my_description->despawn_delay = description->spawn_delay;
  my_description->hp_multiplier = hp_modifier_.getValue() + 1.0f;
  my_description->xp_multiplier = xp_modifier_.getValue() + 1.0f;
  my_description->dps_to_damage_multiplier
    = (dps_modifier_.getValue() + 1.0) * (attack_period / 1000.0);
  my_description->is_magic_projectile = is_magic_projectile_.getValue();
  my_description->projectile_range_sq = projectile_.getRange() * projectile_.getRange();
  projectile_.compile(event_compiler, &my_description->projectile);

  // Generate the treasure that this monster drops
  my_description->treasure
    = treasure_.compile(treasure_compiler);

  return true;
}



//----[  preview  ]------------------------------------------------------------
bool LifeformAI_ProjectileMonster::preview(
    Evidyon::Tools::ActorProfileElement** actor_profile,
    Evidyon::Tools::SkinnedMeshFormElement** form) {
  assert(actor_profile);
  assert(form);
  actor_form_.compile(actor_profile, form);
  return (*actor_profile) && (*form);
}


//----[  getMonsterTreasure  ]-------------------------------------------------
bool LifeformAI_ProjectileMonster::getMonsterTreasure(
    Treasure::Tools::MonsterTreasure** treasure,
    int* intended_level) {
  *treasure = &treasure_;
  *intended_level = treasure_level_.getValue();
  return true;
}




}
}
}
