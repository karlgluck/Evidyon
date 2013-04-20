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
#ifndef __SHARED_TREASURE_TOOLS_MONSTERTREASUREITEMS_H__
#define __SHARED_TREASURE_TOOLS_MONSTERTREASUREITEMS_H__
#pragma once

#include "shared/item/tools/item.h"
#include "shared/item/itemindex.h"
#include <dc/float>
#include <dc/dccollection.h>
#include "treasureweightvalue.h"
#include <list>


namespace Evidyon {
namespace Treasure {
namespace Tools {
struct TreasureDropSelectionArray;
}
}
}


namespace Evidyon {
namespace Treasure {
namespace Tools {

//----[  MonsterTreasureItems  ]-----------------------------------------------
class MonsterTreasureItems : public dc::dcCollection<MonsterTreasureItems> {
public:
  static std::string staticTypeName() { return "MonsterTreasureItems"; }
  typedef std::list<std::pair<Evidyon::Item::ItemIndex,float>> ItemDropPercentages;

public:
  //----[  Element  ]----------------------------------------------------------
  class Element : public dc::dcResource<Element> {
  public:
    static std::string staticTypeName() { return "Element of MonsterTreasureItems"; }
  public:
    Element();
    int compareTo(const dc::dcGenericResource* other) const;
    
    // The weights that affect this treasure are implied by the item reference.
    // The internal treasure weight is just added to the overall weight amount
    // for this item.
    TreasureWeightValue getWeightValue(const TreasureWeightValue* weights);

    dc::dcFloat* getWeight() { return &weight_; }
    Evidyon::Tools::ItemReference* getItem() { return &item_; }

  private:
    dc::dcFloat weight_;
    Evidyon::Tools::ItemReference item_;
  };
  
public:

  // Adds a default element to the table with no item reference.  This is so
  // that the first item added can have a <100% chance of dropping.
  MonsterTreasureItems();

  // Fills in the array with items that can be dropped.  They are filled in
  // proportion to their weight, but all items of the same type are listed
  // in sequence (the list needs to be randomized next).
  void fillDropList(const TreasureWeightValue* weights,
                    Evidyon::Item::ItemIndex* drop_list,
                    size_t drop_list_length);

  // Creates a list of item index -> drop percentage pairs based on the
  // input weights and the internal configuration.  This is used by the item
  // editor to review how changes to the weights affect item drops.
  // The output values are out of 100 (they are percentages), not out of 1.
  void getItemToDropPercentageList(
    const TreasureWeightValue* weights,
    ItemDropPercentages* items);

private:
  virtual bool allowDuplicates() const;
  virtual bool keepSorted() const;
};

}
}
}

#endif