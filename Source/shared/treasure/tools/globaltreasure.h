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
#ifndef __SHARED_TREASURE_TOOLS_GLOBALTREASURE_H__
#define __SHARED_TREASURE_TOOLS_GLOBALTREASURE_H__
#pragma once


#include "shared/treasure/tools/treasureweights.h"
#include "shared/treasure/treasuregroup.h"

namespace Evidyon {
namespace Treasure {
struct TreasureDropSequenceElement;
}
}

namespace Evidyon {
namespace Treasure {
namespace Tools {

//----[  GlobalTreasure  ]-----------------------------------------------------
class GlobalTreasure : public dc::dcResource<GlobalTreasure> {
public:
  static std::string staticTypeName() { return "GlobalTreasure"; }
public:
  GlobalTreasure();
  void compileDropSequence(TreasureDropSequenceElement* elements,
                           size_t number_of_elements);
  void writeBaseWeights(TreasureGroup group, TreasureWeightValue* weights);
  void getDropPercentages(float* percent_commons,
                          float* percent_uncommons,
                          float* percent_rares,
                          float* percent_consumables);
private:
  dc::dcFloat percent_gold_;
  dc::dcFloat percent_equipment_;
  dc::dcFloat percent_uncommon_;
  dc::dcFloat percent_uncommons_rare_;
  dc::dcFloat percent_consumables_;
  TreasureWeights base_treasure_weights_[NUMBER_OF_TREASURE_GROUPS];
};

}
}
}

#endif