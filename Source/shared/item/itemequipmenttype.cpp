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


