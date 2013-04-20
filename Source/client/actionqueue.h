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
#ifndef __CLIENT_ACTIONQUEUE_H__
#define __CLIENT_ACTIONQUEUE_H__
#pragma once

#include <list>
#include "common/kg/singleton.h"
#include "client/actions.h"

namespace Evidyon {
namespace Client {


//----[  ActionQueue  ]--------------------------------------------------------
// Somewhere in the program:
//  ActionQueue q;
// To add elements to the queue:
//  TYPE* t = ActionQueue::add<TYPE>();
//  t->data = ...;
// To use elements:
//  ActionProcessor p;
//  ActionQueue::singleton()->process(&p);
class ActionQueue : public kg::Singleton<ActionQueue> {
  typedef std::pair<ActionType,void*> Pair;
  typedef std::list<Pair> List;

public:

  ~ActionQueue() {
    clear();
  }

  // Puts a new action into the queue.  This method can be called from
  // anywhere in the program.
  template <typename _T> static _T* add() {
    _T* data = new _T;
    singleton()->list_.push_back(Pair(_T::TYPE, data));
    return data;
  }

  // Adds a directive to the queue.  These cannot have any parameters.
  void addCommand(ActionType action) {
    #define CASE(structure)  case structure::TYPE: add<structure>(); break;
    switch (action) {
    default: assert(!"Action type cannot be expressed as a command"); break;
    CASE(Action_LogOutOfWorld);
    CASE(Action_ToggleViewAngle);
    CASE(Action_ToggleShowNames);
    CASE(Action_SayAgain);
    CASE(Action_StopAction);
    CASE(Action_OpenInventory);
    CASE(Action_OpenAttributes);
    CASE(Action_OpenKeyBindings);
    CASE(Action_OpenChatAndActions);
    CASE(Action_OpenStatDisplay);
    CASE(Action_OpenChat);
    CASE(Action_OpenDrop);
    CASE(Action_BazaarOpen);
    CASE(Action_ToggleChatLog);
    CASE(Action_CloseChatAndActions);
    CASE(Action_CloseInventory);
    CASE(Action_CloseKeyBindings);
    CASE(Action_CloseStatDisplay);
    CASE(Action_CloseChat);
    CASE(Action_CloseDrop);
    CASE(Action_CloseBazaar);
    CASE(Action_TakeScreenshot);
    CASE(Action_OpenCast);
    CASE(Action_OpenUseItem);
    CASE(Action_EventEscape);
    CASE(Action_StorageOpen);
    CASE(Action_TradeAccept);
    CASE(Action_TradeWithdraw);
    CASE(Action_TradeCancel);
    CASE(Action_CloseDialogs);
    CASE(Action_ToggleNameDisplay);
    CASE(Action_GuildLead);
    CASE(Action_GuildJoin);
    CASE(Action_GuildLeave);
    CASE(Action_GuildMembers);
    CASE(Action_Show_Help);
    #undef CASE
    }
  }

  // Removes all elements
  void clear() {
    for (List::iterator i = list_.begin(); i != list_.end(); ++i) {
      delete i->second;
    }
    list_.clear();
  }

  // Returns what kind of action is next in the queue.
  ActionType nextType() {
    return list_.front().first;
  }

  // Obtains the next element from the queue.  The element remains at the
  // start until pop() is called.  This method assumes you know the type of
  // the action.
  template <typename _T> const _T* next() {
    assert(_T::TYPE == list_.front().first);
    return reinterpret_cast<const _T*>(list_.front().second);
  }

  // Are there members in this queue?
  bool hasNext() {
    return !list_.empty();
  }

  // Removes the current start of the queue.  Do not call when empty.
  void pop() {
    assert(!list_.empty());
    delete list_.front().second;
    list_.pop_front();
  }

