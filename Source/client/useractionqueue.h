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
#ifndef __USERACTIONQUEUE_H__
#define __USERACTIONQUEUE_H__
#pragma once



#include <queue>
#include "shared/avatar/evidyon_avatar.h"
#include "shared/client-server/bazaar.h"
#include "shared/actor/actorid.h"

enum UserEventType {
  LOG_OUT_OF_WORLD,
  TOGGLE_VIEW_ANGLE,
  TOGGLE_SHOW_NAMES,
  SAY_AGAIN,
  STOP_ACTION, // unimplemented
  OPEN_INVENTORY,
  OPEN_ATTRIBUTES,
  OPEN_KEY_BINDINGS,
  OPEN_CHAT_AND_ACTIONS,
  OPEN_STAT_DISPLAY,
  OPEN_CHAT,
  OPEN_DROP,
  OPEN_BAZAAR,
  TOGGLE_CHAT_LOG,
  CLOSE_CHAT_AND_ACTIONS,
  CLOSE_INVENTORY,
  CLOSE_KEY_BINDINGS,
  CLOSE_STAT_DISPLAY,
  CLOSE_CHAT,
  CLOSE_DROP,
  //CLOSE_BAZAAR,
  INVENTORY_DROP_ITEMS,
  INVENTORY_EQUIP_ITEMS,
  INVENTORY_UNEQUIP_ITEMS,
  ATTRIBUTES_ADD_ABILITY_POINT,
  SPEAK,
  DROP_ITEM,
  DROP_CURRENCY,
  EQUIP_ITEM,
  UNEQUIP_ITEM,
  //BAZAAR_CREATELISTING,   
  //BAZAAR_RETRACTLISTING,
  //BAZAAR_PURCHASELISTING,
  //BAZAAR_REDEEMLISTING,
  //BAZAAR_GETLISTINGS,
  TAKE_SCREENSHOT,
  OPEN_CAST,
  OPEN_USEITEM,
  EVENT_ESCAPE,
  STORAGE_WITHDRAW_ITEM,
  STORAGE_DEPOSIT_ITEM,
  GEOSID_SACRIFICE_ITEM,
  TRADE_ADDITEM,
  TRADE_SETCURRENCY,
  TRADE_REMOVEITEM,
  TRADE_ACCEPT,
  TRADE_WITHDRAW,
  TRADE_CANCEL,
  SPLIT_STACKED_ITEM,
  QUEST_OFFER_RESPONSE,
  QUEST_GIVE_UP,
  CLOSE_DIALOGS,
  TOGGLE_NAME_DISPLAY,
  INVALID_ACTION,
};

struct UserEventSpeak {
  char text_[Evidyon::Avatar::MAX_SPOKEN_TEXT_LENGTH+1];
};

struct UserEventAddAbilityPoint {
  int ability;
};

struct UserEventOnItem {
  int inventory_index_;
};

struct UserEventDropCurrency {
  Evidyon::CurrencyType amount_;
};

struct UserEventTradeSetCurrency {
  Evidyon::CurrencyType currency_;
};

struct UserEventBazaarCreateListing {
  int inventory_index_;
  Evidyon::CurrencyType listing_price_;
};

struct UserEventBazaarRetractListing {
  Evidyon::BazaarListingID listing_id_;
};

struct UserEventBazaarRedeemListing {
  Evidyon::BazaarListingID listing_id_;
};

struct UserEventBazaarPurchaseListing {
  Evidyon::BazaarListingID listing_id_;
};

struct UserEventBazaarGetListings {
  Evidyon::BazaarCategory category_;
};

struct UserEventQuestOfferResponse {
  unsigned int quest_index;
  Evidyon::Actor::ActorID npc_actor_id;
  int npc_quest_index;
  unsigned int validation_key;
  bool accepted;
};

struct UserEventQuestGiveUp {
  int client_quest_index;
};


struct UserEvent {
  UserEventType type_;
  union {
    UserEventSpeak speak_;
    UserEventAddAbilityPoint add_ability_point_;
    UserEventOnItem item_event_;
    UserEventDropCurrency drop_currency_;
    UserEventTradeSetCurrency trade_set_currency_;
    UserEventBazaarCreateListing bazaar_create_listing_;
    UserEventBazaarRetractListing bazaar_retract_listing_;
    UserEventBazaarRedeemListing bazaar_redeem_listing_;
    UserEventBazaarPurchaseListing bazaar_purchase_listing_;
    UserEventBazaarGetListings bazaar_get_listings_;
    UserEventQuestOfferResponse quest_offer_response_;
    UserEventQuestGiveUp quest_give_up_;
  } data_;
};

class UserEventQueue {
public:
  ~UserEventQueue();

  void addLogOutOfWorld() { addTypeOnlyAction(LOG_OUT_OF_WORLD); }
  void addOpenChatAndActions() { addTypeOnlyAction(OPEN_CHAT_AND_ACTIONS); }
  void addOpenInventory() { addTypeOnlyAction(OPEN_INVENTORY); }
  void addOpenAttributes() { addTypeOnlyAction(OPEN_ATTRIBUTES); }
  void addOpenKeyBindings() { addTypeOnlyAction(OPEN_KEY_BINDINGS); }
  void addCloseChatAndActions() { addTypeOnlyAction(CLOSE_CHAT_AND_ACTIONS); }
  void addInventoryDropItems() { addTypeOnlyAction(INVENTORY_DROP_ITEMS); }
  void addInventoryEquipItems() { addTypeOnlyAction(INVENTORY_EQUIP_ITEMS); }
  void addInventoryUnequipItems() { addTypeOnlyAction(INVENTORY_UNEQUIP_ITEMS); }
  void addCloseInventory() { addTypeOnlyAction(CLOSE_INVENTORY); }
  void addCloseKeyBindings() { addTypeOnlyAction(CLOSE_KEY_BINDINGS); }
  void addCloseStatDisplay() { addTypeOnlyAction(CLOSE_STAT_DISPLAY); }
  void addToggleViewAngle() { addTypeOnlyAction(TOGGLE_VIEW_ANGLE); }
  void addToggleShowNames() { addTypeOnlyAction(TOGGLE_SHOW_NAMES); }
  void addOpenChat() { addTypeOnlyAction(OPEN_CHAT); }
  void addCloseChat() { addTypeOnlyAction(CLOSE_CHAT); }
  void addOpenDrop() { addTypeOnlyAction(OPEN_DROP); }
  void addCloseDrop() { addTypeOnlyAction(CLOSE_DROP); }
  UserEventAddAbilityPoint* addAttributesAddAbilityPoint();
  UserEventSpeak* addSpeak();
  UserEventBazaarCreateListing* addBazaarCreateListing();
  UserEventBazaarRetractListing* addBazaarRetractListing();
  UserEventBazaarRedeemListing* addBazaarRedeemListing();
  UserEventBazaarPurchaseListing* addBazaarPurchaseListing();
  UserEventBazaarGetListings* addBazaarGetListings();
  UserEventQuestOfferResponse* addQuestOfferResponse();
  UserEventQuestGiveUp* addQuestGiveUp();
  bool popNextAction(UserEvent* action);

  public:
  void addTypeOnlyAction(UserEventType type);

private:
  typedef std::queue<UserEvent*> ActionQueue;
  ActionQueue action_queue_;
};



#endif
