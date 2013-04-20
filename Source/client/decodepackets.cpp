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
#include "shared/client-server/packets.h"
#include "shared/avatar/evidyon_avatar.h"
#include "shared/actor/evidyon_actor.h"
#include "shared/evidyon_enchantments.h"
#include "shared/evidyon_accounts.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "shared/actor/actorsync.h"
#include "decodepackets.h"
#include "shared/client-server/fastsync.h"
#include "shared/client-server/admin.h"
#include "stdafx.h" // for ItemForTrade only
#include <dc/debug>

namespace Evidyon {
namespace Network {
namespace Packet {
namespace Client {
namespace Decode {


//-----------------------------------------------------------------------------
// Name:  tellServerVersionUpdateInfo
// Desc:  
//-----------------------------------------------------------------------------
bool tellServerVersionUpdateInfo(ENetPacket* packet, bool* versionIsValid, bool* needsNewGamefile, bool* needsNewClient, std::string* updateText) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !versionIsValid || !needsNewGamefile || !needsNewClient || !updateText)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLSERVERVERSIONUPDATEINFO)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<bool>(buffer, endOfBuffer, versionIsValid);
  buffer = decodeStructure<bool>(buffer, endOfBuffer, needsNewGamefile);
  buffer = decodeStructure<bool>(buffer, endOfBuffer, needsNewClient);
  buffer = decodeString(buffer, endOfBuffer, updateText);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellServerVersionUpdateInfo message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellServerVersionUpdateInfo message contained extra data");

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name:  createAccountSucceeded
// Desc:  
//-----------------------------------------------------------------------------
bool createAccountSucceeded(ENetPacket* packet, unsigned int* id) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_CREATEACCOUNT_SUCCEEDED)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode createAccountSucceeded message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createAccountSucceeded message contained extra data");

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name:  logoutAccountConfirmed
// Desc:  
//-----------------------------------------------------------------------------
bool logoutAccountConfirmed(ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_LOGOUTACCOUNT_CONFIRMED)
    return false;

  // Provide feedback about processing
  if (buffer != endOfBuffer) DEBUG_WARNING("logoutAccount message contained extra data");

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name:  tellAccountInfo
// Desc:  
//-----------------------------------------------------------------------------
bool tellAccountInfo(ENetPacket* packet, float* daysLeft, unsigned int* accountCharacterIDs, size_t* numberOfCharacters) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !daysLeft || !accountCharacterIDs)("invalid parameter"))
    return false;

  // Make sure to empty the output variable to prevent problems
  *numberOfCharacters = 0;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLACCOUNTINFO)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<float>(buffer, endOfBuffer, daysLeft);

  // Get the number of character IDs in the following array
  size_t characterIDArraySize;
  buffer = decodeStructure<size_t>(buffer, endOfBuffer, &characterIDArraySize);

  // Validate the number of characters
  if (APP_ERROR(characterIDArraySize > Evidyon::MAX_AVATARS_PER_ACCOUNT)
      ("Invalid number of characters in account array from tellAccountInfo: %lu", characterIDArraySize))
    return false;

  // Assign the number of characters
  *numberOfCharacters = characterIDArraySize;

  // Read each of the characters
  for (size_t i = 0; (i < characterIDArraySize) && (buffer != 0); ++i)
  {
    // Read the ID into the output array
    buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, &accountCharacterIDs[i]);
  }

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellAccountInfo message"))
    return false;

  // Check to make sure we exactly used up the buffer
  if (buffer != endOfBuffer) DEBUG_WARNING("tellAccountInfo message contained extra data");

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name:  requestActionResult
// Desc:  
//-----------------------------------------------------------------------------
bool requestActionResult(ENetPacket* packet, Evidyon::Network::Packet::RequestActionResult* result) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !result)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_REQUESTACTIONRESULT)
    return false;

  // Get the result
  buffer = decodeStructure<RequestActionResult>(buffer, endOfBuffer, result);

  // Provide feedback about processing
  if (buffer != endOfBuffer) DEBUG_WARNING("requestActionResult message contained extra data");

  // Success
  return true;
}


