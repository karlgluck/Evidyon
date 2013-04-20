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
#ifndef __EVIDYONGUIMODULE_BAZAAR_H__
#define __EVIDYONGUIMODULE_BAZAAR_H__
#pragma once

#include "evidyonguiactionbuttonlist.h"
#include "evidyonguiitemdescription.h"
#include "common/gui/guitextlabelcomponent.h"
#include "shared/avatar/evidyon_avatarinventory.h"


#include "evidyonguimodule_bazaarcategoryitems.h"
#include "evidyonguimodule_bazaar_createlistingforinventoryitem.h"
#include "evidyonguimodule_bazaar_createlistingprompt.h"

namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}


namespace Evidyon {

class EvidyonGUIModule_Bazaar;


// Used to interact with the area where players buy/sell items
class EvidyonGUIModule_Bazaar : public GUIModule {
public:
  EvidyonGUIModule_Bazaar();

  void create(GUICanvas* source_canvas,
              GUIFont* button_font,
              const Item::ItemClientDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ClientAvatarInventoryItem* inventory,
              EvidyonGUITooltipBox* tooltip);

  void destroy();

  // Advances the bazaar
  void update(ActionTargeter* action_targeter);

  // Updates the items in the given bazaar category.  If this is
  // the client's own items, it will save the items in the
  // internal list.
  void sync(BazaarCategory category,
            ItemInBazaar* items_in_category,
            size_t number_of_items);

  // Updates the items in the client's own inventory
  void syncInventory();

  // Changes which category to display.  This method will not
  // get the list of items in the category from the server.
  void changeCategory(BazaarCategory category);

  // Returns 'true' if this item is owned by the client.  This is
  // used to disable the buttons for purchasing listings that
  // the client owns.
  bool isOwnListing(const ItemInBazaar* item_in_bazaar) const;

  // Returns the number of items this client has listed
  size_t getNumberOfOwnListedItems() const;

  // Displays the inventory and allows the user to select an item
  void showCreateListing();

  // Displays the prompt to the user and activates it.  The prompt will
  // retain focus until it is canceled or confirmed.
  void showCreateListingPrompt(const char* item_name,
                               const ClientAvatarInventoryItem* item,
                               const Item::ItemClientDescription* description);


private:
  EvidyonGUIButton* getButton(int index);
  EvidyonGUIButton* getCreateListingButton();
  EvidyonGUIButton* getExitButton();
  EvidyonGUIButton* getCategoryButton(BazaarCategory category);

  // Sets up the canvas for the given button
  void alignButton(GUICanvas* source_canvas, EvidyonGUIButton* button, int index);

private:

  // button for each category, own items, create listing, exit
  EvidyonGUIButton buttons_[CATEGORY_COUNT + 3];

  ItemInBazaar own_items_[MAX_ITEMS_PER_BAZAAR_CATEGORY];
  size_t number_of_own_items_;

  EvidyonGUIModule_BazaarCategoryItems items_in_category_;
  EvidyonGUIModule_Bazaar_CreateListingForInventoryItem inventory_;
  EvidyonGUIModule_Bazaar_CreateListingPrompt create_listing_prompt_;

  // Every 15 seconds, the bazaar automatically queries for changes to the items
  // in the current category.  This also prevents the user from querying faster
  // than once every few seconds (to reduce server load)
  unsigned long last_time_queried_for_items_;
};


}




#endif