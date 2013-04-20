//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#include "quest-tools.h"



#define FOR_MEMBERS(object, resource, index, highest_index) \
  { const dc::dcGenericResource::Array& members = object.getMembers();\
    size_t index = 0;\
    for (dc::dcGenericResource::Array::const_iterator iter = members.begin();\
         iter != members.end() && index < (highest_index); ++iter, ++index) {\
         dcGenericResource* resource=(*iter);
#define END_FOR_MEMBERS() }}




//----[  QuestItemReference  ]-------------------------------------------------
QuestItemReference::QuestItemReference() {
  member("Item", &item_);
  member("Quantity", &quantity_);
  quantity_.setValue(1);
}




//----[  compile  ]------------------------------------------------------------
void QuestItemReference::compile(
    Evidyon::Quests::Server::QuestItemReference* compiled_reference) const {
  const dc::dcList<Item>::Element* item = item_.getReferencedResource();
  compiled_reference->item_index = item ? item->getIndex() : 0xFFFFFFFF;
  compiled_reference->quantity = item ? quantity_.getValue() : -1;
}






//----[  QuestAction  ]--------------------------------------------------------
QuestAction::QuestAction() {
  member("Items to Remove", &items_to_remove_);
  member("Items to Give", &items_to_give_);
  member("Number of Items to Give", &number_of_items_to_give_);
  member("Experience", &experience_change_);
  member("Gold", &gold_change_);
  member("Geonite", &geonite_change_);
  member("Strength", &str_change_);
  member("Agility", &agi_change_);
  member("Constitution", &con_change_);
  member("Intelligence", &int_change_);
  member("Wisdom", &wis_change_);

  member("Move to Atlas", &move_to_map_);
  member("Move to X", &move_to_x_);
  member("Move to Y", &move_to_y_);
}



//----[  compile  ]------------------------------------------------------------
void QuestAction::compile(Evidyon::Quests::Server::QuestAction* compiled_action) const {
  for (int i = 0; i < Evidyon::Quests::Server::QuestAction::ITEMS_TO_REMOVE; ++i) {
    compiled_action->items_to_remove[i].item_index = 0xFFFFFFFF;
    compiled_action->items_to_remove[i].quantity = -1;
  }
  FOR_MEMBERS(items_to_remove_,
              item,
              i,
              Evidyon::Quests::Server::QuestAction::ITEMS_TO_REMOVE)
    ((QuestItemReference*)item)->compile(&compiled_action->items_to_remove[i]);
  END_FOR_MEMBERS()

  for (int i = 0; i < Evidyon::Quests::Server::QuestAction::ITEMS_TO_GIVE; ++i) {
    compiled_action->items_to_give[i].item_index = 0xFFFFFFFF;
    compiled_action->items_to_give[i].quantity = -1;
  }
  FOR_MEMBERS(items_to_give_,
              item,
              i,
              Evidyon::Quests::Server::QuestAction::ITEMS_TO_GIVE)
    ((QuestItemReference*)item)->compile(&compiled_action->items_to_give[i]);
  END_FOR_MEMBERS()

  compiled_action->number_of_items_to_give =
    number_of_items_to_give_.getValue();

  compiled_action->change_experience = experience_change_.getValue();
  compiled_action->change_money = gold_change_.getValue();
  compiled_action->change_geonite = geonite_change_.getValue();
  compiled_action->change_str = str_change_.getValue();
  compiled_action->change_agi = agi_change_.getValue();
  compiled_action->change_con = con_change_.getValue();
  compiled_action->change_int = int_change_.getValue();
  compiled_action->change_wis = wis_change_.getValue();

  compiled_action->move_to_map_index = move_to_map_.getReferencedResourceIndex();
  compiled_action->move_to_x = move_to_x_.getValue();
  compiled_action->move_to_y = move_to_y_.getValue();
}




//----[  QuestAvailability  ]--------------------------------------------------
QuestAvailability::QuestAvailability() {
  member("Successfully Completed Quest [0]", &has_successfully_completed_[0]);
  member("Successfully Completed Quest [1]", &has_successfully_completed_[1]);
  member("Successfully Completed Quest [2]", &has_successfully_completed_[2]);
  member("Maximum Minutes Since Completed Quests", &time_since_successfully_completed_maximum_minutes_);
  time_since_successfully_completed_maximum_minutes_.setValue(99999999);
  member("Has Not Succeeded at Quest [0]", &has_not_successfully_completed_[0]);
  member("Has Not Succeeded at Quest [1]", &has_not_successfully_completed_[1]);
  member("Minimum Minutes Between Attempts", &minutes_to_wait_between_attempts_);
  for (int i = 0; i < Evidyon::Avatar::NUMBER_OF_AVATAR_RACES; ++i) {
    member(Evidyon::Avatar::AvatarRaceString(Evidyon::Avatar::AvatarRace(i)), &races_[i]);
    races_[i].setValue(true);
  }
  for (int i = 0; i < Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES; ++i) {
    member(Evidyon::Avatar::AvatarClassString(Evidyon::Avatar::AvatarClass(i)), &classes_[i]);
    classes_[i].setValue(true);
  }
  member("Minimum Level", &minimum_level_);
  member("Maximum Level", &maximum_level_);
  maximum_level_.setValue(150);
  member("Minimum Alignment", &minimum_alignment_);
  minimum_alignment_.setValue(-100);
  member("Maximum Alignment", &maximum_alignment_);
  maximum_alignment_.setValue(+100);
  for (int i = 0; i < DAYS_IN_WEEK; ++i) {
    member(CalendarDayString(CalendarDay(i)), &days_[i]);
    days_[i].setValue(true);
  }
  for (int i = 0; i < WEEKS_IN_MONTH; ++i) {
    member(CalendarWeekString(CalendarWeek(i)), &weeks_[i]);
    weeks_[i].setValue(true);
  }
  for (int i = 0; i < MONTHS_IN_YEAR; ++i) {
    member(CalendarMonthString(CalendarMonth(i)), &months_[i]);
    months_[i].setValue(true);
  }
  member("Carried Items", &carried_items_);
  member("Equipped Items", &equipped_items_);
}





//----[  CopyQuestHash  ]------------------------------------------------------
void CopyQuestHash(Evidyon::Quests::Server::QuestHash* target,
                   const Quest* source) {
  *target = source ? source->getQuestHash() : Evidyon::Quests::Server::QUESTHASH_INVALID;
}



//----[  compile  ]------------------------------------------------------------
void QuestAvailability::compile(Evidyon::Quests::Server::Quest* quest) const {
  CopyQuestHash(&quest->has_successfully_completed[0],
                has_successfully_completed_[0].getReferencedResource());
  CopyQuestHash(&quest->has_successfully_completed[1],
                has_successfully_completed_[1].getReferencedResource());
  CopyQuestHash(&quest->has_successfully_completed[2],
                has_successfully_completed_[2].getReferencedResource());
  quest->max_minutes_since_successfully_completed =
    time_since_successfully_completed_maximum_minutes_.getValue();

  CopyQuestHash(&quest->has_not_successfully_completed[0],
                has_not_successfully_completed_[0].getReferencedResource());
  CopyQuestHash(&quest->has_not_successfully_completed[1],
                has_not_successfully_completed_[1].getReferencedResource());

  quest->minutes_to_wait_between_attempts =
    minutes_to_wait_between_attempts_.getValue();

  unsigned int avatar_profession_mask = 0;
  for (int race = 0; race < int(Evidyon::Avatar::NUMBER_OF_AVATAR_RACES); ++race) {
    if (races_[race].getValue() == false) continue;
    for (int c = 0; c < int(Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES); ++c) {
      if (classes_[c].getValue() == false) continue;
      avatar_profession_mask |= 1 << (race * Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES + c);
    }
  }
  quest->avatar_profession_mask = avatar_profession_mask;

  quest->min_level_to_start = minimum_level_.getValue();
  quest->max_level_to_start = maximum_level_.getValue();
  quest->min_alignment_to_start = minimum_alignment_.getValue();
  quest->max_alignment_to_start = maximum_alignment_.getValue();

#define BUILD_MASK(output, boolean_array, count) { unsigned int output=0;\
  for (int i = 0; i < count; ++i) { if (!boolean_array[i].getValue()) continue;\
  output |= 1<<i; }  quest->output = output; }

  BUILD_MASK(days, days_, DAYS_IN_WEEK);
  BUILD_MASK(weeks, weeks_, WEEKS_IN_MONTH);
  BUILD_MASK(months, months_, MONTHS_IN_YEAR);

  for (int i = 0; i < Evidyon::Quests::Server::Quest::PREREQ_CARRIED_ITEMS; ++i) {
    quest->carried_items[i].item_index = 0xFFFFFFFF;
    quest->carried_items[i].quantity = -1;
  }
  FOR_MEMBERS(carried_items_,
              item,
              i,
              Evidyon::Quests::Server::Quest::PREREQ_CARRIED_ITEMS)
    ((QuestItemReference*)item)->compile(&quest->carried_items[i]);
  END_FOR_MEMBERS()

  for (int i = 0; i < Evidyon::Quests::Server::Quest::PREREQ_EQUIPPED_ITEMS; ++i) {
    quest->equipped_items[i].item_index = 0xFFFFFFFF;
    quest->equipped_items[i].quantity = -1;
  }
  FOR_MEMBERS(equipped_items_,
              item,
              i,
              Evidyon::Quests::Server::Quest::PREREQ_EQUIPPED_ITEMS)
    ((QuestItemReference*)item)->compile(&quest->equipped_items[i]);
  END_FOR_MEMBERS()
}







//----[  QuestAttemptCost  ]---------------------------------------------------
QuestAttemptCost::QuestAttemptCost() {
  member("Money", &money_);
  member("Geonite", &geonite_);
  member("Items", &items_);
}



//----[  compile  ]------------------------------------------------------------
void QuestAttemptCost::compile(Evidyon::Quests::Server::Quest* quest) const {
  quest->currency_cost_to_attempt = money_.getValue();
  quest->geonite_cost_to_attempt = geonite_.getValue();

  for (int i = 0; i < Evidyon::Quests::Server::Quest::COST_ITEMS; ++i) {
    quest->item_cost_to_attempt[i].item_index = 0xFFFFFFFF;
    quest->item_cost_to_attempt[i].quantity = -1;
  }
  FOR_MEMBERS(items_, item, i, Evidyon::Quests::Server::Quest::COST_ITEMS)
    ((QuestItemReference*)item)->compile(&quest->item_cost_to_attempt[i]);
  END_FOR_MEMBERS()
}



//----[  QuestLifeformAIReference  ]-------------------------------------------
QuestLifeformAIReference::QuestLifeformAIReference() {
  member("Lifeform", &lifeform_ai_);
  member("Minimum Level", &minimum_level_);
  member("Quantity", &quantity_);
  quantity_.setValue(-1);
}




//----[  compile  ]------------------------------------------------------------
void QuestLifeformAIReference::compile(int* lifeform_type,
                                       int* lifeform_quantity,
                                       int* minimum_level) const {
  *lifeform_type = lifeform_ai_.getReferencedResourceIndex();
  *lifeform_quantity = quantity_.getValue();
  *minimum_level = minimum_level_.getValue();
}




//----[  QuestTermination  ]---------------------------------------------------
QuestTermination::QuestTermination() {
  member("Competitive?", &competitive_);
  member("Terminate Immediately?", &terminate_immediately_);
  member("Maximum Death Count", &maximum_death_count_);
  maximum_death_count_.setValue(99999999);
  member("Fail on Change Map", &fail_on_change_map_);
  member("Time Limit Minutes", &time_limit_minutes_);
  time_limit_minutes_.setValue(99999999);
  member("Fail if Over Time Limit", &fail_over_time_limit_);
  member("Minimum # of Lifeforms Killed", &minimum_lifeforms_killed_);
  minimum_lifeforms_killed_.setValue(-1);
  member("Lowest Qualifying Lifeform Kill Level", &minimum_lifeforms_killed_lowest_qualifying_level_);
  minimum_lifeforms_killed_lowest_qualifying_level_.setValue(-1);
  member("Specific Lifeforms to Kill", &minimum_specific_lifeforms_to_kill_);
  member("Required Items", &required_items_);
  member("Minimum PK Count", &minimum_pk_count_);
  member("Maximum PK Count", &maximum_pk_count_);
  maximum_pk_count_.setValue(99999999);
  member("Minimum Alignment", &minimum_alignment_);
  minimum_alignment_.setValue(-100);
  member("Maximum Alignment", &maximum_alignment_);
  maximum_alignment_.setValue(+100);
  member("Success", &success_);
  member("Success Over Time Limit", &success_over_time_limit_);
  member("Failure", &failure_);
}




//----[  compile  ]------------------------------------------------------------
void QuestTermination::compile(Evidyon::Quests::Server::Quest* quest) const {
  quest->competitive            = competitive_.getValue();
  quest->terminate_immediately  = terminate_immediately_.getValue();
  quest->maximum_death_count    = maximum_death_count_.getValue();
  quest->fail_on_change_map     = fail_on_change_map_.getValue();
  quest->time_limit_minutes     = time_limit_minutes_.getValue();
  quest->fail_over_time_limit   = fail_over_time_limit_.getValue();

  quest->kill_overall_lifeform_min_level =
    minimum_lifeforms_killed_lowest_qualifying_level_.getValue();
  quest->kill_overall_lifeforms_count =
    minimum_lifeforms_killed_.getValue();

  minimum_specific_lifeforms_to_kill_.compile(
    &quest->kill_specific_lifeform_type,
    &quest->kill_specific_lifeform_count,
    &quest->kill_specific_lifeform_min_level);

  for (int i = 0; i < Evidyon::Quests::Server::Quest::REQUIRED_ITEMS; ++i) {
    quest->required_items[i].item_index = 0xFFFFFFFF;
    quest->required_items[i].quantity = -1;
  }
  FOR_MEMBERS(required_items_,
              resource,
              index,
              Evidyon::Quests::Server::Quest::REQUIRED_ITEMS)
    ((QuestItemReference*)resource)->compile(&quest->required_items[index]);
  END_FOR_MEMBERS()

  quest->min_pk_count_to_succeed = minimum_pk_count_.getValue();
  quest->max_pk_count_to_succeed = maximum_pk_count_.getValue();
  quest->min_alignment_to_succeed = minimum_alignment_.getValue();
  quest->max_alignment_to_succeed = maximum_alignment_.getValue();

  success_.compile(&quest->action_on_success);
  success_over_time_limit_.compile(&quest->action_on_success_over_time_limit);
  failure_.compile(&quest->action_on_failure);
}




//----[  Quest  ]--------------------------------------------------------------
Quest::Quest() {
  member("Displayed Name", &displayed_name_);
  member("Description", &description_);
  member("Availability", &availability_);
  member("Cost", &cost_);
  member("Action on Begin Quest", &action_on_begin_);
  member("Termination", &termination_);
}



//----[  compile  ]------------------------------------------------------------
void Quest::compile(Evidyon::Quests::Server::Quest* quest) const {
  quest->quest_permanent_hash = getQuestHash();
  quest->this_quest_index = getIndex();

  // TODO: use encryption key
  memset(quest->client_encryption_key,
         0,
         sizeof(quest->client_encryption_key));

  availability_.compile(quest);
  cost_.compile(quest);
  action_on_begin_.compile(&quest->action_on_begin_quest);
  termination_.compile(quest);
}



//----[  compile  ]------------------------------------------------------------
void Quest::compile(Evidyon::Quests::Client::Quest* quest) const {
  const Evidyon::Quests::QuestEncryptionWord* key = NULL;
  Evidyon::Quests::encryptQuestDescription(displayed_name_.getValue().c_str(),
                                           key,
                                           quest->name,
                                           sizeof(quest->name));
  Evidyon::Quests::encryptQuestDescription(description_.getValue().c_str(),
                                           key,
                                           quest->description,
                                           sizeof(quest->description));
                                            
}


//----[  getQuestHash  ]-------------------------------------------------------
Evidyon::Quests::Server::QuestHash Quest::getQuestHash() const {
  unsigned int hash = dc::dcResourceManager::uniqueIDOfTypeName(getName());
  if (hash == Evidyon::Quests::Server::QUESTHASH_INVALID) {
    // TODO: ERROR!
    return ~hash;
  } else {
    return hash;
  }
}