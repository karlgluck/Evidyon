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
#include "evidyonguimodule_tradedialog.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguihelpermethods.h"
#include "shared/client-server/tradeitemdescription.h"
#include "actiontargeter.h"
#include "shared/item/itemclientdescription.h"

namespace Evidyon {



//----[  EvidyonGUIModule_TradeDialog  ]---------------------------------------
EvidyonGUIModule_TradeDialog::EvidyonGUIModule_TradeDialog() {
  button_font_ = NULL;
  inventory_ = NULL;
  item_descriptions_ = NULL;
  number_of_item_descriptions_ = 0;

  trade_accepted_ = false;

  tooltip_ = NULL;

  number_of_their_items_ = 0;
}




//----[  create  ]-------------------------------------------------------------
void EvidyonGUIModule_TradeDialog::create(
            GUICanvas* primary_canvas,
            GUIFont* button_font,
            const Item::ItemClientDescription* item_descriptions,
            size_t number_of_item_descriptions,
            const ClientAvatarInventoryItem* inventory,
            EvidyonGUITooltipBox* tooltip) {

  inventory_ = inventory;
  button_font_ = button_font;
  item_descriptions_ = item_descriptions;
  number_of_item_descriptions_ = number_of_item_descriptions;
  addComponent(&inventory_items_list_);
  addComponent(&my_items_list_);
  addComponent(&their_items_list_);
  addComponent(&cancel_);
  addComponent(&accept_or_withdraw_);
  addComponent(&label_inventory_);
  addComponent(&label_my_items_);
  addComponent(&label_their_items_);
  addComponent(&offer_currency_);

  inventory_items_list_.setLineHeight(25);
  my_items_list_.setLineHeight(25);
  their_items_list_.setLineHeight(25);

  {
    GUICanvas* canvas = inventory_items_list_.getCanvas();
    canvas->setSourceCanvas(primary_canvas);
    canvas->absoluteWidth(380);
    canvas->absoluteHeight(500);
    canvas->alignXCenter(380/2+15);
    canvas->alignYCenter(-30);
    canvas->absoluteDepth(0.4f);
  }
  {
    GUICanvas* canvas = my_items_list_.getCanvas();
    canvas->setSourceCanvas(inventory_items_list_.getCanvas());
    canvas->absoluteWidth(370);
    canvas->absoluteHeight(230);
    canvas->alignXLeftOutside(-30);
    canvas->alignYTop(0);
    canvas->absoluteDepth(0.4f);
  }
  {
    GUICanvas* canvas = their_items_list_.getCanvas();
    canvas->setSourceCanvas(my_items_list_.getCanvas());
    canvas->absoluteWidth(370);
    canvas->absoluteHeight(280);
    canvas->alignXCenter(0);
    canvas->alignYBottomOutside(+20);
    canvas->absoluteDepth(0.4f);
  }
  {
    cancel_.setText("Cancel");
    cancel_.setFont(button_font);
    GUICanvas* canvas = cancel_.getCanvas();
    canvas->setSourceCanvas(their_items_list_.getCanvas());
    canvas->absoluteWidth(160);
    canvas->absoluteHeight(35);
    canvas->alignXLeft(5);
    canvas->alignYBottomOutside(+5);
    canvas->absoluteDepth(0.4f);
  }
  {
    accept_or_withdraw_.setFont(button_font);
    GUICanvas* canvas = accept_or_withdraw_.getCanvas();
    canvas->setSourceCanvas(their_items_list_.getCanvas());
    canvas->absoluteWidth(160);
    canvas->absoluteHeight(35);
    canvas->alignXRight(-5);
    canvas->alignYBottomOutside(+5);
    canvas->absoluteDepth(0.4f);
  }
  {
    label_inventory_.textLabel()->setText("Inventory");
    GUICanvas* canvas = label_inventory_.getCanvas();
    canvas->setSourceCanvas(inventory_items_list_.getCanvas());
    canvas->relativeWidth(0);
    canvas->absoluteHeight(35);
    canvas->alignXCenter(0);
    canvas->alignYTopOutside(10);
    canvas->absoluteDepth(0.4f);
  }
  {
    GUICanvas* canvas = label_my_items_.getCanvas();
    canvas->setSourceCanvas(my_items_list_.getCanvas());
    canvas->relativeWidth(0);
    canvas->absoluteHeight(35);
    canvas->alignXCenter(0);
    canvas->alignYTopOutside(10);
    canvas->absoluteDepth(0.4f);

  }
  {
    GUICanvas* canvas = label_their_items_.getCanvas();
    canvas->setSourceCanvas(their_items_list_.getCanvas());
    canvas->relativeWidth(0);
    canvas->absoluteHeight(35);
    canvas->alignXCenter(0);
    canvas->alignYTopOutside(10);
    canvas->absoluteDepth(0.4f);
  }
  {
    offer_currency_.setFont(button_font);
    offer_currency_.setText("Add Gold");
    GUICanvas* canvas = offer_currency_.getCanvas();
    canvas->setSourceCanvas(inventory_items_list_.getCanvas());
    canvas->relativeWidth(-50);
    canvas->absoluteHeight(35);
    canvas->alignXCenter(0);
    canvas->alignYBottomOutside(10);
    canvas->relativeDepth(0.0f);
  }

  tooltip_ = tooltip;
  tooltip_description_.setFont(button_font);

  quantity_prompt_.create(primary_canvas, button_font);
}




//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIModule_TradeDialog::destroy() {
  hide();
  quantity_prompt_.destroy();
  clearComponents();
  button_font_ = NULL;
  trade_accepted_ = false;
}





