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
#include "guildinstance.h"
#include "server/client.h"
#include "server/globaldatabasemanager.h"
#include "shared/networkdecl.h"
#include "shared/client-server/packets.h"
#include "server/geosid/globalgeosidmanager.h"
#include "server/geosid/geosidattack.h"
#include "server/guild/globalguildmanager.h"
#include "server/actor.h"


namespace Evidyon {
namespace Server {


// Prevents this situation:
//    Guild has N members online, N > 0
//    new member joins, logs off, logs on and is no longer in guild
//      - membership won't be detected until all N members log off so the
//        in-database memberships are changed
#define STORE_ON_EACH_CHANGE

#ifdef STORE_ON_EACH_CHANGE
#define guildChanged()  store(GlobalDatabaseManager::singleton())
#else
#define guildChanged()  0
#endif



  
//----[  create  ]-------------------------------------------------------------
void GuildInstance::create(GuildID id,
                           const char* guild_name,
                           AvatarID founder_id_avatar,
                           Client* founder_id_client) {
  founder_id_ = founder_id_avatar;
  online_.ensureEmpty();
  online_.add(founder_id_client);

  guild_id_ = id;
  guild_name_.copyMaxFrom(guild_name);

  Member& member = members_.add();
  member.avatar_id = founder_id_avatar;
  member.leader = true;
  member.name.copyMaxFrom(founder_id_client->getName());
  member.online = true;

  number_of_leaders_ = 1;

  const ActorInstance* actor = founder_id_client->getActor()->dereference();
  assert(actor);
  home_map_ = actor->getMap();
  actor->getPosition(&home_x_, &home_z_);

  founder_id_client->setGuild(this);

  geosids_.count = 0;
  attacked_geosid_ = NULL;
  geosid_attack_type_ = INVALID_GEOSID_ATTACK;
  geosid_attack_timer_.reset();

  hp_multiplier_ = 1.0;
  mp_multiplier_ = 1.0;
  xp_multiplier_ = 1.0;
}





//----[  retrieve  ]-----------------------------------------------------------
bool GuildInstance::retrieve(GuildID id,
                             GlobalDatabaseManager* database,
                             GlobalGeosidManager* geosid_manager) {
  assert(database);
  online_.ensureEmpty();
  number_of_leaders_ = 0;
  if (!database->guildRetrieve(id,
                               guild_name_.characters,
                               guild_name_.CAPACITY,
                               &founder_id_,
                               &home_map_,
                               &home_x_,
                               &home_z_,
                               this)) {
    members_.count = 0;
    return false;
  } else {

    // Can't establish a guild with no members
    if (!hasAnyMembers()) return false;

    // the founder is always a leader, but the founder doesn't necessairily
    // have to exist anymore (the founder can leave)
    Member* member = getMember(founder_id_);
    if (member && !member->leader) {
      number_of_leaders_++;
      member->leader = true;
    }
    guild_id_ = id;

    geosids_.count = 0; // get this guild's geosids
    geosid_manager->guildInstanceRetrieved(this);
    calculateGeosidEffects(false);
    attacked_geosid_ = NULL;
    geosid_attack_type_ = INVALID_GEOSID_ATTACK;
    geosid_attack_timer_.reset();

    return true;
  }
}





//----[  store  ]--------------------------------------------------------------
bool GuildInstance::store(GlobalDatabaseManager* database) {
  assert(database);
  database->guildSet(guild_id_,
                     guild_name_.characters,
                     founder_id_,
                     home_map_,
                     home_x_,
                     home_z_);
  struct StoreMember {
    GuildID guild_id;
    GlobalDatabaseManager* database;
    bool store(Member* member) {
      database->guildMemberSet(guild_id, member->avatar_id, member->leader);
      return true;
    }
  } store_member;
  store_member.guild_id = guild_id_;
  store_member.database = database;
  return members_.forAllElements(&store_member, &StoreMember::store);
}






//----[  loggedOn  ]-----------------------------------------------------------
bool GuildInstance::loggedOn(AvatarID avatar_id, Client* client) {
  Member* member = getMember(avatar_id);
  if (!member) return false;
  assert(member->online == false);
  assert(online_.canHoldMore());
  if (online_.isEmpty() == false) {
    // send before adding new client, since the new client is likely
    // to just flat ignore this message anyway
    char message[512];
    sprintf_s(message,
              512,
              "%s logged on (%lu online)",
              member->name.characters,
              online_.count);
    sendMessage(message);
  }
  online_.add(client);
  member->online = true;
  // if this is the founder member, be sure the leadership role was established
  assert(avatar_id != founder_id_ || member->leader == true);
  client->setGuild(this);
  return true;
}




//----[  loggedOff  ]----------------------------------------------------------
void GuildInstance::loggedOff(AvatarID avatar_id, Client* client) {
  Member* member = getMember(avatar_id);
  confirm(member && member->online) else return;
  size_t online_index = online_.linearFindByEquality(client);
  assert(online_.validIndex(online_index));
  online_.swapErase(online_index, NULL);
  // make sure the client was only in the list once
  assert(!online_.containsByEquality(client));
  member->online = false;
  char message[512];
  sprintf_s(message,
            512,
            "%s logged off (%lu online)",
            member->name.characters,
            online_.count);
  sendMessage(message);
  client->setGuild(NULL); // just for safety
  if (geosid_attack_initiator_ == client) {
    geosid_attack_initiator_ = NULL;
  }
}




//----[  areAnyMembersOnline  ]------------------------------------------------
bool GuildInstance::areAnyMembersOnline() const {
  return !online_.isEmpty();
}




//----[  hasAnyMembers  ]------------------------------------------------------
bool GuildInstance::hasAnyMembers() const {
  return !members_.isEmpty();
}




//----[  getGuildID  ]---------------------------------------------------------
GuildID GuildInstance::getGuildID() const {
  return guild_id_;
}





//----[  isMember  ]-----------------------------------------------------------
bool GuildInstance::isMember(AvatarID avatar_id) {
  return getMember(avatar_id) != NULL;
}








//----[  getTellMemberActorNamePacket  ]---------------------------------------
NetworkPacket* GuildInstance::getTellMemberActorNamePacket(
    AvatarID avatar_id, Client* online_client, Actor::ActorID actor_id) {
  using namespace Evidyon::Network::Packet;
  assert(online_.containsByEquality(online_client));

  size_t guild_name_length = strlen(guild_name_.characters);
  const char* avatar_name = online_client->getName();
  size_t avatar_name_length = strlen(avatar_name);

  if (avatar_id == founder_id_) {
    return NetMsg_Guild_TellFounderActorName::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      actor_id,
      &CopiedArray<char>(avatar_name, avatar_name_length),
      &CopiedArray<char>(guild_name_.characters, guild_name_length));
  } else if (isLeader(avatar_id)) {
    return NetMsg_Guild_TellLeaderActorName::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      actor_id,
      &CopiedArray<char>(avatar_name, avatar_name_length),
      &CopiedArray<char>(guild_name_.characters, guild_name_length));
  } else {
    assert(isMember(avatar_id));
    return NetMsg_Guild_TellMemberActorName::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      actor_id,
      &CopiedArray<char>(avatar_name, avatar_name_length),
      &CopiedArray<char>(guild_name_.characters, guild_name_length));
  }
}




