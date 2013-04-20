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
#include "clienttoclientitemtrade.h"
#include <memory.h>

#include "client.h"
#include "shared/client-server/packets.h"
#include "item.h"
#include "itempointer.h"

#include "actor.h" // for saying inventory full / etc. when trading

#include "shared/item/itemserverdescription.h"

#include <map>

#include "server/globallogger.h"

namespace Evidyon {




  
//----[  ClientToClientItemTrade  ]--------------------------------------------
ClientToClientItemTrade::ClientToClientItemTrade(Client* owner) {
  owner_ = owner;
  zero();
}





//----[  beginTrade  ]---------------------------------------------------------
bool ClientToClientItemTrade::beginTrade(ClientToClientItemTrade* other) {
  cancelTrade();
  if (other == NULL) return false;
  other->cancelTrade();
  other_ = other;
  other->other_ = this;
  clientOpenTradeWindow();
  other->clientOpenTradeWindow();
  return true;
}




//----[  cancelTrade  ]--------------------------------------------------------
void ClientToClientItemTrade::cancelTrade() {
  if (invalid()) return;
  other_->clientCloseTradeWindow();
  other_->zero();
  clientCloseTradeWindow();
  zero();
}




//----[  offerItem  ]----------------------------------------------------------
void ClientToClientItemTrade::offerItem(unsigned int inventory_index, ItemPointer& item) {
  if (invalid()) return;

  if (number_of_items_offered_ >= MAX_ITEMS_IN_TRADE) return;
  items_offered_[number_of_items_offered_].source_inventory_index = inventory_index;
  items_offered_[number_of_items_offered_].item_pointer.copy(item);
  ++number_of_items_offered_;

  offerChanged();
  other_->offerChanged();
}




//----[  offerCurrency  ]------------------------------------------------------
void ClientToClientItemTrade::offerCurrency(CurrencyType amount) {
  if (invalid()) return;
  if (!owner_->hasCurrency(amount)) return;

  currency_offered_ = amount;

  offerChanged();
  other_->offerChanged();
}





//----[  withdrawItem  ]-------------------------------------------------------
void ClientToClientItemTrade::withdrawItem(unsigned int trade_index) {
  if (invalid()) return;

  // by disabling this, a client can pull an item from trade and it
  // will automatically remove acceptance on both sides--that way if
  // they've accidentally stuck something in there, they can remove
  // it without having to hit "unaccept" first...
  //if (accepted_) return false; // can't withdraw if accepting trade

  if (number_of_items_offered_ <= trade_index) {
    offerCurrency(0);
  } else {
    items_offered_[trade_index].item_pointer.reset(); // safety
    for (size_t i = trade_index + 1; i < number_of_items_offered_; ++i) {
      items_offered_[i-1].source_inventory_index = items_offered_[i].source_inventory_index;
      items_offered_[i-1].item_pointer.copy(items_offered_[i].item_pointer);
    }
    items_offered_[--number_of_items_offered_].source_inventory_index = -1;
    items_offered_[number_of_items_offered_].item_pointer.reset();
  }

  offerChanged();
  other_->offerChanged();
}


//
//// Returns the trade index of an item based on its index in the inventory
//unsigned int ClientToClientItemTrade::getItemTradeIndex(unsigned int inventory_index) {
//  if (invalid()) return -1;
//  for (int i = 0; i < number_of_items_offered_; ++i) {
//    if (items_offered_[i].source_inventory_index == inventory_index) return i;
//  }
//  return -1;
//}




//----[  acceptTrade  ]--------------------------------------------------------
void ClientToClientItemTrade::acceptTrade() {
  if (invalid()) return;
  accepted_ = true;
  if (other_->accepted_) {
    executeTrade();
    cancelTrade();
  }
}





//----[  unacceptTrade  ]------------------------------------------------------
void ClientToClientItemTrade::unacceptTrade() {
  if (invalid()) return;
  accepted_ = false;
}


//----[  inProgress  ]---------------------------------------------------------
bool ClientToClientItemTrade::inProgress() const {
  bool valid = (other_ && !owner_->getActor()->invalid() &&
                          !other_->owner_->getActor()->invalid());
  return valid;
}



//----[  invalid  ]------------------------------------------------------------
bool ClientToClientItemTrade::invalid() {
  bool is_invalid = (!other_ || owner_->getActor()->invalid() ||
                        other_->owner_->getActor()->invalid());
  if (is_invalid) { zero(); }
  return is_invalid;
}




//----[  executeTrade  ]-------------------------------------------------------
void ClientToClientItemTrade::executeTrade() {
  if (invalid()) return;

  Client* owner = owner_;
  ClientItemsInterface* owner_items = owner->getItems();
  Client* other_owner = other_->owner_;
  ClientItemsInterface* other_owner_items = other_owner->getItems();

  unsigned int their_items_offered = other_->number_of_items_offered_;
  unsigned int my_items_offered = number_of_items_offered_;

  // Check to make sure there is enough space in each player's inventory.
  // This will have to be modified when item weights are taken into account.
  if (my_items_offered > their_items_offered) {
    if (!other_owner_items->canHoldItems(my_items_offered - their_items_offered)) {
      const ActorInstance* actor = other_owner->getActor()->dereference();
      if (actor) { actor->say("Inventory Full"); }
      cancelTrade();
      return;
    }
  } else if (their_items_offered > my_items_offered) {
    if (!owner_items->canHoldItems(their_items_offered - my_items_offered)) {
      const ActorInstance* actor = owner->getActor()->dereference();
      if (actor) { actor->say("Inventory Full"); }
      cancelTrade();
      return;
    }
  }

  // Get both lists of items
  typedef std::map<unsigned int,ItemToTrade*> Map;
  typedef Map::value_type MapValue;
  typedef Map::reverse_iterator MapRevIter;
  Map my_items, their_items;
  for (int i = 0; i < number_of_items_offered_; ++i) {
    my_items.insert(MapValue(items_offered_[i].source_inventory_index,
                            &items_offered_[i]));
  }
  for (int i = 0; i < other_->number_of_items_offered_; ++i) {
    their_items.insert(MapValue(other_->items_offered_[i].source_inventory_index,
                               &other_->items_offered_[i]));
  }

  // Items to give to each client
  ItemPointer for_me[MAX_ITEMS_IN_TRADE];
  unsigned int number_for_me = 0;
  ItemPointer for_them[MAX_ITEMS_IN_TRADE];
  unsigned int number_for_them = 0;

  // Swap currency
  CurrencyType currency_offered = currency_offered_;
  CurrencyType other_currency_offered = other_->currency_offered_;
  CurrencyType currency_for_me = other_owner->takeCurrency(other_currency_offered);
  CurrencyType currency_for_them = owner->takeCurrency(currency_offered);
  if ((other_currency_offered != currency_for_me) ||
      (currency_offered != currency_for_them)) {
    goto ERROR_UNDO_TRADE;          
  }

  // Go backward through the list, since the list collapses
  // down as items are removed.
  for (MapRevIter j = my_items.rbegin(); j != my_items.rend(); ++j) {
    ItemPointer item;
    owner_items->removeFromInventory(j->first, NULL, &item);
    if (!item.equals(j->second->item_pointer)) goto ERROR_UNDO_TRADE;
    for_them[number_for_them++].copy(item);
  }

  for (MapRevIter j = their_items.rbegin(); j != their_items.rend(); ++j) {
    ItemPointer item;
    other_owner_items->removeFromInventory(j->first, NULL, &item);
    if (!item.equals(j->second->item_pointer)) goto ERROR_UNDO_TRADE;
    for_me[number_for_me++].copy(item);
  }

  // log this trade
  log_trade(owner->getLoggedIntoCharacterID(),
            for_them,
            number_for_them,
            currency_for_them,
            other_owner->getLoggedIntoCharacterID(),
            for_me,
            number_for_me,
            currency_for_me);

  // This trade is going to succeed; swap the items and the currency
  for (unsigned int i = 0; i < number_for_them; ++i) {
    other_owner_items->addToInventory(for_them[i], NULL);
  }
  for (unsigned int i = 0; i < number_for_me; ++i) {
    owner_items->addToInventory(for_me[i], NULL);
  }
  other_owner->giveCurrency(currency_for_them);
  owner->giveCurrency(currency_for_me);

  // Update inventories
  owner->syncCurrency();
  owner->sendFullInventoryUpdate();
  other_owner->syncCurrency();
  other_owner->sendFullInventoryUpdate();

  return;


// we go to this label if something happens such that the trade has to
// abort.
ERROR_UNDO_TRADE:

  // give currency back to me
  owner->giveCurrency(currency_for_them);
  other_owner->giveCurrency(currency_for_me);

  // give items back to me
  for (unsigned int i = 0; i < number_for_them; ++i) {
    owner_items->addToInventory(for_them[i], NULL);
  }

  // give items back to them
  for (unsigned int i = 0; i < number_for_me; ++i) {
    other_owner_items->addToInventory(for_me[i], NULL);
  }

  { // Abort on both clients
    const ActorInstance* actor = other_owner->getActor()->dereference();
    if (actor) { actor->say("Trade Error (Canceled)"); }
  }
  {
    const ActorInstance* actor = owner->getActor()->dereference();
    if (actor) { actor->say("Trade Error (Canceled)"); }
  }
}



// Called when an item is either deposited or withdrawn by either
// of the participants.  Calls clientSendTradeUpdate.
void ClientToClientItemTrade::offerChanged() {
  if (invalid()) return;
  accepted_ = false;
  clientSendTradeUpdate();
}



// Sends a message to the client to open up the trade window and move
// into trading mode.
void ClientToClientItemTrade::clientOpenTradeWindow() {
  clientSendTradeUpdate(); // re-sending the offer opens the window
}



// Sends a message to the client to exit trading mode
void ClientToClientItemTrade::clientCloseTradeWindow() {
  using namespace Evidyon::Network::Packet;
  ENetPacket* packet = NetMsg_Trade_Cancel::writePacket(ENET_PACKET_FLAG_RELIABLE);
  owner_->sendPacket(packet);
}



// Sends all of the items in the trade to this client.
void ClientToClientItemTrade::clientSendTradeUpdate() {
  using namespace Evidyon::Network::Packet;

  unsigned char my_items[MAX_ITEMS_IN_TRADE];
  size_t number_of_my_items = number_of_items_offered_;
  for (size_t i = 0; i < number_of_my_items; ++i) {
    my_items[i] = items_offered_[i].source_inventory_index;
  }

  TradeItemDescription their_items[MAX_ITEMS_IN_TRADE];
  size_t number_of_their_items = other_->number_of_items_offered_;
  for (size_t i = 0; i < number_of_their_items; ++i) {

    // get the item from the other side of the trade
    ItemInstance* item = other_->items_offered_[i].item_pointer.dereference();
    if (!item) { cancelTrade(); return; }

    // fill out the item description
    their_items[i].type     = item->getDescription()->type_index;
    their_items[i].quantity = item->getQuantity();
  }


  // create and send the update packet
  const char* other_name = other_->owner_->getName();
  ENetPacket* packet = NetMsg_Trade_TellItemsInTrade::Serializer::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      &CopiedArray<char>(other_name, strlen(other_name)),
      currency_offered_,
      &CopiedArray<unsigned char>(my_items, number_of_my_items),
      other_->currency_offered_,
      &CopiedArray<TradeItemDescription>(their_items, number_of_their_items));
  owner_->sendPacket(packet);
}


void ClientToClientItemTrade::zero() {
  other_ = NULL;
  accepted_ = false;
  memset(items_offered_, 0, sizeof(items_offered_));
  number_of_items_offered_ = 0;
  currency_offered_ = 0;
}

}