//----[  beginTrade  ]---------------------------------------------------------
void EvidyonGUIModule_TradeDialog::beginTrade() {
  clearSubmodules();
}




//----[  endTrade  ]-----------------------------------------------------------
void EvidyonGUIModule_TradeDialog::endTrade() {
  hide();
  inventory_items_list_.clear();
  my_items_list_.clear();
  their_items_list_.clear();
  syncTrade("", 0, NULL, 0, 0, NULL, 0); // clear trade
  clearSubmodules();
}





//----[  syncInventory  ]------------------------------------------------------
void EvidyonGUIModule_TradeDialog::syncInventory() {
  GUILayer* layer = hide();

  inventory_items_list_.clear();

  GUIHelper_CreateButtonsForInventoryItems(
    &inventory_items_list_,
    button_font_,
    inventory_,
    Evidyon::AVATAR_INVENTORY_SIZE,
    item_descriptions_,
    number_of_item_descriptions_);

  show(layer);
}



//----[  syncTrade  ]----------------------------------------------------------
void EvidyonGUIModule_TradeDialog::syncTrade(
    const char* their_name,
    CurrencyType my_offered_currency,
    unsigned char* my_items, unsigned int number_of_my_items,
    CurrencyType their_offered_currency,
    TradeItemDescription* their_items, unsigned int number_of_their_items) {
  GUILayer* layer = hide();

  {
    char trade_text[256];
    sprintf_s(trade_text, 256, "%s offers", their_name);
    label_their_items_.textLabel()->setText(trade_text);
    sprintf_s(trade_text, 256, "Trading with %s", their_name);
    label_my_items_.textLabel()->setText(trade_text);

    label_inventory_.textLabel()->setAlignment(GUIALIGN_CENTER);
    label_inventory_.textLabel()->setFont(button_font_);
    label_inventory_.textLabel()->setTextColor(0xFFFFFFFF);
    label_their_items_.textLabel()->setAlignment(GUIALIGN_CENTER);
    label_their_items_.textLabel()->setFont(button_font_);
    label_their_items_.textLabel()->setTextColor(0xFFFFFFFF);
    label_my_items_.textLabel()->setAlignment(GUIALIGN_CENTER);
    label_my_items_.textLabel()->setFont(button_font_);
    label_my_items_.textLabel()->setTextColor(0xFFFFFFFF);
  }

  inventory_items_list_.clear();
  my_items_list_.clear();
  their_items_list_.clear();

  // create the offered items list
  std::set<int> offered;
  for (int i = 0; i < number_of_my_items; ++i) {
    int index = my_items[i];
    if (index > Evidyon::AVATAR_INVENTORY_SIZE) continue;
    GUIHelper_CreateButtonForInventoryItem(
      i,
      &my_items_list_,
      button_font_,
      &inventory_[index],
      item_descriptions_,
      number_of_item_descriptions_);
    offered.insert(index);
  }

  // add my currency if it's non-zero
  if (my_offered_currency > 0) {
    EvidyonGUIButton* button = my_items_list_.button(AVATAR_INVENTORY_SIZE + 1);
    button->enable();
    char amount[128];
    sprintf_s(amount, 128, "%lu gold", my_offered_currency);
    button->setText(amount);
    button->setFont(button_font_);
    button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
  }

  // create the inventory with items not in trade
  int offered_item = 0;
  for (int i = 0; i < Evidyon::AVATAR_INVENTORY_SIZE; ++i) {
    if (offered.find(i) != offered.end()) continue;
    GUIHelper_CreateButtonForInventoryItem(
      i,
      &inventory_items_list_,
      button_font_,
      &inventory_[i],
      item_descriptions_,
      number_of_item_descriptions_);
  }

  // create the other player's offered items
  for (int i = 0; i < number_of_their_items; ++i) {
    unsigned int type = their_items[i].type;
    if (type >= number_of_item_descriptions_) continue;
    const Item::ItemClientDescription* desc = &item_descriptions_[type];
    EvidyonGUIButton* button = their_items_list_.button(i);
    char text[256];
    GUIHelper_GetItemNameString(their_items[i].quantity, desc, text, 256);
    button->setText(text);
    button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
    button->setFont(button_font_);
    button->enable();
  }

  // add their currency if it's non-zero
  if (their_offered_currency > 0) {
    EvidyonGUIButton* button = their_items_list_.button(AVATAR_INVENTORY_SIZE + 1);
    button->enable();
    char amount[128];
    sprintf_s(amount, 128, "%lu gold", their_offered_currency);
    button->setText(amount);
    button->setFont(button_font_);
    button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
  }

  // copy their items
  if (their_items == NULL || number_of_their_items == 0) {
    memset(their_items_, 0, sizeof(their_items_));
    number_of_their_items_ = 0;
  } else {
    memcpy(their_items_, their_items, sizeof(TradeItemDescription) * number_of_their_items);
    number_of_their_items_ = number_of_their_items;
  }

  syncAcceptOrWithdrawButton();

  show(layer);
}




