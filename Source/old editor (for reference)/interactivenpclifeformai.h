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
#ifndef __INTERACTIVENPCLIFEFORMAI_H__
#define __INTERACTIVENPCLIFEFORMAI_H__
#pragma once

#include <dc/float>
#include <dc/list>
#include <dc/integer>
#include <dc/string>
#include "actortype.h"
#include "lifeformai.h"
#include "aimeleeattack.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "../shared/actor/evidyon_actor.h"
#include "normalmeleemonsterlifeformaiactortype.h"
#include "treasureclass.h"
#include "(quest).h"


namespace Evidyon {
struct LifeformAIDescription;
}



//----[  NPCQuestInitiation  ]-------------------------------------------------
class NPCQuestInitiation : public dc::dcResource<NPCQuestInitiation> {
public:
  NPCQuestInitiation();
  void compileForServer(Evidyon::LifeformAIDescription_InteractiveNPC::QuestInitiation* description) const;
  static std::string staticTypeName() { return "NPCQuestInitiation"; }
private:
  dc::dcList<Quest>::Reference quest_;
  dc::dcString qualifies_for_quest_;
  dc::dcString accepted_quest_;
  dc::dcString accepted_quest_cant_pay_;
  dc::dcString declined_quest_;
};



//----[  NPCQuestProgress  ]---------------------------------------------------
class NPCQuestProgress : public dc::dcResource<NPCQuestProgress> {
public:
  NPCQuestProgress();
  void compileForServer(Evidyon::LifeformAIDescription_InteractiveNPC::QuestProgress* description) const;
  static std::string staticTypeName() { return "NPCQuestProgress"; }
private:
  dc::dcList<Quest>::Reference quest_;
  dc::dcString getting_started_;  // < 5 minutes since start
  dc::dcString in_progress_;      // > 5 minutes since start, > 50% left
  dc::dcString time_half_expired_;// < 50% left
  dc::dcString nearly_out_of_time_;// <5 minutes left
  dc::dcString time_has_expired_;
};



//----[  NPCQuestTermination  ]------------------------------------------------
class NPCQuestTermination : public dc::dcResource<NPCQuestTermination> {
public:
  NPCQuestTermination();
  void compileForServer(Evidyon::LifeformAIDescription_InteractiveNPC::QuestTermination* description) const;
  static std::string staticTypeName() { return "NPCQuestTermination"; }
private:
  dc::dcList<Quest>::Reference quest_;
  dc::dcString succeeded_;
  dc::dcString time_failure_;
  dc::dcString succeeded_over_time_limit_;
  dc::dcString failed_[Evidyon::Quests::QUEST_IRREVERSIBLE_FAILURE_REASON_COUNT];
};



//----[  InteractiveNPCLifeformAI  ]-------------------------------------------
class InteractiveNPCLifeformAI : public dc::dcResource<InteractiveNPCLifeformAI, LifeformAI> {
public:
  InteractiveNPCLifeformAI();
  virtual const std::string& getLifeformName() const;
  virtual bool compileActorTemplates(ActorDescriptionTemplateCompiler* compiler);
  virtual bool compileForServer(Evidyon::LifeformAIDescription* description) const;

public:
  static std::string staticTypeName();

private:
  dc::dcString name_;
  dc::dcList<ActorType>::Reference actor_type_;
  dc::dcEnum<Evidyon::NPCInteractionType> interaction_type_;
  dc::dcBoolean wander_; // move around?
  dc::dcList<Item>::Reference equipped_items_[Evidyon::ACTORATTACHMENTPOINT_COUNT];

  NPCQuestInitiation quests_initiated_[2];
  dc::dcString has_no_quests_for_avatar_;
  dc::dcString could_have_quest_for_avatar_;
  dc::dcString has_quest_but_avatar_on_too_many_already_;
  NPCQuestProgress quests_progress_[2];
  NPCQuestTermination quests_terminated_[2];

private:
  unsigned int compiled_actor_template_index_;

private:
  static dc::dcClass<InteractiveNPCLifeformAI>::Implements<LifeformAI> implementsLifeformAI;
};



#endif