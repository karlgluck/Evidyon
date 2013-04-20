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
