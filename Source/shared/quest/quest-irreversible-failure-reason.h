//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __QUEST_IRREVERSIBLE_FAILURE_REASON__
#define __QUEST_IRREVERSIBLE_FAILURE_REASON__


namespace Evidyon {
namespace Quests {


  
//----[  QuestIrreversibleFailureReason  ]-------------------------------------
enum QuestIrreversibleFailureReason {
  FAILURE_BY_HIGH_DEATH_COUNT,
  FAILURE_BY_CHANGING_MAP,
  FAILURE_BY_TOO_MANY_PKS,
  FAILURE_BY_COMPETITOR_WINNING,
  QUEST_IRREVERSIBLE_FAILURE_REASON_COUNT,
};



//----[  QuestIrreversibleFailureReasonString  ]-------------------------------
const char* QuestIrreversibleFailureReasonString(
  QuestIrreversibleFailureReason reason);

}
}

#endif