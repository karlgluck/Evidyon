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
#ifndef __LIFEFORMAIDESCRIPTION_H__
#define __LIFEFORMAIDESCRIPTION_H__
#pragma once



#include "../actor/evidyon_actor.h"
#include "../evidyon_npc.h"
#include "../quest/quest-irreversible-failure-reason.h"
#include "shared/actor/actoraction.h"

namespace Evidyon {

static const int LIFEFORMAI_NORMALMELEE_ACTORTYPES = 4;
static const int LIFEFORMAI_MELEEATTACK_TYPES = 3;



//----[  AIMeleeAttackDescription  ]-------------------------------------------
struct AIMeleeAttackDescription {
  float range; // range to target within which to use this ability
  float duration; // seconds before applying damage
  Actor::ActorAction action;
  int base_damage, random_additional_damage;
  int added_damage_per_level;
};



//----[  AIMonsterActorType  ]-------------------------------------------------
struct AIMonsterActorType {
  unsigned int template_index;
  float size; // this is redundant--the template index references a struct with this in it
};



//----[  LifeformAIDescription_NormalMeleeMonster  ]---------------------------
struct LifeformAIDescription_NormalMeleeMonster {
  AIMonsterActorType actor_types[LIFEFORMAI_NORMALMELEE_ACTORTYPES];
  AIMeleeAttackDescription melee_attack_types[LIFEFORMAI_MELEEATTACK_TYPES];
  int hp, added_hp_per_level;
  int experience;
  int added_experience_per_level;
  float movement_speed;
  float time_between_attacks;
  int treasure_class_index;
  float magic_damage_factor; // multiply spell damage by this amount
  unsigned int this_lifeform_ai_type;
};






//----[  LifeformAIDescription_InteractiveNPC  ]-------------------------------
// TODO: this structure got super huge when I added quests--way bigger than the
// normal melee monster, which it is unioned with.  This might not be a big
// deal in the future since different kinds of lifeforms might need more data
// but storing all of the speech for an NPC locally seems inefficient.
struct LifeformAIDescription_InteractiveNPC {
  unsigned int actor_template_index;
  NPCInteractionType interaction; // what kind of interaction this npc allows

  static const int QUEST_PHRASE_LENGTH = 128;

  struct QuestInitiation {
    unsigned int quest_index;

    // This number is unrelated to the quest's index and is not correlated with
    // anything about the quest.  Its purpose is to make sure that a client
    // can't spoof acceptance of a quest they haven't been offered.
    unsigned int quest_validation_number;

    char qualifies_for_quest[QUEST_PHRASE_LENGTH];
    char accepted_quest[QUEST_PHRASE_LENGTH];
    char accepted_quest_cant_pay[QUEST_PHRASE_LENGTH];
    char declined_quest[QUEST_PHRASE_LENGTH];
  } quests_initiated[2];

  char has_no_quests_for_avatar[QUEST_PHRASE_LENGTH];
  char could_have_quest_for_avatar[QUEST_PHRASE_LENGTH];
  char has_quest_but_avatar_on_too_many_already[QUEST_PHRASE_LENGTH];

  struct QuestProgress {
    unsigned int quest_index;
    char getting_started[QUEST_PHRASE_LENGTH];
    char in_progress[QUEST_PHRASE_LENGTH];
    char time_half_expired[QUEST_PHRASE_LENGTH];
    char nearly_out_of_time[QUEST_PHRASE_LENGTH];
    char time_has_expired[QUEST_PHRASE_LENGTH];
  } quests_progress[2];

  struct QuestTermination {
    unsigned int quest_index;
    char succeeded[QUEST_PHRASE_LENGTH];
    char time_failure[QUEST_PHRASE_LENGTH];
    char succeeded_over_time_limit[QUEST_PHRASE_LENGTH];
    char failed[Evidyon::Quests::QUEST_IRREVERSIBLE_FAILURE_REASON_COUNT][QUEST_PHRASE_LENGTH];
  } quests_terminated[2];
};



//----[  LifeformAIDescription  ]----------------------------------------------
struct LifeformAIDescription {
  enum Type {
    NORMAL_MELEE_MONSTER,
    INTERACTIVE_NPC,
  } type;

  union {
    LifeformAIDescription_NormalMeleeMonster normal_melee_monster;
    LifeformAIDescription_InteractiveNPC interactive_npc;
  };
};

}


#endif