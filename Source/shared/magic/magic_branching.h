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
#ifndef __SHARED_MAGIC_MAGIC_BRANCHING_H__
#define __SHARED_MAGIC_MAGIC_BRANCHING_H__
#pragma once

#include "shared/event/eventindex.h"

namespace Evidyon {
namespace Magic {

  
//----[  Magic_Branching_Stage  ]----------------------------------------------
struct Magic_Branching_Stage {
  Event::EventIndex event_index;
  int targets;
  float radius_sq;
  float delay;                    // from this stage to next
  int base_damage, random_damage; // random must != 0
};

//----[  Magic_Branching  ]----------------------------------------------------
// This magic type is used for chain effects such as a lighting bolt that hops
// from one target to the next, or a chain lightning that hits one target, then
// two more, then two more from each of those with successively lower damage.
// A branching effect will not hit a target that has already been hit.
struct Magic_Branching {
  static const int NUMBER_OF_STAGES = 3;
  Magic_Branching_Stage stage[NUMBER_OF_STAGES];
  bool find_enemies;
  bool find_friends;
};


}
}

#endif