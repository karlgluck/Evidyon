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
#include "client.h"
#include "globalnetworkmanager.h"
#include "../common/enet/enet.h"
#include "../common/null.h"
#include "globaldatabasemanager.h"
#include "actor.h"
#include <dc/debug>
#include "item.h"
#include "shared/client-server/packets.h"
#include "encodepackets.h"
#include "decodepackets.h"
#include "shared/evidyon_version.h"
#include "shared/evidyon_accounts.h"
#include "globalgamefilemanager.h"
#include "globalactormanager.h"
#include "shared/server-editor/server_gamefile.h"
#include "globalitemmanager.h"
#include "globalworldmanager.h"
#include "globalclientmanager.h"
#include "server/guild/globalguildmanager.h"
#include "server/guild/guildinstance.h"
#include "time.h"
#include "shared/client-server/fastsync.h"
#include "shared/evidyon_network.h"
#include "common/randint32.h"
#include "shared/actor/actordescription.h"
#include "shared/spell/spellindex.h"
#include "server/geosid/globalgeosidmanager.h"
#include "actorgroup.h"
#include "server/globallogger.h"

// used to access Evidyon::running so that the server can be disabled remotely
#include "main.h"



// atan2f, sqrtf
#include <math.h>
#include <assert.h>
#include "uniqueavatarnamechecker.h"


#include "shared/server-editor/lifeformaidescription.h"
#include "shared/item/itemserverdescription.h"

#include "shared/actor/actordescription.h"

#include "common/fracture/todo.h"



namespace Evidyon {

static const float MAX_NPC_INTERACTION_DISTANCE_SQ = 2.0f; // this should be larger than the client's interaction distance




//----[  NetMsg_TellClientVersionInfo  ]---------------------------------------
void Client::NetMsg_TellClientVersionInfo(NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  unsigned long gamefileVersion, clientVersion;
  CONFIRM(Decode::tellClientVersionInfo(packet, &gamefileVersion, &clientVersion)) else return;

  // Check the versions
  bool needsNewClient = !VERSIONS_ARE_COMPATIBLE(clientVersion, VERSION);
  bool needsNewGamefile = false;
  todo("karl","gamefile version");
  bool versionsAreCompatible = !(needsNewClient || needsNewGamefile);

  // Tell the clients on the server
  char updateText[128];
  sprintf_s(updateText, sizeof(updateText), "Evidyon Server v%s\n%  lu players online", VERSION_TEXT, active_accounts_.size());

  // Reply with the client's version compatibility
  *reply_packet = Encode::tellServerVersionUpdateInfo(versionsAreCompatible, needsNewGamefile, needsNewClient, updateText);

  // If the versions are compatible, move into the correct state
  if (versionsAreCompatible)
    state_ = CONNECTED_VERSIONCONFIRMED;
}





//----[  NetMsg_AccessAccount  ]-----------------------------------------------
void Client::NetMsg_AccessAccount(NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  todo("karl", "version-confirmed needs to be less of a hack");
  if (state_ != CONNECTED_VERSIONCONFIRMED) {
    *reply_packet = Encode::requestActionResult(RAR_FAILED_VERSIONOUTOFDATE);
    return;
  }

  // Get account info
  unsigned int account_id;
  char passwordBuffer[64]; // enough space to hold md5--should only be 32 chars, but hey...why not
  CONFIRM(Decode::accessAccount(packet, &account_id, passwordBuffer, sizeof(passwordBuffer))) else return;

  // Always set the last character in the buffer to \0, just to be safe
  passwordBuffer[64-1] = '\0';

  // This result variable is set when we've determined what happened
  RequestActionResult rar = RAR_INVALID;

  // Try to access the account
  GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
  switch(database->accessAccount(account_id, passwordBuffer))
  {
    case GlobalDatabaseManager::DBR_SUCCESS:       rar = RAR_SUCCEEDED; break;
    case GlobalDatabaseManager::DBR_INCORRECTPASSWORD: rar = RAR_FAILED_INCORRECTPASSWORD; break;
    case GlobalDatabaseManager::DBR_DOESNTEXIST:     rar = RAR_FAILED_ACCOUNTDOESNTEXIST; break;
    default:
      rar = RAR_INVALID;
      break;
  }

  // If the player's login succeeded, check to make sure the account isn't active already
  if ((rar == RAR_SUCCEEDED) && (active_accounts_.end() != active_accounts_.find(account_id))) {
    // Make sure the account is ACTUALLY active and not just in the set by mistake
    GlobalClientManager* client_manager = GlobalClientManager::singleton();
    if (client_manager->accountIsActive(account_id)) {
      rar = RAR_FAILED_ALREADYACTIVE;
    } else {
      active_accounts_.erase(account_id);
    }
  }

  // Send a reply as long as the request was valid (if it isn't valid, the peer will be disconnected)
  if (rar != RAR_INVALID)
    sendPacket(Encode::requestActionResult(rar));

  // If the login succeeded, set up this account
  if (rar == RAR_SUCCEEDED)
  {
    // Add to the list of active accounts
    active_accounts_.insert(account_id);

    // This connection is now logged into the account
    state_ = LOGGED_INTO_ACCOUNT;
    account_id_ = account_id;
    log_account_login(account_id);
  } else {
    log_account_login_failed(account_id, passwordBuffer);
  }
}





//----[  NetMsg_CreateAccount  ]-----------------------------------------------
void Client::NetMsg_CreateAccount(NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  todo("karl","make the version-confirmed login process less of a hack");
  if (state_ != CONNECTED_VERSIONCONFIRMED) return;

  // Get the password
  char passwordBuffer[64]; // password = md5 hash
  if (!APP_WARNING(!Decode::createAccount(packet, passwordBuffer, sizeof(passwordBuffer)))
    ("Unable to decode createAccount message"))
  {
    // Make the buffer safe
    passwordBuffer[64-1] = '\0';

    // Pull the account ID from the database
    unsigned int id;
    GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
    CONFIRM(GlobalDatabaseManager::DBR_SUCCESS == database->createAccount(passwordBuffer, &id)) else {
    return;
    }

    // The account was created
    DEBUG_INFO("Created new account %lu with password '%s'", id, passwordBuffer);

    // Reply with the ID
    *reply_packet = Encode::createAccountSucceeded(id);
  }
}




//-----------------------------------------------------------------------------
void Client::NetMsg_CreateCharacter(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  // This result variable is set when we've determined what happened
  char nameBuffer[Avatar::MAX_AVATAR_NAME_LENGTH];
  bool male;
  int chRace, chClass;
  int chAttributes[5];
  GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
  if (!APP_WARNING(!Decode::createCharacter(packet,
                                            nameBuffer,
                                            sizeof(nameBuffer), 
                                           &male,
                                           &chRace,
                                           &chClass,
                                           &chAttributes[0],
                                           &chAttributes[1],
                                           &chAttributes[2],
                                           &chAttributes[3],
                                           &chAttributes[4]))
    ("Unable to decode createCharacter message") &&
    !APP_WARNING(!gamefile_manager->isValidLevel1Character(
    Avatar::AvatarRace(chRace), Avatar::AvatarClass(chClass), chAttributes))("Client attempted to create invalid character"))
  {
    // Make the name buffer safe
    nameBuffer[Avatar::MAX_AVATAR_NAME_LENGTH-1] = '\0';

    // Create a new character in the database
    unsigned int character_id;
    World::MapIndex map;
    float x, y;
    gamefile_manager->getPlayerRespawnLocation(false, &map, &x, &y);
    switch(db->createCharacter(account_id_,
                               nameBuffer,
                              &character_id,
                               x,
                               y,
                               map,
                               male,
                               chRace,
                               chClass,
                               1,
                               chAttributes))
    {
      case GlobalDatabaseManager::DBR_SUCCESS:     {
        log_create_avatar(
          account_id_,
          character_id,
          nameBuffer,
          Avatar::AvatarRace(chRace),
          Avatar::AvatarClass(chClass),
          Avatar::AvatarGender(male ? Avatar::AVATARGENDER_MALE : Avatar::AVATARGENDER_FEMALE));
        *reply_packet = Encode::createCharacterSucceeded(character_id);
      } break;
      case GlobalDatabaseManager::DBR_OVERLIMIT:   *reply_packet = Encode::requestActionResult(RAR_FAILED_TOOMANYCHARACTERS); break;
      case GlobalDatabaseManager::DBR_ALREADYEXISTS: *reply_packet = Encode::requestActionResult(RAR_FAILED_NAMEALREADYEXISTS); break;
    }
  }
}



//-----------------------------------------------------------------------------
void Client::NetMsg_DeleteCharacter(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  // This result variable is set when we've determined what happened
  RequestActionResult rar = RAR_INVALID;

  // These variables contain data decoded from the message
  char passwordBuffer[MAX_PASSWORD_LENGTH];
  unsigned character_id;

  // Parse fields out of the packet
  if (!APP_WARNING(!Decode::deleteCharacter(packet, &character_id, passwordBuffer, sizeof(passwordBuffer)))
    ("Unable to decode createCharacter message"))
  {
    // Make the password buffer safe
    passwordBuffer[MAX_PASSWORD_LENGTH-1] = '\0';

    // Try to access the account
    //if (GlobalDatabaseManager::DBR_SUCCESS != db->accessAccount(account_id_, passwordBuffer))
    //{
    //  // Failure
    //  rar = RAR_FAILED_INCORRECTPASSWORD;
    //}
    //else
    DEBUG_INFO("Character deletion doesn't require password.");
    {
      // Use the db to remove this character
      switch(db->removeCharacterFromAccount(account_id_, character_id))
      {
        case GlobalDatabaseManager::DBR_SUCCESS:
          log_delete_character(account_id_, character_id);
          rar = RAR_SUCCEEDED;
          break;
        case GlobalDatabaseManager::DBR_DOESNTEXIST: rar = RAR_FAILED_SUSPICIOUS; break;
        //default:
        //  disconnect();
        //  rar = RAR_INVALID;
        //  break;
      }
    }
  }

  // Send a reply as long as the request was valid (if it isn't valid, the peer will be disconnected)
  if (rar != RAR_INVALID)
    *reply_packet = Encode::requestActionResult(rar);

}



//-----------------------------------------------------------------------------
void Client::NetMsg_AskAccountInfo(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  if (!APP_WARNING(!Decode::askAccountInfo(packet))
      ("Unable to decode askAccountInfo message"))
  {
    // This information will be passed back in the reply message
    float daysLeftToPlay;
    unsigned int characterIDArray[MAX_AVATARS_PER_ACCOUNT];
    size_t numberOfCharacters;

    // Get data from the database
    GlobalDatabaseManager::DBResult results[] = {
      db->getCharactersOnAccount(account_id_, characterIDArray, &numberOfCharacters),
      db->getAccountTimeLeft(account_id_, &daysLeftToPlay)
    };

    // Make sure both results succeeded
    if (results[0] == GlobalDatabaseManager::DBR_SUCCESS &&
      results[1] == GlobalDatabaseManager::DBR_SUCCESS)
    {
      // Bounds-check the days left to play that is shown to the user
      if (daysLeftToPlay < 0.0f) daysLeftToPlay = 0.0f;

      // Reply with the account information
      *reply_packet = Encode::tellAccountInfo(daysLeftToPlay, characterIDArray, numberOfCharacters);
    }

  }
}





//----[  NetMsg_AskCharacterName  ]--------------------------------------------
void Client::NetMsg_AskCharacterName(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  unsigned int character_id;
  if (!APP_WARNING(!Decode::askCharacterName(packet, &character_id))
      ("Unable to decode askCharacterName message"))
  {
    // Get data from database
    char nameBuffer[Avatar::MAX_AVATAR_NAME_LENGTH+1];
    unsigned int actor_type;
    size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT];
    float world_x, world_y;
    int map_index;
    if (GlobalDatabaseManager::DBR_SUCCESS ==
      //db->getCharacterName(account_id_, character_id, nameBuffer, Avatar::MAX_AVATAR_NAME_LENGTH+1))
      db->getCharacterDesc(account_id_,
                                 character_id,
                                 nameBuffer,
                                 sizeof(nameBuffer),
                                &actor_type,
                                 visible_equipment,
                                &map_index,
                                &world_x,
                                &world_y))
    {
      *reply_packet = Encode::tellCharacterName(character_id, nameBuffer, map_index, world_x, world_y, actor_type, visible_equipment);
    }
    else
    {
      DEBUG_WARNING("Client requested the name of a character (%lu) that doesn't exist in the database or isn't on account", character_id);
      *reply_packet = Encode::requestActionResult(RAR_FAILED_SUSPICIOUS);
    }
  }
}





