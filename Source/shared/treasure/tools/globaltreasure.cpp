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
#include "globaltreasure.h"
#include "common/randint32.h"
#include "shared/treasure/treasuredropsequenceelement.h"
#include <assert.h>

namespace Evidyon {
namespace Treasure {
namespace Tools {


  
//----[  GlobalTreasure  ]-----------------------------------------------------
GlobalTreasure::GlobalTreasure() {
  member("chance to drop gold (%)", &percent_gold_);
  member("% of time to drop any equipment", &percent_equipment_);
  member("% of equipment that is uncommon", &percent_uncommon_);
  member("% of uncommons that are (instead) rare", &percent_uncommons_rare_);
  member("chance to drop consumable (%)", &percent_consumables_);
  for (int i = 0; i < NUMBER_OF_TREASURE_GROUPS; ++i) {
    member(TreasureGroupString(TreasureGroup(i)), &base_treasure_weights_[i]);
  }

  percent_gold_.setValue(50.0);
  percent_equipment_.setValue(30.0);
  percent_uncommon_.setValue(10.0);
  percent_uncommons_rare_.setValue(10.0);
  percent_consumables_.setValue(30.0);
}




//----[  compileDropSequence  ]------------------------------------------------
void GlobalTreasure::compileDropSequence(
    TreasureDropSequenceElement* elements,
    size_t number_of_elements) {
  unsigned int common_threshold
    = (UINT_MAX-2) * (double)(percent_equipment_.getValue() / 100.0);
  unsigned int uncommon_threshold
    = common_threshold * (double)(percent_uncommon_.getValue() / 100.0);
  unsigned int rare_threshold
    = uncommon_threshold * (double)(percent_uncommons_rare_.getValue() / 100.0);
  rare_threshold = uncommon_threshold - rare_threshold;
  uncommon_threshold = common_threshold - uncommon_threshold;
  common_threshold = (UINT_MAX-2) - common_threshold;
  uncommon_threshold += common_threshold;
  rare_threshold += uncommon_threshold;
  assert(common_threshold < uncommon_threshold);
  assert(uncommon_threshold < rare_threshold);

  unsigned int gold_threshold
    = (UINT_MAX-2) * (double)(1.0 - percent_gold_.getValue() / 100.0);
  unsigned int consumable_threshold
    = (UINT_MAX-2) * (double)(1.0 - percent_consumables_.getValue() / 100.0);

  for (int i = 0; i < number_of_elements; ++i) {
    // use exclusive comparisons (>) instead of inclusive (>=) so that
    // setting something to a drop % of 0 makes it never drop
    unsigned int gold = rand_uint32();
    unsigned int consumable = rand_uint32();
    unsigned int equipment = rand_uint32();
    if (equipment > rare_threshold) {
      elements[i].equipment = TreasureDropSequenceElement::RARE;
    } else if (equipment > uncommon_threshold) {
      elements[i].equipment = TreasureDropSequenceElement::UNCOMMON;
    } else if (equipment > common_threshold) {
      elements[i].equipment = TreasureDropSequenceElement::COMMON;
    } else {
      elements[i].equipment = TreasureDropSequenceElement::NONE;
    }
    elements[i].gold = gold > gold_threshold;
    elements[i].consumable = consumable > consumable_threshold;
  }
}


//----[  writeBaseWeights  ]---------------------------------------------------
void GlobalTreasure::writeBaseWeights(TreasureGroup group, TreasureWeightValue* weights) {
  base_treasure_weights_[group].writeWeights(weights);
}



//----[  getDropPercentages  ]-------------------------------------------------
void GlobalTreasure::getDropPercentages(float* percent_commons,
                                        float* percent_uncommons,
                                        float* percent_rares,
                                        float* percent_consumables) {
  *percent_commons = percent_equipment_.getValue() / 100.0 * (1.0 - percent_uncommon_.getValue() / 100.0);
  *percent_uncommons = percent_equipment_.getValue() / 100.0 *
                        (percent_uncommon_.getValue() / 100.0) *
                        (1.0 - percent_uncommons_rare_.getValue() / 100.0);
  *percent_rares = percent_equipment_.getValue() / 100.0 *
                   (percent_uncommon_.getValue() / 100.0) *
                   (percent_uncommons_rare_.getValue() / 100.0);
  *percent_consumables = percent_consumables_.getValue() / 100.0;
}

}
}
}

