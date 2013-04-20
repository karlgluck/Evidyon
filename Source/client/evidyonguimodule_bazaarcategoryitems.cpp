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
#include "evidyonguimodule_bazaarcategoryitems.h"
#include "evidyonguitooltipbox.h"
#include "shared/client-server/bazaar.h"
#include "evidyonguimodule_bazaar.h"
#include "evidyonguihelpermethods.h"
//#include "actiontargeter.h"
#include "client/actionqueue.h"
#include "shared/item/itemclientdescription.h"

namespace Evidyon {



void EvidyonGUIModule_BazaarCategoryItems::create(
    EvidyonGUIModule_Bazaar* bazaar,
    GUICanvas* source_canvas,
    GUIFont* button_font,
    const Item::ItemClientDescription* item_descriptions,
    size_t number_of_item_descriptions,
    EvidyonGUITooltipBox* tooltip) {

  bazaar_ = bazaar;
  current_category_ = CATEGORY_OWNITEMS;
  item_descriptions_ = item_descriptions;
  number_of_item_descriptions_ = number_of_item_descriptions;

  createActionButtonList(button_font, tooltip);

  {
    GUICanvas* canvas = getListCanvas();
    canvas->setSourceCanvas(source_canvas);
    canvas->absoluteWidth(400);
    canvas->absoluteHeight(500);
    canvas->absoluteDepth(0.4f);
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
  }

  tooltip_description_.setFont(button_font);

  label_.textLabel()->setFont(button_font);
  label_.textLabel()->setAlignment(GUIALIGN_CENTER);
  addComponent(&label_);
  {
    GUICanvas* canvas = label_.getCanvas();
    canvas->setSourceCanvas(getListCanvas());
    canvas->relativeWidth(0);
    canvas->alignYTopOutside(-5);
    canvas->alignXCenter(0);
    canvas->relativeDepth(0.0f);
  }
}


void EvidyonGUIModule_BazaarCategoryItems::destroy() {
  bazaar_ = NULL;
  current_category_ = CATEGORY_COUNT;
  item_descriptions_ = NULL;
  number_of_item_descriptions_ = 0;
  destroyActionButtonList();
}

// Called when the category first switches.  Clears the current set of displayed
// items in preparation for a list from the server.
void EvidyonGUIModule_BazaarCategoryItems::changeCategory(BazaarCategory category) {
  GUILayer* layer = hide();

  clearButtons();
  current_category_ = category;

  label_.textLabel()->setText(BazaarCategoryString(category));

  show(layer);
}




//----[  sync  ]---------------------------------------------------------------
void EvidyonGUIModule_BazaarCategoryItems::sync(
    BazaarCategory category,
    ItemInBazaar* items_in_category,
    size_t number_of_items) {

  if (!isActive() || category != current_category_) return;

  GUILayer* layer = hide();

  clearButtons();

  memset(items_, 0, sizeof(items_));
  memcpy(items_, items_in_category, sizeof(ItemInBazaar) * number_of_items);
  number_of_items_ = number_of_items;

  for (int i = 0; i < number_of_items; ++i) {
    unsigned int type = items_[i].type;
    if (type >= number_of_item_descriptions_) continue;
    const Item::ItemClientDescription* description =  &item_descriptions_[type];
    char buffer[128];
    GUIHelper_GetItemNameString(
      items_[i].quantity,
      description,
      buffer,
      sizeof(buffer));

    char name[128];
    if (category == CATEGORY_OWNITEMS) {
      sprintf_s(name, 128, "%14i    %s%s", items_[i].price, items_[i].sold ? "sold: " : "", buffer);
    } else {
      if (items_[i].sold) {
        sprintf_s(name, 128, "%14i    %s (%sno longer available)",
                  items_[i].price,
                  buffer,
                  bazaar_->isOwnListing(&items_[i]) ? "this is your item - " : "");
      } else {
        sprintf_s(
          name,
          128,
          "%14i    %s%s",
          items_[i].price,
          buffer,
          bazaar_->isOwnListing(&items_[i]) ? " (this is your item)" : "");
      }
    }

    EvidyonGUIButton* button = addButton(i, name);
  }

  show(layer);
}




//----[  getCurrentCategory  ]-------------------------------------------------
BazaarCategory EvidyonGUIModule_BazaarCategoryItems::getCurrentCategory() {
  return current_category_;
}



//----[  onButtonClicked  ]----------------------------------------------------
void EvidyonGUIModule_BazaarCategoryItems::onButtonClicked(int button_id, ActionTargeter* action_targeter) {
  // player wants to purchase or withdraw item
  if (button_id >= number_of_items_) return;
  ItemInBazaar* item = &items_[button_id];
  BazaarListingID listing_id = items_[button_id].listing_id;
  if (bazaar_->isOwnListing(item)) {
    if (item->sold) {
      //action_targeter->bazaarRedeemListing()->listing_id_ = listing_id;
      Client::ActionQueue::add<Action_BazaarRedeemListing>()->listing_id = listing_id;
    } else {
      //action_targeter->bazaarRetractListing()->listing_id_ = listing_id;
      Client::ActionQueue::add<Action_BazaarRetractListing>()->listing_id = listing_id;
    }
    bazaar_->changeCategory(CATEGORY_OWNITEMS);
  } else {
    if (!item->sold) {
      //action_targeter->bazaarPurchaseListing()->listing_id_ = listing_id;
      Client::ActionQueue::add<Action_BazaarPurchaseListing>()->listing_id = listing_id;
    }
  }
}


bool EvidyonGUIModule_BazaarCategoryItems::updateTooltipContent(int button_id, EvidyonGUITooltipBox* tooltip) {
  if (button_id >= number_of_items_) return false;
  const ItemInBazaar* item = &items_[button_id];
  unsigned int type = item->type;
  if (type >= number_of_item_descriptions_) return false;
  GUISize size;
  const char* prefix_text = NULL;
  if (bazaar_->isOwnListing(item)) {
    if (item->sold) {
      prefix_text = "-- Sold, Click to Redeem--\n";
    } else {
      prefix_text = "(you own this item; click to withdraw it)\n";
    }
  } else if (item->sold) {
    prefix_text = "(this item has already been purchased)\n";
  }
  tooltip_description_.setItem(prefix_text,
                               &item_descriptions_[type],
                               &size);
  tooltip->setContentSize(size);
  return true;
}


void EvidyonGUIModule_BazaarCategoryItems::addTooltipContent(EvidyonGUITooltipBox* tooltip) {
  tooltip->addContent(&tooltip_description_);
}




}
