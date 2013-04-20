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
#include "evidyonguimodule_bazaar.h"
//#include "actiontargeter.h"
#include "client/actionqueue.h"
#include "shared/item/itemclientdescription.h"
#include <windows.h> // for GetTickCount

namespace Evidyon {

EvidyonGUIModule_Bazaar::EvidyonGUIModule_Bazaar() {
  last_time_queried_for_items_ = 0;
}

void EvidyonGUIModule_Bazaar::create(GUICanvas* source_canvas,
    GUIFont* button_font,
    const Item::ItemClientDescription* item_descriptions,
    size_t number_of_item_descriptions,
    const ClientAvatarInventoryItem* inventory,
    EvidyonGUITooltipBox* tooltip) {

  items_in_category_.create(this,
                            source_canvas,
                            button_font,
                            item_descriptions,
                            number_of_item_descriptions,
                            tooltip);
  inventory_.create(this,
                    source_canvas,
                    button_font,
                    item_descriptions,
                    number_of_item_descriptions,
                    inventory,
                    tooltip);

  create_listing_prompt_.create(source_canvas, this, button_font);

  addSubmodule(&items_in_category_);

  // Categories only shown if not listing own items
  for (int i = 0; i < CATEGORY_COUNT; ++i) {
    BazaarCategory category = BazaarCategory(i);
    EvidyonGUIButton* button = getCategoryButton(category);
    button->setText(BazaarCategoryString(category));
    button->setFont(button_font);
    alignButton(source_canvas, button, i);
  }

  {
    EvidyonGUIButton* button = getCreateListingButton();
    button->setText("Create Listing");
    button->setFont(button_font);
    alignButton(source_canvas, button, -1);
  }

  {
    EvidyonGUIButton* button = getCategoryButton(CATEGORY_OWNITEMS);
    button->setText("Listed Items");
    button->setFont(button_font);
    alignButton(source_canvas, button, -2);
  }

  {
    EvidyonGUIButton* button = getExitButton();
    button->setText("Leave the Bazaar");
    button->setFont(button_font);
    GUICanvas* canvas = button->getCanvas();
    canvas->setSourceCanvas(items_in_category_.getListCanvas());
    canvas->alignYBottomOutside(+10);
    canvas->alignXCenter(0);
    canvas->relativeWidth(-50);
    canvas->absoluteHeight(35);
    canvas->relativeDepth(0.0f);
  }

  for (int i = 0; i < CATEGORY_COUNT + 3; ++i) {
    addComponent(getButton(i));
  }


  memset(own_items_, 0, sizeof(ItemInBazaar) * MAX_ITEMS_PER_BAZAAR_CATEGORY);
  number_of_own_items_ = 0;
}



//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIModule_Bazaar::destroy() {
  hide();
  inventory_.destroy();
  items_in_category_.destroy();
  create_listing_prompt_.destroy();
  clearComponents();
}




//----[  update  ]-------------------------------------------------------------
void EvidyonGUIModule_Bazaar::update(ActionTargeter* action_targeter) {
  items_in_category_.update(action_targeter);

  inventory_.update(action_targeter);
  create_listing_prompt_.update(action_targeter);

  if (getCreateListingButton()->pollPushed()) {
    // create listing!
    showCreateListing();
  } else if (getCategoryButton(CATEGORY_OWNITEMS)->pollPushed()) {
    // look at own listed items
    changeCategory(CATEGORY_OWNITEMS);
    //action_targeter->bazaarGetListings()->category_ = CATEGORY_OWNITEMS;
    Client::ActionQueue::add<Action_BazaarGetListings>()
      ->category = CATEGORY_OWNITEMS;
  } else if (getExitButton()->pollPushed()) {
    //action_targeter->userEvent(CLOSE_BAZAAR);
    Client::ActionQueue::add<Action_CloseBazaar>();
  } else {
    // browse a different category
    for (int i = 0; i < CATEGORY_COUNT; ++i) {
      BazaarCategory category = BazaarCategory(i);
      if (getCategoryButton(category)->pollPushed()) {
        if (category != items_in_category_.getCurrentCategory() ||
            (GetTickCount() - last_time_queried_for_items_) > 2000) {
          Client::ActionQueue::add<Action_BazaarGetListings>()
            ->category = category;
          //action_targeter->bazaarGetListings()->category_ = category;
          last_time_queried_for_items_ = GetTickCount();
          changeCategory(category);
        }
      }
    }
  }
  if (GetTickCount() - last_time_queried_for_items_ > 10000) {
    last_time_queried_for_items_ = GetTickCount();
    BazaarCategory category = items_in_category_.getCurrentCategory();
    if (category != CATEGORY_OWNITEMS) { // own items don't change
      // re-request the list of items in the current category
      Client::ActionQueue::add<Action_BazaarGetListings>()
        ->category = items_in_category_.getCurrentCategory();
      //action_targeter->bazaarGetListings()->category_ =
      //  items_in_category_.getCurrentCategory();
    }
  }
}


