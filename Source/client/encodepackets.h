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
#ifndef __EVIDYON_PACKETS_CLIENT_ENCODE_H__
#define __EVIDYON_PACKETS_CLIENT_ENCODE_H__
#pragma once

struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

#include "shared/actor/actorid.h"
#include "shared/actor/evidyon_actor.h"
#include "shared/evidyon_enchantments.h"

namespace Evidyon {

struct ItemForTrade;
enum ActorState;
enum ActorAction;
struct AvatarAttributes;
enum EquipmentSlot;
enum AdminSimpleCommand;

namespace Network {

namespace Packet {
enum RequestActionResult;

namespace Client {
namespace Encode {

ENetPacket* tellClientVersionInfo(unsigned long gamefileVersion, unsigned long clientVersion);

ENetPacket* createAccount(const char* password);
ENetPacket* accessAccount(unsigned int id, const char* password);
ENetPacket* logoutAccount();
ENetPacket* createCharacter(const char* name, bool male, int chRace, int chClass, int chStr, int chAgi, int chCon, int chInt, int chWis);
ENetPacket* deleteCharacter(unsigned int id, const char* password);
ENetPacket* askCharacterName(unsigned int id);
ENetPacket* askAccountInfo();
ENetPacket* askCharacterMapLocation(unsigned int id);

ENetPacket* enterWorld(unsigned int id);
ENetPacket* leaveWorld();
ENetPacket* requestMove(float x, float y, Evidyon::Actor::ActorID targetActorID);
ENetPacket* requestMoveStreaming(float x, float y);


ENetPacket* changeEquippedState(unsigned char* inventoryIndices, size_t numberOfIndices);
ENetPacket* dropInventoryItems(unsigned char* inventoryIndices, size_t numberOfIndices);

//ENetPacket* askActorDescription(int actorID);

/**
 * Requests the avatar's stats from the server; if abilityToAddPoint is between 0 and 5, the
 * client would like to add an ability point to that stat.
 */
ENetPacket* askMyAvatarStats(int abilityToAddPoint);

ENetPacket* changeItemsToTrade(unsigned char* trade_item_indices, unsigned int number_of_items);
ENetPacket* acceptTrade(int trade_key);

ENetPacket* listItemsForSale(unsigned char* inventory_indices, unsigned int* prices, unsigned int number_of_items);
ENetPacket* getItemsForSale(unsigned int minimum_listing_id);
ENetPacket* buyItem(unsigned int listing_id);
ENetPacket* getSellerStatus();
ENetPacket* withdrawSellerItems(unsigned int* listing_ids, unsigned int number_of_items);

ENetPacket* geosid_sacrificeItems(unsigned char* inventory_indices, unsigned char number_of_items);
ENetPacket* useItemAbility(unsigned char inventory_index, EquipmentSlot equipment_slot);



ENetPacket* castSpell(const unsigned int spell_ids[3], Evidyon::Actor::ActorID target_actor_id, float target_x, float target_y);
ENetPacket* useItem(const unsigned char inventory_index, Evidyon::Actor::ActorID target_actor_id, float target_x, float target_y);
ENetPacket* requestSpeak(const char* speech);

ENetPacket* askActorName(Evidyon::Actor::ActorID actor_id);

ENetPacket* storageOpen(Evidyon::Actor::ActorID storage_npc_actor_id);
ENetPacket* storageWithdraw(Evidyon::Actor::ActorID storage_npc_actor_id, int storage_index);
ENetPacket* storageDeposit(Evidyon::Actor::ActorID storage_npc_actor_id, int inventory_index);

ENetPacket* geosidSacrifice(unsigned char inventory_index);


ENetPacket* admin_sqlQuery(const std::string& query_text);
ENetPacket* admin_createItem(int type);
ENetPacket* admin_simpleCommand(AdminSimpleCommand simple_command);


}
}
}
}
}

#endif