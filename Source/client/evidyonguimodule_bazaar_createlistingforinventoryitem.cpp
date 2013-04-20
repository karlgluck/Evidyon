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
#include "evidyonguimodule_bazaar_createlistingforinventoryitem.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "evidyonguimodule_bazaar.h"
#include "evidyonguitooltipbox.h"
#include "shared/item/itemclientdescription.h"

namespace Evidyon {


void EvidyonGUIModule_Bazaar_CreateListingForInventoryItem::create(
    EvidyonGUIModule_Bazaar* owner,
    GUICanvas* source_canvas,
    GUIFont* button_font,
    const Item::ItemClientDescription* item_descriptions,
    size_t number_of_item_descriptions,
    const ClientAvatarInventoryItem* inventory,
    EvidyonGUITooltipBox* tooltip) {

  owner_ = owner;

  addComponent(&label_);
  label_.textLabel()->setFont(button_font);
  label_.textLabel()->setAlignment(GUIALIGN_CENTER);
  label_.textLabel()->setText("Pick an Item to Sell");
  {
    GUICanvas* canvas = label_.getCanvas();
    canvas->setSourceCanvas(getListCanvas());
    canvas->relativeWidth(0);
    canvas->alignYTopOutside(-5);
    canvas->alignXCenter(0);
    canvas->relativeDepth(0.0f);
  }

  {
    GUICanvas* canvas = getListCanvas();
    canvas->setSourceCanvas(source_canvas);
    canvas->absoluteWidth(400);
    canvas->absoluteHeight(500);
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
    canvas->absoluteDepth(0.5f);
  }

  createActionInventoryItemList(
    button_font,
    item_descriptions,
    number_of_item_descriptions,
    inventory,
    tooltip);
}


void EvidyonGUIModule_Bazaar_CreateListingForInventoryItem::destroy() {
  destroyActionInventoryItemList();
}


void EvidyonGUIModule_Bazaar_CreateListingForInventoryItem::syncInventory() {
  sync(NULL);
}


bool EvidyonGUIModule_Bazaar_CreateListingForInventoryItem::displayItem(
    const ClientAvatarInventoryItem* item,
    const Item::ItemClientDescription* description,
    std::string* name,
    void* context_pointer) {
  if (item->equipped) return false;
  return true;
}


void EvidyonGUIModule_Bazaar_CreateListingForInventoryItem::onItemClicked(
    const ClientAvatarInventoryItem* item,
    const Item::ItemClientDescription* item_description,
    ActionTargeter* action_targeter) {
  getTooltip()->becomeOwner(this);
  getTooltip()->releaseOwnership(this);
  owner_->showCreateListingPrompt(item_description->name,
                                  item,
                                  item_description);
}



}


