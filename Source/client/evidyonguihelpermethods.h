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
#ifndef __EVIDYONGUIHELPERMETHODS_H__
#define __EVIDYONGUIHELPERMETHODS_H__
#pragma once


#include "shared/avatar/evidyon_avatarinventory.h"

class GUIFont;
class EvidyonGUIScrollableButtonList;

namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}

namespace Evidyon {


void GUIHelper_GetItemNameString(
    int quantity,
    const Item::ItemClientDescription* item_description,
    char* buffer,
    size_t length);

void GUIHelper_CreateButtonForInventoryItem(
  int id,
  EvidyonGUIScrollableButtonList* button_list,
  GUIFont* button_font,
  const ClientAvatarInventoryItem* inventory_item,
  const Item::ItemClientDescription* item_descriptions,
  size_t number_of_item_descriptions);

void GUIHelper_CreateButtonForEquippedItem(
  int id,
  EvidyonGUIScrollableButtonList* button_list,
  GUIFont* button_font,
  const ClientAvatarInventoryItem* inventory_item,
  const Item::ItemClientDescription* item_descriptions,
  size_t number_of_item_descriptions);
//
//void GUIHelper_CreateButtonForConsumableItem(
//  int id,
//  EvidyonGUIScrollableButtonList* button_list,
//  GUIFont* button_font,
//  const ClientAvatarInventoryItem* inventory_item,
//  const Item::ItemClientDescription* item_descriptions,
//  size_t number_of_item_descriptions);


// Initializes a scrollable button list to contain the list of
// items in the client's inventory.
void GUIHelper_CreateButtonsForInventoryItems(
  EvidyonGUIScrollableButtonList* button_list,
  GUIFont* button_font,
  const ClientAvatarInventoryItem* inventory,
  int number_of_items_in_inventory,
  const Item::ItemClientDescription* item_descriptions,
  size_t number_of_item_descriptions);

void GUIHelper_CreateButtonsForEquippedItems(
  EvidyonGUIScrollableButtonList* button_list,
  GUIFont* button_font,
  const ClientAvatarInventoryItem* inventory,
  int number_of_items_in_inventory,
  const Item::ItemClientDescription* item_descriptions,
  size_t number_of_item_descriptions);
//
//void GUIHelper_CreateButtonsForConsumableItems(
//  EvidyonGUIScrollableButtonList* button_list,
//  GUIFont* button_font,
//  const ClientAvatarInventoryItem* inventory,
//  int number_of_items_in_inventory,
//  const Item::ItemClientDescription* item_descriptions,
//  size_t number_of_item_descriptions);

}

#endif