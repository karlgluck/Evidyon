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

#include "stdafx.h"
#include "useractionqueue.h"


//-----------------------------------------------------------------------------
UserEventQueue::~UserEventQueue() {
  while (!action_queue_.empty()) {
    UserEvent* action = action_queue_.front();
    SAFE_DELETE(action);
    action_queue_.pop();
  }
}


//-----------------------------------------------------------------------------
void UserEventQueue::addTypeOnlyAction(UserEventType type) {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = type;
  action_queue_.push(action);
}

//-----------------------------------------------------------------------------
UserEventAddAbilityPoint* UserEventQueue::addAttributesAddAbilityPoint() {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = ATTRIBUTES_ADD_ABILITY_POINT;
  action_queue_.push(action);
  return &action->data_.add_ability_point_;
}

//
////-----------------------------------------------------------------------------
//UserEventSpeak* UserEventQueue::addSpeak() {
//  UserEvent* action = new UserEvent;
//  ZeroMemory(action, sizeof(UserEvent));
//  action->type_ = SPEAK;
//  action_queue_.push(action);
//  return &action->data_.speak_;
//}
//

//
////-----------------------------------------------------------------------------
//UserEventBazaarCreateListing* UserEventQueue::addBazaarCreateListing() {
//  UserEvent*  action = new UserEvent;
//  ZeroMemory(action, sizeof(UserEvent));
//  action->type_ = BAZAAR_CREATELISTING;
//  return &action->data_.bazaar_create_listing_;
//}
//
//
////-----------------------------------------------------------------------------
//UserEventBazaarRetractListing* UserEventQueue::addBazaarRetractListing() {
//  UserEvent*  action = new UserEvent;
//  ZeroMemory(action, sizeof(UserEvent));
//  action->type_ = BAZAAR_RETRACTLISTING;
//  return &action->data_.bazaar_retract_listing_;
//}
//
////-----------------------------------------------------------------------------
//UserEventBazaarRedeemListing* UserEventQueue::addBazaarRedeemListing() {
//  UserEvent*  action = new UserEvent;
//  ZeroMemory(action, sizeof(UserEvent));
//  action->type_ = BAZAAR_REDEEMLISTING;
//  return &action->data_.bazaar_redeem_listing_;
//}
//
////-----------------------------------------------------------------------------
//UserEventBazaarPurchaseListing* UserEventQueue::addBazaarPurchaseListing() {
//  UserEvent*  action = new UserEvent;
//  ZeroMemory(action, sizeof(UserEvent));
//  action->type_ = BAZAAR_PURCHASELISTING;
//  return &action->data_.bazaar_purchase_listing_;
//}
//
//
////-----------------------------------------------------------------------------
//UserEventBazaarGetListings* UserEventQueue::addBazaarGetListings() {
//  UserEvent*  action = new UserEvent;
//  ZeroMemory(action, sizeof(UserEvent));
//  action->type_ = BAZAAR_GETLISTINGS;
//  return &action->data_.bazaar_get_listings_;
//}
//




//----[  addQuestOfferResponse  ]----------------------------------------------
UserEventQuestOfferResponse* addQuestOfferResponse() {
  UserEvent*  action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = QUEST_OFFER_RESPONSE;
  return &action->data_.quest_offer_response_;
}




//----[  addQuestGiveUp  ]-----------------------------------------------------
UserEventQuestGiveUp* addQuestGiveUp() {
  UserEvent*  action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = QUEST_GIVE_UP;
  return &action->data_.quest_give_up_;
}


//----[  popNextAction  ]------------------------------------------------------
bool UserEventQueue::popNextAction(UserEvent* action) {
  ASSERT(action) else return false;
  if (action_queue_.empty()) return false;
  UserEvent* queued_action = action_queue_.front();
  action_queue_.pop();
  memcpy(action, queued_action, sizeof(UserEvent));
  SAFE_DELETE(queued_action);
  return true;
}