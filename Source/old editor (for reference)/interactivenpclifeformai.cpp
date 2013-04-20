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
#include "interactivenpclifeformai.h"
#include "../shared/actor/actordescriptiontemplate.h"
#include "actordescriptiontemplatecompiler.h"

// need access to the global editor
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"

#include "../common/randint32.h"

dc::dcClass<InteractiveNPCLifeformAI>::Implements<LifeformAI>
InteractiveNPCLifeformAI::implementsLifeformAI;




//----[  CompileQuestPhrase  ]-------------------------------------------------
// Writes a quest into the buffer, reformatting and checking to make sure it
// doesn't overrun the buffer.
void CompileQuestPhrase(const dc::dcString& source, char* dest) {
  const char* source_chars = source.getValue().c_str();
  static const int LAST_QUEST_PHRASE_INDEX =
    LifeformAIDescription_InteractiveNPC::QUEST_PHRASE_LENGTH - 1;
  int dest_length = 0;
  char ch;
  while ((ch = *source_chars) != '\0' && dest_length < LAST_QUEST_PHRASE_INDEX) {
    *dest = ch;
    ++source_chars;
    ++dest_length;
    ++dest;
/* // this is no longer done because I changed how the text works on the server
    // make % into %s
    if (ch == '%') {
      if (dest_length == LAST_QUEST_PHRASE_INDEX) {
        --dest; // overwrite the % since we're out of space
      }
      *dest = 's';
      ++dest;
      ++dest_length;
    }*/
  }
  *dest = '\0';
}


//----[  NPCQuestInitiation  ]-------------------------------------------------
NPCQuestInitiation::NPCQuestInitiation() {
  member("Quest", &quest_);
  member("Phrase: Qualifies for Quest (Offer)", &qualifies_for_quest_);
  member("Phrase: Accepted Quest", &accepted_quest_);
  member("Phrase: Accepted Quest, Cant Pay", &accepted_quest_cant_pay_);
  member("Phrase: Declined Quest", &declined_quest_);
}



//----[  compileForServer  ]---------------------------------------------------
void NPCQuestInitiation::compileForServer(
    Evidyon::LifeformAIDescription_InteractiveNPC::QuestInitiation* description) const {
  description->quest_index = quest_.getReferencedResourceIndex();
  description->quest_validation_number = rand_uint32();
  CompileQuestPhrase(qualifies_for_quest_, description->qualifies_for_quest);
  CompileQuestPhrase(accepted_quest_, description->accepted_quest);
  CompileQuestPhrase(accepted_quest_cant_pay_, description->accepted_quest_cant_pay);
  CompileQuestPhrase(declined_quest_, description->declined_quest);
}


//----[  NPCQuestProgress  ]---------------------------------------------------
NPCQuestProgress::NPCQuestProgress() {
  member("Quest", &quest_);
  member("Phrase: Getting Started (<5 mins in)", &getting_started_);
  member("Phrase: In Progress (>5 mins into first half)", &in_progress_);
  member("Phrase: Time Half Expired (>5 mins left in second half)", &time_half_expired_);
  member("Phrase: Nearly Out of Time (< 5 mins left)", &nearly_out_of_time_);
  member("Phrase: Time Has Expired", &time_has_expired_);
}



//----[  compileForServer  ]---------------------------------------------------
void NPCQuestProgress::compileForServer(
    Evidyon::LifeformAIDescription_InteractiveNPC::QuestProgress* description) const {
  description->quest_index = quest_.getReferencedResourceIndex();
  CompileQuestPhrase(getting_started_, description->getting_started);
  CompileQuestPhrase(in_progress_, description->in_progress);
  CompileQuestPhrase(time_half_expired_, description->time_half_expired);
  CompileQuestPhrase(nearly_out_of_time_, description->nearly_out_of_time);
  CompileQuestPhrase(time_has_expired_, description->time_has_expired);
}



//----[  NPCQuestTermination  ]------------------------------------------------
NPCQuestTermination::NPCQuestTermination() {
  member("Quest", &quest_);
  member("Phrase: Succeeded", &succeeded_);
  member("Phrase: Time Failure", &time_failure_);
  member("Phrase: Succeeded Over Time Limit", &succeeded_over_time_limit_);

  for (int i = 0; i < Evidyon::Quests::QUEST_IRREVERSIBLE_FAILURE_REASON_COUNT; ++i) {
    const char* str = Evidyon::Quests::QuestIrreversibleFailureReasonString(
      Evidyon::Quests::QuestIrreversibleFailureReason(i));
    member(std::string("Phrase: Failed (") + str + std::string(")"),
           &failed_[i]);
  }
}



//----[  compileForServer  ]---------------------------------------------------
void NPCQuestTermination::compileForServer(
    Evidyon::LifeformAIDescription_InteractiveNPC::QuestTermination* description) const {
  description->quest_index = quest_.getReferencedResourceIndex();
  CompileQuestPhrase(succeeded_, description->succeeded);
  CompileQuestPhrase(time_failure_, description->time_failure);
  CompileQuestPhrase(succeeded_over_time_limit_, description->succeeded_over_time_limit);
  for (int i = 0; i < Evidyon::Quests::QUEST_IRREVERSIBLE_FAILURE_REASON_COUNT; ++i) {
    CompileQuestPhrase(failed_[i], description->failed[i]);
  }
}