  // Calls processor->process(type) for the next element in the queue, then
  // pops that element from the queue.  This method can be called when the
  // queue is empty without problems.
  // The processor should overload the ->process method for each of the
  // different action types.
  template <typename _C> void process(_C* processor) {
    if (!hasNext()) return;
    switch (nextType()) {
    default: assert(!"Unknown action type"); break;
    #define CASE(structure) case structure::TYPE: processor->process(next<structure>()); break;
    CASE(Action_CastSpell);
    CASE(Action_UseConsumable);
    CASE(Action_Move);
    CASE(Action_ActorInteraction);
    CASE(Action_Attack);
    CASE(Action_NPCInteraction);
    CASE(Action_LogOutOfWorld);
    CASE(Action_ToggleViewAngle);
    CASE(Action_ToggleShowNames);
    CASE(Action_SayAgain);
    CASE(Action_StopAction);
    CASE(Action_OpenInventory);
    CASE(Action_OpenAttributes);
    CASE(Action_OpenKeyBindings);
    CASE(Action_OpenChatAndActions);
    CASE(Action_OpenStatDisplay);
    CASE(Action_OpenChat);
    CASE(Action_OpenDrop);
    CASE(Action_BazaarOpen);
    CASE(Action_ToggleChatLog);
    CASE(Action_CloseChatAndActions);
    CASE(Action_CloseInventory);
    CASE(Action_CloseKeyBindings);
    CASE(Action_CloseStatDisplay);
    CASE(Action_CloseChat);
    CASE(Action_CloseDrop);
    CASE(Action_CloseBazaar);
    CASE(Action_AttributesAddAbilityPoint);
    CASE(Action_Speak);
    CASE(Action_DropItem);
    CASE(Action_DropCurrency);
    CASE(Action_EquipItem);
    CASE(Action_UnequipItem);
    CASE(Action_BazaarCreateListing);
    CASE(Action_BazaarRetractListing);
    CASE(Action_BazaarPurchaseListing);
    CASE(Action_BazaarRedeemListing);
    CASE(Action_BazaarGetListings);
    CASE(Action_TakeScreenshot);
    CASE(Action_OpenCast);
    CASE(Action_OpenUseItem);
    CASE(Action_EventEscape);
    CASE(Action_StorageOpen);
    CASE(Action_StorageWithdrawItem);
    CASE(Action_StorageDepositItem);
    CASE(Action_Geosid_SacrificeItem);
    CASE(Action_Geosid_Burst);
    CASE(Action_Geosid_BurstAttackGeosid);
    CASE(Action_Geosid_BurstSpell);
    CASE(Action_Geosid_BurstUseGeosid);
    CASE(Action_Geosid_GetOwner);
    CASE(Action_TradeAddItem);
    CASE(Action_TradeSetCurrency);
    CASE(Action_TradeRemoveItem);
    CASE(Action_TradeAccept);
    CASE(Action_TradeWithdraw);
    CASE(Action_TradeCancel);
    CASE(Action_SplitStackedItem);
    CASE(Action_QuestOfferResponse);
    CASE(Action_QuestGiveUp);
    CASE(Action_CloseDialogs);
    CASE(Action_ToggleNameDisplay);
    CASE(Action_GuildCreate);
    CASE(Action_GuildLead);
    CASE(Action_GuildJoin);
    CASE(Action_GuildLeave);
    CASE(Action_GuildPromote);
    CASE(Action_GuildDemote);
    CASE(Action_GuildChat);
    CASE(Action_GuildKick);
    CASE(Action_GuildMembers);
    CASE(Action_GuildGeosids);
    CASE(Action_GuildSetHome);
    CASE(Action_GuildHome);
    CASE(Action_GuildAbandon);
    CASE(Action_Party_Chat);
    CASE(Action_Party_Lead);
    CASE(Action_Party_Join);
    CASE(Action_Party_Leave);
    CASE(Action_Party_Kick);
    CASE(Action_Party_Close);
    CASE(Action_Resurrect_StandUp);
    CASE(Action_Resurrect_Respawn);
    CASE(Action_Show_Help);
    CASE(Action_NPCTrigger);
    CASE(Action_Mark);
    CASE(Action_AutoTarget_All);
    CASE(Action_AutoTarget_Monsters);

    #undef CASE
    }
    pop();
  }

private:
  List list_;
};


}
}

#endif