//-----------------------------------------------------------------------------
// Name:  tellCharacterName
// Desc:  
//-----------------------------------------------------------------------------
bool tellCharacterName(ENetPacket* packet, unsigned int* id,
  char* characterNameBuffer, size_t bufferSize, int* map_index, float* world_x,
  float* world_y, unsigned int* actor_type,
  size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT]) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id || !characterNameBuffer || !world_x || !world_y || !actor_type || !visible_equipment)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLCHARACTERNAME)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);
  buffer = decodeString(buffer, endOfBuffer, characterNameBuffer, bufferSize);
  buffer = decodePackedStructure<int, unsigned char>(buffer, endOfBuffer, map_index);
  buffer = decodeStructure<float>(buffer, endOfBuffer, world_x);
  buffer = decodeStructure<float>(buffer, endOfBuffer, world_y);
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, actor_type);
  for (unsigned int i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i)
    buffer = decodeStructure<size_t>(buffer, endOfBuffer, &visible_equipment[i]);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellCharacterName message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellCharacterName message contained extra data");

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name:  createCharacterSucceeded
// Desc:  
//-----------------------------------------------------------------------------
bool createCharacterSucceeded(ENetPacket* packet, unsigned int* id) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id)("invalid parameter to createCharacterSucceeded"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_CREATECHARACTER_SUCCEEDED)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode createCharacterSucceeded message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createCharacterSucceeded message contained extra data");

  // Success
  return true;
}

//-----------------------------------------------------------------------------
// Name:  tellCharacterMapLocation
// Desc:  
//-----------------------------------------------------------------------------
bool tellCharacterMapLocation(ENetPacket* packet, unsigned int* id, int* mapX, int* mapY) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id)("invalid parameter to tellCharacterMapLocation"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLCHARACTERMAPLOCATION)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);
  buffer = decodeStructure<int>(buffer, endOfBuffer, mapX);
  buffer = decodeStructure<int>(buffer, endOfBuffer, mapY);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellCharacterMapLocation message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellCharacterMapLocation message contained extra data");

  // Success
  return true;
}

//-----------------------------------------------------------------------------
// Name:  enterWorldSucceeded
// Desc:  
//-----------------------------------------------------------------------------
bool enterWorldSucceeded(ENetPacket* packet,
                         Evidyon::Actor::ActorID* actorID,
                         unsigned long* time_of_day,
                         float* yourX,
                         float* yourY,
                         int* map_index,
                         bool* male,
                         int* raceIndex,
                         int* classIndex,
                         int* currentHP,
                         int* maxHP,
                         int* currentMP,
                         int* maxMP,
                         unsigned int* money,
                         unsigned int* geonite,
                         Avatar::SpellAvailabilityMask* spellAvailabilityMask) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !actorID || !spellAvailabilityMask)("invalid parameter to enterWorldSucceeded"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_ENTERWORLD_SUCCEEDED)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<Evidyon::Actor::ActorID>(buffer, endOfBuffer, actorID);
  buffer = decodeStructure<unsigned long>(buffer, endOfBuffer, time_of_day);
  buffer = decodeStructure<float>(buffer, endOfBuffer, yourX);
  buffer = decodeStructure<float>(buffer, endOfBuffer, yourY);
  buffer = decodeStructure<int>(buffer, endOfBuffer, map_index);
  buffer = decodeStructure<bool>(buffer, endOfBuffer, male);
  buffer = decodeStructure<int>(buffer, endOfBuffer, raceIndex);
  buffer = decodeStructure<int>(buffer, endOfBuffer, classIndex);
  buffer = decodeStructure<int>(buffer, endOfBuffer, currentHP);
  buffer = decodeStructure<int>(buffer, endOfBuffer, maxHP);
  buffer = decodeStructure<int>(buffer, endOfBuffer, currentMP);
  buffer = decodeStructure<int>(buffer, endOfBuffer, maxMP);
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, money);
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, geonite);
  buffer = decodeStructure<Avatar::SpellAvailabilityMask>(buffer, endOfBuffer, spellAvailabilityMask);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode enterWorldSucceeded message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("enterWorldSucceeded message contained extra data");

  // Success
  return true;
}

//-----------------------------------------------------------------------------
// Name:  leaveWorldConfirmed
// Desc:  
//-----------------------------------------------------------------------------
bool leaveWorldConfirmed(ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  return decodeMessageOnlyPacket(packet, NETMSG_LEAVEWORLD_CONFIRMED);
}