//----[  InteractiveNPCLifeformAI  ]-------------------------------------------
InteractiveNPCLifeformAI::InteractiveNPCLifeformAI() {
  member("Name", &name_);
  member("Actor Type", &actor_type_);
  member("Interaction Type", &interaction_type_);
  member("Wander?", &wander_);

  for (int i = 0; i < Evidyon::ACTORATTACHMENTPOINT_COUNT; ++i) {
    const char* point = dc::dcEnum<Evidyon::ActorAttachmentPoint>::getString((Evidyon::ActorAttachmentPoint)i);
    std::string name = "";
    name.append(point);
    member(name, &equipped_items_[i]);
  }

  interaction_type_.setValue(Evidyon::NPCINTERACTION_STORAGE);

  member("Quest Initiated [0]", &quests_initiated_[0]);
  member("Quest Initiated [1]", &quests_initiated_[1]);
  member("Phrase:  Has no quests for avatar, ever", &has_no_quests_for_avatar_);
  member("Phrase: Could have quest for avatar", &could_have_quest_for_avatar_);
  member("Phrase: Has quest, but avatar on too many",
         &has_quest_but_avatar_on_too_many_already_);
  member("Quest Progress Check [0]", &quests_progress_[0]);
  member("Quest Progress Check [1]", &quests_progress_[1]);
  member("Quest Terminated [0]", &quests_terminated_[0]);
  member("Quest Terminated [1]", &quests_terminated_[1]);
}





//----[  getLifeformName  ]----------------------------------------------------
const std::string& InteractiveNPCLifeformAI::getLifeformName() const {
  return name_.getValue();
}




//----[  compileActorTemplates  ]----------------------------------------------
bool InteractiveNPCLifeformAI::compileActorTemplates(ActorDescriptionTemplateCompiler* compiler) {
  compiled_actor_template_index_ = compiler->getNextTemplateIndex();
  Evidyon::ClientActorDescriptionTemplate client_description;
  Evidyon::ServerActorDescriptionTemplate server_description;

  for (int i = 0; i < Evidyon::ACTORACTION_VARIABLE_SPEED_COUNT; ++i) {
    client_description.action_speeds[i] = 1.0f;
  }

  client_description.enchantments = 0;
  client_description.description_can_change = false;
  client_description.actor_type = actor_type_.getReferencedResourceIndex();
  strncpy_s(client_description.name, MAX_ACTOR_NAME_LENGTH, name_.getValue().c_str(), MAX_ACTOR_NAME_LENGTH);
  client_description.alignment = ALIGNMENT_GOOD; // NPCs are good!
  switch (interaction_type_.getValue()) {
    case Evidyon::NPCINTERACTION_STORAGE:  client_description.template_type = ACTORTEMPLATETYPE_NPC_STORAGE; break;
    case Evidyon::NPCINTERACTION_BAZAAR:   client_description.template_type = ACTORTEMPLATETYPE_NPC_BAZAAR; break;
    case Evidyon::NPCINTERACTION_GEOSID:   client_description.template_type = ACTORTEMPLATETYPE_NPC_GEOSID; break;
    case Evidyon::NPCINTERACTION_QUEST:    client_description.template_type = ACTORTEMPLATETYPE_NPC_QUESTGIVER; break;
    case Evidyon::NPCINTERACTION_MERCHANT: client_description.template_type = ACTORTEMPLATETYPE_NPC_MERCHANT; break;
  }

  for (int i = 0; i < Evidyon::ACTORATTACHMENTPOINT_COUNT; ++i) {
    dcList<Item>::Element* item_element = equipped_items_[i].getReferencedResource();
    size_t scenery_index = 0xFFFFFFFF;
    if (item_element) {
      scenery_index = item_element->getSceneryReference()->getReferencedResourceIndex();
    }
    client_description.equipped_item_scenery[i] = scenery_index;
  }

  server_description.actor_type = client_description.actor_type;

  compiler->addActorDescriptionTemplate(&client_description, &server_description);

  return true;
}




//----[  compileForServer  ]---------------------------------------------------
bool InteractiveNPCLifeformAI::compileForServer(
    Evidyon::LifeformAIDescription* description) const {
  description->type = Evidyon::LifeformAIDescription::INTERACTIVE_NPC;
  description->interactive_npc.interaction = interaction_type_.getValue();
  description->interactive_npc.actor_template_index = compiled_actor_template_index_;

  quests_initiated_[0].compileForServer(&description->interactive_npc.quests_initiated[0]);
  quests_initiated_[1].compileForServer(&description->interactive_npc.quests_initiated[1]);
  CompileQuestPhrase(has_no_quests_for_avatar_,
                     description->interactive_npc.has_no_quests_for_avatar);
  CompileQuestPhrase(could_have_quest_for_avatar_,
                     description->interactive_npc.could_have_quest_for_avatar);
  CompileQuestPhrase(has_quest_but_avatar_on_too_many_already_,
                     description->interactive_npc.has_quest_but_avatar_on_too_many_already);
  quests_progress_[0].compileForServer(&description->interactive_npc.quests_progress[0]);
  quests_progress_[1].compileForServer(&description->interactive_npc.quests_progress[1]);
  quests_terminated_[0].compileForServer(&description->interactive_npc.quests_terminated[0]);
  quests_terminated_[1].compileForServer(&description->interactive_npc.quests_terminated[1]);

  return true;
}





//----[  staticTypeName  ]-----------------------------------------------------
std::string InteractiveNPCLifeformAI::staticTypeName() {
  return "InteractiveNPCLifeformAI";
}
