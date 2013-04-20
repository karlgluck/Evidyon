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
#ifndef __SHARED_ITEM_ITEMEQUIPMENTTYPE_H__
#define __SHARED_ITEM_ITEMEQUIPMENTTYPE_H__
#pragma once


namespace Evidyon {
namespace Item {


  
//----[  ItemEquipmentType  ]--------------------------------------------------
enum ItemEquipmentType {

  // 1-handed melee items
  ITEMEQUIPMENTTYPE_1H,           // one-handed weapon; can be dual-wielded
  ITEMEQUIPMENTTYPE_MAINHAND,     // one-handed weapon, can't be dual-wielded
  ITEMEQUIPMENTTYPE_OFFHAND,      // non-weapon offhand only item
  ITEMEQUIPMENTTYPE_SHIELD,
  ITEMEQUIPMENTTYPE_JAVELIN,      // one-handed projectile, no dual-wielding
  ITEMEQUIPMENTTYPE_THROWN,       // misc. one-handed thrown weapon

  // 2-handed melee items
  ITEMEQUIPMENTTYPE_2H,           // two-handed weapon (big swords)
  ITEMEQUIPMENTTYPE_POLEARM,      // two-handed staff or other pole-arm
  ITEMEQUIPMENTTYPE_BOW,          // two-handed bow
  ITEMEQUIPMENTTYPE_CROSSBOW,
  ITEMEQUIPMENTTYPE_DUALWIELD,    // one item equips on both hands
  ITEMEQUIPMENTTYPE_UNARMED,      // dualwield with punches
  ITEMEQUIPMENTTYPE_SPEAR,        // two-handed

  // other types
  ITEMEQUIPMENTTYPE_ARMOR,
  ITEMEQUIPMENTTYPE_HELMET,
  ITEMEQUIPMENTTYPE_BELT,
  ITEMEQUIPMENTTYPE_RING,
  ITEMEQUIPMENTTYPE_ACCESSORY,    // misc. slot for amulets, charms
  ITEMEQUIPMENTTYPE_BOOTS,
  ITEMEQUIPMENTTYPE_GLOVES,
  ITEMEQUIPMENTTYPE_NONE,         // item can't be equipped

  NUMBER_OF_ITEM_EQUIPMENT_SLOTS,
  INVALID_ITEM_EQUIPMENT_SLOT,    // this item cannot be equipped
};



//----[  ItemEquipmentTypeString  ]--------------------------------------------
const char* ItemEquipmentTypeString(ItemEquipmentType equipment_slot);



}
}

#endif