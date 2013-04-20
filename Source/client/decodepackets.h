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
#ifndef __DECODEPACKETS_H__
#define __DECODEPACKETS_H__
#pragma once


#include <string>
#include "shared/networkdecl.h"
#include "shared/actor/actorsync.h"
#include "shared/evidyon_world.h"
#include "shared/evidyon_storage.h"
#include "shared/avatar/evidyon_avatar.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"
#include "shared/world/mapindex.h"

#define MAX_ITEMS_PER_TRADE AVATAR_INVENTORY_SIZE

#pragma pack(push, 1)
struct TradeItem
{
  unsigned type : 16;
  unsigned quantity : 8;
};
#pragma pack(pop)

namespace Evidyon {
enum ActorState;
enum ActorAction;
struct ClientAvatarInventoryItem;
struct ItemForTrade;

namespace Network {
namespace Packet {
enum RequestActionResult;
struct ActorSyncData;
struct FastSyncPacket;

namespace Client {
namespace Decode {

bool tellServerVersionUpdateInfo(ENetPacket* packet, bool* versionIsValid, bool* needsNewGamefile, bool* needsNewClient, std::string* updateText);

bool createAccountSucceeded(ENetPacket* packet, unsigned int* id);
bool logoutAccountConfirmed(ENetPacket* packet);
bool tellAccountInfo(ENetPacket* packet, float* daysLeft, unsigned int* characterIDArray, size_t* numberOfCharacters);
bool requestActionResult(ENetPacket* packet, Evidyon::Network::Packet::RequestActionResult* result);
bool tellCharacterName(ENetPacket* packet, unsigned int* id, char* characterNameBuffer, size_t bufferSize, int* map_index, float* world_x, float* world_y, unsigned int* actor_type, size_t visible_equipment[Evidyon::ACTORATTACHMENTPOINT_COUNT]);
bool createCharacterSucceeded(ENetPacket* packet, unsigned int* characterID);
bool tellCharacterMapLocation(ENetPacket* packet, unsigned int* id, int* mapX, int* mapY);

bool enterWorldSucceeded(ENetPacket* packet, Evidyon::Actor::ActorID* actorID, unsigned long* time_of_day, float* yourX, float* yourY, int* map_index, bool* male, int* raceIndex, int* classIndex, int* currentHP, int* maxHP, int* currentMP, int* maxMP,  unsigned int* money, unsigned int* geonite, Avatar::SpellAvailabilityMask* spellAvailabilityMask);
bool leaveWorldConfirmed(ENetPacket* packet);
bool tellActorActivity(ENetPacket* packet, int* actorID, float* x, float* y, float* angle, float* speed, float* distanceToMove, ActorState* currentState, ActorAction* action, float* actionSpeed);
bool destroyActor(ENetPacket* packet, int* actorID);
//bool actorSpeaks(ENetPacket* packet, int* actorID, char* source_name_buffer, size_t source_name_buffer_size, char* speechBuffer, size_t bufferSize); // the name is specified if the actor ID == -1

bool updateYourEffectAvailabilityMask(ENetPacket* packet, Avatar::SpellAvailabilityMask* mask);

bool tellYourAvatarStats(ENetPacket* packet, unsigned char* level, unsigned char abilities[5], unsigned char* availableAbilityPoints,
                         int* hp, int* hpMax, int* mp, int* mpMax, int* exp, int* nextLevelExp, int* defense, Evidyon::Avatar::AvatarRace* avatar_race, Evidyon::Avatar::AvatarClass* avatar_class, int* alignment_value);

bool changeMap(ENetPacket* packet, World::MapIndex* map_id);

bool slowSync(ENetPacket* packet, float* time_since_last_sync, unsigned int* actors, ActorSyncData* updates_buffer, unsigned int buffer_size);
bool fastSync(ENetPacket* packet, Evidyon::Network::Packet::FastSyncPacket* data);

bool createMagicFX(ENetPacket* packet, int* type, int* src_actor_id,
           float* src_x, float* src_y,
           int* power, int* target_ids, int target_buffer_entries,
           int* number_of_targets);

bool createProjectile(ENetPacket* packet, int* projectile_id, int* projectile_type, float* x, float* y, float* dx, float* dy);
bool destroyProjectile(ENetPacket* packet, int* projectile_id);

// the item_types buffer should have MAX_ITEMS_VISIBLE_PER_LOCATION elements available
bool tellItemsOnMapLocation(ENetPacket* packet,
                            int* world_x,
                            int* world_y,
                            unsigned short* item_types,
                            int* number_of_item_types);


bool updateOrTellInventory(ENetPacket* packet,
                           Evidyon::ClientAvatarInventoryItem fullClientInventory[Evidyon::AVATAR_INVENTORY_SIZE],
                           int* highest_inventory_update_index,
                           int inventory_update_indices[Evidyon::AVATAR_INVENTORY_SIZE]);

bool tellActorName(ENetPacket* packet,
                   Evidyon::Actor::ActorID* actor_id,
                   std::string* name);
bool updateLocalWorldRegion(ENetPacket* packet,
                            World::RegionID* region_id);

bool storageTell(ENetPacket* packet,
                 Evidyon::ItemInStorage items[Evidyon::MAX_ITEMS_IN_STORAGE],
                 int* number_of_items);

bool geosidTellGeonite(ENetPacket* packet,
                       unsigned int* geonite);


bool admin_serverMessage(ENetPacket* packet,
                         std::string* reply);

}
}
}
}
}


#endif