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
#include <dc/enum>
#include "shared/item/tools/itemintendeduse.h"
#include "common/null.h"


//----[  getString  ]----------------------------------------------------------
template<>
const char* dc::dcEnum<Evidyon::Item::Tools::ItemIntendedUse>::getString(
    Evidyon::Item::Tools::ItemIntendedUse value) {
  switch (value) {
  using namespace Evidyon::Item::Tools;
  default: return NULL;
  case INTENDEDUSE_EQUIPMENT_COMMON: return "Equipment - Common";
  case INTENDEDUSE_EQUIPMENT_UNCOMMON: return "Equipment - Uncommon";
  case INTENDEDUSE_EQUIPMENT_RARE: return "Equipment - Rare";
  case INTENDEDUSE_EPIC: return "Epic Item";
  case INTENDEDUSE_CONSUMABLE_SELF: return "Consumable - Self";
  case INTENDEDUSE_CONSUMABLE_OTHER: return "Consumable - Other";
  case INTENDEDUSE_CONSUMABLE_BUFF: return "Consumable - Buff";
  case INTENDEDUSE_CONSUMABLE_ATTACK: return "Consumable - Attack";
  case INTENDEDUSE_CONSUMABLE_DEFEND: return "Consumable - Defend";
  case INTENDEDUSE_CONSUMABLE_TRANSPORT: return "Consumable - Transport";
  case INTENDEDUSE_CONSUMABLE_RESTORE: return "Consumable - Restore";
  }
}



//----[  staticTypeName  ]-----------------------------------------------------
std::string dc::dcEnum<Evidyon::Item::Tools::ItemIntendedUse>::staticTypeName()
{
    return "Enum-Item-ItemIntendedUse";
}