//----[  NetMsg_LeaveWorld  ]--------------------------------------------------
void Client::NetMsg_LeaveWorld(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  actor->say("<Leaving World in 3 Seconds>");
  request_.changeTo(Evidyon::Server::PlayerRequest::LEAVE_WORLD);
}
//----[  NetMsg_DropInventoryItems  ]------------------------------------------
void Client::NetMsg_DropInventoryItems(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (player_.isDead() || player_.inTrade()) return;

  int x, y;
  WorldRegion* region = NULL;
  actor->getStrictRegion(&x, &y, &region);
  CONFIRM(region) else return;

  unsigned char indices[AVATAR_INVENTORY_SIZE];
  size_t number_of_indices;
  CONFIRM(Decode::dropInventoryItems(packet,
                                     indices,
                                     AVATAR_INVENTORY_SIZE,
                                    &number_of_indices)) else return;
  ASSERT(number_of_indices == 1);

  items_.dropItemOnGround(region, x, y, indices[0]);
  sendFullInventoryUpdate();
}




//----[  NetMsg_EnterWorld  ]--------------------------------------------------
void Client::NetMsg_EnterWorld(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;


  unsigned int character_id;
  CONFIRM(Decode::enterWorld(packet, &character_id)) else return;
/*
  // Be sure that the account has time left
  float daysLeftToPlay;
  if (GlobalDatabaseManager::DBR_SUCCESS != db->getAccountTimeLeft(account_id_, &daysLeftToPlay) ||
    daysLeftToPlay < 0.0f)
  {
    *reply_packet = Encode::requestActionResult(RAR_FAILED_OUTOFACCOUNTTIME);

    // Exit this case
    break;
  }*/



  // Get the character's information
  float x, y; // The character's location in the world
  World::MapIndex map_index;
  int hp, mp;
  GlobalDatabaseManager::DBResult result =
      db->getCharacterData(account_id_,
                           character_id,
                           &x,
                           &y,
                           &avatar_,
                           &hp,
                           &mp,
                           &is_admin_,
                           &map_index,
                           portal_marks_,
                           &hide_helmet_);
  if (APP_WARNING(GlobalDatabaseManager::DBR_SUCCESS != result)
        ("Couldn't log %lu into character %lu", account_id_, character_id))
  {
    *reply_packet = Encode::requestActionResult(RAR_FAILED_SUSPICIOUS);
    return;
  }

  character_id_ = character_id;

  if (isAdmin()) {
    avatar_.attribute_values[0] = 150;
    avatar_.attribute_values[1] = 150;
    avatar_.attribute_values[2] = 150;
    avatar_.attribute_values[3] = 150;
    avatar_.attribute_values[4] = 150;
    avatar_.level = 150;
  }

  avatar_.calculateBaseStats();
  avatar_stats_.hp = hp;
  avatar_stats_.mp = mp;
  update_actor_description_ = true;
  update_enchantment_icons_ = true;
  update_visible_enchantments_ = true;

  { // update this avatar's alignment based on how long
    // it's been since their last pk
    unsigned int hours_since_last_pk = hoursSinceLastPK();
    if (hours_since_last_pk > 20) {
      unsigned int kill_count_at_last_pk =
        (hours_since_last_pk >= 24) ? 0 : avatar_.kill_count_at_last_pk;

      // if the player killed, dont give any points
      if (kill_count_at_last_pk == 0) {
        // give 4 alignment points per day
        int alignment_points_to_gain = 2 * hours_since_last_pk / 24;
        if (alignment_points_to_gain < 4) {
          alignment_points_to_gain = 4;   // min 4
        } else if (alignment_points_to_gain > 30) {
          alignment_points_to_gain = 30;  // max 30
        }

        // Add alignment
        int new_alignment = avatar_.alignment_value + alignment_points_to_gain;
        if (new_alignment >= 100 && avatar_.alignment_value < 100) {
          // you cannot become a Guardian in the normal way
          new_alignment = 99;
        } else {
          // Guardians cannot get more than 100 alignment
          new_alignment = min(new_alignment, 100);
        }
        setAlignmentValue(new_alignment);
      }

      // reset the alignment counter and  the pk hour
      avatar_.kill_count_at_last_pk = 0;
      avatar_.last_pk_hour = Evidyon::Time::hour_counter;
    }
  }


  CONFIRM(items_.checkOut(account_id_,
                          character_id_));

  GlobalWorldManager* world_manager = GlobalWorldManager::singleton();
  Map* map = world_manager->getMapFromIndex(map_index);
  if (!map) {
    GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
    Evidyon::World::MapIndex map_index;
    gamefile_manager->getPlayerRespawnLocation(isEvil(), &map_index, &x, &y);
    map = world_manager->getMapFromIndex(map_index);
    assert(map);
    //map->getRandomChaosLocation(NULL, &respawn_x, &respawn_z);
  }

  Evidyon::Actor::ActorTemplateIndex actor_template;
  GlobalGamefileManager* gamefile_manager =
      GlobalGamefileManager::singleton();
  gamefile_manager->getNudeAvatarActorData(
    avatar_.race,
    avatar_.gender,
    &actor_template,
    &avatar_.actor_profile,
    &avatar_.skinned_mesh_form);
  avatar_.spell_availability.copy(
    gamefile_manager->getClassSpellAvailability(
    avatar_.class_,
    avatar_.level));
  if (isAdmin()) {
    avatar_stats_.mp = avatar_.max_mp;
    avatar_stats_.hp = avatar_.max_hp;
    for (int i = 0; i < Avatar::NUMBER_OF_AVATAR_CLASSES; ++i) {
      avatar_.spell_availability.unionWith(
          gamefile_manager->getClassSpellAvailability(
            Avatar::AvatarClass(i),
            Evidyon::Avatar::HIGHEST_AVATAR_LEVEL));
    }
  }

  GlobalActorManager* actor_manager = GlobalActorManager::singleton();
  CONFIRM(actor_manager->acquireActor(&actor_,
                                      this,
                                      actor_template,
                                      map,
                                      x,
                                      y)) else {
    *reply_packet = Encode::requestActionResult(RAR_FAILED_SUSPICIOUS);
    return;
  }

  ActorInstance* actor = actor_.dereferenceAssumingValid();
  actor->setLevel(Number(int(avatar_.level)));
  actor->setMemberOfFaction(ActorInstance::CHAOTIC);
  setAlignmentValue(avatar_.alignment_value); // this will update the actor's faction
  actor->setSize(0.5f);
  todo("karl", "actor size");

  // Add this actor to the world
  actor->enterWorld();
  WorldRegion* local_region = actor->getRegion();
  assert(local_region);

  // Set up the player-processor and calculate all of the current avatar stats.  This
  // method will also reequip items the player has on.
  player_.acquire(this,
                  &actor_,
                  &request_,
                  &avatar_,
                  &avatar_enchantments_,
                  &avatar_stats_,
                  &items_,
                  &item_trader_);

  // Update all nearby clients (not this one, though!) with the actor's description.
  broadcastUpdateDescriptionPacket();

  // Enter the guild before entering the world so that guild effects can
  // be properly applied to the avatar's stats.
  if (avatar_.guild != Evidyon::Server::INVALID_GUILD_ID) {
    // join up with this guild
    Evidyon::Server::GlobalGuildManager* guild_manager
      = Evidyon::Server::GlobalGuildManager::singleton();
    guild_ = guild_manager->guildMemberLoggedOn(avatar_.guild,
                                                character_id,
                                                this);
    if (!guild_) {
      avatar_.guild = Evidyon::Server::INVALID_GUILD_ID;
    }
  } else {
    guild_ = NULL;
  }

  // Apply the effects of guild membership, if appropriate
  updateGeosidEffects();

  {
    unsigned long time_of_day = GetTickCount();

    Evidyon::Actor::ActorDescription actor_description;
    actor->fillDescription(&actor_description);

    using namespace Evidyon::Network::Packet;
    if (avatar_.experience_to_next_level == 0) {
      // prevent divide-by-0
      avatar_.experience_to_next_level = 1;
    }
    NetworkPacket* packet
      = NetMsg_EnterWorld_Succeeded::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        bool(avatar_.gender == Avatar::AVATARGENDER_MALE),
        avatar_.race,
        avatar_.class_,
        unsigned char(256.0f * (avatar_stats_.hp / avatar_stats_.max_hp).toFloat()),
        unsigned char(256.0f * (avatar_stats_.mp / avatar_stats_.max_mp).toFloat()),
        unsigned char(256.0f * (avatar_.experience_this_level / avatar_.experience_to_next_level)),
        avatar_.money,
        map->getIndex(),
        time_of_day,
        &actor_description,
        &avatar_.spell_availability);
    sendPacket(packet);

    // Put this client into the world and send updates about the world state
    WorldRegion* regions_entered[9];
    int number_of_regions_entered = 0;
    local_region->enterWorld(this,
                             regions_entered,
                            &number_of_regions_entered);
    local_region->tellAllNearbyClientFullDescriptions(this);

    // tell the items in the new regions
    for (int i = 0; i < number_of_regions_entered; ++i) {
      regions_entered[i]->sendLocalItemsOnGroundToClient(this);
    }
  }


  { // Read the client's quests
    loadQuestsFromDatabase(db);
    syncClientQuests();
  }


  // Enter this character into the world
  state_ = LOGGED_INTO_CHARACTER;
  character_id_ = character_id;

  {
    sendFullInventoryUpdate();
    updateClientAvatarStats();
  }

  GlobalClientManager::singleton()->enteredWorld(this);
}





