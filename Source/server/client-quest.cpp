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
#include "client.h"
#include "shared/quest/quest-server.h"
#include "shared/client-server/packets.h"
#include "globalquestmanager.h"
#include "globalitemmanager.h"
#include "time.h"
#include "globaldatabasemanager.h"

#include "common/fracture/todo.h"


namespace Evidyon {


  
//----[  checkQualificationForQuest  ]-----------------------------------------
Quests::QuestQualification Client::checkQualificationForQuest(
    unsigned int quest_index) {

  if (!getEmptyActiveQuest(NULL)) {
    return Quests::QUESTQUALIFICATION_FAILED_ON_TOO_MANY_QUESTS;
  }

  const Quests::Server::Quest* quest
    = Quests::Server::GlobalQuestManager::singleton()->getQuest(quest_index);
  if (!quest) return Quests::QUESTQUALIFICATION_FAILED_PERMANENTLY;;

  // First, check all the reasons that this client cannot ever get this quest:
  //  - The quest isn't offered to this race/class combination
  //  - Has completed a quest that excludes this one
  //  - Is too high level to attempt
  unsigned int profession_shift =
    avatar_.race * Evidyon::Avatar::NUMBER_OF_AVATAR_RACES
    + avatar_.class_;
  if ((((quest->avatar_profession_mask>>profession_shift)&1) == 0) ||
      hasSucceededAtQuestIfValid(false,
                                 quest->has_not_successfully_completed[0],
                                 NULL) ||
      hasSucceededAtQuestIfValid(false,
                                 quest->has_not_successfully_completed[1],
                                 NULL) ||
      avatar_.level > quest->max_level_to_start) {
    return Quests::QUESTQUALIFICATION_FAILED_PERMANENTLY;
  }

  { // Now check to see if it's the right date to offer this quest.
    bool available_today, available_this_week, available_this_month;
    if (!(available_today = 1 == (1&(quest->days >> Time::calendar_weekday))) ||
        !(available_this_week = 1 == (1&(quest->weeks >> Time::calendar_week))) ||
        !(available_this_month = 1 == (1&(quest->months >> Time::calendar_month)))) {
      bool available_later_this_week = 
          available_this_month && available_this_week &&
          ((quest->days >> Time::calendar_weekday) != 0);
      if (available_later_this_week) {
        return Quests::QUESTQUALIFICATION_FAILED_TEMPORARILY;
      }
      bool available_next_week =
        (Time::calendar_week == 3) ?
          ((1 == (1&(quest->months >> ((quest->months + 1) % 12)))) &&
           (1 == (1&(quest->weeks >> 0)))) :
          (available_this_month &&
           (1 == (1&(quest->weeks >> (Time::calendar_week + 1)))));
      if (available_next_week) {
        return Quests::QUESTQUALIFICATION_FAILED_TEMPORARILY;
      } else {
        // this quest won't be available for a while
        return Quests::QUESTQUALIFICATION_FAILED_PERMANENTLY;
      }
    }
  }

  // Next, we check to see if the player isn't wearing the right set of
  // items.  Not fulfilling this requirement results in a "failed permanently"
  // status in order to hide the quests that are triggered this way.
  for (int i = 0; i < Quests::Server::Quest::PREREQ_CARRIED_ITEMS; ++i) {
    int quantity = quest->carried_items[i].quantity;
    if (quantity < 0) break;
    if (!items_.isCarrying(quest->carried_items[i].item_index, quantity))
      return Quests::QUESTQUALIFICATION_FAILED_PERMANENTLY;
  }
  for (int i = 0; i < Quests::Server::Quest::PREREQ_EQUIPPED_ITEMS; ++i) {
    int quantity = quest->equipped_items[i].quantity;
    if (quantity < 0) break;
    if (!items_.hasEquipped(quest->equipped_items[i].item_index, quantity))
      return Quests::QUESTQUALIFICATION_FAILED_PERMANENTLY;
  }

  { // Now check for parameters that don't cause permanent failure, but
    // still block access to the quest.
    int minutes_since_completed = 0;
    static const int max_minutes_since_successfully_completed
      = quest->max_minutes_since_successfully_completed;
    if (avatar_.level < quest->min_level_to_start ||
        avatar_.alignment_value < quest->min_alignment_to_start ||
        avatar_.alignment_value > quest->max_alignment_to_start ||
        (!hasSucceededAtQuestIfValid(true,
                                     quest->has_successfully_completed[0],
                                     &minutes_since_completed) ||
         minutes_since_completed > max_minutes_since_successfully_completed) ||
        (!hasSucceededAtQuestIfValid(true,
                                     quest->has_successfully_completed[1],
                                     &minutes_since_completed) ||
         minutes_since_completed > max_minutes_since_successfully_completed) ||
        (!hasSucceededAtQuestIfValid(true,
                                     quest->has_successfully_completed[2],
                                     &minutes_since_completed) ||
         minutes_since_completed > max_minutes_since_successfully_completed)) {
      return Quests::QUESTQUALIFICATION_FAILED_TEMPORARILY;
    }
  }

  // Finally, check to be sure the correct amount of time has elapsed since
  // the last attempt at this quest
  { int minutes_since_completed;
    if (hasAttemptedQuestIfValid(quest->quest_permanent_hash,
                                 &minutes_since_completed) &&
        minutes_since_completed <= quest->minutes_to_wait_between_attempts) {
      return Quests::QUESTQUALIFICATION_FAILED_TEMPORARILY;
    }
  }

  // client qualifies for quest!
  return Quests::QUESTQUALIFICATION_SUCCEEDED;
}



//----[  sendQuestOffer  ]-----------------------------------------------------
void Client::sendQuestOffer(unsigned int quest_index,
                            Evidyon::Actor::ActorID npc_actor_id,
                            int npc_quest_index,
                            unsigned int validation_number,
                            const void* /*unimplemented: encryption_key*/) {
  using namespace Evidyon::Network::Packet;
  sendPacket(NetMsg_Quest_Offer::writePacket(ENET_PACKET_FLAG_RELIABLE,
                                             quest_index,
                                             npc_actor_id,
                                             npc_quest_index,
                                             validation_number));
}



//----[  startOnQuest  ]-------------------------------------------------------
bool Client::startOnQuest(unsigned int quest_index) {
  const Quests::Server::Quest* quest
    = Quests::Server::GlobalQuestManager::singleton()->getQuest(quest_index);
  if (!getEmptyActiveQuest(NULL) || !quest) { // this shouldn't happen
    addRedFlag(__FILE__, __LINE__);
    return false;
  }

  // Make sure we have a place for this quest
  Quests::Server::QuestParameters* local_quest_parameters = NULL;
  if (!getEmptyActiveQuest(&local_quest_parameters)) return false;

  // Be sure the player has enough resources to initiate this quest before
  // taking any of them.
  bool item_in_cost = false, missing_cost_item = false;
  for (int i = 0; i < Quests::Server::Quest::COST_ITEMS; ++i) {
    int quantity = quest->item_cost_to_attempt[i].quantity;
    if (quantity < 0) continue;
    if (!items_.isCarrying(quest->item_cost_to_attempt[i].item_index,
                           quantity)) {
      missing_cost_item = true;
      break;
    } else {
      item_in_cost = true;
    }
  }
  if (missing_cost_item ||
      avatar_.money < quest->currency_cost_to_attempt) {
    return false; // can't pay
  }

  // Take the cost from this player
  changeMoney(-int(quest->currency_cost_to_attempt));
  if (item_in_cost) {
    for (int i = 0; i < Quests::Server::Quest::COST_ITEMS; ++i) {
      int quantity = quest->item_cost_to_attempt[i].quantity;
      if (quantity < 0) continue;
      items_.eraseItems(quest->item_cost_to_attempt[i].item_index,
                        quantity); // should == 0 always
    }
  }

  // Success! Start the quest!

  // First, set up the quest parameters
  todo("karl","quest competition id");
  local_quest_parameters->competition_id = 0;
  local_quest_parameters->death_count = 0;
  GlobalItemManager::singleton()
      ->generateItemID(&local_quest_parameters->item_id_when_started);
  local_quest_parameters->minute_started = Evidyon::Time::minute_counter;
  // Get the last termination parameters.  Since minute_counter is greater
  // than the termination times (zeroes are returned for a non-attempted quest)
  // it is easy to tell the client is on this quest.
  getQuestTerminationTimes(
      quest->quest_permanent_hash,
      &local_quest_parameters->minute_terminated_success,
      &local_quest_parameters->minute_terminated_failure);
  local_quest_parameters->pk_count = 0;
  local_quest_parameters->qualified_kills = 0;
  local_quest_parameters->quest = quest;
  local_quest_parameters->specific_kills = 0;
  local_quest_parameters->has_changed_map = false;

  // Run the begin-quest action
  applyQuestAction(&quest->action_on_begin_quest);

  // Save the quests' states
  saveQuestsToDatabase(GlobalDatabaseManager::singleton());

  // Send current quests to the client
  syncClientQuests();

  // Successfully started up the quest
  return true;
}



//----[  isOnQuest  ]----------------------------------------------------------
bool Client::isOnQuest(unsigned int quest_index,
                       unsigned int* local_quest_index) {
  const Evidyon::Quests::Server::Quest* quest;

  for (unsigned int i = 0; i < MAX_SIMULTANEOUS_QUESTS; ++i) {
    if (((quest = active_quests_[i].quest) != 0) &&
        quest->this_quest_index == quest_index) {
      if (local_quest_index) *local_quest_index = i;
      return true;
    }
  }

  return false;
}



//----[  getQuestTime  ]-------------------------------------------------------
void Client::getQuestTime(unsigned int local_quest_index,
                          int* minutes_elapsed,
                          int* minutes_remaining) {
  using namespace Quests::Server;
  QuestParameters* parameters = &active_quests_[local_quest_index];
  const Quest* quest = parameters->quest;
  if (!quest) { // fail immediately
    *minutes_elapsed = 9999;
    *minutes_remaining = -9999;
  } else {
    *minutes_elapsed = 
      Evidyon::Time::minute_counter - parameters->minute_started;
    *minutes_remaining =
      parameters->minute_started
      + quest->time_limit_minutes
      - Evidyon::Time::minute_counter;
  }
}


//----[  checkAndApplyQuestStatus  ]-------------------------------------------
Quests::QuestStatus Client::checkAndApplyQuestStatus(
    unsigned int local_quest_index) {
  using namespace Quests::Server;
  QuestParameters* parameters = &active_quests_[local_quest_index];
  const Quest* quest = parameters->quest;
  if (!quest) return Quests::QUESTSTATUS_FAILED_TIMELIMIT;

  bool over_time_limit = false;

  // First, check irreversible reasons for failure
  Quests::QuestStatus status = Quests::QUESTSTATUS_UNCHANGED;
  if (quest->maximum_death_count < parameters->death_count) {
    status = Quests::QUESTSTATUS_FAILED_BY_HIGH_DEATH_COUNT;
    goto PERFORM_STATUS_ACTION;
  }
  if (quest->fail_on_change_map && parameters->has_changed_map) {
    status = Quests::QUESTSTATUS_FAILED_BY_CHANGING_MAP;
    goto PERFORM_STATUS_ACTION;
  }
  if (quest->max_pk_count_to_succeed < parameters->pk_count) {
    status = Quests::QUESTSTATUS_FAILED_BY_TOO_MANY_PKS;
    goto PERFORM_STATUS_ACTION;
  }
  if (quest->competitive) {
    /* missing:  failed by competitor winning
    logic here is:
    - if global quest manager's competition ID doesn't equal the internal
      competition id, fail.
    - when a player succeeds at the quest, notify the global quest manager.
      this will send a message indicating who won to all online participants,
      and increment the global competition ID to terminate the quest. if the
      quest terminates immediately, the failure action will be assessed on
      each participant at the same time...or maybe after a delay?
    */
  }

  // Next, check the time limit.  This must be done before assessing whether
  // or not the person met the success criteria.  Otherwise, players could
  // turn in quests after any amount of time.
  {
    int minutes_elapsed, minutes_remaining;
    getQuestTime(local_quest_index, &minutes_elapsed, &minutes_remaining);
    over_time_limit = (minutes_remaining < 0);
    if (over_time_limit && quest->fail_over_time_limit) {
      status = Quests::QUESTSTATUS_FAILED_TIMELIMIT;
      goto PERFORM_STATUS_ACTION;
    }
  }

  { // Finally, determine if the player has succeeded at the quest
    int alignment = avatar_.alignment_value;
    bool succeeded = false;
    if (parameters->qualified_kills >= quest->kill_overall_lifeforms_count &&
        parameters->specific_kills >= quest->kill_specific_lifeform_count &&
        parameters->pk_count >= quest->min_pk_count_to_succeed &&
        alignment >= quest->min_alignment_to_succeed &&
        alignment <= quest->max_alignment_to_succeed) {
      // Almost there! Next, check items
      succeeded = true;
      for (int i = 0; i < Quests::Server::Quest::REQUIRED_ITEMS; ++i) {
        int quantity = quest->required_items[i].quantity;
        if (quantity <= 0) continue;
        if (!items_.isCarryingNew(quest->required_items[i].item_index,
                                  quantity,
                                  parameters->item_id_when_started)) {
          succeeded = false;
          break;
        }
      }
    }

    if (succeeded) {
      status = over_time_limit ?
        Quests::QUESTSTATUS_SUCCEEDED_OVER_TIME_LIMIT :
        Quests::QUESTSTATUS_SUCCEEDED;
      goto PERFORM_STATUS_ACTION;
    }
  }

  // The player hasn't succeeded or failed at this quest yet
  return Quests::QUESTSTATUS_UNCHANGED;


PERFORM_STATUS_ACTION:

  // The quest has ended.  The ending action of the quest should be performed,
  // its data comitted to the database and the quest removed from the active
  // set.
  bool succeeded
    = status == Quests::QUESTSTATUS_SUCCEEDED
      || status == Quests::QUESTSTATUS_SUCCEEDED_OVER_TIME_LIMIT;
  endQuest(local_quest_index, succeeded, over_time_limit);
  return status;
}




//----[  onKilledMonster  ]----------------------------------------------------
void Client::onKilledMonster(int level, unsigned int type) {
  using namespace Quests::Server;
  for (int i = 0; i < MAX_SIMULTANEOUS_QUESTS; ++i) {
    QuestParameters* parameters = &active_quests_[i];
    const Quest* quest = parameters->quest;
    if (quest == NULL) continue;
    if (quest->kill_overall_lifeform_min_level <= level) {
      ++parameters->qualified_kills;
    }
    if (quest->kill_specific_lifeform_type == type &&
        quest->kill_specific_lifeform_min_level <= level) {
      ++parameters->specific_kills;
    }
  }
}


//----[  getEmptyActiveQuest  ]------------------------------------------------
bool Client::getEmptyActiveQuest(Quests::Server::QuestParameters** empty_slot) {
  for (int i = 0; i < MAX_SIMULTANEOUS_QUESTS; ++i) {
    if (active_quests_[i].quest == NULL) {
      if (empty_slot) *empty_slot = &active_quests_[i];
      return true;
    }
  }
  return false;
}

//----[  hasSucceededAtQuestIfValid  ]-----------------------------------------
bool Client::hasSucceededAtQuestIfValid(bool invalid_hash_return,
                                        Quests::Server::QuestHash quest_hash,
                                        int* minutes_since_completed) {
  if (quest_hash == Quests::Server::QUESTHASH_INVALID) {
    return invalid_hash_return;
  }
  int minute_terminated_success, minute_terminated_failure;
  getQuestTerminationTimes(quest_hash,
                           &minute_terminated_success,
                           &minute_terminated_failure);
  if (minute_terminated_success > 0 &&
      Evidyon::Time::minute_counter > minute_terminated_success) {
    if (minutes_since_completed) {
      *minutes_since_completed =
          Evidyon::Time::minute_counter - minute_terminated_success;
    }
    return true;
  }

  // Hasn't succeeded at quest
  return false;
}


//----[  hasAttemptedQuestIfValid  ]-------------------------------------------
bool Client::hasAttemptedQuestIfValid(Quests::Server::QuestHash quest_hash,
                                      int* minutes_since_attempted) {
  if (quest_hash == Quests::Server::QUESTHASH_INVALID) {
    return false;
  }
  int minute_terminated_success, minute_terminated_failure;
  getQuestTerminationTimes(quest_hash,
                           &minute_terminated_success,
                           &minute_terminated_failure);
  if (minute_terminated_success > 0 &&
      Evidyon::Time::minute_counter > minute_terminated_success) {
    *minutes_since_attempted =
      Evidyon::Time::minute_counter - minute_terminated_success;
    return true;
  }
  if (minute_terminated_failure > 0 &&
      Evidyon::Time::minute_counter > minute_terminated_failure) {
    *minutes_since_attempted =
      Evidyon::Time::minute_counter - minute_terminated_failure;
    return true;
  }

  // Quest has not been attempted
  return false;
}



//----[  getQuestTerminationTimes  ]-------------------------------------------
void Client::getQuestTerminationTimes(
    Quests::Server::QuestHash quest_hash,
    int* minute_terminated_success,
    int* minute_terminated_failure) {
  if (!GlobalDatabaseManager::singleton()->questQueryTerminationTimes(
        character_id_,
        quest_hash,
        minute_terminated_success,
        minute_terminated_failure)) {
    *minute_terminated_success = 0;
    *minute_terminated_failure = 0;
  }
}



//----[  applyQuestAction  ]---------------------------------------------------
void Client::applyQuestAction(const Quests::Server::QuestAction* action) {
  bool changed_items = false;

  for (int i = 0; i < Quests::Server::QuestAction::ITEMS_TO_REMOVE; ++i) {
    int quantity = action->items_to_remove[i].quantity;
    if (quantity < 0) continue;
    items_.eraseItems(action->items_to_remove[i].item_index, quantity);
    changed_items = true;
  }

  int number_of_items_to_give = action->number_of_items_to_give;
  if (number_of_items_to_give < 0) { // give all itmes
    for (int i = 0; i < Quests::Server::QuestAction::ITEMS_TO_GIVE; ++i) {
      int quantity = action->items_to_give[i].quantity;
      if (quantity < 0) continue;
      ItemPointer item;
      if (GlobalItemManager::singleton()
            ->acquireNewItem(quantity,
                             action->items_to_give[i].item_index,
                             &item)) {
        giveItemOrDropOnGround(&item);
        changed_items = true;
      }
    }
  } else { // give a selection of items
    bool item_flags[Quests::Server::QuestAction::ITEMS_TO_GIVE];
    int valid_items = 0;
    for (int i = 0; i < Quests::Server::QuestAction::ITEMS_TO_GIVE; ++i) {
      bool valid = action->items_to_give[i].quantity > 0;
      if (valid) {
        ++valid_items;
        item_flags[i] = false;
      } else {
        item_flags[i] = true;
      }
    }
    if (valid_items < number_of_items_to_give) {
      // There are fewer items in the list than we are going to give
      // to the player, so just pick items and give them until
      // the quantity is satisfied.  Repeating items isn't a problem.
      int i = rand()%Quests::Server::QuestAction::ITEMS_TO_GIVE;
      while (number_of_items_to_give > 0) {
        if (item_flags[i] == false) { // give item
          ItemPointer item;
          if (GlobalItemManager::singleton()
                ->acquireNewItem(action->items_to_give[i].quantity,
                                 action->items_to_give[i].item_index,
                                 &item)) {
            giveItemOrDropOnGround(&item);
            changed_items = true;
          }
          --number_of_items_to_give;
        }
        i = (i + 1) % Quests::Server::QuestAction::ITEMS_TO_GIVE;
      }
    } else {
      // more valid items than items to give, choose randomly and only
      // give an item once
      static const int SENTINEL = 2*Quests::Server::QuestAction::ITEMS_TO_GIVE;
      int times_generated = 0;
      while (number_of_items_to_give > 0 && times_generated < SENTINEL) {
        ++times_generated;
        int index = rand()%Quests::Server::QuestAction::ITEMS_TO_GIVE;
        if (item_flags[index]) continue; // flagged if invalid or given
        item_flags[index] = true;
        --number_of_items_to_give;
        ItemPointer item;
        if (GlobalItemManager::singleton()
              ->acquireNewItem(action->items_to_give[index].quantity,
                               action->items_to_give[index].item_index,
                               &item)) {
          giveItemOrDropOnGround(&item);
          changed_items = true;
        }
      }
    }
  }

  bool changed_stats = action->change_str != 0 ||
                       action->change_agi != 0 ||
                       action->change_con != 0 ||
                       action->change_int != 0 ||
                       action->change_wis != 0;
  if (changed_stats) {
    avatar_.attribute_values[Avatar::ATTRIBUTE_STRENGTH] += action->change_str;
    avatar_.attribute_values[Avatar::ATTRIBUTE_AGILITY] += action->change_agi;
    avatar_.attribute_values[Avatar::ATTRIBUTE_CONSTITUTION] += action->change_con;
    avatar_.attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE] += action->change_int;
    avatar_.attribute_values[Avatar::ATTRIBUTE_WISDOM] += action->change_wis;
  }