//----[  kick  ]---------------------------------------------------------------
bool GuildInstance::kick(AvatarID kicker, AvatarID kicked) {
  if (kicker == kicked) return false;
  Member* kicker_member = getMember(kicker);
  confirm(kicker_member) else return false;
  if (!kicker_member->leader) return false;
  Member* kicked_member = getMember(kicked);
  // founder can kick leaders; otherwise, leaders can't be kicked
  if (!kicked_member || (kicked_member->leader && (kicker != founder_id_))) return false;
  char message[512];
  sprintf_s(message,
            512,
            "%s kicked %s from the guild",
            kicker_member->name.characters,
            kicked_member->name.characters);
  sendMessage(message);
  remove(kicked_member);

  guildChanged();
  return true;
}







//----[  leave  ]--------------------------------------------------------------
void GuildInstance::leave(AvatarID avatar) {
  Member* leaving_member = getMember(avatar);
  confirm(leaving_member) else return;
  char message[512];
  sprintf_s(message,
            512,
            "%s left the guild",
            leaving_member->name.characters);
  sendMessage(message);
  remove(leaving_member);

  guildChanged();
}






//----[  promote  ]------------------------------------------------------------
bool GuildInstance::promote(AvatarID promoter,
                            AvatarID promoted) {
  if (promoter == promoted) return false;
  if (promoter != founder_id_) return false;
  Member* promoter_member = getMember(promoter);
  confirm(promoter_member) else return false;
  Member* promoted_member = getMember(promoted);
  if (!promoted_member ||
      promoted_member->leader ||
      number_of_leaders_ >= Guild::MAX_NUMBER_OF_LEADERS) return false;
  bool online = promoted_member->online;
  promoted_member->leader = true;
  ++number_of_leaders_;

  char message[512];
  sprintf_s(message,
            512,
            "%s has been promoted to a Leader of the guild",
            promoted_member->name.characters);
  sendMessage(message);

  if (online) {
    Client* client;
    getOnlineMemberClient(promoted, &client);
    assert(client);
    client->combineUpdateNameFlag(true);
  }

  guildChanged();
  return true;
}




