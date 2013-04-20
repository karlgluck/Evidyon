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
#ifndef __GLOBALQUESTMANAGER_H__
#define __GLOBALQUESTMANAGER_H__
#pragma once


#include "shared/quest/quest-server.h"
#include <hash_map>
#include "itemidtype.h"

namespace Evidyon {
class Client;

namespace Quests {
namespace Server {


  
//----[  QuestDatabaseParameters  ]--------------------------------------------
// This structure contains all of the persistent data associated with a quest.
// It is saved int the database between character logins.
struct QuestParameters {
  const Evidyon::Quests::Server::Quest* quest;
  int competition_id;
  int minute_started;
  int minute_terminated_success;  // minute last succeeded; negative = never
  int minute_terminated_failure;  // minute last failed; negative = never
  ItemIDType item_id_when_started;
  int qualified_kills;
  int specific_kills;
  int pk_count;
  int death_count; // includes deaths where the player gets back up
  bool has_changed_map;
};



//----[  GlobalQuestManager  ]-------------------------------------------------
//  TODO: competition ID numbers
class GlobalQuestManager {
  typedef stdext::hash_map<QuestHash, QuestIndex> QuestHashToIndexMap;
public:
  static GlobalQuestManager* singleton();
private:
  static GlobalQuestManager* singleton_;
public:
  GlobalQuestManager();

  // Reads configuration data from the game file
  bool create(FILE* game_file);

  // Frees data stored in the quest manager
  void destroy();

  // Turns a stored quest hash into its current index.
  QuestIndex translateToQuestIndex(QuestHash hash);

  // Finds the data structure for a stored quest hash
  const Quest* translateToQuest(QuestHash hash);

  // Returns a quest's persistent storage hash given its index.  If the index
  // is invalid, the returned hash is QUESTHASH_INVALID
  //QuestHash translateToQuestHash(QuestIndex index);

  // Obtains a quest's descriptive parameters from its index.  If the index
  // is invalid, this method returns NULL.
  const Quest* getQuest(QuestIndex index);


private:

  // List of quest descriptions that were loaded from the game file
  Quest* quests_;
  size_t number_of_quests_;

  // Defines the competition ID number for each quest.  If the quest
  // isn't a competition, the value stored here is zero.
  unsigned int* quest_competition_ids_;

  // Lookup table for permanent (database-stored) quest hashes
  QuestHashToIndexMap quest_hash_to_index_;
};


}
}
}




#endif