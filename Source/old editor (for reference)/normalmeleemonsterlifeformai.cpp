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
#include "normalmeleemonsterlifeformai.h"
#include "../shared/actor/actordescriptiontemplate.h"
#include "actordescriptiontemplatecompiler.h"

// need access to the global editor
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"


dc::dcClass<NormalMeleeMonsterLifeformAI>::Implements<LifeformAI> NormalMeleeMonsterLifeformAI::implementsLifeformAI;



NormalMeleeMonsterLifeformAI::NormalMeleeMonsterLifeformAI() {
  member("Name", &name_);
  member("Actor Types", &actor_types_);
  for (int i = 0; i < (int)Evidyon::ACTORATTACK_COUNT; ++i) {
    std::string name = dc::dcEnum<ActorAttack>::getString((ActorAttack)i);
    name.append(" Duration");
    member(name,
          &attack_durations_[i]);
  }
  for (int i = 0; i < Evidyon::LIFEFORMAI_MELEEATTACK_TYPES; ++i) {
    char name[64];
    sprintf_s(name, "Melee Attack %i", i+1);
    member(name, &attack_types_[i]);
  }
  member("Movement Speed", &movement_speed_);
  member("Time Between Attacks", &time_between_attacks_);
  member("Intended Level", &intended_level_);
  member("Seconds to Kill:  Melee", &seconds_to_kill_melee_);
  member("Seconds to Kill:  Magic", &seconds_to_kill_magic_);
  member("Additional Seconds Per Level", &additional_seconds_per_level_);
  member("Average Time Between Monsters", &average_time_between_monsters_);
  member("Additional EXP Per Level", &additional_exp_per_level_);
  member("Treasure Class", &treasure_class_);
}


const std::string& NormalMeleeMonsterLifeformAI::getLifeformName() const {
  return name_.getValue();
}

bool NormalMeleeMonsterLifeformAI::compileForServer(Evidyon::LifeformAIDescription* description) const {
  description->type = Evidyon::LifeformAIDescription::NORMAL_MELEE_MONSTER;

  const dc::dcGenericResource::Array& types = actor_types_.getMembers();
  CONFIRM(types.empty() == false) else return false;

  dc::dcGenericResource::Array::const_iterator types_iter = types.begin();
  int types_compiled = 0;
  for (; types_compiled < Evidyon::LIFEFORMAI_NORMALMELEE_ACTORTYPES; ++types_iter, ++types_compiled) {
    if (types_iter == types.end()) {
      types_iter = types.begin();
    }
    NormalMeleeMonsterLifeformAIActorType* type = 
      (NormalMeleeMonsterLifeformAIActorType*)(*types_iter);

    CONFIRM(type->compileForServer(&description->normal_melee_monster.actor_types[types_compiled])) else {
      return false;
    }
  }

  description->normal_melee_monster.this_lifeform_ai_type =
    getOwner()->getIndex();

  float attack_durations[ACTORATTACK_COUNT];
  for (int i = 0; i < Evidyon::ACTORATTACK_COUNT; ++i) {
    attack_durations[i] = attack_durations_[i].getValue();
  }

  int intended_level = intended_level_.getValue();
  const LevelAttributes* intended_level_attributes =
    globalEditor()->getRootResource()->getLevelDefinitions()->getLevelAttributes(intended_level);
  ASSERT(intended_level_attributes) else return false;

  { // configure the monster's attacks
    float time_between_attacks = time_between_attacks_.getValue();
    for (int i = 0; i < Evidyon::LIFEFORMAI_MELEEATTACK_TYPES; ++i) {
      attack_types_[i].compile(intended_level_attributes,
                               intended_level,
                               time_between_attacks,
                               attack_durations,
                              &description->normal_melee_monster.melee_attack_types[i]);
    }

    description->normal_melee_monster.time_between_attacks = time_between_attacks;
  }

  description->normal_melee_monster.movement_speed = movement_speed_.getValue();


  { // configure the monster's HP

    // get the amount of damage characters can deal at the
    // level for which this monster is intended
    float intended_level_melee_dps = intended_level_attributes->getMeleeDPS();
    float intended_level_magic_dps = intended_level_attributes->getMagicDPS();

    // calculate how much HP the monster has to have in order
    // to survive for the configured amount of time
    float melee_hp = intended_level_melee_dps * seconds_to_kill_melee_.getValue();
    float magic_hp = intended_level_magic_dps * seconds_to_kill_magic_.getValue();

    // how much less is the magic HP?  this is the magic damage factor
    description->normal_melee_monster.magic_damage_factor =
      melee_hp / magic_hp;

    float additional_health_per_level = intended_level_melee_dps * additional_seconds_per_level_.getValue();
    description->normal_melee_monster.hp =
      (int)(melee_hp - additional_health_per_level * intended_level);
    description->normal_melee_monster.added_hp_per_level =
      (int)additional_health_per_level;
  }

  { // configure how much experience this monster gives
    int intended_level = intended_level_.getValue();
    int exp_to_advance = intended_level_attributes->getExperienceToNextLevel();

    float seconds_to_kill_average = (seconds_to_kill_melee_.getValue() +
                                     seconds_to_kill_magic_.getValue()) / 2.0f;
    float seconds_to_find_and_kill = seconds_to_kill_average + average_time_between_monsters_.getValue();
    float exp = seconds_to_find_and_kill * intended_level_attributes->getExpectedExpPerSecond();

    { // make sure nothing went wrong with this
      float min_exp = (float)intended_level;
      CONFIRM(exp > min_exp) else {
        exp = min_exp;
      }
    }

    int additional_exp_per_level = additional_exp_per_level_.getValue();
    int exp_base = ((int)exp) - additional_exp_per_level * intended_level;
    
    // always give at least 'intended_level' exp
    description->normal_melee_monster.experience = max(exp_base, intended_level);
    description->normal_melee_monster.added_experience_per_level = additional_exp_per_level;
  }

  // save the monster's loot
  description->normal_melee_monster.treasure_class_index = treasure_class_.getReferencedResourceIndex();
  
  return true;
}




