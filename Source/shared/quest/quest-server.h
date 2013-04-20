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
#ifndef __QUEST_SERVER_H__
#define __QUEST_SERVER_H__
#pragma once

#include "../avatar/evidyon_avatar.h"
#include "../calendar/calendar.h"
#include "quest-encryption.h"
#include "quest-irreversible-failure-reason.h"
#include "../currencytype.h"



namespace Evidyon {
namespace Quests {

//----[  QuestQualification  ]-------------------------------------------------
enum QuestQualification {
  QUESTQUALIFICATION_SUCCEEDED,
  QUESTQUALIFICATION_FAILED_ON_TOO_MANY_QUESTS,
  QUESTQUALIFICATION_FAILED_TEMPORARILY,
  QUESTQUALIFICATION_FAILED_PERMANENTLY,
};



//----[  QuestStatus  ]--------------------------------------------------------
enum QuestStatus {
  QUESTSTATUS_SUCCEEDED,
  QUESTSTATUS_SUCCEEDED_OVER_TIME_LIMIT,
  QUESTSTATUS_FAILED_TIMELIMIT,
  QUESTSTATUS_FAILED_BY_HIGH_DEATH_COUNT,
  QUESTSTATUS_FAILED_BY_CHANGING_MAP,
  QUESTSTATUS_FAILED_BY_TOO_MANY_PKS,
  QUESTSTATUS_FAILED_BY_COMPETITOR_WINNING,
  QUESTSTATUS_UNCHANGED,
};

namespace Server {



typedef unsigned int QuestHash;
typedef int QuestIndex;
static const QuestHash QUESTHASH_INVALID = 0;




//----[  QuestItemReference  ]-------------------------------------------------
struct QuestItemReference {

  // The item's index in the global array; this is NOT the permanent hash.
  unsigned int item_index;

  // The quantity of items.  If this is less than zero, the item reference
  // should be discarded.
  int quantity;

};




//----[  QuestAction  ]--------------------------------------------------------
struct QuestAction {
  static const int ITEMS_TO_REMOVE = 3;
  static const int ITEMS_TO_GIVE = 8;

  // Take these items from the player
  QuestItemReference items_to_remove[ITEMS_TO_REMOVE];

  // Give these items to the player.  If the number is less than 0, give all;
  // otherwise, give a random mixture.  A single item will not be given more
  // than one time, unless the number of items to give is greater than the
  // number of valid items.
  QuestItemReference items_to_give[ITEMS_TO_GIVE];
  int number_of_items_to_give;

  // Changes to various parameters
  int change_experience,
      change_money,
      change_geonite,
      change_str,
      change_agi,
      change_con,
      change_int,
      change_wis;

  // Move to a location.  If the map index is less than 0, ignore this field.
  //static const bool move_instantly = false; // true: teleport, false: geonite portal, always true for now...effect not implemented
  int move_to_map_index;
  int move_to_x, move_to_y;
};






//----[  Quest  ]--------------------------------------------------------------
struct Quest {
  static const QuestIndex QUEST_INDEX_NONE = -1;
  static const QuestIndex QUEST_INDEX_THIS = -2;
  static const int PREREQ_CARRIED_ITEMS = 3;
  static const int PREREQ_EQUIPPED_ITEMS = 3;
  static const int COST_ITEMS = 2;
  static const int REQUIRED_ITEMS = 4;

  // This unique ID number is storeable in the database to identify this quest
  // and is persistent between versions of the game file that contain the
  // same quest.
  QuestHash quest_permanent_hash;
  QuestIndex this_quest_index;

  // This key is passed to the client in order to decrypt the description of
  // this quest.
  QuestEncryptionWord client_encryption_key[QUEST_ENCRYPTION_KEY_WORDS];

  // All of these quests must be completed in order for this quest to count be
  // available to the player.
  QuestHash has_successfully_completed[3];

  // If this value is less than zero, it indicates that no time limit applies.
  int max_minutes_since_successfully_completed;

  // Index of a quest that, if completed, will make this quest unavailable
  QuestHash has_not_successfully_completed[2];

  // Must wait at least a certain number of minutes after attempting this quest
  // (resulting in either failure or success) before trying again.
  int minutes_to_wait_between_attempts;

  // This quest is only available to those avatars for whom:
  //  (1 & (mask >> (race * AVATARCLASS_COUNT + class))) == 1
  unsigned int avatar_profession_mask;

  // Level and alignment ranges at which the quest is available.
  // Values are inclusive.
  int min_level_to_start, max_level_to_start;
  int min_alignment_to_start, max_alignment_to_start;

  // Time-based masks for availability.  These follow the Calendar patterns.
  unsigned int days, weeks, months;

  // The player must have these items in inventory for the quest to be available
  QuestItemReference carried_items[PREREQ_CARRIED_ITEMS];
  QuestItemReference equipped_items[PREREQ_EQUIPPED_ITEMS];


  // The player has to pay this amount in order to attempt the quest
  CurrencyType currency_cost_to_attempt;
  unsigned int geonite_cost_to_attempt;
  QuestItemReference item_cost_to_attempt[COST_ITEMS];


  // Once the player has accepted the quest, this action is performed.
  QuestAction action_on_begin_quest;

  // Whether having one person succeed at this quest causes all others to fail.
  // This flag is usually combined with terminate_immediately.
  bool competitive;

  // Whether to execute the success/failure actions of a quest immediately
  // when the conditions for that result are met, or to wait until an
  // external trigger occurs.
  bool terminate_immediately;

  // The player fails the quest if their death count exceeds this value
  int maximum_death_count;

  // If the user changes maps, this quest is failed.  This can be used
  // for map-specific quests, such as one that is automatically
  // bestowed when the user enters a dungeon.
  bool fail_on_change_map;

  // Can't take more time than this to finish the quest.
  int time_limit_minutes;

  // If this flag is set, once time expires the user completely fails the
  // quest and the failure action is taken immediately.  Otherwise, the
  // user can either succeed and the over-time-limit action is taken or
  // fail some other way.
  bool fail_over_time_limit;

  // # of lifeforms killed overall
  int kill_overall_lifeform_min_level;
  int kill_overall_lifeforms_count;

  // Quester must kill this number of lifeforms of a specific type
  int kill_specific_lifeform_type;
  int kill_specific_lifeform_count;
  int kill_specific_lifeform_min_level;

  // The user must possess the given items (if valid) in order to
  // succeed at the quest.  Furthermore, these items must have been
  // created at or after the character began the quest.
  QuestItemReference required_items[REQUIRED_ITEMS];

  // Inclusive ranges that define success
  int min_pk_count_to_succeed, max_pk_count_to_succeed;
  int min_alignment_to_succeed, max_alignment_to_succeed;

  // Actions that are taken at the end of a quest based on the results.
  // The success over-time-limit action is always equal to the failure
  // action if fail_over_time_limit is true.
  QuestAction action_on_success,
              action_on_success_over_time_limit,
              action_on_failure;
};

}
}
}
#endif