//----[  demote  ]-------------------------------------------------------------
bool GuildInstance::demote(AvatarID demoter,
                           AvatarID demoted) {
  if (demoter == demoted) return false;
  if (demoter != founder_id_) return false;
  Member* demoter_member = getMember(demoter);
  confirm(demoter_member) else return false;
  Member* demoted_member = getMember(demoted);
  if (!demoted_member || !demoted_member->leader) return false;
  bool online = demoted_member->online;
  demoted_member->leader = false;
  --number_of_leaders_;

  char message[512];
  sprintf_s(message,
            512,
            "%s has been demoted",
            demoted_member->name.characters);
  sendMessage(message);


  // since only the founder can demote players, and the founder is a leader,
  // there must be at least 1 leader remaining for this to make sense.
  assert(number_of_leaders_ >= 1);

  if (online) {
    Client* client;
    getOnlineMemberClient(demoted, &client);
    assert(client);
    client->combineUpdateNameFlag(true);
  }

  guildChanged();
  return true;
}







//----[  lead  ]---------------------------------------------------------------
bool GuildInstance::lead(AvatarID leader,
                         AvatarID follower,
                         Client* follower_client) {
  if (leader == follower) return false;
  assert(follower_client);
  Member* leader_member = getMember(leader);
  confirm(leader_member) else return false;
  if (!leader_member->leader) return false;
  if (members_.isFull() || isMember(follower)) return false;

  Member& follower_member = members_.add();
  follower_member.avatar_id = follower;
  follower_member.leader = false;
  follower_member.name.copyMaxFrom(follower_client->getName());
  follower_member.online = true;

  // Put the entry into the online table, then make sure the follower was
  // added correctly
  size_t online_index = online_.add(follower_client);
  assert(online_index == getOnlineMemberClient(follower, &follower_client));

  // This client is now a guild member
  follower_client->setGuild(this);

  char message[512];
  sprintf_s(message,
            512,
            "%s brought %s into the guild",
            leader_member->name.characters,
            follower_member.name.characters);
  sendMessage(message);

  // Successfully added a new member
  guildChanged();
  return true;
}



//----[  setHome  ]------------------------------------------------------------
bool GuildInstance::setHome(AvatarID avatar_id,
                            Map* map,
                            Number x,
                            Number z) {
  assert(map);
  Member* leader_member = getMember(avatar_id);
  confirm(leader_member) else return false;
  if (!leader_member->leader) return false;

  home_map_ = map;
  home_x_ = x;
  home_z_ = z;

  char message[512];
  sprintf_s(message,
            512,
            "%s changed the home point of the guild",
            leader_member->name.characters);
  sendMessage(message);

  // Successfully changed the home point
  guildChanged();
  return true;
}




//----[  goHome  ]-------------------------------------------------------------
bool GuildInstance::goHome(Map** map, Number* x, Number* z) {
  assert(map && x && z);
  if (!home_map_) return false;

  *map = home_map_;
  *x = home_x_;
  *z = home_z_;

  return true;
}



//----[  abandonGeosid  ]------------------------------------------------------
bool GuildInstance::abandonGeosid(Geosid::GeosidIndex geosid_index, AvatarID avatar_id) {
  Member* leader_member = getMember(avatar_id);
  confirm(leader_member) else return false;
  if (!leader_member->leader) return false;

  GeosidInstance* geosid
    = GlobalGeosidManager::singleton()->getGeosidByIndex(geosid_index);
  size_t at = geosids_.linearFindByEquality(geosid);
  if (!geosid ||
      (at >= geosids_.count) ||
      !geosid->isOwner(guild_id_)) {
    return false;
  }

  // remove us as the owner, and be sure the geosid updates
  geosid->removeOwner();
  geosid->store(GlobalDatabaseManager::singleton());
  geosids_.swapErase(at, NULL);

  char message[512];
  sprintf_s(message,
            512,
            "%s abandoned a geosid",
            leader_member->name.characters);
  sendMessage(message);

  calculateGeosidEffects(true);

  return true;
}


