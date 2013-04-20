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
#include "shared/server-editor/server_gamefile.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "encodepackets.h"
#include <dc/debug>
#include "shared/avatar/evidyon_avatar.h"
#include "shared/evidyon_accounts.h"

#include "shared/actor/actorsync.h"



#include "shared/actor/actordescription.h"
#include "shared/client-server/fastsync.h"
#include "item.h"
#include "avatarinventoryitem.h"

#include "shared/evidyon_storage.h"
#include "shared/item/itemserverdescription.h"


namespace Evidyon {
namespace Network {
namespace Packet {
namespace Server {
namespace Encode {


#pragma pack(push, 1)

struct ItemForTrade {
  unsigned listing_id  : 32;
  unsigned price     : 32;
  unsigned quantity  : 8;
  unsigned type    : 15;
  unsigned purchased   : 1;
  unsigned you_sell  : 1; // only valid for "get listed items" by client; denotes that the viewer is the seller
  unsigned __void    : 7;
};

#pragma pack(pop)


  
//-----------------------------------------------------------------------------
// Name:  tellServerVersionUpdateInfo
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* tellServerVersionUpdateInfo(bool versionIsValid, bool needsNewGamefile, bool needsNewClient, const char* updateText){
  using namespace ::Evidyon::Network::Packet;


  //typedef ::Evidyon::Network::Packet
  // Allocate a packet for this message.  Using Avatar::MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0,NETWORK_MESSAGE_SIZE +
    sizeof(bool) * 4 + NETWORK_STRING_PADDING_BYTES + 256, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLSERVERVERSIONUPDATEINFO, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<bool>(versionIsValid, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<bool>(needsNewGamefile, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<bool>(needsNewClient, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(updateText ? updateText : "", packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellServerVersionUpdateInfo"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}


//-----------------------------------------------------------------------------
// Name:  createAccountSucceeded
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* createAccountSucceeded(unsigned int account_id){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  Using Avatar::MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_CREATEACCOUNT_SUCCEEDED, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(account_id, packetBuffer, packetBufferEnd);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during createAccountSucceeded"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//-----------------------------------------------------------------------------
// Name:  tellAccountInfo
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* tellAccountInfo(float daysLeft, const unsigned int* characterIDArray, size_t numberOfCharacters){
  using namespace ::Evidyon;
  using namespace ::Evidyon::Network::Packet;

  // Validate parameters
  if (APP_ERROR(daysLeft < 0.0f || !characterIDArray ||
    (numberOfCharacters > Evidyon::MAX_AVATARS_PER_ACCOUNT))("Invalid parameter to tellAccountInfo"))
    return 0;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(float) +
                        sizeof(size_t) +
                        sizeof(unsigned int) * numberOfCharacters, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet header
    packetBuffer = encodeMessageType(NETMSG_TELLACCOUNTINFO, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(daysLeft, packetBuffer, packetBufferEnd);

    // Write the number of characters in the account
    packetBuffer = encodeStructure<size_t>(numberOfCharacters, packetBuffer, packetBufferEnd);

    // Write the array of character IDs
    for (size_t i = 0; i < numberOfCharacters; ++i)
    {
      packetBuffer = encodeStructure<unsigned int>(characterIDArray[i], packetBuffer, packetBufferEnd);
    }
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellAccountInfo"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//-----------------------------------------------------------------------------
// Name:  requestActionResult
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* requestActionResult(::Evidyon::Network::Packet::RequestActionResult result){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  Using Avatar::MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(RequestActionResult), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_REQUESTACTIONRESULT, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<RequestActionResult>(result, packetBuffer, packetBufferEnd);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during requestActionResult"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}


//-----------------------------------------------------------------------------
// Name:  logoutAccountConfirmed
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* logoutAccountConfirmed(){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_LOGOUTACCOUNT_CONFIRMED, packetBuffer, packetBufferEnd);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during logoutAccountConfirmed"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}

//-----------------------------------------------------------------------------
// Name:  tellCharacterName
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* tellCharacterName(unsigned int id,
                              const char* characterName,
                              int map_index,
                              float world_x,
                              float world_y,
                              unsigned int actor_type,
                              size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT]){
  using namespace ::Evidyon;
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  Using Avatar::MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                                          sizeof(unsigned int) +
                                          NETWORK_STRING_PADDING_BYTES +
                                          Avatar::MAX_AVATAR_NAME_LENGTH +
                                          sizeof(unsigned char) + // packed map index
                                          sizeof(float)*2 + sizeof(actor_type) +
                                          sizeof(size_t)*ACTORATTACHMENTPOINT_COUNT, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLCHARACTERNAME, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(characterName, packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned char>(map_index, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(world_x, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(world_y, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(actor_type, packetBuffer, packetBufferEnd);
    for (size_t i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i)
      packetBuffer = encodeStructure<size_t>(visible_equipment[i], packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellCharacterName"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}


//-----------------------------------------------------------------------------
// Name:  createCharacterSucceeded
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* createCharacterSucceeded(unsigned int character_id){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_CREATECHARACTER_SUCCEEDED, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(character_id, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during createCharacterSucceeded"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}




//-----------------------------------------------------------------------------
// Name:  tellCharacterMapLocation
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* tellCharacterMapLocation(unsigned int id, int mapX, int mapY){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int) +
                        sizeof(int) * 2, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLCHARACTERMAPLOCATION, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(mapX, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(mapY, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellCharacterMapLocation"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}





//-----------------------------------------------------------------------------
// Name:  leaveWorldConfirmed
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* leaveWorldConfirmed() {
  using namespace ::Evidyon::Network::Packet;

  return encodeMessageOnlyPacket(NETMSG_LEAVEWORLD_CONFIRMED);
}




//
//
////-----------------------------------------------------------------------------
//// Name:  actorSpeaks
//// Desc:  
////-----------------------------------------------------------------------------
//ENetPacket* actorSpeaks(int actorID, const char* name, const char* speech){
//  using namespace ::Evidyon;
//  using namespace ::Evidyon::Network::Packet;
//
//  //if (name) actorID = -1;
//  bool include_name = actorID == -1 || actorID == -2 || actorID == -3 || actorID == -4;
//  if (include_name && name == NULL) name = ""; // Don't let the name be invalid
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
//                        sizeof(int) +
//                        (include_name ? NETWORK_STRING_PADDING_BYTES + Avatar::MAX_AVATAR_NAME_LENGTH*2 : 0) +
//                        NETWORK_STRING_PADDING_BYTES +
//                        Avatar::MAX_SPOKEN_TEXT_LENGTH,
//                        ENET_PACKET_FLAG_RELIABLE);
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_ACTOR_SPEAKS, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<int>(actorID, packetBuffer, packetBufferEnd);
//    if (include_name) packetBuffer = encodeString(name, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeString(speech, packetBuffer, packetBufferEnd);
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during actorSpeaks"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}


//-----------------------------------------------------------------------------
// Name:  updateYourEffectAvailabilityMask
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* updateYourEffectAvailabilityMask(const Avatar::SpellAvailabilityMask* mask){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(Avatar::SpellAvailabilityMask), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_UPDATEYOUREFFECTAVAILABILITYMASK, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<Avatar::SpellAvailabilityMask>(*mask, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during updateYourEffectAvailabilityMask"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//-----------------------------------------------------------------------------
// Name:  tellYourAvatarStats
// Desc:  
//-----------------------------------------------------------------------------
ENetPacket* tellYourAvatarStats(
  unsigned char level, unsigned char abilities[5], unsigned char availableAbilityPoints,
  int hp, int hpMax, int mp, int mpMax, int exp, int nextLevelExp, int defense,
                                Evidyon::Avatar::AvatarRace avatar_race,
                                Evidyon::Avatar::AvatarClass avatar_class,
                                int alignment_value){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned char) * 7 +
                        sizeof(int) * 8, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet) {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLYOURAVATARSTATS, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(level, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[0], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[1], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[2], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[3], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[4], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(availableAbilityPoints, packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(hp,       packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(hpMax,    packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(mp,       packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(mpMax,    packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(exp,      packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(nextLevelExp, packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(defense,    packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<Evidyon::Avatar::AvatarRace, unsigned char>(avatar_race, packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<Evidyon::Avatar::AvatarClass, unsigned char>(avatar_class, packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, char>(alignment_value, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellYourAvatarStats"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}






//----[  changeMap  ]----------------------------------------------------------
ENetPacket* changeMap(World::MapIndex map_id) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_CHANGEMAP,
          sizeof(World::MapIndex),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<World::MapIndex>(map_id, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;

}







ENetPacket* fastSync(const ::Evidyon::Network::Packet::FastSyncPacket& data) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_FASTSYNC,
          sizeof(FastSyncPacket),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<FastSyncPacket>(data, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}



ENetPacket* tellItemsOnMapLocation(unsigned int world_x,
                                   unsigned int world_y,
                                   unsigned short* item_types,
                                   int number_of_item_types) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_TELLITEMSONMAPLOCATION,
          sizeof(unsigned int) +
          sizeof(unsigned char) +
          sizeof(unsigned short) * (number_of_item_types),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  // pack the coordinates
  buffer = encodeStructure<unsigned int>(((world_x & 0xFFFF) << 16) | (world_y & 0xFFFF),
                                         buffer,
                                         buffer_end);

  // save the items here
  unsigned char byte_size =
    static_cast<unsigned char>(number_of_item_types);
  buffer = encodeArray<unsigned short, unsigned char>(item_types,
                                                      byte_size,
                                                      buffer,
                                                      buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}



ENetPacket* updateLocalWorldRegion(World::RegionID region_id) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_UPDATELOCALWORLDREIGON,
          sizeof(World::RegionID),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<World::RegionID>(region_id,
                                            buffer,
                                            buffer_end);


  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}




ENetPacket* updateInventory(bool complete_update,
                            AvatarInventoryItem* inventory,
                            int* inventory_indices,
                            int number_of_indices) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          complete_update ? NETMSG_TELLINVENTORY : NETMSG_UPDATEINVENTORY,
          sizeof(unsigned char) + sizeof(ClientAvatarInventoryItem) * number_of_indices,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  // pack the size
  CONFIRM(number_of_indices <= AVATAR_INVENTORY_SIZE) else return NULL;
  unsigned char byte_number_of_indices = (unsigned char)number_of_indices;
  buffer = encodeStructure<unsigned char>(byte_number_of_indices,
                                          buffer,
                                          buffer_end);

  // add each entry
  for (int i = 0; i < number_of_indices; ++i) {
    int index = inventory_indices[i];
    CONFIRM(index >= 0 && index < AVATAR_INVENTORY_SIZE) else return false;
    AvatarInventoryItem* inventory_item = &inventory[index];
    ItemInstance* item = inventory_item->item.dereference();
    ClientAvatarInventoryItem item_description;
    item_description.equipped = inventory_item->equipped;
    item_description.inventory_index = (unsigned)index;

    const Evidyon::Item::ItemServerDescription* server_desc = NULL;
    if (item != NULL) {
      server_desc = item->getDescription();
      item_description.quantity = item->getQuantity();
    }
    if (server_desc != NULL) {
      item_description.type = server_desc->type_index;
    } else {
      item_description.quantity = 0;
      item_description.type = (unsigned)(~0L);
    }

    buffer = encodeStructure<ClientAvatarInventoryItem>(item_description,
                                                        buffer,
                                                        buffer_end);
  }


  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
                                  
}

ENetPacket* tellActorName(Evidyon::Actor::ActorID actor_id,
                          const char* name) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_TELLACTORNAME,
          sizeof(actor_id) + NETWORK_STRING_PADDING_BYTES + Avatar::MAX_AVATAR_NAME_LENGTH,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned long>(actor_id,
                                          buffer,
                                          buffer_end);
  buffer = encodeString(name, buffer, buffer_end);


  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}

ENetPacket* storageTell(ItemInStorage* storage, int number_of_items) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
                    NETMSG_STORAGE_TELL,
                    sizeof(unsigned char) + sizeof(ItemInStorage) * number_of_items,
                    &packet,
                    &buffer,
                    &buffer_end)) {
    return 0;
  }

  unsigned char byte_number_of_items = (unsigned char)number_of_items;
  buffer = encodeArray<ItemInStorage, unsigned char>(storage,
                                                     byte_number_of_items,
                                                     buffer,
                                                     buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}


ENetPacket* geosidTellGeonite(unsigned int geonite) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
                    NETMSG_GEOSID_TELLGEONITE,
                    sizeof(unsigned int),
                    &packet,
                    &buffer,
                    &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned int>(geonite, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}



ENetPacket* admin_serverMessage(const std::string& reply_text) {
  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
            NETMSG_ADMIN_SERVERMESSAGE,
            reply_text.length() + NETWORK_STRING_PADDING_BYTES,
            &packet,
            &buffer,
            &buffer_end)) {
    return 0;
  }

  buffer = encodeString(reply_text, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return 0;
  return packet;
}



}
}
}
}
}