//----[  NetMsg_LogoutAccount  ]-----------------------------------------------
void Client::NetMsg_LogoutAccount(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  if (!APP_WARNING(!Decode::logoutAccount(packet))
    ("Unable to decode logoutAccount message"))
  {
    logoutAccount();

    // Send the reply to the client
    *reply_packet = Encode::logoutAccountConfirmed();
  }
}






//----[  NetMsg_AskActorFullDescriptions  ]------------------------------------
void Client::NetMsg_AskActorFullDescriptions(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  WorldRegion* local_region = actor->getRegion();
  if (local_region) {
    static const int BUFFER_SIZE = 512 / sizeof(Evidyon::Actor::ActorID);
    Evidyon::Actor::ActorID actor_ids[BUFFER_SIZE];
    size_t number_of_actors;
    bool result = NetMsg_AskActorFullDescriptions::readPacket(packet,
      &CopiedArray<Evidyon::Actor::ActorID>(actor_ids, BUFFER_SIZE, &number_of_actors));
    assert(result);
    local_region->tellNearbyActorFullDescriptions(actor_ids, number_of_actors, this);
  }
}





//----[  NetMsg_RequestMoveStreaming  ]----------------------------------------
void Client::NetMsg_RequestMoveStreaming(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (player_.isDead()) return;

  float x, z;
  CONFIRM(Decode::requestMoveStreaming(packet, &x, &z)) else return;
  if (actor->locationIsOutOfRange(x, z)) {
    todo("karl","red flag - requested streaming move is out of range");
    return;
  }

  if (request_.changeTo(Evidyon::Server::PlayerRequest::MOVE)) {
    request_.move.x.set(Number(x));
    request_.move.z.set(Number(z));
    request_.move.streaming = true;
  }
}





//----[  NetMsg_RequestMove  ]-------------------------------------------------
void Client::NetMsg_RequestMove(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (player_.isDead()) return;

  float x, z;
  Actor::ActorID actor_id;
  CONFIRM(Decode::requestMove(packet,
                              &x,
                              &z,
                              &actor_id)) else return;
  if (actor->locationIsOutOfRange(x, z)) {
    todo("karl", "red flag - requested move location is out of range");
    return;
  }

  if (actor_id == Actor::INVALID_ACTOR_ID) {
    if (request_.changeTo(Evidyon::Server::PlayerRequest::MOVE)) {
      request_.move.streaming = false;
      request_.move.x.set(Number(x));
      request_.move.z.set(Number(z));
    }
  } else {
    if (request_.changeTo(Evidyon::Server::PlayerRequest::ATTACK)) {
      request_.attack.actor_id = actor_id;
    }
  }


}