//----[  getGuildMemberID  ]---------------------------------------------------
AvatarID GuildInstance::getGuildMemberID(const char* avatar_name) const {
  for (size_t i = 0; i < members_.count; ++i) {
    if (0 == _stricmp(members_[i].name.characters, avatar_name)) {
      return members_[i].avatar_id;
    }
  }
  return INVALID_AVATAR_ID;
}




//----[  sendMessage  ]--------------------------------------------------------
void GuildInstance::sendMessage(const char* text) {
  sendMessage(text, strlen(text));
}


//----[  sendMessage  ]--------------------------------------------------------
void GuildInstance::sendMessage(const char* text, size_t length) {
  if (online_.isEmpty()) return;
  assert(text);
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet;
  assert(length > 0 && length == strlen(text));
  packet = NetMsg_Guild_Message::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    &CopiedArray<char>(text, length));
  broadcast(packet);
}






//----[  sendChat  ]-----------------------------------------------------------
void GuildInstance::sendChat(const char* member_name, size_t name_length,
                             const char* chat_text, size_t chat_length) {
  confirm(!online_.isEmpty()) else return;
  assert(member_name && chat_text);
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Chat_Distribute::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        &CopiedArray<char>(member_name, name_length),
        &CopiedArray<char>(chat_text, chat_length));
  broadcast(packet);
}





//----[  membersText  ]--------------------------------------------------------
void GuildInstance::membersText(char* output_buffer, size_t size) {
  // example list: "Ecrofresia (Erich) Joe"
  output_buffer[0] = '\0';
  assert(output_buffer);
  for (size_t i = 0; i < members_.count; ++i) {
    kg::CharBuffer<128> member;
    sprintf_s(member.characters,
              member.CAPACITY,
              "%s%s%s",
              i > 0 ? (members_[i].online ? " " : " (") : (members_[i].online ? "" : "("),
              members_[i].name.characters,
              members_[i].online ? "" : ")");
    member.appendTo(output_buffer, size);
  }
}



//----[  tellGeosidsPacket  ]--------------------------------------------------
NetworkPacket* GuildInstance::tellGeosidsPacket() {
  Geosid::GeosidIndex geosids_owned[Guild::MAX_GEOSIDS_OWNED];
  for (size_t i = 0; i < geosids_.count; ++i) {
    geosids_owned[i] = geosids_[i]->getGeosidIndex();
  }

  using namespace Evidyon::Network::Packet;
  return NetMsg_Guild_Message_TellGeosids::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    &CopiedArray<Geosid::GeosidIndex>(geosids_owned, geosids_.count));
}



//----[  attackGeosid  ]-------------------------------------------------------
bool GuildInstance::attackGeosid(GeosidInstance* geosid, Client* initiator) {
  assert(geosid);
  assert(initiator);
  assert(isMember(initiator->getLoggedIntoCharacterID()));

  // Does this guild already own the geosid?
  if (geosid->isOwner(guild_id_)) return false;

  // Is another attack already underway?
  if (geosid_attack_type_ != INVALID_GEOSID_ATTACK) {
    // it's actually ok if this assert fails, but almost 100% of the time
    // it shouldn't (it will fail
    assert(attacked_geosid_ && geosid_attack_initiator_);
    return false;
  }


  const char* client_name = initiator->getName();
  size_t client_name_length = strlen(client_name);

  Time::Milliseconds attack_delay;
  if (!geosid->attack(this, initiator, &geosid_attack_type_, &attack_delay)) {
    // this is a white geosid, or the initiator doesn't have enough geonite
    return false;
  }
  geosid_attack_initiator_ = initiator;
  attacked_geosid_ = geosid;
  geosid_attack_timer_.enable(attack_delay);

  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Message_TellAttack::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        geosid->getGeosidIndex(),
        geosid_attack_type_ == GEOSIDATTACK_ESTABLISHOWNERSHIP,
        &CopiedArray<char>(client_name, client_name_length));
  broadcast(packet);

  GlobalGuildManager::singleton()->addGuildAttackingGeosid(this);

  return true;
}



