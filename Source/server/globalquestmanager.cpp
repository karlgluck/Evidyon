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
#include "globalquestmanager.h"
#include "../common/safemacros.h"


namespace Evidyon {
namespace Quests {
namespace Server {


GlobalQuestManager* GlobalQuestManager::singleton_;

  
//----[  singleton  ]----------------------------------------------------------
GlobalQuestManager* GlobalQuestManager::singleton() { return singleton_; }


//----[  GlobalQuestManager  ]-------------------------------------------------
GlobalQuestManager::GlobalQuestManager() {
  singleton_ = this;
  quests_ = NULL;
  number_of_quests_ = 0;
  quest_competition_ids_ = NULL;
}


//----[  create  ]-------------------------------------------------------------
bool GlobalQuestManager::create(FILE* game_file) {
  size_t number;
  fread(&number, sizeof(number), 1, game_file);
  quests_ = new Quest[number];
  quest_competition_ids_ = new unsigned int[number];
  if (!quests_ || !quest_competition_ids_) {
    destroy();
    return false;
  }
  fread(quests_, sizeof(Quest), number, game_file);
  memset(quest_competition_ids_, 0, sizeof(unsigned int) * number);
  number_of_quests_ = number;

  // fill out the hash table
  for (size_t i = 0; i < number; ++i) {
    if (quests_[i].this_quest_index != i) {
      // this quest was not compiled correctly
      destroy();
      return false;
    }
    quest_hash_to_index_.insert(
      QuestHashToIndexMap::value_type(quests_[i].quest_permanent_hash,
                                      quests_[i].this_quest_index));
  }

  return true;
}


//----[  destroy  ]------------------------------------------------------------
void GlobalQuestManager::destroy() {
  SAFE_DELETE_ARRAY(quests_);
  SAFE_DELETE_ARRAY(quest_competition_ids_);
  number_of_quests_ = 0;
}


//----[  translateToQuestIndex  ]----------------------------------------------
QuestIndex GlobalQuestManager::translateToQuestIndex(QuestHash hash) {
  QuestHashToIndexMap::iterator i = quest_hash_to_index_.find(hash);
  if (i == quest_hash_to_index_.end()) return 0xFFFFFFFF; // invalid
  return i->second;
}



//----[  translateToQuest  ]---------------------------------------------------
const Quest* GlobalQuestManager::translateToQuest(QuestHash hash) {
  QuestHashToIndexMap::iterator i = quest_hash_to_index_.find(hash);
  if (i == quest_hash_to_index_.end()) return NULL;
  return &quests_[i->second]; // it is implied that this is valid
}



//----[  getQuest  ]-----------------------------------------------------------
const Quest* GlobalQuestManager::getQuest(QuestIndex index) {
  if (number_of_quests_ <= index) return NULL;
  return &quests_[index];
}

}
}
}