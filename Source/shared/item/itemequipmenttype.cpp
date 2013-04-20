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
#include "itemequipmenttype.h"
#include "common/null.h"


namespace Evidyon {
namespace Item {




//----[  ItemEquipmentSlotString  ]--------------------------------------------
const char* ItemEquipmentTypeString(ItemEquipmentType value) {
  switch (value) {
  case ITEMEQUIPMENTTYPE_1H: return "1-Handed Weapon";
  case ITEMEQUIPMENTTYPE_MAINHAND: return "Mainhand";
  case ITEMEQUIPMENTTYPE_OFFHAND: return "Offhand Item";
  case ITEMEQUIPMENTTYPE_SHIELD: return "Shield";
  case ITEMEQUIPMENTTYPE_JAVELIN: return "Javelin";
  case ITEMEQUIPMENTTYPE_THROWN: return "Thrown";
  case ITEMEQUIPMENTTYPE_2H: return "2-Handed Weapon";
  case ITEMEQUIPMENTTYPE_POLEARM: return "Polearm";
  case ITEMEQUIPMENTTYPE_BOW: return "Bow";
  case ITEMEQUIPMENTTYPE_CROSSBOW: return "Crossbow";
  case ITEMEQUIPMENTTYPE_DUALWIELD: return "Dual-wield";
  case ITEMEQUIPMENTTYPE_UNARMED: return "Unarmed";
  case ITEMEQUIPMENTTYPE_SPEAR: return "Spear";
  case ITEMEQUIPMENTTYPE_ARMOR: return "Armor";
  case ITEMEQUIPMENTTYPE_HELMET: return "Helmet";
  case ITEMEQUIPMENTTYPE_BELT: return "Belt";
  case ITEMEQUIPMENTTYPE_RING: return "Ring";
  case ITEMEQUIPMENTTYPE_ACCESSORY: return "Accessory";
  case ITEMEQUIPMENTTYPE_BOOTS: return "Boots";
  case ITEMEQUIPMENTTYPE_GLOVES: return "Gloves";
  case ITEMEQUIPMENTTYPE_NONE: return "(none)";
  default: return NULL;
  }
}



}
}