//----[  updateGeosidAttack  ]-------------------------------------------------
bool GuildInstance::updateGeosidAttack() {
  if (geosid_attack_type_ == INVALID_GEOSID_ATTACK) return false;
  if (!geosid_attack_initiator_) goto ATTACK_ENDED;
  const ActorInstance* actor_instance
    = geosid_attack_initiator_->getActor()->dereference();
  confirm(actor_instance) else {
    goto ATTACK_ENDED;
  }
  assert(attacked_geosid_);
  if (!attacked_geosid_->isWithinRange(actor_instance)) {
    // attack failed because client left
    goto ATTACK_ENDED;
  }

  if (geosid_attack_timer_.poll()) {
    attacked_geosid_->burstAttackTimerExpired(this, geosid_attack_type_);
    goto ATTACK_ENDED;
  }

  // attack is still in progress
  return true;

ATTACK_ENDED:
  geosid_attack_timer_.disable();
  geosid_attack_initiator_ = NULL;
  attacked_geosid_ = NULL;
  geosid_attack_type_ = INVALID_GEOSID_ATTACK;
  return false;
}


//----[  gainedGeosid  ]-------------------------------------------------------
void GuildInstance::gainedGeosid(GeosidInstance* geosid) {
  assert(geosid);

  assert(geosids_.count < Guild::MAX_GEOSIDS_OWNED);
  assert(0 == geosids_.swapRemoveAllByLinearFindByEquality(geosid));
  geosids_.add(geosid);

  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Message_GeosidGained::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        geosid->getGeosidIndex());
  broadcast(packet);

  calculateGeosidEffects(true);
}


//----[  lostGeosid  ]---------------------------------------------------------
void GuildInstance::lostGeosid(GeosidInstance* geosid) {
  assert(geosid);

  geosids_.swapRemoveAllByLinearFindByEquality(geosid);

  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Message_GeosidLost::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        geosid->getGeosidIndex());
  broadcast(packet);

  calculateGeosidEffects(true);
}


//----[  geosidUnderAttack  ]--------------------------------------------------
void GuildInstance::geosidUnderAttack(GeosidInstance* geosid,
                                      GuildInstance* attackers) {
  assert(geosid);
  assert(attackers);

  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Message_GeosidUnderAttack::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        geosid->getGeosidIndex(),
        &CopiedArray<char>(attackers->guild_name_.characters,
                           strlen(attackers->guild_name_.characters)));
  broadcast(packet);
}


//----[  canAcquireAnotherGeosid  ]--------------------------------------------
bool GuildInstance::canAcquireAnotherGeosid() const {
  return members_.count / 2 > geosids_.count;
}



//----[  getTellGeosidOwnerDataPacket  ]---------------------------------------
NetworkPacket* GuildInstance::getTellGeosidOwnerDataPacket(
    Geosid::GeosidIndex geosid,
    Geosid::Geonite geonite) {
  using namespace Evidyon::Network::Packet;
  return NetMsg_Geosid_TellOwnerData::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    geosid,
    &CopiedArray<char>(guild_name_.characters, strlen(guild_name_.characters)),
    geonite);
}



//----[  getChangeGeosidOwnerPacket  ]-----------------------------------------
NetworkPacket* GuildInstance::getChangeGeosidOwnerPacket(Geosid::GeosidIndex geosid,
                                                         Geosid::Geonite geonite) {
  using namespace Evidyon::Network::Packet;
  return NetMsg_Geosid_ChangeOwner::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    geosid,
    &CopiedArray<char>(guild_name_.characters, strlen(guild_name_.characters)),
    geonite);
}



//----[  addRetrievedMember  ]-------------------------------------------------
void GuildInstance::addRetrievedMember(AvatarID avatar_id,
                                       const char* name,
                                       bool leader) {
  assert(members_.canHoldMore());
  Member& member = members_.add();
  member.avatar_id = avatar_id;
  if (number_of_leaders_ > Guild::MAX_NUMBER_OF_LEADERS) {
    leader = false;
  }
  if (leader) ++number_of_leaders_;
  member.leader = leader;
  member.name.copyMaxFrom(name);
  member.online = false;
}




//----[  addRetrievedGeosid  ]-------------------------------------------------
bool GuildInstance::addRetrievedGeosid(GeosidInstance* geosid) {
  if (geosids_.count > (members_.count / 2)) {
    return false;
  } else {
    assert(geosids_.count < Guild::MAX_GEOSIDS_OWNED);
    geosids_.add(geosid);
    return true;
  }
}