//----[  compileActorTemplates  ]----------------------------------------------
bool NormalMeleeMonsterLifeformAI::compileActorTemplates(ActorDescriptionTemplateCompiler* compiler) {

  const dc::dcGenericResource::Array& types = actor_types_.getMembers();
  dc::dcGenericResource::Array::const_iterator types_iter = types.begin();
  int types_compiled = 0;
  for (; types_compiled < Evidyon::LIFEFORMAI_NORMALMELEE_ACTORTYPES &&
         (types_iter != types.end()); ++types_iter) {

    NormalMeleeMonsterLifeformAIActorType* type = 
      (NormalMeleeMonsterLifeformAIActorType*)(*types_iter);
    Evidyon::ClientActorDescriptionTemplate client_description;
    Evidyon::ServerActorDescriptionTemplate server_description;

    CONFIRM(type->compileActorTemplate(compiler->getNextTemplateIndex(),
                                      &client_description,
                                      &server_description)) else {
      continue;
    }

    ++types_compiled;

    client_description.description_can_change = false;
    client_description.alignment = ALIGNMENT_EVIL;

    strcpy_s(client_description.name,
             Evidyon::MAX_ACTOR_NAME_LENGTH,
             name_.getValue().c_str());

    { // compile the action speeds

      // initialize default speeds
      for (int i = 0; i < Evidyon::ACTORACTION_VARIABLE_SPEED_COUNT; ++i) {
        client_description.action_speeds[i] = 1.0f;
      }

      // modify the attack speeds
      for (int i = 0; i < Evidyon::ACTORATTACK_COUNT; ++i) {
        client_description.action_speeds[ACTORACTION_ATTACK1 + i] =
            1.0f / attack_durations_[i].getValue();
      }
    }

    // Append this description to the compiler
    compiler->addActorDescriptionTemplate(&client_description,
                                          &server_description);
  }

  bool succeeded = types_compiled > 0;
  CONFIRM(succeeded);
  return succeeded;
}



std::string NormalMeleeMonsterLifeformAI::staticTypeName() {
  return "NormalMeleeMonsterLifeformAI";
}
