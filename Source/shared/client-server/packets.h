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
#ifndef __PACKETS_H__
#define __PACKETS_H__
#pragma once



#include "../../common/enet/enet.h"
#include <string>

#include "evidyonpacketserializer.h"
#include "../../common/serializer/serializer.h"
#include "tradeitemdescription.h"
#include "bazaar.h"
#include "../actor/actorid.h"
#include "../currencytype.h"
#include "../../shared/actor/actordescription.h"
#include "../../shared/actor/actorslowsync.h"
#include "../../shared/actor/actorfastsync.h"
#include "../../shared/avatar/spellavailabilitymask.h"

// TODO: move all packet stuff into the "packets" namespace
#include "shared/event/eventid.h"
#include "shared/packets/eventpackets.h"
#include "shared/magic/buffmask.h"
#include "shared/geosid/geosidindex.h"
#include "shared/geosid/geonite.h"
#include "shared/spell/spellindex.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/avatar/avatarattribute.h"

namespace Evidyon {

static const int MAX_ITEMS_VISIBLE_PER_LOCATION = 15;

namespace Network {
namespace Packet {

// Specifies the different kinds of messages that can be passed in a
// network packet.
enum Message {

  NETMSG_INVALID,

  NETMSG_TELLCLIENTVERSIONINFO,   // client informs server of its version information

  // client->server requests
  NETMSG_ASKFORACCOUNTNUMBER, // client wants an account number
  NETMSG_LOGINTOACCOUNT,
  NETMSG_CREATEACCOUNT,     // client wants to make a new account
  NETMSG_ACCESSACCOUNT,     // request to access an account
  NETMSG_LOGOUTACCOUNT,     // logs the client out of the account on the server
  NETMSG_CREATECHARACTER,     // attempts to create a new character
  NETMSG_DELETECHARACTER,     // attempts to delete a character on the account
  NETMSG_ASKCHARACTERNAME,    // gets the name of a character
  NETMSG_ASKACCOUNTINFO,    // gets the characters on this account
  NETMSG_ASKCHARACTERMAPLOCATION,   // gets the map location in the world where the character is; used to prime the map renderer before logging in

  NETMSG_LEAVEWORLD,      // leaves the world
  NETMSG_ENTERWORLD,      // logs into the world with a character (reply is NETMSG_ENTERWORLD_SUCCEEDED or NETMSG_REQUESTACTIONRESULT)
  NETMSG_DROPINVENTORYITEMS,    // client wants to drop items
  NETMSG_CHANGEEQUIPPEDSTATE,   // client wants to move items from equipped->unequipped or vice-versa
  NETMSG_DROP_CURRENCY,       // drop gold on ground

  NETMSG_REQUEST_MOVE_STREAMING,  // move avatar in the direction of the given coordinates, but don't stop there.  doesn't contain target actor ID
  NETMSG_REQUEST_MOVE,            // move avatar; change combat target actor

  NETMSG_ASKMYAVATARSTATS,    // requests the avatar's displayable statistics (hp/mp values, defense, level, str/agi...etc) from the server
  NETMSG_ASKACTORDESCRIPTION,   // gets information about an actor, such as their mesh type


  NETMSG_SPLITSTACKEDITEM,
  NETMSG_STOPACTION,


  // server->client messages
  NETMSG_TELLSERVERVERSIONUPDATEINFO, // server tells client whether its version is valid

  NETMSG_CREATECHARACTER_SUCCEEDED,   // if a character was created, the server replies with this message instead of NETMSG_REQUESTACTIONRESULT
  NETMSG_CREATEACCOUNT_SUCCEEDED,   // creating an account never fails, unless something is wrong with the server
  NETMSG_LOGOUTACCOUNT_CONFIRMED,   // confirms that the user is now logged out of the account (also used to force client logouts when bumped from server)
  NETMSG_TELLACCOUNTINFO,     // server is telling the client account information, like days left and which characters are on the account
  NETMSG_TELLCHARACTERMAPLOCATION,  // provides the map location where a character is located
  NETMSG_REQUESTACTIONRESULT,   // provides a result code to a user request

  NETMSG_ENTERWORLD_SUCCEEDED,  // character logs in; tells the client its player's actor ID and other character-specific information
  NETMSG_TELLCHARACTERNAME,     // gives the client the name of a character
  NETMSG_LEAVEWORLD_CONFIRMED,  // player left the world successfully

  NETMSG_ACTOR_SPEAKS,     // an actor says some text
  NETMSG_ACTOR_SPEAKS_TO_ACTOR,  // an actor is saying something to another actor.  if this client's actor is the target, the text is always shown and the source actor's speech is never overwritten by another speech message

  NETMSG_UPDATEYOUREFFECTAVAILABILITYMASK,  // tell the client that the set of abilities it can use has been modified
  NETMSG_TELLYOURBUFFICONS,
  NETMSG_TELLYOURAVATARSTATS,   // updates the client's avatar's statistics

  // These messages are sent frequently from server->client to update the
  // client with the current world state.
  // The fast sync is much smaller and sent more frequently.  It informs the
  //   client of its own location in the world.
  // The slow sync tells the client where all visible actors are in
  //   the world.  This message is sent 2-5 times per second.
  NETMSG_FASTSYNC,
  NETMSG_SLOWSYNC,

  // Switches the map that the client is on
  NETMSG_CHANGEMAP,

  // An actor has been completely removed from the world
  NETMSG_ERASEACTORFROMWORLD,
  // Finds out what an actor's name is.  Most of the time, an actor's name
  // is specified by its template type and this command is avoided.
  NETMSG_ASKACTORNAME,
  NETMSG_TELLACTORNAME,

  // Client wants to know what actors in the world that it has never seen
  // before look like.  Sent in response to NETMSG_SLOWSYNC when a client
  // has never seen an actor before.
  NETMSG_ASKACTORFULLDESCRIPTIONS,
  NETMSG_TELLACTORFULLDESCRIPTIONS,
  NETMSG_TELLACTORDESCRIPTION,  // tells the client a given actor's description