//----[  NetMsg_CastSpell  ]---------------------------------------------------
void Client::NetMsg_CastSpell(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (player_.isDead()) return;

  //unsigned int spell_ids[3];
  /*float target_x, target_z;
  Evidyon::Actor::ActorID target_actor;
  *///CONFIRM(Decode::castSpell(packet,
  //              spell_ids,
  //             &target_actor,
  //             &target_x,
  //             &target_z)) else return;
  
  unsigned int packed_x, packed_z;
  Evidyon::Actor::ActorID target_actor;
  unsigned int parameter;
  Evidyon::Spell::SpellIndex spell_index;
  if (!NetMsg_CastSpell::readPacket(
        packet,
        &target_actor,
        &spell_index,
        &packed_x,
        &packed_z,
        &parameter)) {
    return;
  }

  float target_x = packed_x / 32.0f;
  float target_z = packed_z / 32.0f;


  if (actor->locationIsOutOfRange(target_x, target_z)) {
    todo("karl", "red flag - requested spell target out of range");
    return;
  }

  // Make sure the user can use the spells in this combo
  if (spell_index>= Evidyon::Avatar::MAX_SPELL_TYPES) return;
  bool can_cast = avatar_.spell_availability.isBitSet(spell_index);
  if (!can_cast) return;

  //Evidyon::Spell::SpellIndex spell_index = spell_ids[0];
  const Evidyon::SpellDescription* spell
    = GlobalGamefileManager::singleton()->getSpell(spell_index);

  if (spell && request_.changeTo(Evidyon::Server::PlayerRequest::CAST)) {
    request_.cast.spell = spell;
    request_.cast.x.set(Number(target_x));
    request_.cast.z.set(Number(target_z));
    request_.cast.actor_id = target_actor;
    request_.cast.parameter = parameter;
  }
}





//----[  NetMsg_UseItem  ]-----------------------------------------------------
void Client::NetMsg_UseItem(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (player_.isDead() || player_.inTrade()) return;
/*
  unsigned int inventory_index;
  Evidyon::Actor::ActorID target_actor_id;
  float target_x, target_z;
  CONFIRM(Decode::useItem(packet,
                          &inventory_index,
                          &target_actor_id,
                          &target_x,
                          &target_z)) else return;
  if (actor->locationIsOutOfRange(target_x, target_z)) {
    todo("karl","red flag - requested use-item location out of range");
    return;
  }

  if (request_.changeTo(Evidyon::Server::PlayerRequest::USE_ITEM)) {
    request_.use_item.x.set(Number(target_x));
    request_.use_item.z.set(Number(target_z));
    request_.use_item.actor_id = target_actor_id;
    request_.use_item.inventory_index = inventory_index;
    request_.use_item.parameter = parameter;
  }*/
  unsigned int packed_x, packed_z;
  Evidyon::Actor::ActorID target_actor;
  unsigned int parameter, inventory_index;
  if (!NetMsg_UseItem::readPacket(
        packet,
        &target_actor,
        &inventory_index,
        &packed_x,
        &packed_z,
        &parameter)) {
    return;
  }

  float target_x = packed_x / 32.0f;
  float target_z = packed_z / 32.0f;


  if (actor->locationIsOutOfRange(target_x, target_z)) {
    todo("karl", "red flag - requested item target out of range");
    return;
  }

  if (request_.changeTo(Evidyon::Server::PlayerRequest::USE_ITEM)) {
    request_.use_item.inventory_index = inventory_index;
    request_.use_item.x.set(Number(target_x));
    request_.use_item.z.set(Number(target_z));
    request_.use_item.actor_id = target_actor;
    request_.use_item.parameter = parameter;
  }
}





//----[  NetMsg_Request_Speak  ]------------------------------------------------
void Client::NetMsg_Request_Speak(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  kg::CharBuffer<Avatar::MAX_SPOKEN_TEXT_LENGTH+1> text;
  size_t text_len;

  if (NetMsg_Request_Speak::readPacket(
      packet,
      &CopiedArray<char>(text.characters,
                         text.CAPACITY,
                         &text_len))) {
    text[text_len] = '\0';

    if (text[0] == '>') {
      // send global chat
      // HAX!!!
      const char* global_chat = &text.characters[1];
      log_guild_chat(character_id_, -1, global_chat, text_len - 1);

      // send to everyone
      NetworkPacket* packet
        = NetMsg_Global_Chat_Distribute::writePacket(
            ENET_PACKET_FLAG_RELIABLE,
            &CopiedArray<char>(avatar_.name, strlen(avatar_.name)),
            &CopiedArray<char>(global_chat, text_len - 1));
      GlobalClientManager::singleton()->sendToAll(packet);

    } else if (text[0] == '/') {
      if (0 == stricmp(&text[1], "showhelmet")) {
        hide_helmet_ = false;
        combineUpdateActorDescriptionFlag(true);
      } else if (0 == stricmp(&text[1], "hidehelmet")) {
        hide_helmet_ = true;
        combineUpdateActorDescriptionFlag(true);
      } else {
        // just make it do something
        actor->say(getName());
      }
    } else {

      {
        float x, z;
        actor->getPosition(&x, &z);
        log_chat(character_id_, x, z, text.characters, text_len);
      }

      // This is the only request type for which it isn't necessary that the request
      // be successfully changed before the data is set.  See PlayerRequest.
      request_.changeTo(Evidyon::Server::PlayerRequest::SPEAK);
      request_.speak.text.copyFrom(text.characters);
    }
  }
}





//----[  NetMsg_ChangeEquippedState  ]-----------------------------------------
void Client::NetMsg_ChangeEquippedState(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (player_.isDead() || player_.inTrade()) return;

  unsigned char indices[AVATAR_INVENTORY_SIZE];
  size_t number_of_indices;
  CONFIRM(Decode::changeEquippedState(packet, 
                                      indices,
                                      AVATAR_INVENTORY_SIZE,
                                     &number_of_indices)) else return;

  ASSERT(number_of_indices == 1);

  bool was_equipped;
  ItemPointer item_ptr;
  if (!items_.removeFromInventory(indices[0], &was_equipped, &item_ptr)) return;

  unsigned int assigned_index;
  CONFIRM(items_.giveItem(&item_ptr, &assigned_index)) else {
    GlobalItemManager::singleton()->releaseItem(&item_ptr); // uhoh
    return;
  }

  // If the pointer is no longer valid, it was completely stacked
  // with other items.
  if (false == item_ptr.invalid() && !was_equipped) {
    items_.changeEquipped(assigned_index);
  }
}





//----[  NetMsg_Drop_Currency  ]-----------------------------------------------
void Client::NetMsg_Drop_Currency(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || player_.inTrade()) return;
  CurrencyType amount;
  NetMsg_Drop_Currency::readPacket(packet, &amount);
  if (hasCurrency(amount)) {
    WorldRegion* region = NULL;
    int x, y;
    actor->getStrictRegion(&x, &y, &region);
    CONFIRM(region) else return;
    region->addCurrencyOnGround(x, y, takeCurrency(amount));
    syncCurrency();
  }
}





//----[  NetMsg_AskActorName  ]------------------------------------------------
void Client::NetMsg_AskActorName(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  Evidyon::Actor::ActorID actor_id;
  NetMsg_AskActorName::readPacket(packet, &actor_id);

  GlobalActorManager* global_actor_manager = GlobalActorManager::singleton();
  ActorInstance* queried_actor = global_actor_manager->getActor(actor_id);
  if (queried_actor) {
    queried_actor->tellName(this);
  }
}





//----[  NetMsg_AskMyAvatarStats  ]--------------------------------------------
void Client::NetMsg_AskMyAvatarStats(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  int abilityToAddPoint;
  CONFIRM(Decode::askMyAvatarStats(packet, &abilityToAddPoint)) else return;
  ASSERT(actor) else return;


  // Add an ability point if the parameter is valid
  if ((abilityToAddPoint >= 0) &&
      (abilityToAddPoint < 5) &&
      (avatar_.available_attribute_points > 0)) {


    int effective_level = avatar_.level - avatar_.available_attribute_points;
    if (effective_level < 0) {
      effective_level = 0;
      switch (abilityToAddPoint) {
      case 0: if (avatar_.attribute_values[Avatar::ATTRIBUTE_STRENGTH] >= 8) return;     break;
      case 1: if (avatar_.attribute_values[Avatar::ATTRIBUTE_AGILITY] >= 8) return;      break;
      case 2: if (avatar_.attribute_values[Avatar::ATTRIBUTE_CONSTITUTION] >= 8) return; break;
      case 3: if (avatar_.attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE] >= 8) return; break;
      case 4: if (avatar_.attribute_values[Avatar::ATTRIBUTE_WISDOM] >= 8) return;       break;
      }
    }
    avatar_.available_attribute_points--;
    switch (abilityToAddPoint) {
      case 0: avatar_.attribute_values[Avatar::ATTRIBUTE_STRENGTH]++;     break;
      case 1: avatar_.attribute_values[Avatar::ATTRIBUTE_AGILITY]++;      break;
      case 2: avatar_.attribute_values[Avatar::ATTRIBUTE_CONSTITUTION]++; break;
      case 3: avatar_.attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE]++; break;
      case 4: avatar_.attribute_values[Avatar::ATTRIBUTE_WISDOM]++;       break;
    }

    // be sure everything is recalculated
    avatar_.calculateBaseStats();
    items_.setEquipmentChanged();
    avatar_stats_.recalculate_timer.trigger();

    // save this character
    commitToDatabase(GlobalDatabaseManager::singleton());
  }

  combineUpdateClientAvatarStatsFlag(true);
}





