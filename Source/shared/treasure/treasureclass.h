//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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