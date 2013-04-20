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
#include "monstertreasure.h"
#include "shared/treasure/treasureclass.h"
#include "shared/treasure/tools/treasurecompiler.h"


namespace Evidyon {
namespace Treasure {
namespace Tools {



//----[  MonsterTreasure  ]----------------------------------------------------
MonsterTreasure::MonsterTreasure() {
  for (int i = 0; i < NUMBER_OF_TREASURE_GROUPS; ++i) {
    member(TreasureGroupString(TreasureGroup(i)), &treasure_groups_[i]);
  }
}



//----[  compile  ]------------------------------------------------------------
TreasureClassIndex MonsterTreasure::compile(
    TreasureCompiler* compiler) {
  Evidyon::Treasure::TreasureClass treasure_class;
  treasure_groups_[TREASUREGROUP_COMMON_EQUIPMENT].compile(
    compiler->getTreasureGroupWeights(TREASUREGROUP_COMMON_EQUIPMENT),
    treasure_class.common_list,
    Evidyon::Treasure::TreasureClass::COMMON_LENGTH);
  treasure_groups_[TREASUREGROUP_UNCOMMON_EQUIPMENT].compile(
    compiler->getTreasureGroupWeights(TREASUREGROUP_UNCOMMON_EQUIPMENT),
    treasure_class.uncommon_list,
    Evidyon::Treasure::TreasureClass::UNCOMMON_LENGTH);
  treasure_groups_[TREASUREGROUP_RARE_EQUIPMENT].compile(
    compiler->getTreasureGroupWeights(TREASUREGROUP_RARE_EQUIPMENT),
    treasure_class.rare_list,
    Evidyon::Treasure::TreasureClass::RARE_LENGTH);
  treasure_groups_[TREASUREGROUP_CONSUMABLE].compile(
    compiler->getTreasureGroupWeights(TREASUREGROUP_CONSUMABLE),
    treasure_class.consumable_list,
    Evidyon::Treasure::TreasureClass::CONSUMABLE_LENGTH);

  return compiler->add(&treasure_class);
}



//----[  getItemToDropPercentageList  ]----------------------------------------
void MonsterTreasure::getItemToDropPercentageList(
    TreasureGroup treasure_group,
    const TreasureWeightValue* global_weights,
    MonsterTreasureItems::ItemDropPercentages* items) {
  treasure_groups_[treasure_group].getItemToDropPercentageList(global_weights, items);
}




//----[  automaticallyAssignTreasure  ]----------------------------------------
void MonsterTreasure::automaticallyAssignTreasure(
    bool clear_first,
    int monster_level,
    const dc::dcGenericResource::Array& items) {
  if (clear_first) {
    for (int i = 0; i < NUMBER_OF_TREASURE_GROUPS; ++i) {
      treasure_groups_[i].clearItems();
    }
  }
  for (dc::dcGenericResource::Array::const_iterator i = items.begin();
       i != items.end(); ++i) {
    Evidyon::Tools::ItemElement* item
      = (Evidyon::Tools::ItemElement*)(*i);
    Evidyon::Treasure::TreasureGroup group;
    if (item->getTreasureGroup(&group)) {
      treasure_groups_[group].addItem(
        item,
        item->getTreasureDropWeight(monster_level));
    }
  }
}



}
}
}