//----[  NetMsg_StorageOpen  ]-------------------------------------------------
void Client::NetMsg_StorageOpen(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (player_.isDead()) return;

  Evidyon::Actor::ActorID storage_npc_actor_id = 0;
  CONFIRM(Decode::storageOpen(packet, &storage_npc_actor_id)) else return;
  ActorInstance* storage_npc = GlobalActorManager::singleton()->getActor(storage_npc_actor_id);
  if (!storage_npc ||
      actor->actorIsOutOfRange(storage_npc) ||
      !storage_npc->getController()->onClientInteraction(this, actor, Actor::ACTORINTERACTION_OPEN_STORAGE)) {
    addRedFlag(__FILE__, __LINE__);
  } else {
    request_.changeTo(Evidyon::Server::PlayerRequest::STORAGE_OPEN);
    storageOpen();
  }
}





//----[  NetMsg_StorageWithdraw  ]---------------------------------------------
void Client::NetMsg_StorageWithdraw(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (!player_.atStorage()) return;

  Evidyon::Actor::ActorID storage_npc_actor_id = 0;
  int storage_index = 0;
  CONFIRM(Decode::storageWithdraw(packet, &storage_npc_actor_id, &storage_index)) else return;
  //ActorInstance* actor = GlobalActorManager::singleton()->getActor(storage_npc_actor_id);
  //if (!actor || actor->distanceToSq(actor) > MAX_NPC_INTERACTION_DISTANCE_SQ) {
  //  addRedFlag(__FILE__, __LINE__);
  //}
  storageWithdraw(storage_index);
}





//----[  NetMsg_StorageDeposit  ]----------------------------------------------
void Client::NetMsg_StorageDeposit(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (!player_.atStorage()) return;

  Evidyon::Actor::ActorID storage_npc_actor_id = 0;
  int inventory_index = 0;
  CONFIRM(Decode::storageDeposit(packet, &storage_npc_actor_id, &inventory_index)) else return;
  //ActorInstance* actor = GlobalActorManager::singleton()->getActor(storage_npc_actor_id);
  //if (!actor || actor->distanceToSq(actor) > MAX_NPC_INTERACTION_DISTANCE_SQ) {
  //  addRedFlag(__FILE__, __LINE__);
  //}
  storageDeposit(inventory_index);
}




//
////----[  NetMsg_GeosidSacrifice  ]---------------------------------------------
//void Client::NetMsg_GeosidSacrifice(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
//  using namespace ::Evidyon::Network::Packet;
//  using namespace ::Evidyon::Network::Packet::Server;
//  if (!player_.atGeosid()) return;
//
//  unsigned char inventory_index;
//  CONFIRM(Decode::geosidSacrifice(packet, &inventory_index)) else return;
//  unsigned int geonite_value;
//
//  if (items_.geosidSacrificeItem(inventory_index, &geonite_value)) {
//    changeGeonite(geonite_value);
//    sendFullInventoryUpdate();
//  }
//}
//




//----[  NetMsg_TradeAddItem  ]------------------------------------------------
void Client::NetMsg_TradeAddItem(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.inTrade()) return;

  unsigned int inventory_index;
  CONFIRM(NetMsg_Trade_AddItem::readPacket<unsigned int>(packet, &inventory_index)) else return;


  // add the item to trade
  bool equipped;
  ItemPointer item;
  if (!items_.getInventoryItem(inventory_index, &equipped, &item) || equipped) return;
  item_trader_.offerItem(inventory_index, item);
}




//----[  NetMsg_Trade_SetCurrency  ]-------------------------------------------
void Client::NetMsg_Trade_SetCurrency(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.inTrade()) return;

  CurrencyType amount;
  CONFIRM(NetMsg_Trade_SetCurrency::readPacket(packet, &amount)) else return;

  item_trader_.offerCurrency(amount);
}



//----[  NetMsg_TradeRemoveItem  ]---------------------------------------------
void Client::NetMsg_TradeRemoveItem(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.inTrade()) return;

  unsigned int trade_index;
  CONFIRM(NetMsg_Trade_RemoveItem::readPacket<unsigned int>(packet, &trade_index)) else return;

  // remove the item from trade
  item_trader_.withdrawItem(trade_index);

}



//----[  NetMsg_TradeAccept  ]-------------------------------------------------
void Client::NetMsg_TradeAccept(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.inTrade()) return;
  CONFIRM(NetMsg_Trade_Accept::readPacket(packet)) else return;

  item_trader_.acceptTrade();
}



//----[  NetMsg_TradeWithdraw  ]-----------------------------------------------
void Client::NetMsg_TradeWithdraw(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.inTrade()) return;
  CONFIRM(NetMsg_Trade_Withdraw::readPacket(packet)) else return;

  item_trader_.unacceptTrade();
}



//----[  NetMsg_TradeCancel  ]-------------------------------------------------
void Client::NetMsg_TradeCancel(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.inTrade()) return;
  CONFIRM(NetMsg_Trade_Cancel::readPacket(packet)) else return;

  item_trader_.cancelTrade();
}



//----[  NetMsg_SplitStackedItem  ]--------------------------------------------
void Client::NetMsg_SplitStackedItem(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  unsigned int inventory_index;
  // ensure we are idle so that the player can't pull any tricks while in
  // some sort of special state (dead, during a trade, etc.)
  if (!player_.isIdle()) return;
  CONFIRM(NetMsg_SplitStackedItem::readPacket(packet, &inventory_index)) else return;
  if (items_.splitItem(inventory_index)) {
    sendFullInventoryUpdate();
  }
}



//----[  NetMsg_StopAction  ]--------------------------------------------------
void Client::NetMsg_StopAction(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  // don't even have to decode this!
  //NetMsg_StopAction::
  request_.changeTo(Evidyon::Server::PlayerRequest::STOP_ACTION);
}



//----[  NetMsg_Bazaar_Open  ]-------------------------------------------------
void Client::NetMsg_Bazaar_Open(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  if (player_.isDead()) return;

  Evidyon::Actor::ActorID npc_actor_id = 0;
  if (!NetMsg_Bazaar_Open::readPacket(packet, &npc_actor_id)) return;
  ActorInstance* npc = GlobalActorManager::singleton()->getActor(npc_actor_id);
  if (!npc ||
      actor->actorIsOutOfRange(npc) ||
      !npc->getController()->onClientInteraction(this, actor, Actor::ACTORINTERACTION_OPEN_BAZAAR)) {
    addRedFlag(__FILE__, __LINE__);
  } else {
    request_.changeTo(Evidyon::Server::PlayerRequest::BAZAAR_OPEN);
    storageOpen();
  }
}



//----[  NetMsg_Bazaar_CreateListing  ]----------------------------------------
void Client::NetMsg_Bazaar_CreateListing(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.atBazaar()) return;
  unsigned int inventory_index;
  CurrencyType price;
  CONFIRM(NetMsg_Bazaar_CreateListing::readPacket(packet, &inventory_index, &price)) else return;

  ItemPointer item_pointer;
  if (!items_.removeFromInventory(inventory_index, NULL, &item_pointer)) return;

  GlobalBazaarManager* bazaar = GlobalDatabaseManager::singleton()->bazaar();
  if (!bazaar->createListing(&item_pointer,
                              account_id_,
                              price,
                              avatar_.money,
                             &avatar_.money)) {
    ASSERT(items_.giveItem(&item_pointer, NULL));
    *reply_packet = NetMsg_TellCurrency::writePacket(ENET_PACKET_FLAG_RELIABLE, avatar_.money);
  }

  sendFullInventoryUpdate();
  syncCurrency();
  bazaar_tellOwnListedItems();  // the client goes to see own listings next
}




