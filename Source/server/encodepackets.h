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
#ifndef __EVIDYON_PACKETS_SERVER_ENCODE_H__
#define __EVIDYON_PACKETS_SERVER_ENCODE_H__
#pragma once

struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

#include "shared/actor/evidyon_actor.h"
#include "shared/evidyon_enchantments.h"
#include "shared/actor/actorsync.h"
#include "shared/evidyon_world.h"
#include "shared/avatar/evidyon_avatar.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"
#include "shared/world/mapindex.h"

namespace Evidyon {

struct AvatarInventoryItem;
struct ItemForTrade;
enum ActorState;
struct AvatarAttributes;
class ActorInstance;
class Client;
struct ItemInStorage;

namespace Network {
enum RequestActionResult;

namespace Packet {
struct ActorDescriptionData;
struct FastSyncPacket;

namespace Server {
namespace Encode {

ENetPacket* tellServerVersionUpdateInfo(bool versionIsValid, bool needsNewGamefile, bool needsNewClient, const char* updateText = 0);

ENetPacket* createAccountSucceeded(unsigned int account_id);
ENetPacket* logoutAccountConfirmed();
ENetPacket* tellAccountInfo(float daysLeft, const unsigned int* characterIDArray, size_t numberOfCharacters);
ENetPacket* requestActionResult(RequestActionResult result);
ENetPacket* tellCharacterName(unsigned int id, const char* characterName, int map_index, float world_x, float world_y, unsigned int actor_type, size_t* visible_equipment);
ENetPacket* createCharacterSucceeded(unsigned int character_id);
ENetPacket* tellCharacterMapLocation(unsigned int id, int mapX, int mapY);

ENetPacket* enterWorldSucceeded(Evidyon::Actor::ActorID actorID,
                                unsigned long time_of_day,
                                float yourX,
                                float yourY,
                                int map_index,
                                const AvatarAttributes* avatar,
                                const Avatar::SpellAvailabilityMask* spellAvailabilityMask);
ENetPacket* leaveWorldConfirmed();

//ENetPacket* actorSpeaks(int actorID, const char* name, const char* speech);


ENetPacket* updateYourEffectAvailabilityMask(const Avatar::SpellAvailabilityMask* mask);

ENetPacket* tellYourAvatarStats(unsigned char level,
                                unsigned char abilities[5],
                                unsigned char availableAbilityPoints,
                                int hp,
                                int hpMax,
                                int mp,
                                int mpMax,
                                int exp,
                                int nextLevelExp,
                                int defense,
                                Evidyon::Avatar::AvatarRace avatar_race,
                                Evidyon::Avatar::AvatarClass avatar_class,
                                int alignment_value);


ENetPacket* changeMap(World::MapIndex map_id);


ENetPacket* fastSync(const FastSyncPacket& data);
ENetPacket* createMagicFX(int type, int src_actor_id, float src_x, float src_y,
              int power, int* target_ids, int number_of_targets);
ENetPacket* createProjectile(int projectile_id, int projectile_type, float x, float y, float dx, float dy);
ENetPacket* destroyProjectile(int projectile_id);

// since we have defined the world as being no bigger than 2^16 tiles wide,
// (it is actually substantially less) world_x and world_y can be packed
// into 16 bits
ENetPacket* tellItemsOnMapLocation(unsigned int world_x,
                                   unsigned int world_y,
                                   unsigned short* item_types,  // derived from item description
                                   int number_of_item_types);   // entries in array; always <= MAX_ITEMS_VISIBLE_PER_LOCATION

// The client should get rid of state-dependent structures whose location is
// beyond a world region bordering the one with this ID number.
// This means, erase items on the ground in far-off areas.
ENetPacket* updateLocalWorldRegion(World::RegionID region_id);


ENetPacket* updateInventory(bool complete_update,
                            AvatarInventoryItem* inventory,
                            int* inventory_indices,
                            int number_of_indices);

ENetPacket* tellActorName(Evidyon::Actor::ActorID actor_id,
                          const char* name);

ENetPacket* storageTell(ItemInStorage* storage, int number_of_items);

ENetPacket* geosidTellGeonite(unsigned int geonite);

ENetPacket* admin_serverMessage(const std::string& reply_text);

}
}
}
}
}

#endif