  NETMSG_CAST_SPELL,
  NETMSG_USE_ITEM,
  NETMSG_REQUEST_SPEAK,     // say something


  // Tells the client to create some items on the map
  NETMSG_TELLITEMSONMAPLOCATION,
  NETMSG_UPDATELOCALWORLDREIGON,

  // Updates specific indices in the client's inventory.  These messages
  // have the same structure, but the client should clear all existing
  // items from the inventory before setting the results of the second.
  NETMSG_UPDATEINVENTORY,
  NETMSG_TELLINVENTORY,

  // Interaction with a geosid
  //NETMSG_GEOSID_OPEN,         // client wants to start interacting with a geosid
  //NETMSG_GEOSID_TELLGEONITE,  // tell the client how much geonite they have
  //NETMSG_GEOSID_SACRIFICE,    // request to extract genite from items (from inventory).  can only be done at a crystal.

  NETMSG_TELLCURRENCY,            // server updates client with amount of gold currently held
  NETMSG_BAZAAR_OPEN,             // client wants to start interacting with the bazaar
  NETMSG_BAZAAR_TELLITEMS,        // s->c, items in a category
  NETMSG_BAZAAR_CREATELISTING,    // moves item from inventory to bazaar
  NETMSG_BAZAAR_RETRACTLISTING,   // pulls item from bazaar to inventory
  NETMSG_BAZAAR_REDEEMLISTING,    // gets money for a sold item
  NETMSG_BAZAAR_PURCHASELISTING,  // buy an item
  NETMSG_BAZAAR_GETLISTINGS,      // obtain a list of items in the given category
  NETMSG_BAZAAR_PURCHASELISTING_RESULT, // bool parameter; did the purchase go through?  accompanied by other updates.

  NETMSG_MERCHANT_GETSALES,   // player wants to see merchant wares; param = NPC id
  NETMSG_MERCHANT_TELLSALES,  // server telling client what the merchant has; params = array of TradeItemDescription
  NETMSG_MERCHANT_SELLTO,     // sells inventory item to merchant; params = NPC id, inventory index
  NETMSG_MERCHANT_BUYFROM,    // purchases something from merchant;  params = NPC id, merchant index 

  // Manage the contents of this account's storage
  NETMSG_STORAGE_OPEN,      // gets the storage status; needs storage NPC
  NETMSG_STORAGE_TELL,      // response to "open"
  NETMSG_STORAGE_DEPOSIT,   // item from inventory -> storage; needs storage NPC
  NETMSG_STORAGE_WITHDRAW,  // item from storage -> inventory; needs storage NPC
  NETMSG_STORAGE_UPDATE,    // response to deposit/withdraw

  // messages for p2p trade
  NETMSG_TRADE_ADDITEM,
  NETMSG_TRADE_SETCURRENCY,
  NETMSG_TRADE_REMOVEITEM,
  NETMSG_TRADE_ACCEPT,
  NETMSG_TRADE_WITHDRAW,
  NETMSG_TRADE_CANCEL,
  NETMSG_TRADE_TELLITEMSINTRADE,  // server updates client with items in trade

  // add messages for p2p gift

  // quests
  NETMSG_QUEST_TRIGGER,
  NETMSG_QUEST_OFFER,           // server NPC has offered client a quest
  NETMSG_QUEST_OFFER_RESPONSE,  // client accepts/declines quest
  NETMSG_QUEST_TELLCURRENTQUESTS, // server is updating client's quest log
  NETMSG_QUEST_GIVEUP,          // client wants to terminate a quest

  // event messages
  NETMSG_EVENT_GENERATEBASICPROJECTILE,   // creates a projectile going from a -> b
  NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCTOTGT, // source actor->target actor effect
  NETMSG_EVENT_GENERATETARGETEDSPECIALFX_TGTXZ, // source actor->target location
  NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCXZTGTXZ, // source location->target location
  NETMSG_EVENT_GENERATETARGETEDSPECIALFX_TGTXZAOE,  // source actor -> AOE w/ target actors @ location
  NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCXZTGTXZAOE, // source location -> AOE w/ target actors @ location
  NETMSG_EVENT_TERMINATE,                 // kills an event before its duration expires

  // guilds
	NETMSG_GUILD_CREATE,  // g/create Guild Name
	NETMSG_GUILD_LEAD,    // g/lead
	NETMSG_GUILD_JOIN,    // g/join
	NETMSG_GUILD_LEAVE,   // g/lead
	NETMSG_GUILD_PROMOTE, // g/promote [name] - founder promotes someone to be a leader
	NETMSG_GUILD_DEMOTE,  // g/demote [name] - founder removes someone from leadership
	NETMSG_GUILD_CHAT, // g- or -- to send text to guild members (bidirectional message)
	NETMSG_GUILD_KICK,      // removes someone from the guild
	NETMSG_GUILD_MEMBERS,   // gets a list of the guild members and sends it to the client
  NETMSG_GUILD_CHAT_DISTRIBUTE, // sends chat message to online players
  NETMSG_GUILD_MESSAGE,   // guild itself is sending a message

  NETMSG_GUILD_TELLFOUNDERACTORNAME, // set the name of an actor that is the founder of a guild
  NETMSG_GUILD_TELLLEADERACTORNAME,  //  "   "   "   "   "   "   " "   "   " leader  "   "   "
  NETMSG_GUILD_TELLMEMBERACTORNAME,  //  "   "   "   "   "   "   " "   "   " member  "   "   "