//----[  pollCancelTrade  ]----------------------------------------------------
bool EvidyonGUIModule_TradeDialog::pollCancelTrade() {
  if (cancel_.wasPushed()) {
    cancel_.resetPushed();
    return true;
  }

  return false;
}





//----[  pollAcceptTrade  ]----------------------------------------------------
bool EvidyonGUIModule_TradeDialog::pollAcceptTrade() {
  if (false == trade_accepted_) {
    if (accept_or_withdraw_.wasPushed()) {
      accept_or_withdraw_.resetPushed();
      return true;
    }
  }

  return false;
}





//----[  pollWithdrawTrade  ]--------------------------------------------------
bool EvidyonGUIModule_TradeDialog::pollWithdrawTrade() {
  if (true == trade_accepted_) {
    if (accept_or_withdraw_.wasPushed()) {
      accept_or_withdraw_.resetPushed();
      return true;
    }
  }

  return false;
}





//----[  tradeWasModified  ]---------------------------------------------------
void EvidyonGUIModule_TradeDialog::tradeWasModified() {
  trade_accepted_ = false;
  syncAcceptOrWithdrawButton();
}





//----[  pollAddItemToTrade  ]-------------------------------------------------
bool EvidyonGUIModule_TradeDialog::pollAddItemToTrade(unsigned int* inventory_index) {
  const ClientAvatarInventoryItem* inventory_item = NULL;
  if (getClickedInventoryItem(&inventory_item, NULL)) {
    *inventory_index = inventory_item->inventory_index;
    return true;
  }
  return false;
}




//----[  pollRemoveItemFromTrade  ]--------------------------------------------
bool EvidyonGUIModule_TradeDialog::pollRemoveItemFromTrade(unsigned int* trade_index) {
  return getClickedTradeItem(trade_index);
}