  // Modify avatar stats
  changeEXP(action->change_experience);
  changeMoney(action->change_money);

  // Move
  if (action->move_to_map_index >= 0) {
    //if (action->move_instantly) {
      moveTo(action->move_to_map_index,
             action->move_to_x,
             action->move_to_y);
    //}
  }

  // Update this character's abilities and items
  if (changed_items || changed_stats) {
    sendFullInventoryUpdate();
    //rebuildBaseAvatarAttributes();
    updateClientAvatarStats();
  }

  // Save this character
  commitToDatabase(GlobalDatabaseManager::singleton());
}



//----[  endQuest  ]-----------------------------------------------------------
void Client::endQuest(unsigned int local_quest_index,
                      bool succeeded,
                      bool over_time_limit) {
  Quests::Server::QuestParameters* parameters
      = &active_quests_[local_quest_index];
  const Quests::Server::Quest* quest = parameters->quest;
  if (!quest) return;

  if (succeeded) {
    if (over_time_limit) {
      applyQuestAction(&quest->action_on_success_over_time_limit);
    } else {
      applyQuestAction(&quest->action_on_success);
    }
    parameters->minute_terminated_success = Time::minute_counter;
  } else {
    applyQuestAction(&quest->action_on_failure);
    parameters->minute_terminated_failure = Time::minute_counter;
  }

  // Save the quest to the database
  GlobalDatabaseManager::singleton()
    ->questWrite(character_id_, parameters);

  // Erase this quest
  parameters->quest = NULL;
  //memset(&parameters, 0, sizeof(QuestParameters));

  // Send current quests to the client
  syncClientQuests();
}