  NETMSG_GUILD_ASKGEOSIDS,        // c->s, get message of which geosids this guild owns
  NETMSG_GUILD_SETHOME,            // set the home point for the guild; where members rally
  NETMSG_GUILD_HOME,              // go to the home point
  NETMSG_GUILD_MESSAGE_TELLGEOSIDS,       // s->c; array of geosid ID #s
  NETMSG_GUILD_MESSAGE_TELLATTACK,  // s->c, "<<Guild>> Your guild is now (attacking/attempting to take over) the NAME geosid; it will be (freed from the owners' control/under your control) in 30 seconds as long as MEMBER'S_NAME stays alive and nearby"
  NETMSG_GUILD_MESSAGE_GEOSIDGAINED,      // s->c, inform guild it now owns geosid; geosid index
  NETMSG_GUILD_MESSAGE_GEOSIDLOST,        // s->c, guild no longer owns geosid; geosid index, name of guild who took geosid
  NETMSG_GUILD_MESSAGE_GEOSIDUNDERATTACK, // s->c, geosid index, name of attacker

  NETMSG_GEOSID_DROPSACRIFICE,     // drop items to create geonite; geosid id, inventory index
  NETMSG_GEOSID_BURST,             // burst at a map location; x, z
  NETMSG_GEOSID_BURSTATTACKGEOSID, // burst on a geosid; geosid_id
  NETMSG_GEOSID_BURSTSPELL,        // burst-cast a spell; same data as cast request
  NETMSG_GEOSID_BURSTUSEGEOSID,    // burst within the radius of a geosid; geosid_id, target actor/x/z
  NETMSG_GEOSID_GETOWNER,          // obtains the name of the geosid's owning guild (wild only); geosid id
  NETMSG_GEOSID_TELLOWNERDATA,     // s->c, tells name of owning guild & geonite to burst; empty = not owned; geosid id, name, geonite
  NETMSG_GEOSID_CHANGEOWNER,       // s->c, tells new name of owner; empty = not owned, client should play geosid owner change event; geosid id, new owner name
  NETMSG_GEOSID_TELLGEONITE,       // s->c, updates the amount of geonite a geosid has
  NETMSG_GUILD_ABANDON,            // voluntarily release ownership of a geosid; geosid id

  NETMSG_PARTY_CHAT,
  NETMSG_PARTY_CHAT_DISTRIBUTE,
  NETMSG_PARTY_LEAD,
  NETMSG_PARTY_JOIN,
  NETMSG_PARTY_LEAVE,
  NETMSG_PARTY_KICK,
  NETMSG_PARTY_CLOSE,

  NETMSG_YOUHAVEBEENRESURRECTED, // s->c, informs player can /standup or /respawn
  NETMSG_RESURRECT_STANDUP, // player wants to get up here
  NETMSG_RESURRECT_RESPAWN, // player wants to respawn

  NETMSG_MARK,
  NETMSG_AUTOTARGET_ALL,
  NETMSG_AUTOTARGET_MONSTERS,

  NETMSG_GLOBAL_CHAT_DISTRIBUTE,

  // NETMSG_ZONE_GETOWNER,
  // NETMSG_ZONE_TELLOWNER,

