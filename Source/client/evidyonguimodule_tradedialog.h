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
#ifndef __EVIDYONGUIMODULE_TRADEDIALOG_H__
#define __EVIDYONGUIMODULE_TRADEDIALOG_H__
#pragma once



#include "evidyonguiscrollablebuttonlist.h"
#include "common/gui/guimodule.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "evidyonguiitemdescription.h"
#include "common/gui/guitextlabelcomponent.h"
#include "evidyonguimodule_quantityprompt.h"
#include "shared/client-server/tradeitemdescription.h"
#include "shared/client-server/bazaar.h"

namespace Evidyon {

class EvidyonGUITooltipBox;
class EvidyonGUIKeyBoundActionsSentinel;
class ActionTargeter;



//----[  EvidyonGUIModule_TradeDialog  ]---------------------------------------
class EvidyonGUIModule_TradeDialog : public GUIModule {
public:
  EvidyonGUIModule_TradeDialog();
  void create(GUICanvas* primary_canvas,
              GUIFont* button_font,
              const Item::ItemClientDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ClientAvatarInventoryItem* inventory,
              EvidyonGUITooltipBox* tooltip);
  void destroy();

  // Initializes the trade window
  void beginTrade();

  // Closes the trade window and hides the interface
  void endTrade();

  // Updates the list with the current state of the inventory
  void syncInventory();
  void syncTrade(const char* their_name,
                 CurrencyType my_offered_currency,
                 unsigned char* my_items, unsigned int number_of_my_items,
                 CurrencyType their_offered_currency,
                 TradeItemDescription* their_items, unsigned int number_of_their_items);

  // Checks for user input
  void update(ActionTargeter* action_targeter);


private:
  // Client wants to end trade; exit trade screen & send message to
  // server so the other client leaves also.
  bool pollCancelTrade();

  // Wants trade to go through
  bool pollAcceptTrade();

  // Doesn't want trade to go through
  bool pollWithdrawTrade();

  // Other person changed items they are trading
  void tradeWasModified();

  // Returns 'true' and the inventory index of the item to change
  bool pollAddItemToTrade(unsigned int* inventory_index);
  bool pollRemoveItemFromTrade(unsigned int* trade_index);

  // Syncs the mouse-over tooltip
  void updateTooltip();


private:
  bool getMouseOverItem(const Item::ItemClientDescription** item_description);
  bool getClickedInventoryItem(const ClientAvatarInventoryItem** inventory_item,
                               const Item::ItemClientDescription** item_description);
  bool getClickedTradeItem(unsigned int* trade_index);
  void syncAcceptOrWithdrawButton();


private:
  GUIFont* button_font_;
  const ClientAvatarInventoryItem* inventory_;
  const Item::ItemClientDescription* item_descriptions_;
  size_t number_of_item_descriptions_;

  EvidyonGUIScrollableButtonList inventory_items_list_;
  EvidyonGUIScrollableButtonList my_items_list_;
  EvidyonGUIScrollableButtonList their_items_list_;

  GUITextLabelComponent label_inventory_;
  GUITextLabelComponent label_my_items_;
  GUITextLabelComponent label_their_items_;

  bool trade_accepted_;
  EvidyonGUIButton cancel_, accept_or_withdraw_;
  EvidyonGUIButton offer_currency_;

  // When the mouse hovers over a button
  EvidyonGUITooltipBox* tooltip_;
  EvidyonGUIItemDescription tooltip_description_;

  // The last updated set of items that the other player offered
  TradeItemDescription their_items_[AVATAR_INVENTORY_SIZE]; // inventory size is way too large
  size_t number_of_their_items_;

  // Employed to ask the player how much money to offer
  EvidyonGUIModule_QuantityPrompt quantity_prompt_;
};

}



#endif