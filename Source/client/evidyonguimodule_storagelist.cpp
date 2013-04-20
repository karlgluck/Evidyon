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
#include "evidyonguimodule_storagelist.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguikeyboundactionssentinel.h"
#include "shared/item/itemclientdescription.h"


namespace Evidyon {


EvidyonGUIModule_StorageList::EvidyonGUIModule_StorageList() {
  tooltip_ = NULL;
  button_font_ = NULL;
  storage_ = NULL;
  item_descriptions_ = NULL;
  number_of_item_descriptions_ = 0;
}



void EvidyonGUIModule_StorageList::create(GUICanvas* source_canvas,
                                          GUIFont* button_font,
                                          const Item::ItemClientDescription* item_descriptions,
                                          size_t number_of_item_descriptions,
                                          const ItemInStorage* storage,
                                          EvidyonGUITooltipBox* tooltip) {
  storage_ = storage;
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
    label_.textLabel()->setText("Your Account's Item Storage");
  }

  tooltip_ = tooltip;

  tooltip_description_.setFont(button_font);
}



void EvidyonGUIModule_StorageList::destroy() {
  clearComponents();
  button_font_ = NULL;
  hide();
}


void EvidyonGUIModule_StorageList::resetStorageContent() {
  GUILayer* layer = hide();
  item_list_.clear();
  item_list_.setScrollPosition(0.0f);
  show(layer);
}



GUICanvas* EvidyonGUIModule_StorageList::getCanvas() {
  return item_list_.getCanvas();
}



void EvidyonGUIModule_StorageList::sync() {
  GUILayer* layer = hide();
  float scroll_position = item_list_.getScrollPosition();
  item_list_.clear();

  for (int i = 0; i < Evidyon::MAX_ITEMS_IN_STORAGE; ++i) {
    size_t type = storage_[i].type;
    int quantity = storage_[i].quantity;
    if (storage_[i].index != i ||
        type >= number_of_item_descriptions_ ||
        quantity <= 0) continue;
    EvidyonGUIButton* button = item_list_.button(i);
    std::string text;
    if (quantity > 1 || item_descriptions_[type].consumable) {
      char num[64];
      sprintf_s(num, 64, "%i ",quantity );
      text = num;
    }
    text.append(item_descriptions_[type].name);
    button->setText(text.c_str());
    button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
    button->setFont(button_font_);
    button->enable();
  }

  item_list_.setScrollPosition(scroll_position);
  show(layer);
}


bool EvidyonGUIModule_StorageList::getMouseOverItem(const ItemInStorage** storage_item,
                                                    const Item::ItemClientDescription** item_description) {
  int id;
  if (item_list_.getMouseOverButton(&id)) {
    if (storage_item != NULL) *storage_item = &storage_[id];
    if (item_description != NULL) *item_description = &item_descriptions_[storage_[id].type];
    return true;
  } else {
    return false;
  }
}

bool EvidyonGUIModule_StorageList::getClickedItem(const ItemInStorage** storage_item,
                                                  const Item::ItemClientDescription** item_description) {
  int id;
  if (item_list_.getClickedButton(&id)) {
    if (storage_item != NULL) *storage_item = &storage_[id];
    if (item_description != NULL) *item_description = &item_descriptions_[storage_[id].type];
    return true;
  } else {
    return false;
  }
}


void EvidyonGUIModule_StorageList::updateTooltip() {
  if (!tooltip_ || !isActive()) return;
  const ItemInStorage* storage_item;
  const Item::ItemClientDescription* item_description;
  if (getMouseOverItem(&storage_item, &item_description)) {
    GUISize size;
    tooltip_description_.setItem(storage_item,
                                 item_description,
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




//----[  scrollToTop  ]--------------------------------------------------------
void EvidyonGUIModule_StorageList::scrollToTop() {
  item_list_.setScrollPosition(0.0f);
}



}