//----[  getOnlineMemberClient  ]----------------------------------------------
size_t GuildInstance::getOnlineMemberClient(AvatarID avatar_id,
                                            Client** client) {
  for (size_t i = 0; i < online_.count; ++i) {
    if (online_[i]->getLoggedIntoCharacterID() == avatar_id) {
      *client = online_[i];
      return i;
    }
  }

  // clean up this problem be taking the member offline
  Member* member = getMember(avatar_id);
  if (member) {
    assert(member->online); // should have been a precondition
    member->online = false;
  }
  assert(false && "invalid call to getOnlineMemberClient");
  *client = NULL;
  return -1;
}




//----[  getGeosidBonuses  ]---------------------------------------------------
void GuildInstance::getGeosidBonuses(float* hp_multiplier,
                                     float* mp_multiplier,
                                     float* xp_multiplier) {
  assert(hp_multiplier);
  assert(mp_multiplier);
  assert(xp_multiplier);
  *hp_multiplier = hp_multiplier_;
  *mp_multiplier = mp_multiplier_;
  *xp_multiplier = xp_multiplier_;
}



//----[  remove  ]-------------------------------------------------------------
void GuildInstance::remove(Member* member) {
  if (member->leader) {
    --number_of_leaders_;
    assert(number_of_leaders_ >= 0);
  }
  AvatarID avatar_id = member->avatar_id;
  bool online = member->online;
  members_.swapRemoveMemcpy(members_.indexOf(member));
  if (online) {
    Client* client;
    size_t online_index = getOnlineMemberClient(avatar_id, &client);
    assert(client);
    if (geosid_attack_initiator_ == client) {
      geosid_attack_initiator_ = NULL;
      attacked_geosid_ = NULL;
      geosid_attack_timer_.disable();
      geosid_attack_type_ = INVALID_GEOSID_ATTACK;
    }
    online_.swapErase(online_index, NULL);
    client->setGuild(NULL); // updates name & geosid effects
  }

  // Do we have too few members for the number of geosids we have?  If so,
  // erase the last geosid.
  if (members_.count / 2 < geosids_.count) {
    assert(geosids_.isEmpty() == false);
    size_t last_geosid_index = geosids_.count - 1;
    GeosidInstance* geosid = geosids_[last_geosid_index];
    geosid->removeOwner(); // will call this->lostGeosid()
    geosids_.swapRemove(last_geosid_index);
  }
}


//----[  isLeader  ]-----------------------------------------------------------
bool GuildInstance::isLeader(AvatarID avatar_id) {
  Member* member = getMember(avatar_id);
  return member && member->leader;
}



//----[  getMember  ]----------------------------------------------------------
GuildInstance::Member* GuildInstance::getMember(AvatarID id) {
  for (size_t i = 0; i < members_.count; ++i) {
    if (members_[i].avatar_id == id) return &members_[i];
  }
  return NULL;
}




//----[  calculateGeosidEffects  ]---------------------------------------------
void GuildInstance::calculateGeosidEffects(bool trigger_client_updates) {
  hp_multiplier_ = 1.0;
  mp_multiplier_ = 1.0;
  xp_multiplier_ = 1.0;
  for (size_t i = 0; i < geosids_.count; ++i) {
    GeosidInstance* geosid = geosids_[i];
    assert(geosid);
    float hp, mp, xp;
    bool owner
      = geosid->getOwnershipBenefits(guild_id_, &hp, &mp, &xp);
    confirm(owner) else {
      geosids_.swapRemove(i);
      calculateGeosidEffects(trigger_client_updates);
      return;
    }
    hp_multiplier_ += hp;
    mp_multiplier_ += mp;
    xp_multiplier_ += xp;
  }

  if (trigger_client_updates) {
    for (size_t i = 0; i < online_.count; ++i) {
      Client* client = online_[i];
      assert(client);
      client->combineUpdateGeosidEffectsFlag(true);
    }
  }
}




//----[  broadcast  ]----------------------------------------------------------
void GuildInstance::broadcast(NetworkPacket* packet) {
  for (size_t i = 0; i < online_.count; ++i) {
    Client* client = online_[i];
    assert(client);
    client->sendAsyncPacket(packet);
  }
}



}
}