//----[  sync  ]---------------------------------------------------------------
void EvidyonGUIModule_Bazaar::sync(
    BazaarCategory category,
    ItemInBazaar* items_in_category,
    size_t number_of_items) {

  if (category == CATEGORY_OWNITEMS) {
    memcpy(own_items_, items_in_category, sizeof(ItemInBazaar) * number_of_items);
    number_of_own_items_ = number_of_items;
  }

  items_in_category_.sync(category, items_in_category, number_of_items);
}



//----[  changeCategory  ]-----------------------------------------------------
void EvidyonGUIModule_Bazaar::syncInventory() {
  inventory_.syncInventory();
}



//----[  changeCategory  ]-----------------------------------------------------
void EvidyonGUIModule_Bazaar::changeCategory(BazaarCategory category) {
  clearSubmodules();
  addSubmodule(&items_in_category_);
  items_in_category_.changeCategory(category);
}



//----[  isOwnListing  ]-------------------------------------------------------
bool EvidyonGUIModule_Bazaar::isOwnListing(const ItemInBazaar* item_in_bazaar) const {
  for (size_t i = 0; i < number_of_own_items_; ++i) {
    if (own_items_[i].listing_id == item_in_bazaar->listing_id) return true;
  }
  return false;
}



//----[  getNumberOfOwnListedItems  ]------------------------------------------
size_t EvidyonGUIModule_Bazaar::getNumberOfOwnListedItems() const {
  return number_of_own_items_;
}



//----[  showCreateListing  ]--------------------------------------------------
void EvidyonGUIModule_Bazaar::showCreateListing() {
  clearSubmodules();
  addSubmodule(&inventory_);
}


//----[  showCreateListingPrompt  ]--------------------------------------------
void EvidyonGUIModule_Bazaar::showCreateListingPrompt(
    const char* item_name,
    const ClientAvatarInventoryItem* item,
    const Item::ItemClientDescription* description) {
  clearSubmodules();
  create_listing_prompt_.setListingPrompt(item_name, item, description);
  addSubmodule(&create_listing_prompt_);
}


//----[  getButton  ]----------------------------------------------------------
EvidyonGUIButton* EvidyonGUIModule_Bazaar::getButton(int index) {
  return &buttons_[index];
}



//----[  getCreateListingButton  ]---------------------------------------------
EvidyonGUIButton* EvidyonGUIModule_Bazaar::getCreateListingButton() {
  return &buttons_[0];
}




//----[  getExitButton  ]------------------------------------------------------
EvidyonGUIButton* EvidyonGUIModule_Bazaar::getExitButton() {
  return &buttons_[2];
}



//----[  getCategoryButton  ]--------------------------------------------------
EvidyonGUIButton* EvidyonGUIModule_Bazaar::getCategoryButton(BazaarCategory category) {
  int int_category = int(category);
  if (int_category >= 0  && int_category < int(CATEGORY_COUNT)) {
    return &buttons_[int_category+3];
  } else if (category == CATEGORY_OWNITEMS) {
    return &buttons_[1];
  } else {
    return NULL;
  }
}



//----[  alignButton  ]--------------------------------------------------------
void EvidyonGUIModule_Bazaar::alignButton(GUICanvas* source_canvas, EvidyonGUIButton* button, int index) {
  static const int WIDTH = 180;
  static const int SPACING = 20;
  static const int HEIGHT = 35;
  GUICanvas* canvas = button->getCanvas();
  canvas->setSourceCanvas(source_canvas);
  if (index < 0) { // -1 create listing / -2 own listed items
    canvas->alignXCenter((-400 - WIDTH) / 2 - SPACING/2);
    canvas->alignYCenter((-2 - index) * (HEIGHT + SPACING) + SPACING / 2);
  } else { // category button, starting with 0
    canvas->alignXCenter((400 + WIDTH) / 2 + SPACING/2);
    canvas->alignYCenter((index - CATEGORY_COUNT / 2) * (HEIGHT + SPACING) - SPACING / 2);
  }
  canvas->absoluteDepth(0.4f);
  canvas->absoluteWidth(WIDTH);
  canvas->absoluteHeight(35);
}




}

