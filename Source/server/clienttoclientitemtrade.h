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
#ifndef __CLIENTTOCLIENTITEMTRADE_H__
#define __CLIENTTOCLIENTITEMTRADE_H__
#pragma once



#include "itempointer.h"
#include "shared/client-server/bazaar.h" // for CurrencyType


namespace Evidyon {

class Client;



class ClientToClientItemTrade {
  struct ItemToTrade {
    unsigned int source_inventory_index;
    ItemPointer item_pointer;
  };

public:
  static const int MAX_ITEMS_IN_TRADE = 16;
  
public:

  // Initializes this trade structure
  ClientToClientItemTrade(Client* owner);
  
  // Starts a trade with the other structure
  bool beginTrade(ClientToClientItemTrade* other);

  // Forcibly closes the trade; no items are swapped and both
  // clients close their trade window.
  void cancelTrade();
  
  // Puts an item into the trade window and calls offerChanged.
  void offerItem(unsigned int inventory_index, ItemPointer& item);

  // Changes the amount of currency offered.  This amount is validated
  // to be sure it's less than that posessed by the owner.
  void offerCurrency(CurrencyType amount);
  
  // Pulls an item from the trade.  This erases it from the offering
  // list, shifts down the rest of the items and calls offerChanged
  // on both this client AND the partner.
  // This is especially important because it prevents scamming a trade
  // by changing it after one client has accepted an offer.
  // If the trade index is higher than the number of items offered, the
  // method removes the offered currency amount by calling offerCurrency(0)
  void withdrawItem(unsigned int trade_index);

  // Returns the trade index of an item based on its index in the inventory
  //unsigned int getItemTradeIndex(unsigned int inventory_index);

  // Indicates that this client wants to go through with the trade.
  // If the other client has accepted the trade, executeTrade is
  // invoked and both clients' trade windows are closed.
  void acceptTrade();

  // Turns off acceptance
  void unacceptTrade();

  // Returns whether or not this trader is active
  bool inProgress() const;


private:

  // Makes sure that both this client and its partner are still in
  // a valid state to be trading.  Performs all validation steps
  // such as checking that both clients are logged into the world,
  // they are close to each other and are in a trade with one-
  // another.  If anything goes wrong, this method closes the
  // trade for this client.
  bool invalid();

  // Trades this client's items for those offered by the other.  Before
  // moving any items, both clients are checked to be sure that they
  // can carry the items they are about to receive.  If this check fails,
  // the window is closed and the faulting client gets a message above
  // their avatar's head (ex. "inventory full", "too much weight")
  void executeTrade();
  
  // Called when an item is either deposited or withdrawn by either
  // of the participants.  Calls clientSendTradeUpdate.
  void offerChanged();
  
  // Sends a message to the client to open up the trade window and move
  // into trading mode.
  void clientOpenTradeWindow();
  
  // Sends a message to the client to exit trading mode
  void clientCloseTradeWindow();
  
  // Sends all of the items in the trade to this client.
  void clientSendTradeUpdate();

  // Resets the class
  void zero();


private:
  Client* owner_;
  ClientToClientItemTrade* other_;

  bool accepted_;

  ItemToTrade items_offered_[MAX_ITEMS_IN_TRADE];
  unsigned int number_of_items_offered_;
  CurrencyType currency_offered_;
};


}



#endif
