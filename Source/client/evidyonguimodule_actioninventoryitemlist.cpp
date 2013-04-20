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
#include "evidyonguimodule_actioninventoryitemlist.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguihelpermethods.h"
#include "shared/item/itemclientdescription.h"


namespace Evidyon {



void EvidyonGUIModule_ActionInventoryItemList::sync(void* context_pointer) {
  GUILayer* layer = hide();
  clearButtons();
  for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
    const ClientAvatarInventoryItem* item = &inventory_[i];
    unsigned int type = item->type;
    if (item->inventory_index != i ||
        item->quantity == 0 ||
        type >= number_of_item_descriptions_) continue;
    const Item::ItemClientDescription* description = &item_descriptions_[type];
    std::string name;
    {
      char buffer[128];
      GUIHelper_GetItemNameString(
        item->quantity,
        description,
        buffer,
        sizeof(buffer));
      name.assign(buffer);
    }
    if (displayItem(item, description, &name, context_pointer)) {
      addButton(i, name.c_str());
    }
  }
  show(layer);
}



void EvidyonGUIModule_ActionInventoryItemList::createActionInventoryItemList(
    GUIFont* button_font,
    const Item::ItemClientDescription* item_descriptions,
    size_t number_of_item_descriptions,
    const ClientAvatarInventoryItem* inventory,
    EvidyonGUITooltipBox* tooltip) {

  inventory_ = inventory;
  item_descriptions_ = item_descriptions;
  number_of_item_descriptions_ = number_of_item_descriptions;

  createActionButtonList(button_font, tooltip);

  tooltip_description_.setFont(button_font);
}



void EvidyonGUIModule_ActionInventoryItemList::destroyActionInventoryItemList() {
  inventory_ = NULL;
  item_descriptions_ = NULL;
  number_of_item_descriptions_ = 0;
  destroyActionButtonList();
}



void EvidyonGUIModule_ActionInventoryItemList::onButtonClicked(
    int button_id,
    ActionTargeter* action_targeter) {
  const ClientAvatarInventoryItem* item = &inventory_[button_id];
  unsigned int type = item->type;
  if (type >= number_of_item_descriptions_) return;
  onItemClicked(item, &item_descriptions_[type], action_targeter);
}



bool EvidyonGUIModule_ActionInventoryItemList::updateTooltipContent(
    int button_id, EvidyonGUITooltipBox* tooltip) {
  const ClientAvatarInventoryItem* item = &inventory_[button_id];
  unsigned int type = item->type;
  if (type >= number_of_item_descriptions_) return false;
  GUISize size;
  tooltip_description_.setItem(&item_descriptions_[type],
                               &size);
  tooltip->setContentSize(size);
  return true;
}



void EvidyonGUIModule_ActionInventoryItemList::addTooltipContent(
    EvidyonGUITooltipBox* tooltip) {
  tooltip->addContent(&tooltip_description_);
}



bool EvidyonGUIModule_ActionInventoryItemList::displayItem(
    const ClientAvatarInventoryItem* item,
    const Item::ItemClientDescription* description,
    std::string* name,
    void* context_pointer) {
  return true;
}



// Implemented by the derived class to do whatever it needs to do when an
// item is clicked.
void EvidyonGUIModule_ActionInventoryItemList::onItemClicked(
    const ClientAvatarInventoryItem* item,
    const Item::ItemClientDescription* item_description,
    ActionTargeter* action_targeter) {
}




}