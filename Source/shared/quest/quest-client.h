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
#ifndef __QUEST_CLIENT_H__
#define __QUEST_CLIENT_H__
#pragma once

#include "quest-encryption.h"

namespace Evidyon {
namespace Quests {
namespace Client {


//----[  Quest  ]--------------------------------------------------------------
// This structure is stored in the client's game file
struct Quest {
  static const int QUEST_NAME_LENGTH = 128;
  static const int QUEST_DESCRIPTION_LENGTH = 1024;

  // What this quest is referred to by the client
  char name[QUEST_NAME_LENGTH];

  // This text is displayed to the user when the quest is offered.  It is also
  // displayed in the quest log when requested by the user.
  char description[QUEST_DESCRIPTION_LENGTH];
};

}
}
}




#endif