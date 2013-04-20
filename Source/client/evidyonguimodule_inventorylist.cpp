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
#include "evidyonguimodule_inventorylist.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguikeyboundactionssentinel.h"
#include "evidyonguihelpermethods.h"
#include "shared/item/itemclientdescription.h"


namespace Evidyon {



  
//----[  EvidyonGUIModule_InventoryList  ]-------------------------------------
EvidyonGUIModule_InventoryList::EvidyonGUIModule_InventoryList() {
  tooltip_ = NULL;
  button_font_ = NULL;
  inventory_ = NULL;
  item_descriptions_ = NULL;
  number_of_item_descriptions_ = 0;
}




  
//----[  create  ]-------------------------------------------------------------
void EvidyonGUIModule_InventoryList::create(GUICanvas* source_canvas,
                                            GUIFont* button_font,
                                            const Item::ItemClientDescription* item_descriptions,
                                            size_t number_of_item_descriptions,
                                            const ClientAvatarInventoryItem* inventory,
                                            EvidyonGUITooltipBox* tooltip) {
  inventory_ = inventory;
  button_font_ = button_font;
  item_descriptions_ = item_descriptions;
  number_of_item_descriptions_ = number_of_item_descriptions;
  addComponent(&item_list_);
  addComponent(&label_);

  item_list_.setLineHeight(25);
  item_list_.setBackgroundColor(0xA0FFFFFF);

  {
    GUICanvas* canvas = item_list_.getCanvas();
    canvas->setSourceCanvas(source_canvas);
    canvas->absoluteWidth(400);
    canvas->absoluteHeight(300);
    canvas->alignXLeft(50);
    canvas->alignYTop(100);
    canvas->absoluteDepth(0.4f);
  }

  {
    GUICanvas* canvas = label_.getCanvas();
    canvas->setSourceCanvas(item_list_.getCanvas());
    canvas->relativeWidth(0);
    canvas->absoluteHeight(35);
    canvas->alignXCenter(0);
    canvas->alignYTopOutside(5);
    canvas->absoluteDepth(0.4f);
    label_.textLabel()->setFont(button_font);
    label_.textLabel()->setAlignment(GUIALIGN_CENTER);
    label_.textLabel()->setTextColor(0xFFFFFFFF); // white
    label_.textLabel()->setText("Inventory");
  }


  tooltip_ = tooltip;

  tooltip_description_.setFont(button_font);
}




//----[  setBackgroundColor  ]-------------------------------------------------
void EvidyonGUIModule_InventoryList::setBackgroundColor(D3DCOLOR color) {
  item_list_.setBackgroundColor(color);
}

void EvidyonGUIModule_InventoryList::destroy() {
  clearComponents();
  button_font_ = NULL;
  hide();
}

GUICanvas* EvidyonGUIModule_InventoryList::getCanvas() {
  return item_list_.getCanvas();
}



void EvidyonGUIModule_InventoryList::sync(DisplayType display_type,
                                          EvidyonGUIKeyBoundActionsSentinel* key_bindings) {
  GUILayer* layer = hide();
  float position = item_list_.getScrollPosition();
  item_list_.clear();
  switch (display_type) {
  case DISPLAY_UNEQUIPPED:
      label_.textLabel()->setText("Inventory");
      GUIHelper_CreateButtonsForInventoryItems(&item_list_,
        button_font_, inventory_, Evidyon::AVATAR_INVENTORY_SIZE,
        item_descriptions_, number_of_item_descriptions_);
      break;
      //goto DISPLAY_INVENTORY_COMMON;
  case DISPLAY_EQUIPPED: {
      label_.textLabel()->setText("Equipment");
      GUIHelper_CreateButtonsForEquippedItems(&item_list_,
        button_font_, inventory_, Evidyon::AVATAR_INVENTORY_SIZE,
        item_descriptions_, number_of_item_descriptions_);
    } break;
  case DISPLAY_CONSUMABLES: {
      label_.textLabel()->setText("Click to Use Item or Right-Click to Set Key");
      for (int i = 0; i < Evidyon::AVATAR_INVENTORY_SIZE; ++i) {
        size_t type = inventory_[i].type;
        int quantity = inventory_[i].quantity;
        if (inventory_[i].inventory_index != i ||
            type >= number_of_item_descriptions_ ||
            item_descriptions_[type].consumable == false ||
            quantity <= 0) continue;
        EvidyonGUIButton* button = item_list_.button(i);
        //std::string text;
        char text[128];
        GUIHelper_GetItemNameString(quantity, &item_descriptions_[type], text, 128);
        if (key_bindings) {
          GUIKey bound_key = key_bindings->getKeyForConsumable(type);
          if (bound_key != GUIKEY_NULL) {
            strcat_s(text, 128, " (");
            strcat_s(text, 128, GUIKeyboardKeyName(bound_key));
            strcat_s(text, 128, ")");
          }
        }
        button->setText(text);
        button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
        button->setFont(button_font_);
        button->enable();
      }
    } break;
  }
  item_list_.setScrollPosition(position);
  show(layer);
}


bool EvidyonGUIModule_InventoryList::getMouseOverItem(const ClientAvatarInventoryItem** inventory_item,
                                                      const Item::ItemClientDescription** item_description) {
  int id;
  if (item_list_.getMouseOverButton(&id)) {
    if (inventory_item != NULL) *inventory_item = &inventory_[id];
    if (item_description != NULL) *item_description = &item_descriptions_[inventory_[id].type];
    return true;
  } else {
    return false;
  }
}

bool EvidyonGUIModule_InventoryList::getClickedItem(const ClientAvatarInventoryItem** inventory_item,
                                                    const Item::ItemClientDescription** item_description) {
  int id;
  if (item_list_.getClickedButton(&id)) {
    if (inventory_item != NULL) *inventory_item = &inventory_[id];
    if (item_description != NULL) *item_description = &item_descriptions_[inventory_[id].type];
    return true;
  } else {
    return false;
  }
}



//----[  updateTooltip  ]------------------------------------------------------
void EvidyonGUIModule_InventoryList::updateTooltip() {
  if (!tooltip_ || !isActive()) return;
  const ClientAvatarInventoryItem* inventory_item;
  const Item::ItemClientDescription* item_description;
  if (getMouseOverItem(&inventory_item, &item_description)) {
    GUISize size;
    tooltip_description_.setItem(item_description,
                                &size);
    tooltip_->setContentSize(size);
    if (tooltip_->becomeOwner(this)) {
      tooltip_->showContentAfter(0.0);
      tooltip_->addContent(&tooltip_description_);
    }
  } else {
    tooltip_->releaseOwnership(this);
  }
}



//----[  validateScrollBar  ]--------------------------------------------------
void EvidyonGUIModule_InventoryList::validateScrollBar() {
  float p = item_list_.getScrollPosition();
  if (p >  1.0f) {
    item_list_.setScrollPosition(1.0f);
  }  
}



//----[  setText  ]------------------------------------------------------------
void EvidyonGUIModule_InventoryList::setText(const std::string& text) {
  label_.textLabel()->setText(text);
}



//----[  scrollToTop  ]--------------------------------------------------------
void EvidyonGUIModule_InventoryList::scrollToTop() {
  item_list_.setScrollPosition(0.0f);
}


}