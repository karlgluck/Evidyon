//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __QUEST_TOOLS_H__
#define __QUEST_TOOLS_H__
#pragma once

#include <dc/string>
#include <dc/boolean>
#include <dc/list>
#include <dc/dcreference.h>
#include <dc/integer>
#include <dc/table>

#include "../avatar/avatarrace.h"
#include "../avatar/avatarclass.h"
#include "../avatar/avatargender.h"
#include "../calendar/calendar.h"

// Compiled formats
#include "quest-server.h"
#include "quest-client.h"


class Quest;


//----[  QuestItemReference  ]-------------------------------------------------
class QuestItemReference : public dc::dcResource<QuestItemReference> {
public:
  QuestItemReference();

  // Generates the game file description for the server.
  void compile(Evidyon::Quests::Server::QuestItemReference* compiled_reference) const;

  // Returns the name of this resource
  dc::dcList<Item>::Reference* getItemReference() { return &item_; }
  static std::string staticTypeName() { return "QuestItemReference"; }
private:
  dc::dcList<Item>::Reference item_;
  dc::dcInteger quantity_;
};



//----[  QuestAction  ]--------------------------------------------------------
class QuestAction : public dc::dcResource<QuestAction> {
public:
  QuestAction();

  // Generates the game file description for the server.
  void compile(Evidyon::Quests::Server::QuestAction* compiled_action) const;

  // Returns the name of this resource
  static std::string staticTypeName() { return "QuestAction"; }
private:
  dc::dcList<QuestItemReference> items_to_remove_;
  dc::dcList<QuestItemReference> items_to_give_;
  dc::dcInteger number_of_items_to_give_;
  dc::dcInteger experience_change_;
  dc::dcInteger gold_change_;
  dc::dcInteger geonite_change_;
  dc::dcInteger str_change_;
  dc::dcInteger agi_change_;
  dc::dcInteger con_change_;
  dc::dcInteger int_change_;
  dc::dcInteger wis_change_;

  dc::dcList<Atlas>::Reference move_to_map_;
  dc::dcInteger move_to_x_, move_to_y_;
};





class Quest;


//----[  QuestAvailability  ]--------------------------------------------------
class QuestAvailability : public dc::dcResource<QuestAvailability> {
public:
  QuestAvailability();

  // Generates the game file description for the server.
  void compile(Evidyon::Quests::Server::Quest* quest) const;

  // Returns the name of this resource
  static std::string staticTypeName() { return "QuestAvailability"; }
private:

  // Quest-based flags
  dc::dcList<Quest>::Reference has_successfully_completed_[3];
  dc::dcInteger time_since_successfully_completed_maximum_minutes_;
  dc::dcList<Quest>::Reference has_not_successfully_completed_[2];
  dc::dcInteger minutes_to_wait_between_attempts_;

  // Avatar-based flags
  dc::dcBoolean races_[Evidyon::Avatar::NUMBER_OF_AVATAR_RACES];
  dc::dcBoolean classes_[Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES];
  dc::dcInteger minimum_level_;
  dc::dcInteger maximum_level_;
  dc::dcInteger minimum_alignment_;
  dc::dcInteger maximum_alignment_;

  // Time-based flags
  dc::dcBoolean days_[DAYS_IN_WEEK];
  dc::dcBoolean weeks_[WEEKS_IN_MONTH];
  dc::dcBoolean months_[MONTHS_IN_YEAR];

  // Misc. flags
  dc::dcList<QuestItemReference> carried_items_;
  dc::dcList<QuestItemReference> equipped_items_;
};










//----[  QuestAttemptCost  ]---------------------------------------------------
class QuestAttemptCost : public dc::dcResource<QuestAttemptCost> {
public:
  QuestAttemptCost();

  // Generates the game file description for the server.
  void compile(Evidyon::Quests::Server::Quest* quest) const;

  // Returns the name of this resource
  static std::string staticTypeName() { return "QuestAttemptCost"; }
private:
  dc::dcInteger money_;
  dc::dcInteger geonite_;
  dc::dcList<QuestItemReference> items_;
};





//----[  QuestLifeformAIReference  ]-------------------------------------------
class QuestLifeformAIReference : public dc::dcResource<QuestLifeformAIReference> {
public:
  QuestLifeformAIReference();

  // Generates the game file description for the server.
  void compile(int* lifeform_type,
               int* lifeform_quantity,
               int* minimum_level) const;

  // Returns the name of this resource
  static std::string staticTypeName() { return "QuestLifeformAIReference"; }
private:
  dc::dcTable<LifeformAI>::Reference lifeform_ai_;
  dc::dcInteger quantity_;
  dc::dcInteger minimum_level_;
};




//----[  QuestTermination  ]---------------------------------------------------
// These conditions are checked every time a player dies, pks, changes map,
// clicks a quest-trigger (NPC or location, defined externally), or the quest
// timer expires.
class QuestTermination : public dc::dcResource<QuestTermination> {
public:
  QuestTermination();

  // Generates the game file description for the server.
  void compile(Evidyon::Quests::Server::Quest* quest) const;

  // Returns the name of this resource
  static std::string staticTypeName() { return "QuestTermination"; }
private:
  // once one succeeds, reset fail, quest ends and competition resets
  dc::dcBoolean competitive_;
  dc::dcBoolean terminate_immediately_; // check conditions constantly?
  dc::dcInteger maximum_death_count_; // inclusive; 1 = 1 death ok
  dc::dcBoolean fail_on_change_map_; // triggered if you change maps
  dc::dcInteger time_limit_minutes_;
  dc::dcBoolean fail_over_time_limit_;
  dc::dcInteger minimum_lifeforms_killed_lowest_qualifying_level_;
  dc::dcInteger minimum_lifeforms_killed_;
  QuestLifeformAIReference minimum_specific_lifeforms_to_kill_;
  dc::dcList<QuestItemReference> required_items_; // you need these
  dc::dcInteger minimum_pk_count_, maximum_pk_count_; // inclusive
  dc::dcInteger minimum_alignment_, maximum_alignment_; // inclusive
  QuestAction success_,
              success_over_time_limit_,
              failure_;
};






//----[  Quest  ]--------------------------------------------------------------
class Quest : public dc::dcResource<Quest> {
public:
  Quest();

  // Generates the game file description for the server.
  void compile(Evidyon::Quests::Server::Quest* quest) const;

  // Builds the game file description for the client.  This method will
  // encrypt the quest description.
  void compile(Evidyon::Quests::Client::Quest* quest) const;

  // Returns the unique hash of this quest
  Evidyon::Quests::Server::QuestHash getQuestHash() const;

  // Returns the name of this resource
  static std::string staticTypeName() { return "Quest"; }


private:

  // What this class is commonly known as
  dc::dcString displayed_name_;

  // The proposal that is displayed to the client playing the game when this
  // quest is available and can be attempted.  It needs to include all of the
  // relavent information for the quest, since it's not built automatically
  // during compilation.
  // The character \ will be interpreted as a line break and \\ as a \.
  dc::dcString description_;

  // What has to be true in order for this quest to be displayed
  QuestAvailability availability_;

  // What must be taken from a player in order to accept the quest
  QuestAttemptCost cost_;

  // This happens when the quest begins
  QuestAction action_on_begin_;

  // Ways this quest can end
  QuestTermination termination_;
};




#endif