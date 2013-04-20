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
#ifndef __SHARED_TREASURE_TOOLS_TREASUREWEIGHT_H__
#define __SHARED_TREASURE_TOOLS_TREASUREWEIGHT_H__
#pragma once

namespace Evidyon {
namespace Treasure {
namespace Tools {


//----[  TreasureWeight  ]-----------------------------------------------------
enum TreasureWeight {
  TREASUREWEIGHT_ARMOR,
  TREASUREWEIGHT_WEAPON,

  TREASUREWEIGHT_HELM,
  TREASUREWEIGHT_BODY,
  TREASUREWEIGHT_SHIELD,
  TREASUREWEIGHT_BELT,
  TREASUREWEIGHT_BOOTS,
  TREASUREWEIGHT_GLOVES,
  TREASUREWEIGHT_RING,

  TREASUREWEIGHT_NONSPECIFIC_USER,
  TREASUREWEIGHT_CASTER,
  TREASUREWEIGHT_WARRIOR,

  TREASUREWEIGHT_RANGED,

  TREASUREWEIGHT_ONE_HANDED,
  TREASUREWEIGHT_TWO_HANDED,

  TREASUREWEIGHT_CONSUMABLE_SELF,
  TREASUREWEIGHT_CONSUMABLE_OTHER,
  TREASUREWEIGHT_CONSUMABLE_BUFF,
  TREASUREWEIGHT_CONSUMABLE_ATTACK,
  TREASUREWEIGHT_CONSUMABLE_DEFEND,
  TREASUREWEIGHT_CONSUMABLE_TRANSPORT,
  TREASUREWEIGHT_CONSUMABLE_RESTORE,
  
  NUMBER_OF_TREASURE_WEIGHTS,
};





//----[  TreasureWeightString  ]-----------------------------------------------
const char* TreasureWeightString(TreasureWeight value);



}
}
}

#endif