//----[  updateTooltip  ]------------------------------------------------------
void EvidyonGUIModule_TradeDialog::updateTooltip() {
  if (!tooltip_ || !isActive()) return;
  const Item::ItemClientDescription* item_description;
  if (getMouseOverItem(&item_description)) {
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




//----[  update  ]-------------------------------------------------------------
void EvidyonGUIModule_TradeDialog::update(ActionTargeter* action_targeter) {
  if (pollAcceptTrade())   action_targeter->userEvent(TRADE_ACCEPT);
  if (pollCancelTrade())   action_targeter->userEvent(TRADE_CANCEL);
  if (pollWithdrawTrade()) action_targeter->userEvent(TRADE_WITHDRAW);

  unsigned int inventory_index;
  if (pollAddItemToTrade(&inventory_index)) {
    action_targeter->tradeAddItem()->inventory_index_ = inventory_index;
  }

  unsigned int trade_index;
  if (pollRemoveItemFromTrade(&trade_index)) {
    action_targeter->tradeRemoveItem()->inventory_index_ = trade_index;
  }

  updateTooltip();

  if (quantity_prompt_.isActive()) {
    //TODO:
    //CurrencyType quantity = quantity_prompt_.getValue();
    //if (quantity > myCharacter.money) { disable the OK button }
    if (quantity_prompt_.pollOK()) {
      action_targeter->tradeSetCurrency()->currency_ = quantity_prompt_.getValue();
      removeSubmodule(&quantity_prompt_);
    }
    if (quantity_prompt_.pollCancel()) {
      removeSubmodule(&quantity_prompt_);
    }
  } else {
    if (offer_currency_.pollPushed()) {
      addSubmodule(&quantity_prompt_);
      quantity_prompt_.initialize("How much gold do you want to offer?", "Add Gold", "Cancel");
    }
  }
}



//----[  getMouseOverItem  ]---------------------------------------------------
bool EvidyonGUIModule_TradeDialog::getMouseOverItem(const Item::ItemClientDescription** item_description) {
  int id;

  if (inventory_items_list_.getMouseOverButton(&id)) {
    if (id > AVATAR_INVENTORY_SIZE) return false;
    const ClientAvatarInventoryItem* item = &inventory_[id];
    if (item->inventory_index != id || item->type >= number_of_item_descriptions_) return false;
    if (item_description != NULL) *item_description = &item_descriptions_[item->type];
    return true;
  }

  if (my_items_list_.getMouseOverButton(&id)) {
    if (id > AVATAR_INVENTORY_SIZE) return false;
    const ClientAvatarInventoryItem* item = &inventory_[id];
    if (item->inventory_index != id || item->type >= number_of_item_descriptions_) return false;
    if (item_description != NULL) *item_description = &item_descriptions_[item->type];
    return true;
  }

  if (their_items_list_.getMouseOverButton(&id)) {
    if (id > number_of_their_items_) return false;
    TradeItemDescription* item = &their_items_[id];
    if (item->type >= number_of_item_descriptions_) return false;
    if (item_description != NULL) *item_description = &item_descriptions_[item->type];
    return true;
  }

  return false;
}




//----[  getClickedInventoryItem  ]--------------------------------------------
bool EvidyonGUIModule_TradeDialog::getClickedInventoryItem(
    const ClientAvatarInventoryItem** inventory_item,
    const Item::ItemClientDescription** item_description) {
  int id;
  if (inventory_items_list_.getClickedButton(&id)) {
    if (inventory_item != NULL) *inventory_item = &inventory_[id];
    if (item_description != NULL) *item_description = &item_descriptions_[inventory_[id].type];
    return true;
  } else {
    return false;
  }
}




//----[  getClickedTradeItem  ]------------------------------------------------
bool EvidyonGUIModule_TradeDialog::getClickedTradeItem(unsigned int* trade_index) {
  int id;
  if (my_items_list_.getClickedButton(&id)) {
    if (trade_index != NULL) *trade_index = (unsigned int)id;
    return true;
  } else {
    return false;
  }
}




//----[  syncAcceptOrWithdrawButton  ]-----------------------------------------
void EvidyonGUIModule_TradeDialog::syncAcceptOrWithdrawButton() {
  accept_or_withdraw_.setText(trade_accepted_ ? "withdraw" : "accept");
}

}