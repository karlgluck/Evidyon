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
#ifndef __SHARED_TREASURE_TOOLS_MONSTERTREASUREGROUP_H__
#define __SHARED_TREASURE_TOOLS_MONSTERTREASUREGROUP_H__
#pragma once

#include "monstertreasureitems.h"
#include "treasureweights.h"
#include "monstertreasureitems.h"

namespace Evidyon {
namespace Treasure {
namespace Tools {

//----[  MonsterTreasureGroup  ]-----------------------------------------------
  class MonsterTreasureGroup : public dc::dcResource<MonsterTreasureGroup> {
public:
  static std::string staticTypeName() { return "MonsterTreasureList"; }
public:
  MonsterTreasureGroup();
  void compile(const TreasureWeightValue* global_weights,
               Evidyon::Item::ItemIndex* drop_list,
               size_t drop_list_length);

  void getItemToDropPercentageList(
    const TreasureWeightValue* global_weights,
    MonsterTreasureItems::ItemDropPercentages* items);

  void clearItems() { items_.clear(); }

  // Adds a new item to this group.  If the item's weight is less than or
  // equal to zero, the item is not added.
  void addItem(
    Evidyon::Tools::ItemElement* item,
    TreasureWeightValue weight);
               
private:
  MonsterTreasureItems items_;
  TreasureWeights weights_;
};

}
}
}

#endif