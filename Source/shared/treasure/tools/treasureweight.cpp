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
#include "treasureweight.h"
#include "common/null.h"

namespace Evidyon {
namespace Treasure {
namespace Tools {

//----[  TreasureWeightString  ]-----------------------------------------------
const char* TreasureWeightString(TreasureWeight value) {
  switch (value) {
    default: return NULL;
    case TREASUREWEIGHT_ARMOR: return "Armor";
    case TREASUREWEIGHT_WEAPON: return "Weapon";
    case TREASUREWEIGHT_HELM: return "Helm";
    case TREASUREWEIGHT_BODY: return "Body";
    case TREASUREWEIGHT_SHIELD: return "Shield";
    case TREASUREWEIGHT_BELT: return "Belt";
    case TREASUREWEIGHT_BOOTS: return "Boots";
    case TREASUREWEIGHT_GLOVES: return "Gloves";
    case TREASUREWEIGHT_RING: return "Ring";
    case TREASUREWEIGHT_NONSPECIFIC_USER: return "Non-Specific User";
    case TREASUREWEIGHT_CASTER: return "Caster";
    case TREASUREWEIGHT_WARRIOR: return "Warrior";
    case TREASUREWEIGHT_RANGED: return "Ranged";
    case TREASUREWEIGHT_ONE_HANDED: return "One-Handed";
    case TREASUREWEIGHT_TWO_HANDED: return "Two-Handed";
    case TREASUREWEIGHT_CONSUMABLE_SELF: return "Consumable - Self";
    case TREASUREWEIGHT_CONSUMABLE_OTHER: return "Consumable - Other";
    case TREASUREWEIGHT_CONSUMABLE_BUFF: return "Consumable - Buff";
    case TREASUREWEIGHT_CONSUMABLE_ATTACK: return "Consumable - Attack";
    case TREASUREWEIGHT_CONSUMABLE_DEFEND: return "Consumable - Defend";
    case TREASUREWEIGHT_CONSUMABLE_TRANSPORT: return "Consumable - Transport";
    case TREASUREWEIGHT_CONSUMABLE_RESTORE: return "Consumable - Restore";
  }
}


}
}
}

