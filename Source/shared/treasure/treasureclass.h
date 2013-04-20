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
#ifndef __SHARED_TREASURE_TREASURECLASS_H__
#define __SHARED_TREASURE_TREASURECLASS_H__
#pragma once


#include "shared/item/itemindex.h"
#include "shared/avatar/avatarlevel.h"

namespace Evidyon {
namespace Treasure {



//----[  TreasureClass  ]------------------------------------------------------
// Describes some generic source of treasure.  Right now, it is only used by
// monsters but in the future it could describe quest loot and treasure-chests.
struct TreasureClass {
  static const int COMMON_LENGTH = 2048;
  static const int UNCOMMON_LENGTH = 512;
  static const int RARE_LENGTH = 128;
  static const int CONSUMABLE_LENGTH = 512;
  
  // The global drop algorithm rolls through a list that specifies whether the
  // monster should drop a common, uncommon or rare item on the next drop.
  // These lists contain counters that specify which items should be dropped
  // next from each list.
  // It is important to start each counter at a random place in the list,
  // otherwise the same items will be dropped in sequence after a restart.
  Evidyon::Item::ItemIndex common_list[COMMON_LENGTH];
  Evidyon::Item::ItemIndex uncommon_list[UNCOMMON_LENGTH];
  Evidyon::Item::ItemIndex rare_list[RARE_LENGTH];

  // Consumables are like equipment, but there is only one class.  Again, the
  // global algorithm defines when to drop them.
  Evidyon::Item::ItemIndex consumable_list[CONSUMABLE_LENGTH];
};



}
}

#endif