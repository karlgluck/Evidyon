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
#include "treasureweights.h"


namespace Evidyon {
namespace Treasure {
namespace Tools {



  
//----[  TreasureWeights  ]----------------------------------------------------
TreasureWeights::TreasureWeights() {
  for (int i = 0; i < NUMBER_OF_TREASURE_WEIGHTS; ++i) {
    member(TreasureWeightString(TreasureWeight(i)), &values_[i]);
    values_[i].setValue(1.0);
  }
}



//----[  writeWeights  ]-------------------------------------------------------
void TreasureWeights::writeWeights(TreasureWeightValue* weights) {
  for (int i = 0; i < NUMBER_OF_TREASURE_WEIGHTS; ++i) {
    weights[i] = values_[i].getValue();
  }
}


//----[  combineWeights  ]-----------------------------------------------------
void TreasureWeights::combineWeights(const TreasureWeightValue* source_weights,
                                     TreasureWeightValue* weights) {
  for (int i = 0; i < NUMBER_OF_TREASURE_WEIGHTS; ++i) {
    weights[i] = source_weights[i] + values_[i].getValue();
  }
}



}
}
}

