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
#include "monstertreasuregroup.h"
#include "common/randint32.h"

namespace Evidyon {
namespace Treasure {
namespace Tools {



  
//----[  MonsterTreasureGroup  ]-----------------------------------------------
MonsterTreasureGroup::MonsterTreasureGroup() {
  member("Items", &items_);
  member("Weights", &weights_);
}




//----[  compile  ]------------------------------------------------------------
void MonsterTreasureGroup::compile(const TreasureWeightValue* global_weights,
                                   Evidyon::Item::ItemIndex* drop_list,
                                   size_t drop_list_length) {
  TreasureWeightValue weights[NUMBER_OF_TREASURE_WEIGHTS];
  weights_.combineWeights(global_weights, weights);
  items_.fillDropList(weights, drop_list, drop_list_length);

  // randomly swap around entries so that the drops appear random
  for (size_t i = 0; i < drop_list_length; ++i) {
    size_t swap_with = rand_uint32()%drop_list_length;
    Evidyon::Item::ItemIndex swap = drop_list[swap_with];
    drop_list[swap_with] = drop_list[i];
    drop_list[i] = swap;
  }
}




//----[  getItemToDropPercentageList  ]----------------------------------------
void MonsterTreasureGroup::getItemToDropPercentageList(
    const TreasureWeightValue* global_weights,
    MonsterTreasureItems::ItemDropPercentages* items) {
  TreasureWeightValue weights[NUMBER_OF_TREASURE_WEIGHTS];
  weights_.combineWeights(global_weights, weights);
}


//----[  addItem  ]------------------------------------------------------------
void MonsterTreasureGroup::addItem(Evidyon::Tools::ItemElement* item,
                                   TreasureWeightValue weight) {
  if (weight <= 0.0) return;
  MonsterTreasureItems::Element* element
    = (MonsterTreasureItems::Element*)items_.generateElement(item->getName());
  element->getWeight()->setValue(weight);
  element->getItem()->setReferencedResource(item);
}


}
}
}