//----[  NetMsg_Bazaar_RetractListing  ]---------------------------------------
void Client::NetMsg_Bazaar_RetractListing(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.atBazaar()) return;
  BazaarListingID listing_id;
  CONFIRM(NetMsg_Bazaar_RetractListing::readPacket(packet, &listing_id)) else return;

  if (false == items_.canHoldItems(1)) return;

  GlobalBazaarManager* bazaar = GlobalDatabaseManager::singleton()->bazaar();
  ItemPointer item_pointer;
  if (bazaar->retractListing(listing_id,
                             account_id_,
                             &item_pointer)) {
    ASSERT(items_.giveItem(&item_pointer, NULL));
    sendFullInventoryUpdate();
    bazaar_tellOwnListedItems();
  }
}




//----[  NetMsg_Bazaar_RedeemListing  ]----------------------------------------
void Client::NetMsg_Bazaar_RedeemListing(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.atBazaar()) return;
  BazaarListingID listing_id;
  CONFIRM(NetMsg_Bazaar_RedeemListing::readPacket(packet, &listing_id)) else return;


  GlobalBazaarManager* bazaar = GlobalDatabaseManager::singleton()->bazaar();
  ItemPointer item_pointer;
  if (bazaar->redeemListing(listing_id,
                            account_id_,
                            avatar_.money,
                           &avatar_.money)) {
    // send a money-update to the client
    *reply_packet = NetMsg_TellCurrency::writePacket(ENET_PACKET_FLAG_RELIABLE, avatar_.money);
    bazaar_tellOwnListedItems();
  }
}



//----[  NetMsg_Bazaar_PurchaseListing  ]--------------------------------------
void Client::NetMsg_Bazaar_PurchaseListing(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.atBazaar()) return;
  BazaarListingID listing_id;
  CONFIRM(NetMsg_Bazaar_PurchaseListing::readPacket(packet, &listing_id)) else return;

  if (items_.canHoldItems(1) == false) return;

  GlobalBazaarManager* bazaar = GlobalDatabaseManager::singleton()->bazaar();
  ItemPointer item_pointer;
  unsigned int category;

  unsigned int money_before = avatar_.money;
  bool result = bazaar->purchaseListing(listing_id,
                                        avatar_.money,
                                       &avatar_.money,
                                       &item_pointer,
                                       &category);
  if (result) {
    {
      ItemInstance* item = item_pointer.dereferenceAssumingValid();
      log_bazaar_purchase(
        character_id_,
        item->getDescription()->true_name,
        money_before - avatar_.money); // calculate the price
    }

    sendPacket(NetMsg_TellCurrency::writePacket(ENET_PACKET_FLAG_RELIABLE, avatar_.money));
    ASSERT(items_.giveItem(&item_pointer, NULL));
    sendFullInventoryUpdate();
  }

  bazaar_tellCategoryListedItems(BazaarCategory(category));

  *reply_packet =
    NetMsg_Bazaar_PurchaseListing_Result::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      result);
}





//----[  NetMsg_Bazaar_GetListings  ]------------------------------------------
void Client::NetMsg_Bazaar_GetListings(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!player_.atBazaar()) return;
  unsigned int category;
  CONFIRM(NetMsg_Bazaar_GetListings::readPacket(packet, &category)) else return;

  GlobalBazaarManager* bazaar = GlobalDatabaseManager::singleton()->bazaar();
  if (category == CATEGORY_OWNITEMS) {
    bazaar_tellOwnListedItems();
  } else {
    bazaar_tellCategoryListedItems(BazaarCategory(category));
  }
}




//----[  NetMsg_Quest_Trigger  ]-----------------------------------------------
void Client::NetMsg_Quest_Trigger(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  Evidyon::Actor::ActorID actor_id;
  float x, y;
  CONFIRM(NetMsg_Quest_Trigger::readPacket(packet, &actor_id, &x, &y)) else return;

  ActorInstance* targeted_actor = GlobalActorManager::singleton()->getActor(actor_id);
  if (!targeted_actor) return;

  static const float MAX_SCREEN_DISTANCE_SQ = 16.0 * 16.0;
  if (!targeted_actor ||
      actor->actorIsOutOfRange(targeted_actor) ||
      !targeted_actor->getController()->onClientInteraction(this, actor, Actor::ACTORINTERACTION_TRIGGER)) {
    addRedFlag(__FILE__, __LINE__);
    return;
  }

  //Evidyon::InteractiveNPCLifeformAIController* controller =
  //  targeted_actor->getController()->getInteractiveNPCLifeformAIController();
  //if (!controller) return;
  //controller->triggerQuest(this, actor);
}



//----[  NetMsg_Quest_OfferResponse  ]-----------------------------------------
void Client::NetMsg_Quest_OfferResponse(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  unsigned int quest_index;
  Evidyon::Actor::ActorID npc_offering_quest;
  int npc_quest_index;
  unsigned int validation_key;
  bool accepted;
  CONFIRM(NetMsg_Quest_Offer_Response::readPacket(packet,
                                                  &quest_index,
                                                  &npc_offering_quest,
                                                  &npc_quest_index,
                                                  &validation_key,
                                                  &accepted)) else return;

  ActorInstance* npc = GlobalActorManager::singleton()->getActor(npc_offering_quest);

  static const float MAX_SCREEN_DISTANCE_SQ = 16.0 * 16.0;
  if (!npc || actor->distanceToSq(npc) > MAX_SCREEN_DISTANCE_SQ) {
    addRedFlag(__FILE__, __LINE__);
    return;
  }

  //Evidyon::InteractiveNPCLifeformAIController* controller =
  //  npc->getController()->getInteractiveNPCLifeformAIController();
  //if (!controller) return;
  //if (!controller->clientQuestOfferResponse(this,
  //                                          actor,
  //                                          quest_index,
  //                                          npc_quest_index,
  //                                          validation_key,
  //                                          accepted)) {
  //  addRedFlag(__FILE__, __LINE__);
  //  return;
  //}
}


//----[  NetMsg_Quest_GiveUp  ]------------------------------------------------
void Client::NetMsg_Quest_GiveUp(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  int local_quest_index;
  CONFIRM(NetMsg_Quest_GiveUp::readPacket(packet, &local_quest_index)) else return;
  if (local_quest_index < 0 || local_quest_index >= 3) return;
  endQuest(local_quest_index, false, false);
  syncClientQuests();
}





//----[  NetMsg_Guild_Create  ]------------------------------------------------
void Client::NetMsg_Guild_Create(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  Evidyon::Server::GlobalGuildManager* guild_manager
    = Evidyon::Server::GlobalGuildManager::singleton();
  if (guild_) {
    guild_manager->leaveGuild(guild_, character_id_);
    guild_ = NULL;
  }
  if (request_.changeTo(Evidyon::Server::PlayerRequest::CREATE_GUILD)) {
    size_t chars;
    if (NetMsg_Guild_Create::readPacket(
          packet,
          &CopiedArray<char>(request_.create_guild.guild_name,
                             Guild::MAX_NAME_LENGTH-1,
                             &chars))) {
      request_.create_guild.guild_name[Guild::MAX_NAME_LENGTH-1] = '\0';
      request_.create_guild.guild_name[chars] = '\0';
      assert(strlen(request_.create_guild.guild_name) == chars);
      
      UniqueAvatarNameChecker* names
        = GlobalDatabaseManager::singleton()->getUniqueAvatarNameChecker();
      if (chars == 0 ||
        !names->isNameAvailable(request_.create_guild.guild_name)) {
        actor->say("<Can't start a guild with this name>");
        request_.reset();
      } else {
        actor->say("<Trying to Start Guild>");
      }
    }
  }
}




//----[  NetMsg_Guild_Lead  ]--------------------------------------------------
void Client::NetMsg_Guild_Lead(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  if (!guild_) return;
  request_.changeTo(Evidyon::Server::PlayerRequest::LEAD_GUILD);
  actor->say("<Invites New Guild Members>");
}




//----[  NetMsg_Guild_Join  ]--------------------------------------------------
void Client::NetMsg_Guild_Join(ActorInstance* my_actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;

  WorldRegion* region = my_actor->getRegion();


  Client* other_client
    = region->findNearbyClientInvitingGuildMembers(this, my_actor);
  if (other_client) {
    if (joinGuild(other_client)) {
      assert(guild_);
      my_actor->say("<Joins Guild>");
    } else {
      my_actor->say("I don't think I can do that.");
    }
  }
}




//----[  NetMsg_Guild_Leave  ]-------------------------------------------------
void Client::NetMsg_Guild_Leave(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  if (guild_) {
    Evidyon::Server::GlobalGuildManager* guild_manager
      = Evidyon::Server::GlobalGuildManager::singleton();
    guild_manager->leaveGuild(guild_, character_id_);
    guild_ = NULL;
    avatar_.guild = Evidyon::Server::INVALID_GUILD_ID;
    actor->say("<Leaves Guild>");
  }
}




