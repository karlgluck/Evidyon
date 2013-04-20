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
#ifndef __CLIENT_ACTIONS_H__
#define __CLIENT_ACTIONS_H__
#pragma once

#include "shared/spell/spellindex.h"
#include "shared/actor/actorid.h"
#include "shared/item/itemindex.h"
#include "shared/avatar/avatarattribute.h"
#include "shared/geosid/GeosidIndex.h"

///////////////////////////////
#include "shared/evidyon_npc.h"
///////////////////////////////


namespace Evidyon {


//----[  ActionType  ]---------------------------------------------------------
enum ActionType {
  ACTION_CASTSPELL,
  ACTION_USECONSUMABLE,
  ACTION_MOVE,
  ACTION_ACTORINTERACTION,
  ACTION_ATTACK,
  ACTION_NPCINTERACTION,
  ACTION_LOG_OUT_OF_WORLD,
  ACTION_TOGGLE_VIEW_ANGLE,
  ACTION_TOGGLE_SHOW_NAMES,
  ACTION_SAY_AGAIN,
  ACTION_STOP_ACTION, // unimplemented
  ACTION_OPEN_INVENTORY,
  ACTION_OPEN_ATTRIBUTES,
  ACTION_OPEN_KEY_BINDINGS,
  ACTION_OPEN_CHAT_AND_ACTIONS,
  ACTION_OPEN_STAT_DISPLAY,
  ACTION_OPEN_CHAT,
  ACTION_OPEN_DROP,
  ACTION_BAZAAR_OPEN,
  ACTION_TOGGLE_CHAT_LOG,
  ACTION_CLOSE_CHAT_AND_ACTIONS,
  ACTION_CLOSE_INVENTORY,
  ACTION_CLOSE_KEY_BINDINGS,
  ACTION_CLOSE_STAT_DISPLAY,
  ACTION_CLOSE_CHAT,
  ACTION_CLOSE_DROP,
  ACTION_CLOSE_BAZAAR,
  ACTION_INVENTORY_DROP_ITEMS,
  ACTION_INVENTORY_EQUIP_ITEMS,
  ACTION_INVENTORY_UNEQUIP_ITEMS,
  ACTION_ATTRIBUTES_ADD_ABILITY_POINT,
  ACTION_SPEAK,
  ACTION_DROP_ITEM,
  ACTION_DROP_CURRENCY,
  ACTION_EQUIP_ITEM,
  ACTION_UNEQUIP_ITEM,
  ACTION_BAZAAR_CREATELISTING,   
  ACTION_BAZAAR_RETRACTLISTING,
  ACTION_BAZAAR_PURCHASELISTING,
  ACTION_BAZAAR_REDEEMLISTING,
  ACTION_BAZAAR_GETLISTINGS,
  ACTION_TAKE_SCREENSHOT,
  ACTION_OPEN_CAST,
  ACTION_OPEN_USEITEM,
  ACTION_EVENT_ESCAPE,
  ACTION_STORAGE_OPEN,
  ACTION_STORAGE_WITHDRAW_ITEM,
  ACTION_STORAGE_DEPOSIT_ITEM,
  ACTION_GEOSID_SACRIFICE_ITEM,
  ACTION_GEOSID_BURST,
  ACTION_GEOSID_BURSTATTACKGEOSID,
  ACTION_GEOSID_BURSTSPELL,
  ACTION_GEOSID_BURSTUSEGEOSID,
  ACTION_GEOSID_GETOWNER,
  ACTION_TRADE_ADDITEM,
  ACTION_TRADE_SETCURRENCY,
  ACTION_TRADE_REMOVEITEM,
  ACTION_TRADE_ACCEPT,
  ACTION_TRADE_WITHDRAW,
  ACTION_TRADE_CANCEL,
  ACTION_SPLIT_STACKED_ITEM,
  ACTION_QUEST_OFFER_RESPONSE,
  ACTION_QUEST_GIVE_UP,
  ACTION_CLOSE_DIALOGS,
  ACTION_TOGGLE_NAME_DISPLAY,
  ACTION_GUILD_CREATE,
  ACTION_GUILD_LEAD,
  ACTION_GUILD_JOIN,
  ACTION_GUILD_LEAVE,
  ACTION_GUILD_PROMOTE,
  ACTION_GUILD_DEMOTE,
  ACTION_GUILD_CHAT,
  ACTION_GUILD_KICK,
  ACTION_GUILD_MEMBERS,
  ACTION_GUILD_GEOSIDS,
  ACTION_GUILD_SETHOME,
  ACTION_GUILD_HOME,
  ACTION_GUILD_ABANDON,
  ACTION_PARTY_CHAT,
  ACTION_PARTY_LEAD,
  ACTION_PARTY_JOIN,
  ACTION_PARTY_LEAVE,
  ACTION_PARTY_KICK,
  ACTION_PARTY_CLOSE,
  ACTION_RESURRECT_STANDUP,
  ACTION_RESURRECT_RESPAWN,
  ACTION_SHOW_HELP,
  ACTION_NPC_TRIGGER,
  ACTION_MARK,
  ACTION_AUTOTARGET_ALL,
  ACTION_AUTOTARGET_MONSTERS,
  INVALID_ACTION_TYPE,
};


//----[  Action_CastSpell  ]---------------------------------------------------
struct Action_CastSpell {
  static const ActionType TYPE = ACTION_CASTSPELL;
  Evidyon::Spell::SpellIndex spell_type;
  Evidyon::Actor::ActorID target_actor_id;
  float x, y;
};


//----[  Action_UseConsumable  ]-----------------------------------------------
struct Action_UseConsumable {
  static const ActionType TYPE = ACTION_USECONSUMABLE;
  Evidyon::Item::ItemIndex item_type;
  int inventory_index_hint;
  bool needs_target;
  float x, y;
};



//----[  Action_Move  ]--------------------------------------------------------
struct Action_Move {
  static const ActionType TYPE = ACTION_MOVE;
  bool streaming;
  float x, y;
};



//----[  Action_ActorInteraction  ]--------------------------------------------
struct Action_ActorInteraction {
  static const ActionType TYPE = ACTION_ACTORINTERACTION;
  Evidyon::Actor::ActorID actor_id;
};



//----[  Action_Attack  ]------------------------------------------------------
struct Action_Attack {
  static const ActionType TYPE = ACTION_ATTACK;
  Evidyon::Actor::ActorID actor_id;
  float x, y;
};



//----[  Action_NPCInteraction  ]----------------------------------------------
struct Action_NPCInteraction {
  static const ActionType TYPE = ACTION_NPCINTERACTION;
  Evidyon::Actor::ActorID npc_actor_id;
  NPCInteractionType interaction_type;
};



//----[  Action_LogOutOfWorld  ]-----------------------------------------------
struct Action_LogOutOfWorld {
  static const ActionType TYPE = ACTION_LOG_OUT_OF_WORLD;
};



//----[  Action_ToggleViewAngle  ]---------------------------------------------
struct Action_ToggleViewAngle {
  static const ActionType TYPE = ACTION_TOGGLE_VIEW_ANGLE;
};



//----[  Action_ToggleShowNames  ]---------------------------------------------
struct Action_ToggleShowNames {
  static const ActionType TYPE = ACTION_TOGGLE_SHOW_NAMES;
};



//----[  Action_SayAgain  ]----------------------------------------------------
struct Action_SayAgain {
  static const ActionType TYPE = ACTION_SAY_AGAIN;
};



//----[  Action_StopAction  ]--------------------------------------------------
struct Action_StopAction {
  static const ActionType TYPE = ACTION_STOP_ACTION;
};



//----[  Action_OpenInventory  ]-----------------------------------------------
struct Action_OpenInventory {
  static const ActionType TYPE = ACTION_OPEN_INVENTORY;
};



//----[  Action_OpenAttributes  ]----------------------------------------------
struct Action_OpenAttributes {
  static const ActionType TYPE = ACTION_OPEN_ATTRIBUTES;
};



//----[  Action_OpenKeyBindings  ]---------------------------------------------
struct Action_OpenKeyBindings {
  static const ActionType TYPE = ACTION_OPEN_KEY_BINDINGS;
};



//----[  Action_OpenChatAndActions  ]------------------------------------------
struct Action_OpenChatAndActions {
  static const ActionType TYPE = ACTION_OPEN_CHAT_AND_ACTIONS;
};



//----[  Action_OpenStatDisplay  ]---------------------------------------------
struct Action_OpenStatDisplay {
  static const ActionType TYPE = ACTION_OPEN_STAT_DISPLAY;
};



//----[  Action_OpenChat  ]----------------------------------------------------
struct Action_OpenChat {
  static const ActionType TYPE = ACTION_OPEN_CHAT;
};



//----[  Action_OpenDrop  ]----------------------------------------------------
struct Action_OpenDrop {
  static const ActionType TYPE = ACTION_OPEN_DROP;
};



//----[  Action_BazaarOpen  ]--------------------------------------------------
struct Action_BazaarOpen {
  static const ActionType TYPE = ACTION_BAZAAR_OPEN;
  Actor::ActorID npc_actor_id;
};



//----[  Action_ToggleChatLog  ]-----------------------------------------------
struct Action_ToggleChatLog {
  static const ActionType TYPE = ACTION_TOGGLE_CHAT_LOG;
};



//----[  Action_CloseChatAndActions  ]-----------------------------------------
struct Action_CloseChatAndActions {
  static const ActionType TYPE = ACTION_CLOSE_CHAT_AND_ACTIONS;
};



//----[  Action_CloseInventory  ]----------------------------------------------
struct Action_CloseInventory {
  static const ActionType TYPE = ACTION_CLOSE_INVENTORY;
};



//----[  Action_CloseKeyBindings  ]--------------------------------------------
struct Action_CloseKeyBindings {
  static const ActionType TYPE = ACTION_CLOSE_KEY_BINDINGS;
};



//----[  Action_CloseStatDisplay  ]--------------------------------------------
struct Action_CloseStatDisplay {
  static const ActionType TYPE = ACTION_CLOSE_STAT_DISPLAY;
};



//----[  Action_CloseChat  ]---------------------------------------------------
struct Action_CloseChat {
  static const ActionType TYPE = ACTION_CLOSE_CHAT;
};



//----[  Action_CloseDrop  ]---------------------------------------------------
struct Action_CloseDrop {
  static const ActionType TYPE = ACTION_CLOSE_DROP;
};



//----[  Action_CloseBazaar  ]-------------------------------------------------
struct Action_CloseBazaar {
  static const ActionType TYPE = ACTION_CLOSE_BAZAAR;
};




//----[  Action_AttributesAddAbilityPoint  ]-----------------------------------
struct Action_AttributesAddAbilityPoint {
  static const ActionType TYPE = ACTION_ATTRIBUTES_ADD_ABILITY_POINT;
  Evidyon::Avatar::AvatarAttribute attribute;
};



//----[  Action_Speak  ]-------------------------------------------------------
struct Action_Speak {
  static const ActionType TYPE = ACTION_SPEAK;
  std::string text;
};



//----[  Action_DropItem  ]----------------------------------------------------
struct Action_DropItem {
  static const ActionType TYPE = ACTION_DROP_ITEM;
  int inventory_index;
};



//----[  Action_DropCurrency  ]------------------------------------------------
struct Action_DropCurrency {
  static const ActionType TYPE = ACTION_DROP_CURRENCY;
  Evidyon::CurrencyType amount;
};



//----[  Action_EquipItem  ]---------------------------------------------------
struct Action_EquipItem {
  static const ActionType TYPE = ACTION_EQUIP_ITEM;
  int inventory_index;
};



//----[  Action_UnequipItem  ]-------------------------------------------------
struct Action_UnequipItem {
  static const ActionType TYPE = ACTION_UNEQUIP_ITEM;
  int inventory_index;
};



//----[  Action_BazaarCreateListing  ]-----------------------------------------
struct Action_BazaarCreateListing {
  static const ActionType TYPE = ACTION_BAZAAR_CREATELISTING;
  int inventory_index;
  Evidyon::CurrencyType listing_price;
};



//----[  Action_BazaarRetractListing  ]----------------------------------------
struct Action_BazaarRetractListing {
  static const ActionType TYPE = ACTION_BAZAAR_RETRACTLISTING;
  Evidyon::BazaarListingID listing_id;
};



//----[  Action_BazaarPurchaseListing  ]---------------------------------------
struct Action_BazaarPurchaseListing {
  static const ActionType TYPE = ACTION_BAZAAR_PURCHASELISTING;
  Evidyon::BazaarListingID listing_id;
};



//----[  Action_BazaarRedeemListing  ]-----------------------------------------
struct Action_BazaarRedeemListing {
  static const ActionType TYPE = ACTION_BAZAAR_REDEEMLISTING;
  Evidyon::BazaarListingID listing_id;
};



//----[  Action_BazaarGetListings  ]-------------------------------------------
struct Action_BazaarGetListings {
  static const ActionType TYPE = ACTION_BAZAAR_GETLISTINGS;
  Evidyon::BazaarCategory category;
};



//----[  Action_TakeScreenshot  ]----------------------------------------------
struct Action_TakeScreenshot {
  static const ActionType TYPE = ACTION_TAKE_SCREENSHOT;
};



//----[  Action_OpenCast  ]----------------------------------------------------
struct Action_OpenCast {
  static const ActionType TYPE = ACTION_OPEN_CAST;
};



//----[  Action_OpenUseItem  ]-------------------------------------------------
struct Action_OpenUseItem {
  static const ActionType TYPE = ACTION_OPEN_USEITEM;
};



//----[  Action_EventEscape  ]-------------------------------------------------
struct Action_EventEscape {
  static const ActionType TYPE = ACTION_EVENT_ESCAPE;
};




//----[  Action_StorageOpen  ]-------------------------------------------------
struct Action_StorageOpen {
  static const ActionType TYPE = ACTION_STORAGE_OPEN;
  Actor::ActorID npc_actor_id;
};

//----[  Action_StorageWithdrawItem  ]-----------------------------------------
struct Action_StorageWithdrawItem {
  static const ActionType TYPE = ACTION_STORAGE_WITHDRAW_ITEM;
  int storage_index;
};



//----[  Action_StorageDepositItem  ]------------------------------------------
struct Action_StorageDepositItem {
  static const ActionType TYPE = ACTION_STORAGE_DEPOSIT_ITEM;
  int inventory_index;
};



//----[  Action_Geosid_SacrificeItem  ]----------------------------------------
struct Action_Geosid_SacrificeItem {
  static const ActionType TYPE = ACTION_GEOSID_SACRIFICE_ITEM;
  int inventory_index;
};


//----[  Action_Geosid_Burst  ]------------------------------------------------
struct Action_Geosid_Burst {
  static const ActionType TYPE = ACTION_GEOSID_BURST;
  float x, z;
};



//----[  Action_Geosid_BurstAttackGeosid  ]------------------------------------
struct Action_Geosid_BurstAttackGeosid {
  static const ActionType TYPE = ACTION_GEOSID_BURSTATTACKGEOSID;
  Geosid::GeosidIndex geosid;
};



//----[  Action_Geosid_BurstSpell  ]-------------------------------------------
struct Action_Geosid_BurstSpell {
  static const ActionType TYPE = ACTION_GEOSID_BURSTSPELL;
  Evidyon::Spell::SpellIndex spell_type;
  Evidyon::Actor::ActorID target_actor_id;
  float x, y;
};



//----[  Action_Geosid_BurstUseGeosid  ]---------------------------------------
struct Action_Geosid_BurstUseGeosid {
  static const ActionType TYPE = ACTION_GEOSID_BURSTUSEGEOSID;
  Geosid::GeosidIndex geosid;
  Evidyon::Actor::ActorID actor_id;
  float x, z;
};




//----[  Action_Geosid_GetOwner  ]---------------------------------------------
struct Action_Geosid_GetOwner {
  static const ActionType TYPE = ACTION_GEOSID_GETOWNER;
  Geosid::GeosidIndex geosid;
};



//----[  Action_TradeAddItem  ]------------------------------------------------
struct Action_TradeAddItem {
  static const ActionType TYPE = ACTION_TRADE_ADDITEM;
  int inventory_index;
};



//----[  Action_TradeSetCurrency  ]--------------------------------------------
struct Action_TradeSetCurrency {
  static const ActionType TYPE = ACTION_TRADE_SETCURRENCY;
  Evidyon::CurrencyType amount;
};



//----[  Action_TradeRemoveItem  ]---------------------------------------------
struct Action_TradeRemoveItem {
  static const ActionType TYPE = ACTION_TRADE_REMOVEITEM;
  int trade_index;
};



//----[  Action_TradeAccept  ]-------------------------------------------------
struct Action_TradeAccept {
  static const ActionType TYPE = ACTION_TRADE_ACCEPT;
};



//----[  Action_TradeWithdraw  ]-----------------------------------------------
struct Action_TradeWithdraw {
  static const ActionType TYPE = ACTION_TRADE_WITHDRAW;
};



//----[  Action_TradeCancel  ]-------------------------------------------------
struct Action_TradeCancel {
  static const ActionType TYPE = ACTION_TRADE_CANCEL;
};



//----[  Action_SplitStackedItem  ]--------------------------------------------
struct Action_SplitStackedItem {
  static const ActionType TYPE = ACTION_SPLIT_STACKED_ITEM;
  int inventory_index;
};



//----[  Action_QuestOfferResponse  ]------------------------------------------
struct Action_QuestOfferResponse {
  static const ActionType TYPE = ACTION_QUEST_OFFER_RESPONSE;
  unsigned int quest_index;
  Evidyon::Actor::ActorID npc_actor_id;
  int npc_quest_index;
  unsigned int validation_key;
  bool accepted;
};



//----[  Action_QuestGiveUp  ]-------------------------------------------------
struct Action_QuestGiveUp {
  static const ActionType TYPE = ACTION_QUEST_GIVE_UP;
};



//----[  Action_CloseDialogs  ]------------------------------------------------
struct Action_CloseDialogs {
  static const ActionType TYPE = ACTION_CLOSE_DIALOGS;
};



//----[  Action_ToggleNameDisplay  ]-------------------------------------------
struct Action_ToggleNameDisplay {
  static const ActionType TYPE = ACTION_TOGGLE_NAME_DISPLAY;
};



//----[  Action_GuildCreate  ]-------------------------------------------------
struct Action_GuildCreate {
  static const ActionType TYPE = ACTION_GUILD_CREATE;
  std::string guild_name;
};



//----[  Action_GuildLead  ]---------------------------------------------------
struct Action_GuildLead {
  static const ActionType TYPE = ACTION_GUILD_LEAD;
};



//----[  Action_GuildJoin  ]---------------------------------------------------
struct Action_GuildJoin {
  static const ActionType TYPE = ACTION_GUILD_JOIN;
};



//----[  Action_GuildLeave  ]--------------------------------------------------
struct Action_GuildLeave {
  static const ActionType TYPE = ACTION_GUILD_LEAVE;
};



//----[  Action_GuildPromote  ]------------------------------------------------
struct Action_GuildPromote {
  static const ActionType TYPE = ACTION_GUILD_PROMOTE;
  std::string name;
};



//----[  Action_GuildDemote  ]-------------------------------------------------
struct Action_GuildDemote {
  static const ActionType TYPE = ACTION_GUILD_DEMOTE;
  std::string name;
};



//----[  Action_GuildChat  ]---------------------------------------------------
struct Action_GuildChat {
  static const ActionType TYPE = ACTION_GUILD_CHAT;
  std::string text;
};



//----[  Action_GuildKick  ]---------------------------------------------------
struct Action_GuildKick {
  static const ActionType TYPE = ACTION_GUILD_KICK;
  std::string name;
};



//----[  Action_GuildMembers  ]------------------------------------------------
struct Action_GuildMembers {
  static const ActionType TYPE = ACTION_GUILD_MEMBERS;
};



//----[  Action_GuildGeosids  ]------------------------------------------------
struct Action_GuildGeosids {
  static const ActionType TYPE = ACTION_GUILD_GEOSIDS;
};



//----[  Action_GuildSetHome  ]------------------------------------------------
struct Action_GuildSetHome {
  static const ActionType TYPE = ACTION_GUILD_SETHOME;
};



//----[  Action_GuildHome  ]---------------------------------------------------
struct Action_GuildHome {
  static const ActionType TYPE = ACTION_GUILD_HOME;
};



//----[  Action_GuildAbandon  ]------------------------------------------------
struct Action_GuildAbandon {
  static const ActionType TYPE = ACTION_GUILD_ABANDON;
  Geosid::GeosidIndex geosid;
};



//----[  Action_Party_Chat  ]--------------------------------------------------
struct Action_Party_Chat {
  static const ActionType TYPE = ACTION_PARTY_CHAT;
  std::string text;
};



//----[  Action_Party_Lead  ]--------------------------------------------------
struct Action_Party_Lead {
  static const ActionType TYPE = ACTION_PARTY_LEAD;
};



//----[  Action_Party_Join  ]--------------------------------------------------
struct Action_Party_Join {
  static const ActionType TYPE = ACTION_PARTY_JOIN;
};



//----[  Action_Party_Leave  ]-------------------------------------------------
struct Action_Party_Leave {
  static const ActionType TYPE = ACTION_PARTY_LEAVE;
};



//----[  Action_Party_Kick  ]--------------------------------------------------
struct Action_Party_Kick {
  static const ActionType TYPE = ACTION_PARTY_KICK;
  std::string name;
};


//----[  Action_Party_Close  ]--------------------------------------------------
struct Action_Party_Close {
  static const ActionType TYPE = ACTION_PARTY_CLOSE;
};




//----[  Action_Resurrect_StandUp  ]-------------------------------------------
struct Action_Resurrect_StandUp {
  static const ActionType TYPE = ACTION_RESURRECT_STANDUP;
};


//----[  Action_Resurrect_Respawn  ]-------------------------------------------
struct Action_Resurrect_Respawn {
  static const ActionType TYPE = ACTION_RESURRECT_RESPAWN;
};


//----[  Action_Show_Help  ]---------------------------------------------------
struct Action_Show_Help {
  static const ActionType TYPE = ACTION_SHOW_HELP;
};

//----[  Action_NPCTrigger  ]--------------------------------------------------
struct Action_NPCTrigger {
  static const ActionType TYPE = ACTION_NPC_TRIGGER;
  Actor::ActorID npc_actor_id;
};


//----[  Action_Mark  ]--------------------------------------------------------
struct Action_Mark {
  static const ActionType TYPE = ACTION_MARK;
  int mark_index;
};



//----[  Action_AutoTarget_All  ]----------------------------------------------
struct Action_AutoTarget_All {
  static const ActionType TYPE = ACTION_AUTOTARGET_ALL;
};



//----[  Action_AutoTarget_Monsters  ]-----------------------------------------
struct Action_AutoTarget_Monsters {
  static const ActionType TYPE = ACTION_AUTOTARGET_MONSTERS;
};

}

#endif