//----[  saveQuestsToDatabase  ]-----------------------------------------------
void Client::saveQuestsToDatabase(GlobalDatabaseManager* db) {
  using namespace Quests::Server;
  for (int i = 0; i < MAX_SIMULTANEOUS_QUESTS; ++i) {
    QuestParameters* parameters = &active_quests_[i];
    if (parameters->quest == NULL) continue;
    db->questWrite(character_id_, parameters);
  }
}




//----[  loadQuestsFromDatabase  ]---------------------------------------------
void Client::loadQuestsFromDatabase(GlobalDatabaseManager* db) {
  memset(active_quests_, 0, sizeof(active_quests_));
  db->questReadActive(character_id_, active_quests_, MAX_SIMULTANEOUS_QUESTS);
}




//----[  syncClientQuests  ]---------------------------------------------------
void Client::syncClientQuests() {
  unsigned int minute = Time::minute_counter;
  using namespace Network::Packet;
  using namespace Quests::Server;
  const Quest* quests[MAX_SIMULTANEOUS_QUESTS] = {
    active_quests_[0].quest,
    active_quests_[1].quest,
    active_quests_[2].quest,
  };
  unsigned int indices[MAX_SIMULTANEOUS_QUESTS] = {
    quests[0] ? quests[0]->this_quest_index : 0xFFFF,
    quests[1] ? quests[1]->this_quest_index : 0xFFFF,
    quests[2] ? quests[2]->this_quest_index : 0xFFFF,
  };
  unsigned short times[MAX_SIMULTANEOUS_QUESTS] = {
    active_quests_[0].minute_started + (quests[0] ? quests[0]->time_limit_minutes : 0) - minute,
    active_quests_[1].minute_started + (quests[1] ? quests[1]->time_limit_minutes : 0) - minute,
    active_quests_[2].minute_started + (quests[2] ? quests[2]->time_limit_minutes : 0) - minute,
  };
  sendPacket(NetMsg_Quest_TellCurrentQuests::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      indices[0],
      indices[1],
      indices[2],
      times[0],
      times[1],
      times[2]));
}



}