//----[  NetMsg_Guild_Promote  ]-----------------------------------------------
void Client::NetMsg_Guild_Promote(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || !guild_) return;
  kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
  size_t chars;
  if (NetMsg_Guild_Promote::readPacket(
          packet,
          &CopiedArray<char>(name.characters,
                             name.CAPACITY,
                             &chars))) {
    name[name.CAPACITY] = '\0';
    name[chars] = '\0';
    if (guild_->promote(character_id_, guild_->getGuildMemberID(name.characters))) {
      actor->say("<Promotes Guild Member>");
    }
  }
}




//----[  NetMsg_Guild_Demote  ]------------------------------------------------
void Client::NetMsg_Guild_Demote(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || !guild_) return;
  kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
  size_t chars;
  if (NetMsg_Guild_Demote::readPacket(
          packet,
          &CopiedArray<char>(name.characters,
                             name.CAPACITY,
                             &chars))) {
    name[chars] = '\0';
    name[name.CAPACITY] = '\0';
    if (guild_->demote(character_id_, guild_->getGuildMemberID(name.characters))) {
      actor->say("<Demotes Guild Member>");
    }
  }
}




//----[  NetMsg_Guild_Chat  ]--------------------------------------------------
void Client::NetMsg_Guild_Chat(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!guild_) return;
  kg::CharBuffer<256> chat;
  size_t chat_chars;
  if (NetMsg_Guild_Chat::readPacket(
          packet,
          &CopiedArray<char>(chat.characters,
                             chat.CAPACITY,
                             &chat_chars))) {
    chat[chat_chars] = '\0';
    chat[chat.CAPACITY] = '\0';
    log_guild_chat(character_id_, guild_->getGuildID(), chat.characters, chat_chars);
    guild_->sendChat(avatar_.name,
                     strlen(avatar_.name),
                     chat.characters,
                     chat_chars);
  }
}




//----[  NetMsg_Guild_Kick  ]--------------------------------------------------
void Client::NetMsg_Guild_Kick(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || !guild_) return;
  kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
  size_t chars;
  if (NetMsg_Guild_Kick::readPacket(
          packet,
          &CopiedArray<char>(name.characters,
                             name.CAPACITY,
                             &chars))) {
    name[chars] = '\0';
    name[name.SIZE-1] = '\0';
    if (guild_->kick(character_id_, guild_->getGuildMemberID(name.characters))) {
      actor->say("<Kicks Guild Member>");
    }
  }
}




//----[  NetMsg_Guild_Members  ]-----------------------------------------------
void Client::NetMsg_Guild_Members(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || !guild_) return;
  assert(Guild::MAX_GUILD_MEMBERS_TEXT_LENTH <= Guild::MAX_GUILD_MESSAGE_LENGTH);
  kg::CharBuffer<Guild::MAX_GUILD_MEMBERS_TEXT_LENTH> members_text;
  guild_->membersText(members_text.characters, members_text.CAPACITY);
  sendAsyncPacket(NetMsg_Guild_Message::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    &CopiedArray<char>(members_text.characters, strlen(members_text.characters))));
}





//----[  NetMsg_Guild_AskGeosids  ]--------------------------------------------
void Client::NetMsg_Guild_AskGeosids(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || !guild_) return;
  if (!NetMsg_Guild_AskGeosids::readPacket(packet)) return;
  *reply_packet = guild_->tellGeosidsPacket();
}





//----[  NetMsg_Guild_SetHome  ]-----------------------------------------------
void Client::NetMsg_Guild_SetHome(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || !guild_) return;
  if (!NetMsg_Guild_SetHome::readPacket(packet)) return;
  Number x, z;
  actor->getPosition(&x, &z);
  guild_->setHome(character_id_,
                  actor->getRegion()->getMap(),
                  x,
                  z);
}



//----[  NetMsg_Guild_Home  ]--------------------------------------------------
void Client::NetMsg_Guild_Home(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || !guild_) return;
  if (!NetMsg_Guild_Home::readPacket(packet)) return;
  Map* map;
  Number x, z;
  if (guild_->goHome(&map,
                     &x,
                     &z)) {
    todo("karl","g/home is instant; needs a delay");
    actor->changeMap(map, x, z);
  }
}




//----[  NetMsg_Guild_Abandon  ]-----------------------------------------------
void Client::NetMsg_Guild_Abandon(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  Geosid::GeosidIndex geosid_index;
  if (player_.isDead() || !guild_) return;
  if (!NetMsg_Guild_Abandon::readPacket(packet, &geosid_index)) return;
  if (guild_->abandonGeosid(geosid_index, character_id_)) {
    actor->say("<Abandons Guild Geosid>");
  }
}



//----[  NetMsg_Geosid_DropSacrifice  ]-----------------------------------------
void Client::NetMsg_Geosid_DropSacrifice(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;

  Geosid::GeosidIndex geosid_id;
  unsigned int inventory_index;
  if (!NetMsg_Geosid_DropSacrifice::readPacket(packet, &geosid_id, &inventory_index)) return;

  using namespace Evidyon::Server;
  GlobalGeosidManager* geosid_manager
    = GlobalGeosidManager::singleton();

  Number x, z;
  actor->getPosition(&x, &z);

  GeosidInstance* geosid = geosid_manager->getGeosidIfInRange(
    geosid_id,
    actor->getMapIndex(),
    x,
    z);

  if (geosid && geosid->canSacrificeItems(this)) {
    Geosid::Geonite geonite;
    if (items_.geosidSacrificeItem(inventory_index, &geonite)) {
      geosid->changeGeonite(-geonite, actor->getRegion());
    }
  }
}



//----[  NetMsg_Geosid_Burst  ]-------------------------------------------------
void Client::NetMsg_Geosid_Burst(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  //using namespace ::Evidyon::Network::Packet;
  //if (player_.isDead()) return;
  //float x, z;
  //if (!NetMsg_Geosid_Burst::readPacket(packet, &x, &z)) return;
  //// no effect for now...?

  //const Evidyon::Server::MapLocationInfo* location
  //  = actor->getRegion()->getMap()->getMapLocationInfo(int(x), int(z));
  //if (location && Magic::INVALID_MAGIC_INDEX != location->geonite_burst_magic) {
  //  const Evidyon::Magic::Magic* magic
  //    = GlobalGamefileManager::singleton()->getMagic(location->geonite_burst_magic);
  //  if ((avatar_stats_.geonite >= location->geonite_burst_cost) &&
  //      request_.changeTo(Evidyon::Server::PlayerRequest::GEOSID_BURST)) {
  //    request_.geosid_burst.x.set(Number(x));
  //    request_.geosid_burst.z.set(Number(z));
  //    request_.geosid_burst.actor_id = Actor::INVALID_ACTOR_ID;
  //    request_.geosid_burst.parameter = 0;
  //    request_.geosid_burst.geosid_magic = magic;
  //    request_.geosid_burst.geonite_cost = location->geonite_burst_cost;
  //    actor->getRegion()->broadcastOrphanedNetworkPacket(
  //      *reply_packet = NetMsg_Geosid_Burst::writePacket(
  //        ENET_PACKET_FLAG_UNSEQUENCED, x, z));
  //  }
  //} else {
  //  if (changeGeonite(-(location->geonite_burst_cost + avatar_.level))) {
  //    actor->getRegion()->broadcastOrphanedNetworkPacket(
  //      *reply_packet = NetMsg_Geosid_Burst::writePacket(
  //        ENET_PACKET_FLAG_UNSEQUENCED, x, z));
  //  }
  //}
}



//----[  NetMsg_Geosid_BurstAttackGeosid  ]-------------------------------------
void Client::NetMsg_Geosid_BurstAttackGeosid(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead() || !guild_) return;

  Geosid::GeosidIndex geosid_index;
  if (!NetMsg_Geosid_BurstAttackGeosid::readPacket(packet, &geosid_index)) return;
  Evidyon::Server::GeosidInstance* geosid
    = Evidyon::Server::GlobalGeosidManager::singleton()->getGeosidByIndex(geosid_index);

  // can the client actually see this geosid?
  if (!geosid || !geosid->canBeSeenBy(actor)) {
    //redFlag();
    return;
  }

  // start an attack on this geosid
  if (guild_->attackGeosid(geosid, this)) {

    // create a burst effect!
    actor->getRegion()->broadcastOrphanedNetworkPacket(
      NetMsg_Geosid_BurstAttackGeosid::writePacket(
          ENET_PACKET_FLAG_UNSEQUENCED, geosid_index));

  }
}



