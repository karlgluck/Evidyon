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
#ifndef __SHARED_TREASURE_TOOLS_MONSTERTREASURE_H__
#define __SHARED_TREASURE_TOOLS_MONSTERTREASURE_H__
#pragma once

#include "monstertreasuregroup.h"
#include "treasureweightvalue.h"
#include "shared/treasure/treasuregroup.h"
#include "shared/item/itemindex.h"
#include "shared/treasure/treasureclassindex.h"

namespace Evidyon {
namespace Treasure {
namespace Tools {
class TreasureCompiler;
}
}
}

namespace Evidyon {
namespace Treasure {
namespace Tools {



//----[  MonsterTreasure  ]----------------------------------------------------
class MonsterTreasure : public dc::dcResource<MonsterTreasure> {
public:
  static std::string staticTypeName() { return "MonsterTreasure"; }
public:
  MonsterTreasure();

  // Creates the lists of treasure that this monster drops according to the
  // internally configured drop probabilities.
  TreasureClassIndex compile(TreasureCompiler* compiler);
 
  // Returns a list of all of the items in the given group for this monster
  // paired with the percent of the time that they will drop among items in
  // that group.
  void getItemToDropPercentageList(
    TreasureGroup treasure_group,
    const TreasureWeightValue* global_weights,
    MonsterTreasureItems::ItemDropPercentages* items);

  // Puts treasure into the groups based on the monster's level
  void automaticallyAssignTreasure(
    bool clear_first,
    int monster_level,
    const dc::dcGenericResource::Array& items);

private:
  MonsterTreasureGroup treasure_groups_[NUMBER_OF_TREASURE_GROUPS];
};



}
}
}

#endif