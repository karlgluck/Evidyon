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