//
////-----------------------------------------------------------------------------
//// Name:  actorSpeaks
//// Desc:  
////-----------------------------------------------------------------------------
//bool actorSpeaks(ENetPacket* packet,
//                    int* actorID,
//                    char* source_name_buffer,
//                    size_t source_name_buffer_size,
//                    char* speechBuffer,
//                    size_t bufferSize) {
//  using namespace ::Evidyon::Network::Packet;
//  // Make sure the packet is valid
//  if (APP_ERROR(!packet || !actorID || !source_name_buffer ||
//           !source_name_buffer_size || !speechBuffer || !bufferSize)("invalid parameter"))
//    return false;
//
//  // Obtain pointers to packet data
//  const char* buffer = (const char*)packet->data;
//  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;
//
//  // Make sure that the message type is correct
//  Message message;
//  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
//    message != NETMSG_ACTOR_SPEAKS)
//    return false;
//
//  // Obtain the data from the packet
//  buffer = decodeStructure<int>(buffer, endOfBuffer, actorID);
//  int actor_id = *actorID;
//  if (actor_id == -1 || actor_id==-2 || actor_id==-3 || actor_id==-4) {
//    buffer = decodeString(buffer, endOfBuffer, source_name_buffer, source_name_buffer_size);
//  }
//  buffer = decodeString(buffer, endOfBuffer, speechBuffer, bufferSize);
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode actorSpeaks message")) return false;
//  if (buffer != endOfBuffer) DEBUG_WARNING("actorSpeaks message contained extra data");
//
//  // Success
//  return true;
//}
//






//-----------------------------------------------------------------------------
// Name:  updateYourEffectAvailabilityMask
// Desc:  
//-----------------------------------------------------------------------------
bool updateYourEffectAvailabilityMask(ENetPacket* packet, Avatar::SpellAvailabilityMask* mask) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !mask)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_UPDATEYOUREFFECTAVAILABILITYMASK)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<Avatar::SpellAvailabilityMask>(buffer, endOfBuffer, mask);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode updateYourEffectAvailabilityMask message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("updateYourEffectAvailabilityMask message contained extra data");

  // Success
  return true;
}


//-----------------------------------------------------------------------------
// Name:  tellYourAvatarStats
// Desc:  
//-----------------------------------------------------------------------------
bool tellYourAvatarStats(ENetPacket* packet, unsigned char* level, unsigned char abilities[5], unsigned char* availableAbilityPoints,
                         int* hp, int* hpMax, int* mp, int* mpMax, int* exp, int* nextLevelExp, int* defense,
                         Evidyon::Avatar::AvatarRace* avatar_race, Evidyon::Avatar::AvatarClass* avatar_class, int* alignment_value) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !level || !abilities || !availableAbilityPoints || !hp || !hpMax || !mp || !mpMax ||!exp || !nextLevelExp || !defense)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLYOURAVATARSTATS)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, level);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[0]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[1]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[2]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[3]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[4]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, availableAbilityPoints);
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, hp      );
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, hpMax     );
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, mp      );
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, mpMax     );
  buffer = decodeStructure<int>(buffer, endOfBuffer, exp      );
  buffer = decodeStructure<int>(buffer, endOfBuffer, nextLevelExp);
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, defense);
  buffer = decodePackedStructure<Evidyon::Avatar::AvatarRace, unsigned char>(buffer, endOfBuffer, avatar_race);
  buffer = decodePackedStructure<Evidyon::Avatar::AvatarClass, unsigned char>(buffer, endOfBuffer, avatar_class);
  buffer = decodePackedStructure<int, char>(buffer, endOfBuffer, alignment_value);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellYourAvatarStats message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellYourAvatarStats message contained extra data");

  // Success
  return true;
}


//-----------------------------------------------------------------------------
bool changeMap(ENetPacket* packet, World::MapIndex* map_id) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_CHANGEMAP, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<World::MapIndex>(buffer, buffer_end, map_id);

  return buffer != NULL;
}



//-----------------------------------------------------------------------------
bool tellItemsOnMapLocation(ENetPacket* packet,
                            int* world_x,
                            int* world_y,
                            unsigned short* item_types,
                            int* number_of_item_types) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_TELLITEMSONMAPLOCATION, &buffer, &buffer_end))
  return false;

  unsigned long packed_location;
  buffer = decodeStructure<unsigned long>(buffer, buffer_end, &packed_location);
  *world_x = static_cast<int>(packed_location>>16);
  *world_y = static_cast<int>(packed_location&0xFFFF);

  {
    unsigned char byte_number = 0;
    buffer = decodeArray<unsigned short, unsigned char>(buffer,
                                                        buffer_end,
                                                        item_types,
                                                        MAX_ITEMS_VISIBLE_PER_LOCATION,
                        &byte_number);
    *number_of_item_types = byte_number;
  }

  return buffer != NULL;
}




