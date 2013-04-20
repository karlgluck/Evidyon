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


struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

#include "shared/actor/actorid.h"

namespace Evidyon {
enum AdminSimpleCommand;

namespace Network {
namespace Packet {
namespace Server {
namespace Decode {

bool tellClientVersionInfo(ENetPacket* packet, unsigned long* gamefileVersion, unsigned long* clientVersion);
bool createAccount(ENetPacket* packet, char* passwordBuffer, size_t bufferSize);
bool accessAccount(ENetPacket* packet, unsigned int* id, char* passwordBuffer, size_t bufferSize);
bool logoutAccount(ENetPacket* packet);
bool createCharacter(ENetPacket* packet, char* nameBuffer, size_t bufferSize, bool* male, int* chRace, int* chClass, int* chStr, int* chAgi, int* chCon, int* chInt, int* chWis);
bool deleteCharacter(ENetPacket* packet, unsigned int* id, char* passwordBuffer, size_t bufferSize);
bool askCharacterName(ENetPacket* packet, unsigned int* id);
bool askAccountInfo(ENetPacket* packet);
bool askCharacterMapLocation(ENetPacket* packet, unsigned int* id);

bool enterWorld(ENetPacket* packet, unsigned int* id);
bool leaveWorld(ENetPacket* packet);
bool requestMove(ENetPacket* packet, float* x, float* y, Evidyon::Actor::ActorID* targetActorID);
bool requestMoveStreaming(ENetPacket* packet, float* x, float* y);
bool requestSpeak(ENetPacket* packet, char* speechBuffer, size_t bufferSize);

bool changeEquippedState(ENetPacket* packet, unsigned char* inventoryIndices, size_t bufferSize, size_t* numberOfIndices);
bool dropInventoryItems(ENetPacket* packet, unsigned char* inventoryIndices, size_t bufferSize, size_t* numberOfIndices);
//
//bool askActorDescription(ENetPacket* packet, int* actorID);

bool askMyAvatarStats(ENetPacket* packet, int* abilityToAddPoint);

bool castSpell(ENetPacket* packet, unsigned int spell_ids[3], Evidyon::Actor::ActorID* target_actor_id, float* target_x, float* target_y);
bool useItem(ENetPacket* packet, unsigned int* inventory_index, Evidyon::Actor::ActorID* target_actor_id, float* target_x, float* target_y);

bool askActorName(ENetPacket* packet, Evidyon::Actor::ActorID* actor_id);

bool storageOpen(ENetPacket* packet, Evidyon::Actor::ActorID* storage_npc_actor_id);
bool storageWithdraw(ENetPacket* packet, Evidyon::Actor::ActorID* storage_npc_actor_id, int* storage_index);
bool storageDeposit(ENetPacket* packet, Evidyon::Actor::ActorID* storage_npc_actor_id, int* inventory_index);

bool geosidSacrifice(ENetPacket* packet, unsigned char* inventory_index);

bool admin_sqlQuery(ENetPacket* packet, std::string* reply);
bool admin_createItem(ENetPacket* packet, int* type);
bool admin_simpleCommand(ENetPacket* packet, AdminSimpleCommand* command);


}
}
}
}
}


#endif