  // do administrative work!
  NETMSG_ADMIN_SERVERMESSAGE,
  NETMSG_ADMIN_SQLQUERY,
  NETMSG_ADMIN_CREATEITEM,
  NETMSG_ADMIN_SIMPLECOMMAND, // invoke a command
  NETMSG_ADMIN_SETALIGNMENT,
  NETMSG_ADMIN_FIRSTMESSAGE = NETMSG_ADMIN_SERVERMESSAGE,
};


/**
* Various requests clients make of the server end in the server simply acknowledging that the
* request has been executed, or giving a status code of that request.  For example, logging into
* or out of an account, logging off of a character, creating an account, deleting a character,
* changing account password and more are all mutually exclusive and have similar replies.  Thus,
* it is most efficient to combine all of the server replies into a single message type with a
* result code.  This is the result code enumeration.
*/
enum RequestActionResult
{
  RAR_INVALID,
  RAR_SUCCEEDED,      // The requested action completed successfully
  RAR_FAILED_ACCOUNTDOESNTEXIST,  // The account doesn't exist
  RAR_FAILED_INCORRECTPASSWORD,   // The password for the account was incorrect
  RAR_FAILED_TOOMANYCHARACTERS,   // There are too many characters on the account to create a new one
  RAR_FAILED_NAMEALREADYEXISTS,   // The character's name has already been taken
  RAR_FAILED_SUSPICIOUS,    // The action wasn't completed, and has been flagged as suspicious activity (ex. delete character message for a character not on the account)
  RAR_FAILED_OUTOFACCOUNTTIME,  // No more time left on the account
  RAR_FAILED_ALREADYACTIVE,   // The account trying to be accessed is already active on the server
  RAR_FAILED_VERSIONOUTOFDATE,  // The client's version is outdated
};



/// These constants are used when encoding/decoding string values.  Any string in a message
/// sent to a peer is always prefixed by a 16-bit magic number and a 16-bit length.  These
/// values are concatenated and XORed with the constant STRING_MAGICLENGTHMASK.  When the
/// string is received, the value is XORed by the constant again to restore the original
/// values.  The magic number is checked; if it doesn't match, the message is discarded.  The
/// null-terminator is sent in all strings, as well.  The string routines check to make sure
/// that, at the specified number of characters into the buffer, the null-terminator is found.
static const unsigned long NETWORK_STRING_MAGICLENGTHMASK  = ((unsigned long)0xD156C79);
static const unsigned short NETWORK_STRING_MAGICNUMBER   = ((unsigned short)0x5346);

/// The number of bytes added to a transmitted string (in addition to the characters themselves)
static const size_t NETWORK_STRING_PADDING_BYTES  = (sizeof(unsigned long) + 1);


/// The number of bytes added to an encoded array
static const size_t NETWORK_ARRAY_PADDING_BYTES   = (sizeof(size_t));

/// The size of the network message structure that starts messages
//static const size_t NETWORK_MESSAGE_SIZE  = sizeof(Message);
static const size_t NETWORK_MESSAGE_SIZE  = 1;


/**
 * Writes the network message into the buffer
 */
char* encodeMessageType(Message message, char* buffer, const char* endOfBuffer);

/**
 * Reads the network message type from the packet.  This doesn't do anything to the packet.
 * If the packet is invalid or doesn't contain enough information to read the message, this
 * method returns false.
 */
bool decodeMessageType(ENetPacket* packet, Message* message);

/**
 * Reads the network message type from the provided buffer.  If anything is wrong with the source
 * buffer (it is null, the end is null, or is too short) then this method returns null.
 */
const char* decodeMessageType(const char* sourceBuffer, const char* endOfSourceBuffer, Message* message);

/**
 * Takes the indicated string and writes it into the given buffer, and returns a pointer to
 * the next unwritten byte in the buffer.  If the return value is null (0) then the buffer
 * is too small to contain the string.
 */
char* encodeString(const std::string& in, char* buffer, const char* endOfBuffer);

/**
 * Takes the indicated string and writes it into the given buffer, and returns a pointer to
 * the next unwritten byte in the buffer.  If the return value is null (0) then the buffer
 * is too small to contain the string.
 */
char* encodeString(const char* in, char* buffer, const char* endOfBuffer);

/**
 * Reads a string that was encoded with encodeString from the given buffer, and returns a
 * pointer to the next unread byte in the buffer.  If the source buffer was too small to read a
 * string or there was an error reading the string, the return value is null (0)
 */
const char* decodeString(const char* sourceBuffer, const char* endOfSourceBuffer, std::string* outString);

/**
 * Reads a string that was encoded with encodeString from the given buffer, and returns a
 * pointer to the next unread byte in the buffer.  If the source buffer was too small to read a
 * string or there was an error reading the string, the return value is null (0).  If the
 * destination buffer is too small to print the string that was read, as many characters as can
 * fit are read into the destination buffer.
 */
const char* decodeString(const char* sourceBuffer, const char* endOfSourceBuffer, char* outBuffer, size_t outBufferSize);


/**
 * Writes a structure's raw byte data to the given buffer
 */
template <typename T> char* encodeStructure(const T& structure, char* buffer, const char* endOfBuffer)
{
  if (!buffer || (buffer + sizeof(structure) > endOfBuffer)) return 0;
  memcpy_s(buffer, endOfBuffer - buffer, (const void*)&structure, sizeof(T));
  return buffer + sizeof(T);
}

// Casts a structure into a packed type, then writes it
template <typename T, typename E> char* encodePackedStructure(const T& structure, char*buffer, const char* endOfBuffer) {
  E packed_structure = (E)structure;
  if (!buffer || (buffer + sizeof(packed_structure) > endOfBuffer)) return 0;
  memcpy_s(buffer, endOfBuffer - buffer, (const void*)&structure, sizeof(packed_structure));
  return buffer + sizeof(packed_structure);
}

/**
 * Reads a structure's raw byte data from the given buffer
 */
template <typename T> const char* decodeStructure(const char* buffer, const char* endOfBuffer, T* out)
{
  if (!buffer || (buffer + sizeof(T) > endOfBuffer) || !out) return 0;
  memcpy_s(out, sizeof(T), buffer, sizeof(T));
  return buffer + sizeof(T);
}


template <typename T, typename E> const char* decodePackedStructure(const char* buffer, const char* endOfBuffer, T* out) {
  E packed_structure;
  if (!buffer || (buffer + sizeof(packed_structure) > endOfBuffer) || !out) return 0;
  memcpy_s(&packed_structure, sizeof(packed_structure), buffer, sizeof(packed_structure));
  *out = (T)packed_structure;
  return buffer + sizeof(packed_structure);
}


/**
 * Writes an array into the buffer
 */
template <typename E, typename T> char* encodeArray(E* arrayPointer, T arrayEntries, char* buffer, const char* endOfBuffer) {
  size_t array_bytes = arrayEntries * sizeof(E);
  if (!buffer || (buffer + sizeof(T) + array_bytes > endOfBuffer)) return 0;
  buffer = encodeStructure<T>(arrayEntries, buffer, endOfBuffer);
  if (arrayEntries) {
    memcpy_s(buffer, endOfBuffer - buffer, arrayPointer, array_bytes);
  }
  return buffer + array_bytes;
}

/**
 * Reads an array from the buffer
 */
template <typename E, typename T> const char* decodeArray(
    const char* buffer, const char* endOfBuffer,
    E* arrayBuffer, size_t arrayBufferEntries, T* arrayElements) {
  T entries;
  const char* internalBuffer = decodeStructure<T>(buffer, endOfBuffer, &entries);
  size_t array_bytes = entries * sizeof(E);
  if (!internalBuffer || (entries > arrayBufferEntries) ||
      (internalBuffer + array_bytes > endOfBuffer)) return 0;
  if (entries) {
    memcpy_s(arrayBuffer, arrayBufferEntries*sizeof(E), internalBuffer, array_bytes);
    internalBuffer += array_bytes;
  }
  *arrayElements = entries;
  return internalBuffer;
}

/**
 * Creates a packet with no extra data that holds only a message
 */
ENetPacket* encodeMessageOnlyPacket(Message message);

/**
 * Ensures that the packet in the parameter represents a message of the provided type
 * that was encoded by encodeMessageOnlyPacket
 */
bool decodeMessageOnlyPacket(ENetPacket* packet, Message messageToConfirm);


bool createPacket(bool reliable, Message message, size_t payload_size,
      ENetPacket** packet_out, char** start, const char** end);
bool validateAndTrimPacket(ENetPacket* packet, char* next_unwritten_byte);

bool openPacket(ENetPacket* packet, Message message, const char** payload, const char** end);



typedef EvidyonPacketSerializer<NETMSG_TELLCLIENTVERSIONINFO,
                                unsigned long, // gamefile version
                                unsigned long> // client version
                                NetMsg_TellClientVersionInfo;
typedef EvidyonPacketSerializer<NETMSG_CREATEACCOUNT,
                                Array<char, unsigned char>> // password text
                                NetMsg_CreateAccount;
typedef EvidyonPacketSerializer<NETMSG_ACCESSACCOUNT,
                                unsigned int,               // account ID number
                                Array<char, unsigned char>> // password
                                NetMsg_AccessAccount;
typedef EvidyonPacketSerializer<NETMSG_LOGOUTACCOUNT>
                                NetMsg_LogoutAcciont;
typedef EvidyonPacketSerializer<NETMSG_CREATECHARACTER,
                                Array<char, unsigned char>,
                                bool,           // male
                                unsigned char,  // race
                                unsigned char,  // class
                                unsigned char,  // str
                                unsigned char,  // agi
                                unsigned char,  // con
                                unsigned char,  // int
                                unsigned char>  // wis
                                NetMsg_CreateCharacter;
typedef EvidyonPacketSerializer<NETMSG_DELETECHARACTER,
                                unsigned int> // character ID #
                                NetMsg_DeleteCharacter;
typedef EvidyonPacketSerializer<NETMSG_ASKCHARACTERNAME,
                                unsigned int> // character ID
                                NetMsg_AskCharacterName;
typedef EvidyonPacketSerializer<NETMSG_ASKACCOUNTINFO>
                                NetMsg_AskAccountInfo;
typedef EvidyonPacketSerializer<NETMSG_ENTERWORLD,
                                unsigned int> // id
                                NetMsg_EnterWorld;
typedef EvidyonPacketSerializer<NETMSG_LEAVEWORLD>
                                NetMsg_LeaveWorld;
                                    

typedef EvidyonPacketSerializer<NETMSG_ENTERWORLD_SUCCEEDED,
                                bool,           // male?
                                unsigned char,  // race
                                unsigned char,  // class
                                unsigned char,  // hp %
                                unsigned char,  // mp %
                                unsigned char,  // exp %
                                Evidyon::CurrencyType,  // money
                                unsigned char,  // map index
                                unsigned long,  // time of day
                                Evidyon::Actor::ActorDescription,
                                Evidyon::Avatar::SpellAvailabilityMask>
                                NetMsg_EnterWorld_Succeeded;

struct NetMsg_TellYourAvatarStats_Data {
  Avatar::AvatarExperience current_xp, next_level_xp, penalty_xp;
  unsigned short max_hp, max_mp;
  short hp, mp;
  //bool max_hp_boosted, max_mp_boosted;
  unsigned short defense;
  unsigned char attribute_values[Avatar::NUMBER_OF_AVATAR_ATTRIBUTES];
  unsigned char available_attribute_points;
  unsigned char guild_max_hp, guild_max_mp, guild_xp;
  unsigned char race, class_, gender, alignment;
  unsigned char level;
  char hp_regen, mp_regen;
};

typedef EvidyonPacketSerializer<NETMSG_TELLYOURAVATARSTATS,
                                NetMsg_TellYourAvatarStats_Data>
                                NetMsg_TellYourAvatarStats;

typedef EvidyonPacketSerializer<NETMSG_TELLACTORDESCRIPTION,
                                Evidyon::Actor::ActorDescription>
                                NetMsg_TellActorDescription;
typedef EvidyonPacketSerializer<NETMSG_ASKACTORFULLDESCRIPTIONS,
                                Array<Evidyon::Actor::ActorID,
                                      unsigned char>>
                                NetMsg_AskActorFullDescriptions;
typedef EvidyonPacketSerializer<NETMSG_TELLACTORFULLDESCRIPTIONS,
                                Array<Evidyon::Actor::ActorDescription,
                                      unsigned char>>
                                NetMsg_TellActorFullDescriptions;
typedef EvidyonPacketSerializer<NETMSG_ASKACTORNAME,
                                Evidyon::Actor::ActorID>
                                NetMsg_AskActorName;
typedef EvidyonPacketSerializer<NETMSG_TELLACTORNAME,
                                Evidyon::Actor::ActorID,
                                Array<char, unsigned char>>
                                NetMsg_TellActorName;
typedef EvidyonPacketSerializer<NETMSG_FASTSYNC,
                                Evidyon::Actor::ActorFastSync>
                                NetMsg_FastSync;
typedef EvidyonPacketSerializer<NETMSG_SLOWSYNC,
                                float, // time since last sync
                                Array<Evidyon::Actor::ActorSlowSync,
                                      unsigned char>>
                                NetMsg_SlowSync;
typedef EvidyonPacketSerializer<NETMSG_ERASEACTORFROMWORLD,
                                Evidyon::Actor::ActorID>
                                NetMsg_EraseActorFromWorld;




//----[  NetMsg_StopAction  ]--------------------------------------------------
typedef EvidyonPacketSerializer<NETMSG_STOPACTION> NetMsg_StopAction;


//struct MD5Hash {
//  MD5Hash& operator = (const MD5Hash& rhs);
//  unsigned char data_[16];
//};
//typedef EvidyonPacketSerializer<NETMSG_ACCESSACCOUNT,unsigned int,MD5Hash> NetMsg_AccessAccount;


typedef EvidyonPacketSerializer<NETMSG_DROP_CURRENCY, CurrencyType> NetMsg_Drop_Currency;


typedef EvidyonPacketSerializer<NETMSG_ADMIN_SETALIGNMENT, int> NetMsg_Admin_SetAlignment;

typedef EvidyonPacketSerializer<NETMSG_REQUEST_SPEAK,
                                Array<char, unsigned char>> // text
                                NetMsg_Request_Speak;

typedef EvidyonPacketSerializer<NETMSG_ACTOR_SPEAKS,
                                Evidyon::Actor::ActorID,    // speaking actor
                                Array<char, unsigned char>> // text
                                NetMsg_Actor_Speaks;

typedef EvidyonPacketSerializer<NETMSG_ACTOR_SPEAKS_TO_ACTOR,
                                Evidyon::Actor::ActorID,           // speaking actor
                                Evidyon::Actor::ActorID,           // target of speech
                                Array<char, unsigned char>> // text.  can be very long.
                                NetMsg_Actor_Speaks_To_Actor;


typedef EvidyonPacketSerializer<NETMSG_TELLYOURBUFFICONS,
                                Evidyon::Magic::BuffMask>
                                NetMsg_TellYourBuffIcons;

//----[  Inventory  ]----------------------------------------------------------
typedef EvidyonPacketSerializer<NETMSG_SPLITSTACKEDITEM,unsigned char> NetMsg_SplitStackedItem;



//----[  Trade  ]--------------------------------------------------------------
typedef EvidyonPacketSerializer<NETMSG_TRADE_ACCEPT> NetMsg_Trade_Accept;
typedef EvidyonPacketSerializer<NETMSG_TRADE_WITHDRAW> NetMsg_Trade_Withdraw;
typedef EvidyonPacketSerializer<NETMSG_TRADE_CANCEL> NetMsg_Trade_Cancel;
typedef EvidyonPacketSerializer<NETMSG_TRADE_ADDITEM, unsigned char> NetMsg_Trade_AddItem;
typedef EvidyonPacketSerializer<NETMSG_TRADE_SETCURRENCY, CurrencyType> NetMsg_Trade_SetCurrency;
typedef EvidyonPacketSerializer<NETMSG_TRADE_REMOVEITEM, unsigned char> NetMsg_Trade_RemoveItem;
struct NetMsg_Trade_TellItemsInTrade {
  typedef Array<unsigned char, unsigned char> MyItemDescriptionArray;           // list of inventory indices
  typedef Array<TradeItemDescription, unsigned char> TheirItemDescriptionArray; // list of item descriptions
  typedef EvidyonPacketSerializer<NETMSG_TRADE_TELLITEMSINTRADE,
                                  Array<char, unsigned char>,   // name of trade partner
                                  CurrencyType,                 // my offered currency amount
                                  MyItemDescriptionArray,
                                  CurrencyType,                 // their offered currency amount
                                  TheirItemDescriptionArray> Serializer;
};


typedef EvidyonPacketSerializer<NETMSG_USE_ITEM,
                                Actor::ActorID, // target actor
                                unsigned char,  // inventory item index
                                unsigned short, // packed 11:5 x
                                unsigned short, // packed 11:5 z
                                unsigned char>  // parameter
                                NetMsg_UseItem;


typedef EvidyonPacketSerializer<NETMSG_CAST_SPELL,
                                Actor::ActorID, // target actor
                                unsigned short, // spell index
                                unsigned short, // packed 11:5 x
                                unsigned short, // packed 11:5 z
                                unsigned char>  // parameter
                                NetMsg_CastSpell;


//----[  Bazaar  ]-------------------------------------------------------------
typedef EvidyonPacketSerializer<NETMSG_BAZAAR_OPEN, Actor::ActorID> NetMsg_Bazaar_Open; // ID of bazaar merchant NPC
typedef EvidyonPacketSerializer<NETMSG_TELLCURRENCY,CurrencyType> NetMsg_TellCurrency;
typedef EvidyonPacketSerializer<NETMSG_BAZAAR_TELLITEMS, unsigned char, Array<ItemInBazaar,unsigned char>> NetMsg_Bazaar_TellItems; // 1: BazaarCategory
typedef EvidyonPacketSerializer<NETMSG_BAZAAR_CREATELISTING, unsigned char, CurrencyType> NetMsg_Bazaar_CreateListing; // 1: src inventory item, 2: price
typedef EvidyonPacketSerializer<NETMSG_BAZAAR_RETRACTLISTING, BazaarListingID> NetMsg_Bazaar_RetractListing;
typedef EvidyonPacketSerializer<NETMSG_BAZAAR_REDEEMLISTING, BazaarListingID> NetMsg_Bazaar_RedeemListing;
typedef EvidyonPacketSerializer<NETMSG_BAZAAR_PURCHASELISTING, BazaarListingID> NetMsg_Bazaar_PurchaseListing;
typedef EvidyonPacketSerializer<NETMSG_BAZAAR_GETLISTINGS, unsigned char> NetMsg_Bazaar_GetListings;  // 1: BazaarCategory
typedef EvidyonPacketSerializer<NETMSG_BAZAAR_PURCHASELISTING_RESULT, bool> NetMsg_Bazaar_PurchaseListing_Result;




//----[  Quests  ]-------------------------------------------------------------
typedef EvidyonPacketSerializer<NETMSG_QUEST_TRIGGER,
                                //unsigned int,
                                Evidyon::Actor::ActorID,
                                float,
                                float> NetMsg_Quest_Trigger;
typedef EvidyonPacketSerializer<NETMSG_QUEST_OFFER,
                                unsigned short,   // quest index (make unsigned int if > 65 thousand quests)
                                Evidyon::Actor::ActorID, // npc offering quest
                                char,             // npc-local quest index
                                unsigned int>     // validation key
                                NetMsg_Quest_Offer;
typedef EvidyonPacketSerializer<NETMSG_QUEST_OFFER_RESPONSE,
                                unsigned short,   // quest index (make unsigned int if > 65 thousand quests)
                                Evidyon::Actor::ActorID, // npc offering quest
                                char,             // npc-local quest index
                                unsigned int,     // validation key
                                bool>             // client accepts/declines quest
                                NetMsg_Quest_Offer_Response;
typedef EvidyonPacketSerializer<NETMSG_QUEST_TELLCURRENTQUESTS,
                                unsigned short, unsigned short, unsigned short, // current quest indices
                                unsigned short, unsigned short, unsigned short> // minutes left to complete quest
                                NetMsg_Quest_TellCurrentQuests;
typedef EvidyonPacketSerializer<NETMSG_QUEST_GIVEUP,
                                char>             // local quest index
                                NetMsg_Quest_GiveUp;




//----[  Events  ]-------------------------------------------------------------
// TODO: all of these need an "intensity" added to them!
typedef EvidyonPacketSerializer<NETMSG_EVENT_GENERATEBASICPROJECTILE,
                                Event::EventID, // unique ID number for this projectile
                                Evidyon::Packets::EventIndexPacketType, // event type
                                unsigned char,  // packed angle; 2pi radians -> 256
                                unsigned short, // packed 11:5 source x
                                unsigned short> // packed 11:5 source z
                                NetMsg_Event_GenerateBasicProjectile;
typedef EvidyonPacketSerializer<NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCTOTGT,
                                Event::EventID, // unique ID number for this event
                                Evidyon::Packets::EventIndexPacketType,
                                Actor::ActorID, // source
                                Actor::ActorID> // target
                                NetMsg_Event_GenerateTargetedSpecialFX_SrcToTgt;
typedef EvidyonPacketSerializer<NETMSG_EVENT_GENERATETARGETEDSPECIALFX_TGTXZ,
                                Event::EventID, // unique ID number for this event
                                Evidyon::Packets::EventIndexPacketType,
                                Actor::ActorID, // source actor
                                unsigned short, // packed 11:5 target x
                                unsigned short> // packed 11:5 target z
                                NetMsg_Event_GenerateTargetedSpecialFX_TgtXZ;
typedef EvidyonPacketSerializer<NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCXZTGTXZ,
                                Event::EventID, // unique ID number for this event
                                Evidyon::Packets::EventIndexPacketType,
                                unsigned short, // packed 11:5 source x
                                unsigned short, // packed 11:5 source z
                                unsigned short, // packed 11:5 target x
                                unsigned short> // packed 11:5 target z
                                NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZ;
typedef EvidyonPacketSerializer<NETMSG_EVENT_GENERATETARGETEDSPECIALFX_TGTXZAOE,
                                Event::EventID, // unique ID number for this event
                                Evidyon::Packets::EventIndexPacketType,
                                Actor::ActorID, // source actor
                                unsigned short, // packed 11:5 target x
                                unsigned short, // packed 11:5 target z
                                Array<Actor::ActorID, unsigned char>> // target actors
                                NetMsg_Event_GenerateTargetedSpecialFX_TgtXZAOE;
typedef EvidyonPacketSerializer<NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCXZTGTXZAOE,
                                Event::EventID, // unique ID number for this event
                                Evidyon::Packets::EventIndexPacketType,
                                unsigned short, // packed 11:5 source x
                                unsigned short, // packed 11:5 source z
                                unsigned short, // packed 11:5 target x
                                unsigned short, // packed 11:5 target z
                                Array<Actor::ActorID, unsigned char>> // target actors
                                NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZAOE;
typedef EvidyonPacketSerializer<NETMSG_EVENT_TERMINATE,
                                Event::EventID>
                                NetMsg_Event_Terminate;

//----[  Guild  ]--------------------------------------------------------------
typedef EvidyonPacketSerializer<NETMSG_GUILD_CREATE,
                                Array<char, unsigned char>> // name of guild
                                NetMsg_Guild_Create;
typedef EvidyonPacketSerializer<NETMSG_GUILD_LEAD>
                                NetMsg_Guild_Lead;
typedef EvidyonPacketSerializer<NETMSG_GUILD_JOIN>
                                NetMsg_Guild_Join;
typedef EvidyonPacketSerializer<NETMSG_GUILD_LEAVE>
                                NetMsg_Guild_Leave;
typedef EvidyonPacketSerializer<NETMSG_GUILD_PROMOTE,
                                Array<char, unsigned char>> // name of member
                                NetMsg_Guild_Promote;
typedef EvidyonPacketSerializer<NETMSG_GUILD_DEMOTE,
                                Array<char, unsigned char>> // name of member
                                NetMsg_Guild_Demote;
typedef EvidyonPacketSerializer<NETMSG_GUILD_CHAT,
                                Array<char, unsigned char>> // text
                                NetMsg_Guild_Chat;
typedef EvidyonPacketSerializer<NETMSG_GUILD_KICK,
                                Array<char, unsigned char>> // name of member
                                NetMsg_Guild_Kick;
typedef EvidyonPacketSerializer<NETMSG_GUILD_MEMBERS>
                                NetMsg_Guild_Members;
typedef EvidyonPacketSerializer<NETMSG_GUILD_ASKGEOSIDS>
                                NetMsg_GuildAskGeosids;
typedef EvidyonPacketSerializer<NETMSG_GUILD_CHAT_DISTRIBUTE,
                                Array<char, unsigned char>, // name
                                Array<char, unsigned char>> // chat
                                NetMsg_Guild_Chat_Distribute;
typedef EvidyonPacketSerializer<NETMSG_GUILD_MESSAGE,
                                Array<char, unsigned char>> // message text
                                NetMsg_Guild_Message;

typedef EvidyonPacketSerializer<NETMSG_GUILD_TELLFOUNDERACTORNAME,
                                Actor::ActorID,
                                Array<char, unsigned char>, // avatar name
                                Array<char, unsigned char>> // guild name
                                NetMsg_Guild_TellFounderActorName;
typedef EvidyonPacketSerializer<NETMSG_GUILD_TELLLEADERACTORNAME,
                                Actor::ActorID,
                                Array<char, unsigned char>, // avatar name
                                Array<char, unsigned char>> // guild name
                                NetMsg_Guild_TellLeaderActorName;
typedef EvidyonPacketSerializer<NETMSG_GUILD_TELLMEMBERACTORNAME,
                                Actor::ActorID,
                                Array<char, unsigned char>, // avatar name
                                Array<char, unsigned char>> // guild name
                                NetMsg_Guild_TellMemberActorName;

typedef EvidyonPacketSerializer<NETMSG_GUILD_ASKGEOSIDS>
                                NetMsg_Guild_AskGeosids;
typedef EvidyonPacketSerializer<NETMSG_GUILD_SETHOME>
                                NetMsg_Guild_SetHome;  // sets the point at which guild members show up after g/rally (leaders only)
typedef EvidyonPacketSerializer<NETMSG_GUILD_HOME>  // teleports the player to the guild's home geosid after 3 seconds
                                NetMsg_Guild_Home;

typedef EvidyonPacketSerializer<NETMSG_GUILD_MESSAGE_TELLGEOSIDS,
                                Array<Geosid::GeosidIndex, unsigned char>> // list of geosids
                                NetMsg_Guild_Message_TellGeosids;
typedef EvidyonPacketSerializer<NETMSG_GUILD_MESSAGE_TELLATTACK,
                                Geosid::GeosidIndex,
                                bool,                         // is this (false) a steal-from or (true) a become-owners attack
                                Array<char, unsigned char>>   // name of attacking guild member
                                NetMsg_Guild_Message_TellAttack;
typedef EvidyonPacketSerializer<NETMSG_GUILD_MESSAGE_GEOSIDGAINED,
                                Geosid::GeosidIndex>
                                NetMsg_Guild_Message_GeosidGained;
typedef EvidyonPacketSerializer<NETMSG_GUILD_MESSAGE_GEOSIDLOST,
                                Geosid::GeosidIndex>
                                NetMsg_Guild_Message_GeosidLost;
typedef EvidyonPacketSerializer<NETMSG_GUILD_MESSAGE_GEOSIDUNDERATTACK,
                                Geosid::GeosidIndex,
                                Array<char, unsigned char>>   // name of the attacking guild
                                NetMsg_Guild_Message_GeosidUnderAttack;

typedef EvidyonPacketSerializer<NETMSG_GEOSID_DROPSACRIFICE,
                                Evidyon::Geosid::GeosidIndex, // target geosid
                                unsigned char>                // inventory index
                                NetMsg_Geosid_DropSacrifice;
typedef EvidyonPacketSerializer<NETMSG_GEOSID_BURST,
                                float,  // x
                                float>  // z
                                NetMsg_Geosid_Burst;
typedef EvidyonPacketSerializer<NETMSG_GEOSID_BURSTATTACKGEOSID,
                                Geosid::GeosidIndex>
                                NetMsg_Geosid_BurstAttackGeosid;
typedef EvidyonPacketSerializer<NETMSG_GEOSID_BURSTSPELL,
                                Spell::SpellIndex,
                                Actor::ActorID,
                                float,  // x
                                float>  // z
                                NetMsg_Geosid_BurstSpell;
typedef EvidyonPacketSerializer<NETMSG_GEOSID_BURSTUSEGEOSID,
                                Geosid::GeosidIndex,
                                Actor::ActorID,
                                float, // x
                                float> // z
                                NetMsg_Geosid_BurstUseGeosid;
typedef EvidyonPacketSerializer<NETMSG_GEOSID_GETOWNER,
                                Geosid::GeosidIndex>
                                NetMsg_Geosid_GetOwner;
typedef EvidyonPacketSerializer<NETMSG_GEOSID_TELLOWNERDATA,
                                Geosid::GeosidIndex,
                                Array<char, unsigned char>,
                                Geosid::Geonite>
                                NetMsg_Geosid_TellOwnerData;
typedef EvidyonPacketSerializer<NETMSG_GEOSID_CHANGEOWNER,
                                Geosid::GeosidIndex,
                                Array<char, unsigned char>,
                                Geosid::Geonite>
                                NetMsg_Geosid_ChangeOwner;
typedef EvidyonPacketSerializer<NETMSG_GEOSID_TELLGEONITE,
                                Geosid::GeosidIndex,
                                Geosid::Geonite>
                                NetMsg_Geosid_TellGeonite;
typedef EvidyonPacketSerializer<NETMSG_GUILD_ABANDON,
                                Geosid::GeosidIndex>
                                NetMsg_Guild_Abandon;


typedef EvidyonPacketSerializer<NETMSG_PARTY_CHAT,
                                Array<char, unsigned char>> // chat text
                                NetMsg_Party_Chat;
typedef EvidyonPacketSerializer<NETMSG_PARTY_CHAT_DISTRIBUTE,
                                Array<char, unsigned char>, // name
                                Array<char, unsigned char>> // chat
                                NetMsg_Party_Chat_Distribute;
typedef EvidyonPacketSerializer<NETMSG_PARTY_LEAD>
                                NetMsg_Party_Lead;
typedef EvidyonPacketSerializer<NETMSG_PARTY_JOIN>
                                NetMsg_Party_Join;
typedef EvidyonPacketSerializer<NETMSG_PARTY_LEAVE>
                                NetMsg_Party_Leave;
typedef EvidyonPacketSerializer<NETMSG_PARTY_KICK,
                                Array<char, unsigned char>> // name of client to kick
                                NetMsg_Party_Kick;
typedef EvidyonPacketSerializer<NETMSG_PARTY_CLOSE>
                                NetMsg_Party_Close;

typedef EvidyonPacketSerializer<NETMSG_YOUHAVEBEENRESURRECTED>
                                NetMsg_YouHaveBeenResurrected;
typedef EvidyonPacketSerializer<NETMSG_RESURRECT_STANDUP>
                                NetMsg_Resurrect_StandUp;
typedef EvidyonPacketSerializer<NETMSG_RESURRECT_RESPAWN>
                                NetMsg_Resurrect_Respawn;


typedef EvidyonPacketSerializer<NETMSG_MARK, unsigned char>
                                NetMsg_Mark;
typedef EvidyonPacketSerializer<NETMSG_AUTOTARGET_ALL>
                                NetMsg_AutoTarget_All;
typedef EvidyonPacketSerializer<NETMSG_AUTOTARGET_MONSTERS>
                                NetMsg_AutoTarget_Monsters;


typedef EvidyonPacketSerializer<NETMSG_GLOBAL_CHAT_DISTRIBUTE,
                                Array<char, unsigned char>, // name
                                Array<char, unsigned char>> // chat
                                NetMsg_Global_Chat_Distribute;

}
}
}

#endif