//-----------------------------------------------------------------------------
bool updateOrTellInventory(ENetPacket* packet,
                           Evidyon::ClientAvatarInventoryItem fullClientInventory[Evidyon::AVATAR_INVENTORY_SIZE],
                           int* highest_inventory_update_index,
                           int inventory_update_indices[Evidyon::AVATAR_INVENTORY_SIZE]) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_UPDATEINVENTORY, &buffer, &buffer_end)) {
    if (!openPacket(packet, NETMSG_TELLINVENTORY, &buffer, &buffer_end)) {
      return false;
    }

    // Clear the inventory
    *highest_inventory_update_index = Evidyon::AVATAR_INVENTORY_SIZE;

    for (int i = 0; i < Evidyon::AVATAR_INVENTORY_SIZE; ++i) {
      fullClientInventory[i].equipped = false;
      fullClientInventory[i].inventory_index = Evidyon::AVATAR_INVENTORY_SIZE;
      fullClientInventory[i].quantity = 0;
      fullClientInventory[i].type = (unsigned)(~0L);
      inventory_update_indices[i] = 0;
    }
  }

  int new_inventory_update_index = (*highest_inventory_update_index) + 1;
  *highest_inventory_update_index = new_inventory_update_index;

  int number_of_indices = 0;
  {
    unsigned char byte_number = 0;
    buffer = decodeStructure<unsigned char>(buffer, buffer_end, &byte_number);
    number_of_indices = byte_number;
  }

  for (int i = 0; i < number_of_indices; ++i) {
    ClientAvatarInventoryItem inventory_item;
    buffer = decodeStructure<ClientAvatarInventoryItem>(buffer,
                                                        buffer_end,
                                                       &inventory_item);

    int inventory_index = inventory_item.inventory_index;
    CONFIRM(inventory_index < Evidyon::AVATAR_INVENTORY_SIZE) else continue;
    memcpy(&fullClientInventory[inventory_index],
           &inventory_item,
            sizeof(inventory_item));
    inventory_update_indices[inventory_index] = new_inventory_update_index;
  }

  return buffer != NULL;
}


bool updateLocalWorldRegion(ENetPacket* packet,
                            World::RegionID* region_id) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_UPDATELOCALWORLDREIGON, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<World::RegionID>(buffer, buffer_end, region_id);

  return buffer != NULL;
}


bool tellActorName(ENetPacket* packet,
                   Evidyon::Actor::ActorID* actor_id,
                   std::string* name) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_TELLACTORNAME, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<Evidyon::Actor::ActorID>(buffer, buffer_end, actor_id);
  buffer = decodeString(buffer, buffer_end, name);

  return buffer != NULL;
}


bool storageTell(ENetPacket* packet,
                 Evidyon::ItemInStorage items[Evidyon::MAX_ITEMS_IN_STORAGE],
                 int* number_of_items) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_STORAGE_TELL, &buffer, &buffer_end))
  return false;

  unsigned char byte_number_of_items = 0;
  buffer = decodeArray<ItemInStorage, unsigned char>(buffer, buffer_end, items, MAX_ITEMS_IN_STORAGE, &byte_number_of_items);
  *number_of_items = byte_number_of_items;
  for (int i = (int)byte_number_of_items; i < MAX_ITEMS_IN_STORAGE; ++i) {
    items[i].index = -1;
    items[i].type = -1;
  }

  return buffer != NULL;
}



bool geosidTellGeonite(ENetPacket* packet, unsigned int* geonite) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_GEOSID_TELLGEONITE, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<unsigned int>(buffer, buffer_end, geonite);

  return buffer != NULL;
}



bool admin_serverMessage(ENetPacket* packet,
                         std::string* reply) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_ADMIN_SERVERMESSAGE, &buffer, &buffer_end))
  return false;

  buffer = decodeString(buffer, buffer_end, reply);

  return buffer != NULL;
}




}
}
}
}
}