//----[  NetMsg_Geosid_BurstSpell  ]--------------------------------------------
void Client::NetMsg_Geosid_BurstSpell(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  if (!NetMsg_Geosid_BurstSpell::readPacket(packet)) return;
}



//----[  NetMsg_Geosid_BurstUseGeosid  ]----------------------------------------
void Client::NetMsg_Geosid_BurstUseGeosid(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  todo("karl","erase GeosidBurstUseGeosid");
  //using namespace ::Evidyon::Network::Packet;
  //if (player_.isDead()) return;
  //Geosid::GeosidIndex geosid_index;
  //Actor::ActorID actor_id;
  //float x, z;
  //if (!NetMsg_Geosid_BurstUseGeosid::readPacket(packet, &geosid_index, &actor_id, &x, &z)) return;
  //Evidyon::Server::GeosidInstance* geosid
  //  = Evidyon::Server::GlobalGeosidManager::singleton()->getGeosidByIndex(geosid_index);

  //// can the client actually see this geosid?
  //Magic::MagicIndex burst_magic;
  //Geosid::Geonite geonite_cost;
  //if (geosid && geosid->getBurstMagic(this, actor, &burst_magic, &geonite_cost)) {
  //  const Magic::Magic* magic
  //    = GlobalGamefileManager::singleton()->getMagic(burst_magic);
  //  if (magic && request_.changeTo(Evidyon::Server::PlayerRequest::GEOSID_BURST)) {
  //    request_.geosid_burst.actor_id = actor_id;
  //    request_.geosid_burst.x.set(Number(x));
  //    request_.geosid_burst.z.set(Number(z));
  //    request_.geosid_burst.parameter = 0;
  //    request_.geosid_burst.geosid_magic = magic;
  //    request_.geosid_burst.geonite_cost = geonite_cost;
  //  }
  //  return;
  //}
}



//----[  NetMsg_Geosid_GetOwner  ]----------------------------------------------
void Client::NetMsg_Geosid_GetOwner(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  Geosid::GeosidIndex geosid_index;
  if (!NetMsg_Geosid_GetOwner::readPacket(packet, &geosid_index)) return;
  *reply_packet
    = Evidyon::Server::GlobalGeosidManager::singleton()
        ->getTellGeosidOwnerDataPacket(geosid_index, actor);
}





//----[  NetMsg_Party_Chat  ]--------------------------------------------------
void Client::NetMsg_Party_Chat(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;

  kg::CharBuffer<256> chat;
  size_t chat_chars;
  if (NetMsg_Party_Chat::readPacket(
          packet,
          &CopiedArray<char>(chat.characters,
                             chat.CAPACITY,
                             &chat_chars))) {
    chat[chat_chars] = '\0';
    chat[chat.CAPACITY] = '\0';
 
    ActorGroup* group = actor->getGroup();
    if (group) {
      log_party_chat(
        character_id_,
        group->getPartyID(),
        chat.characters,
        chat_chars);

      NetworkPacket* packet =
        NetMsg_Party_Chat_Distribute::writePacket(
          ENET_PACKET_FLAG_RELIABLE,
           &CopiedArray<char>(avatar_.name, strlen(avatar_.name)),
           &CopiedArray<char>(chat.characters, chat_chars));
      group->sendToClients(packet);
      GlobalNetworkManager::handleOrphanedPacket(packet);
    }
  }
}



//----[  NetMsg_Party_Lead  ]--------------------------------------------------
void Client::NetMsg_Party_Lead(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  if (!NetMsg_Party_Lead::readPacket(packet)) return;

  openActorGroup();
  actor->say("<Party Open to New Members>");
}



//----[  NetMsg_Party_Join  ]--------------------------------------------------
void Client::NetMsg_Party_Join(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  if (!NetMsg_Party_Join::readPacket(packet)) return;

  ActorInstance* nearby_actor;
  if (actor->getRegion()->findClosestActor(actor, 3.0f*3.0f, &nearby_actor)) {
    Client* client = nearby_actor->getController()->getClient();
    if (client && joinActorGroup(nearby_actor)) {
      char speech[Avatar::MAX_SPOKEN_TEXT_LENGTH+1];
      sprintf_s(speech, Avatar::MAX_SPOKEN_TEXT_LENGTH, "<Joined %s>", client->getName());
      actor->say(speech);
      assert(actor->getGroup());
    }
  }
}



//----[  NetMsg_Party_Leave  ]-------------------------------------------------
void Client::NetMsg_Party_Leave(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  if (!NetMsg_Party_Leave::readPacket(packet)) return;

  leaveActorGroup();
  actor->say("<Leaves Party>");
}



//----[  NetMsg_Party_Kick  ]--------------------------------------------------
void Client::NetMsg_Party_Kick(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
  size_t chars;
  if (NetMsg_Party_Kick::readPacket(
          packet,
          &CopiedArray<char>(name.characters,
                             name.CAPACITY,
                             &chars))) {
    name[name.CAPACITY] = '\0';
    name[chars] = '\0';
    if (chars == 0) {
      ActorInstance* nearby_actor;
      if (actor->getRegion()->findClosestActor(actor, 2.0f*2.0f, &nearby_actor)) {
        Client* client = nearby_actor->getController()->getClient();
        //assert(client);
        if (client && nearby_actor->inSameGroupAs(actor)) {
          nearby_actor->setGroup(NULL);
          char speech[Avatar::MAX_SPOKEN_TEXT_LENGTH+1];
          sprintf_s(speech, Avatar::MAX_SPOKEN_TEXT_LENGTH, "<Kicks %s from group>", client->getName());
          actor->say(speech);
        }
      }
    } else {
      ActorGroup* group = actor->getGroup();
      if (group->kickMember(name.characters)) {
        char speech[Avatar::MAX_SPOKEN_TEXT_LENGTH+1];
        sprintf_s(speech, Avatar::MAX_SPOKEN_TEXT_LENGTH, "<Kicks %s from group>", name.characters);
        actor->say(speech);
      }
    }
  }
}


//----[  NetMsg_Party_Close  ]-------------------------------------------------
void Client::NetMsg_Party_Close(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (player_.isDead()) return;
  if (!NetMsg_Party_Close::readPacket(packet)) return;
  closeActorGroup();
  actor->say("<Party Closed>");
}




//----[  NetMsg_Resurrect_StandUp  ]-------------------------------------------
void Client::NetMsg_Resurrect_StandUp(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace::Evidyon::Network::Packet;
  if (!player_.isDead()) return; // can only do while dead
  if (!NetMsg_Resurrect_StandUp::readPacket(packet)) return;
  request_.changeTo(Evidyon::Server::PlayerRequest::STANDUP);
}



//----[  NetMsg_Resurrect_Respawn  ]-------------------------------------------
void Client::NetMsg_Resurrect_Respawn(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace::Evidyon::Network::Packet;
  if (!player_.isDead()) return; // can only do while dead
  if (!NetMsg_Resurrect_Respawn::readPacket(packet)) return;
  request_.changeTo(Evidyon::Server::PlayerRequest::RESPAWN);
}



//----[  NetMsg_AutoTarget_All  ]----------------------------------------------
void Client::NetMsg_AutoTarget_All(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!NetMsg_AutoTarget_All::readPacket(packet)) return;
  if (player_.setAttackPlayers(true)) {
    actor->say("<Attacking All Targets>");
  }
}



//----[  NetMsg_AutoTarget_Monsters  ]-----------------------------------------
void Client::NetMsg_AutoTarget_Monsters(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  if (!NetMsg_AutoTarget_Monsters::readPacket(packet)) return;
  if (player_.setAttackPlayers(false)) {
    actor->say("<Area Spells and Projectiles Only Affect Monsters>");
  }
}



//----[  NetMsg_Mark  ]--------------------------------------------------------
void Client::NetMsg_Mark(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet) {
  using namespace ::Evidyon::Network::Packet;
  int mark;
  if (!NetMsg_Mark::readPacket(packet, &mark)) return;
  if (mark >= 0 &&
      mark < Evidyon::Server::ClientPortalMark::NUMBER_PER_CLIENT) {
    Map* map = actor->getRegion()->getMap();
    portal_marks_[mark].map_index = map->getIndex();
    actor->getPosition(&portal_marks_[mark].x,
                       &portal_marks_[mark].z);
    actor->say("<Marks Location>");
  }
}




} // end of namespace Evidyon

