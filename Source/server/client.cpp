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
#include "time.h"
#include "shared/client-server/fastsync.h"
#include "shared/evidyon_network.h"

#include "shared/magic/magic_buff.h"

#include "shared/actor/actordescription.h"

#include "shared/client-server/admin.h"
#include "server/guild/globalguildmanager.h"
#include "server/guild/guildinstance.h"

#include "actorgroup.h"
#include "server/globallogger.h"
#include "shared/design/newdesign.h"

// used to access Evidyon::running so that the server can be disabled remotely
#include "main.h"

#include "server/geosid/globalgeosidmanager.h"
#include "server/geosid/geosidinstance.h"


// atan2f, sqrtf
#include <math.h>




#include "uniqueavatarnamechecker.h"
#include "shared/item/itemserverdescription.h"

#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"
#include "shared/actor/actorfastsync.h"
#include "shared/design/design.h"
#include "shared/design/monsterxp.h"

#include "server/trigger/triggerinstance.h"

#include "common/fracture/todo.h"

/*

TODO:
  - change all coordinate info to use fixed-point?
  - fill out addRedFlag
  - insert addRedFlag whenever a client-fault error occurs
  - report red flags to server

*/





namespace Evidyon {

static const Avatar::AvatarLevel AVATAR_LEVEL_CAP = 60;
static const Avatar::AvatarLevel AVATAR_LEVEL_SUPERBRAKES = 50;
static const Avatar::AvatarLevel AVATAR_LEVEL_BRAKES = 45;

static const float MAX_VISIBLE_DISTANCE_SQ = 8.0f*8.0f*1.1f;

static const float TIME_DEAD_BEFORE_RESPAWN = 10.0;
static const float MAX_NPC_INTERACTION_DISTANCE_SQ = 2.0f; // this should be larger than the client's interaction distance

std::set<unsigned int> Client::active_accounts_;

static const double PERCENT_XP_TO_PENALIZE_ON_DEATH = 0.10;

// p2p; trade, join guild, etc
static const float INTERACTION_THRESHOLD_SQ = 6.0;

//----[  Client  ]-------------------------------------------------------------
Client::Client() : item_trader_(this) {
  zero();
}





//----[  acquire  ]------------------------------------------------------------
void Client::acquire(NetworkPeer* peer) {
  zero();
  peer_ = peer;
  state_ = CONNECTED;
  peer->data = (void*)this;
}





//----[  release  ]------------------------------------------------------------
void Client::release() {

  {
    // stuff goes here
    leaveWorld();
    logoutAccount();
  }

  zero();
}






//----[  update  ]-------------------------------------------------------------
void Client::update(double time, double time_since_last_update) {
  if (state_ != LOGGED_INTO_CHARACTER) return;

  // Check this before doing player processing so that a Player that forces
  // an immediate logoff will not get another update() call.
  if (player_.leaveWorld()) {
    leaveWorld();
  } else {

    // done before player update so that equipment isn't recalculated 2x
    if (update_geosid_effects_) updateGeosidEffects();

    player_.update(time, time_since_last_update);

    NetworkPacket* packet;
    if (items_.updateInventory(&packet)) sendAsyncPacket(packet);

    if (update_actor_description_ || update_visible_enchantments_) {
      broadcastUpdateDescriptionPacket();
    }

    if (update_enchantment_icons_) sendBuffIconsUpdate();
    if (update_name_) broadcastNameUpdate();
    if (update_client_avatar_stats_) updateClientAvatarStats();
  }
}



//----[  getLevel  ]-----------------------------------------------------------
int Client::getLevel() {
  return avatar_.level;
}




//----[  giveMonsterEXP  ]-----------------------------------------------------
void Client::giveMonsterEXP(Avatar::AvatarLevel monster_intended_level,
                            Avatar::AvatarExperience amount) {
  ActorInstance* my_actor = actor_.dereference();
  assert(my_actor);
  ActorGroup* group = my_actor->getGroup();
  if (group) {
    group->shareMonsterEXP(my_actor, monster_intended_level, amount);
  } else {
    changeEXP(amount);
  }
}



//----[  shareMonsterEXP  ]----------------------------------------------------
void Client::shareMonsterEXP(Avatar::AvatarLevel monster_intended_level,
                             Avatar::AvatarLevel sum_of_party_level_squares,
                             Avatar::AvatarExperience amount) {

  todo("karl", "fix the party monster exp sharing");
  //double monster_xp_adjustment
  //  = (avatar_.level - monster_intended_level);
  //monster_xp_adjustment *= monster_xp_adjustment;
  //monster_xp_adjustment = 1 + (-0.002 * monster_xp_adjustment + 0.2);
  //monster_xp_adjustment = max(monster_xp_adjustment, 0.05); // bottom = 5%
  //amount
  // *= monster_xp_adjustment
  //    * double(avatar_.level * avatar_.level)
  //      / double(sum_of_party_level_squares);
  changeEXP(amount);
}


//----[  changeEXP  ]----------------------------------------------------------
void Client::changeEXP(Avatar::AvatarExperience amount) {
  if (isAdmin()) return;

  ++avatar_.total_monsters_killed;

  // can only gain EXP up to the amount equal to killing one monster
  // of this player's level
  const Avatar::AvatarExperience max_xp
    = Design::MONSTER_XP[avatar_.level];
  amount = max_xp > amount ? amount : max_xp;

  assert(amount >= 0);
  assert((avatar_.guild_xp_multiplier == Number(1)) || guild_);
  amount *= avatar_.guild_xp_multiplier.toFloat(); // 1 if not in guild


  // TEMPORARY:  don't give EXP to players at this level
  if (avatar_.level >= AVATAR_LEVEL_CAP) return;
  if (avatar_.level >= AVATAR_LEVEL_SUPERBRAKES) {
    amount /= 4;
  } else if (avatar_.level >= AVATAR_LEVEL_BRAKES) {
    amount /= 2;
  }

  // pay off the penalty xp amount, if it exists
  if (avatar_.penalty_experience > 0) {
    amount /= 2; // divide by two
    avatar_.penalty_experience -= min(amount, avatar_.penalty_experience);
  }

  // Need the experience type because EXP can get huge
  Avatar::AvatarExperience exp = avatar_.experience_this_level + amount;
  if (exp < avatar_.experience_to_next_level) {
    avatar_.experience_this_level = exp;
  } else { // level up!

    Avatar::AvatarLevel level = avatar_.level + 1;
    avatar_.level = level;
    avatar_.available_attribute_points++;

    ActorInstance* actor = actor_.dereference();
    assert(actor);
    if (actor) {
      actor->say("<Level Up>");
      actor->setLevel(Number(int(level)));
    }

    avatar_.calculateBaseStats();
    items_.setEquipmentChanged();
    avatar_stats_.recalculate_timer.trigger();
    combineUpdateGeosidEffectsFlag(true);
    combineUpdateClientAvatarStatsFlag(true);

    restoreHPMP();

    // reset experience
    GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
    exp -= avatar_.experience_to_next_level;
    avatar_.experience_this_level = max(exp, 0); // just for safety
    avatar_.experience_to_next_level = gamefile_manager->getLevelUpExperience(level);

    avatar_.spell_availability.copy(
      GlobalGamefileManager::singleton()->getClassSpellAvailability(
      avatar_.class_,
      avatar_.level));
    sendSpellAvailabilityUpdate();

    // save the character
    commitToDatabase(GlobalDatabaseManager::singleton());
  }
}



//----[  changeMoney  ]--------------------------------------------------------
void Client::changeMoney(int amount) {
  avatar_.money += amount;
  ASSERT(avatar_.money>0) else avatar_.money=0;
  syncCurrency();
}



//----[  combineUpdateActorDescriptionFlag  ]----------------------------------
void Client::combineUpdateActorDescriptionFlag(bool flag) {
  update_actor_description_ = update_actor_description_  || flag;
}

//----[  combineUpdateEnchantmentIconsFlag  ]----------------------------------
void Client::combineUpdateEnchantmentIconsFlag(bool flag) {
  update_enchantment_icons_ = update_enchantment_icons_  || flag;
}


//----[  combineUpdateVisibleEnchantmentsFlag  ]-------------------------------
void Client::combineUpdateVisibleEnchantmentsFlag(bool flag) {
  update_visible_enchantments_ = update_visible_enchantments_  || flag;
}


//----[  combineUpdateNameFlag  ]----------------------------------------------
void Client::combineUpdateNameFlag(bool flag) {
  update_name_ = update_name_ || flag;
}


//----[  combineUpdateGeosidEffectsFlag  ]-------------------------------------
void Client::combineUpdateGeosidEffectsFlag(bool flag) {
  update_geosid_effects_ = update_geosid_effects_ || flag;
  if (flag) {
    items_.setEquipmentChanged();
    avatar_stats_.recalculate_timer.trigger();
  }
}



//----[  combineUpdateClientAvatarStatsFlag  ]---------------------------------
void Client::combineUpdateClientAvatarStatsFlag(bool flag) {
  update_client_avatar_stats_ = update_client_avatar_stats_ || flag;
}


//----[  sendSpellAvailabilityUpdate  ]----------------------------------------
void Client::sendSpellAvailabilityUpdate() {
  using namespace Evidyon::Network::Packet::Server;
  NetworkPacket* packet = Encode::updateYourEffectAvailabilityMask(
    &avatar_.spell_availability);
  sendPacket(packet);
}




//----[  sendBuffIconsUpdate  ]------------------------------------------------
void Client::sendBuffIconsUpdate() {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_TellYourBuffIcons::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        &avatar_stats_.buff_icons);
  sendPacket(packet);
  update_enchantment_icons_ = false;
}



//----[  broadcastNameUpdate  ]------------------------------------------------
void Client::broadcastNameUpdate() {
  ActorInstance* my_actor = actor_.dereference();
  if (my_actor) {
    NetworkPacket* tell_name_packet = buildTellNamePacket();
    my_actor->getRegion()->broadcastOrphanedNetworkPacket(tell_name_packet);
  }

  update_name_ = false;
}




//----[  updateGeosidEffects  ]------------------------------------------------
void Client::updateGeosidEffects() {
  if (guild_) {
    float hp, mp, xp;
    guild_->getGeosidBonuses(&hp, &mp, &xp);
    avatar_.guild_max_hp = avatar_.max_hp.toFloat() * hp;
    avatar_.guild_max_mp = avatar_.max_mp.toFloat() * mp;
    avatar_.guild_xp_multiplier = xp;
  } else {
    avatar_.guild_max_hp = avatar_.max_hp;
    avatar_.guild_max_mp = avatar_.max_mp;
    avatar_.guild_xp_multiplier = 1;
  }
  update_geosid_effects_ = false;
  avatar_stats_.recalculate_timer.trigger();
}




//----[  getAvatarStats  ]-----------------------------------------------------
const Server::AvatarStats* Client::getAvatarStats() const {
  assert(state_ == LOGGED_INTO_CHARACTER);
  return &avatar_stats_;
}

//----[  getAvatarStats  ]-----------------------------------------------------
const Server::AvatarInstance* Client::getAvatarInstance() const {
  assert(state_ == LOGGED_INTO_CHARACTER);
  return &avatar_;
}


//----[  resurrect  ]----------------------------------------------------------
void Client::resurrect() {
  if (player_.resurrect()) {
    // send a resurrection message to the client indicating what they can do
    using namespace Evidyon::Network::Packet;
    sendAsyncPacket(
      NetMsg_YouHaveBeenResurrected::writePacket(ENET_PACKET_FLAG_RELIABLE));
  }
}


//----[  applyBuff  ]----------------------------------------------------------
bool Client::applyBuff(const Magic::Magic_Buff* buff,
                       const Server::AvatarInstance* caster_avatar) {
  bool affected = false;
  for (int i = 0; i < Magic::NUMBER_OF_BUFFS; ++i) {
    if (buff->enabled_mask.get((Magic::Buff)i) == false) continue;
    float buff_value = avatar_enchantments_.buff_values[i];
    float interpolated
      = (buff->buff_values[i] - buff_value) / 2 + buff_value;

    // is the new value more than 1/8th different from the current value?
    if (fabsf(interpolated - buff_value) >
        (fabsf(buff->buff_values[i]) / 8.0)) {
      avatar_enchantments_.buff_values[i] = interpolated;
      avatar_enchantments_.buff_timers[i].set(buff->duration);
      affected = true;
    } else if (fabsf(buff->buff_values[i] - buff_value) > 0.00001) {
      avatar_enchantments_.buff_values[i] = buff->buff_values[i];
      avatar_enchantments_.buff_timers[i].set(buff->duration);
      affected = true;
    }
  }
  if (affected) avatar_stats_.recalculate_timer.trigger();
  return affected;
}


//----[  canBeginTradeWith  ]--------------------------------------------------
bool Client::canBeginTradeWith(Client* other_client) const {
  static const float TRADE_THRESHOLD_SQ = 6.0f;
  assert(other_client);
  const ActorInstance* my_actor = actor_.dereference();
  const ActorInstance* other_actor = other_client->actor_.dereference();
  return this != other_client &&
         other_client->player_.isLookingForTrade() &&
         my_actor->distanceToSq(other_actor) < TRADE_THRESHOLD_SQ;
}


//----[  beginTrade  ]---------------------------------------------------------
void Client::beginTrade(Client* other_client) {
  item_trader_.beginTrade(&other_client->item_trader_);
}


//----[  cancelTrade  ]--------------------------------------------------------
void Client::cancelTrade() {
  item_trader_.cancelTrade();
}



//----[  canJoinGuildOf  ]-----------------------------------------------------
bool Client::canJoinGuildOf(Client* other_client) const {
  assert(other_client);
  const ActorInstance* my_actor = actor_.dereference();
  assert(my_actor);
  const ActorInstance* other_actor = other_client->actor_.dereference();
  confirm(other_actor) else return false;
  return this != other_client &&
         other_client->player_.isInvitingGuildMembers() &&
         my_actor->distanceToSq(other_actor) < INTERACTION_THRESHOLD_SQ;
}


//----[  joinGuild  ]----------------------------------------------------------
bool Client::joinGuild(Client* leader) {
  assert(leader->player_.isInvitingGuildMembers());

  // make sure we are not in a guild
  if (guild_) {
    guild_->leave(character_id_);
  }

  kg::CharBuffer<Guild::MAX_NAME_LENGTH> create_guild_name;
  if (leader->player_.guildMemberJoined(create_guild_name.characters,
                                        create_guild_name.SIZE)) {
    Evidyon::Server::GlobalGuildManager* guild_manager
      = Evidyon::Server::GlobalGuildManager::singleton();
    confirm(!leader->guild_) else {
      guild_manager->leaveGuild(leader->guild_, leader->character_id_);
    }

    // Create the guild.  The leader is automatically added to the guild as
    // its founding member and setGuild is called.
    Evidyon::Server::GuildInstance* guild =
      guild_manager->createGuild(leader->character_id_,
                                 leader,
                                 create_guild_name.characters);
    if (!guild) {
      const ActorInstance* leader_actor = leader->getActor()->dereference();
      confirm(leader_actor) else return false;
      leader_actor->say("<Couldn't Start Guild>");
      return false;
    }
  }
  confirm(leader->guild_) else return false;
  return (leader->guild_->lead(leader->character_id_, character_id_, this));
}


//----[  setGuild  ]-----------------------------------------------------------
void Client::setGuild(Evidyon::Server::GuildInstance* guild) {
  if (guild != guild_) {
    combineUpdateNameFlag(true);
    combineUpdateGeosidEffectsFlag(true);
    guild_ = guild;
    if (guild) avatar_.guild = guild->getGuildID();
  }
}




//----[  getGuild  ]-----------------------------------------------------------
Evidyon::Server::GuildInstance* Client::getGuild() {
  return guild_;
}




//----[  logSnapshot  ]--------------------------------------------------------
void Client::logSnapshot() {

}



//----[  sendDisconnectionPacket  ]--------------------------------------------
void Client::sendDisconnectionPacket() {
  enet_peer_disconnect(peer_, 0);
  leaveWorld();
  logoutAccount();
}





//----[  DynamicArraySlowSyncFunctionoid  ]------------------------------------
class DynamicArraySlowSyncFunctionoid
  : public DynamicArray<Evidyon::Actor::ActorSlowSync>::ElementFunctionoid {
public:
  virtual bool hasMoreElements() {
    return i != end;
  }
  virtual bool fill(size_t count, Evidyon::Actor::ActorSlowSync* element, bool* failed) {
    if (!element) return true;
    if (i == end) return false;
    ActorInstance* actor = *i;
    ++i;
    if (actor == client_actor || !actor || // THIS IS A HACK--ACTORS SHOULD ALWAYS EXIST! (but sometimes they don't...)
        !actor->canSee(client_actor)) return fill(count, element, failed);
    actor->getSlowSyncData(element);
    return true;
  }
  ActorInstance* client_actor;
  WorldRegion::ActorSet::iterator i, end;
};



//----[  sendSlowSync  ]-------------------------------------------------------
void Client::sendSlowSync(float time_since_last_sync) {
  ActorInstance* actor = actor_.dereference();
  if (!actor) return;
  WorldRegion* local_region = actor->getRegion();
  if (local_region) {
    DynamicArraySlowSyncFunctionoid functionoid;
    functionoid.client_actor = actor;
    functionoid.i = local_region->nearbyActorsBegin();
    functionoid.end = local_region->nearbyActorsEnd();
    while (functionoid.i != functionoid.end) {
      using namespace Evidyon::Network::Packet;
      NetworkPacket* packet = NetMsg_SlowSync::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        time_since_last_sync,
        &DynamicArray<Evidyon::Actor::ActorSlowSync>(&functionoid));
      assert(packet);
      sendAsyncPacket(packet);
    }
    // write the rest using the overflow type--negative time since last sync
    while (functionoid.i != functionoid.end) {
      using namespace Evidyon::Network::Packet;
      NetworkPacket* packet = NetMsg_SlowSync::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        -time_since_last_sync,
        &DynamicArray<Evidyon::Actor::ActorSlowSync>(&functionoid));
      assert(packet);
      sendAsyncPacket(packet);
    }
  }
}




//----[  sendFastSync  ]-------------------------------------------------------
void Client::sendFastSync(float time_since_last_sync, int sync_id) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  ActorInstance* actor = actor_.dereference();
  if (!actor) return;

  Evidyon::Actor::ActorFastSync sync;
  sync.time_since_last_sync = time_since_last_sync;
  actor->getFastSyncData(&sync);

  int sync_stat_type = sync_id % 5;
  sync.stat_type_reset = sync_stat_type == 0;
  {
    Number stat_value;
    switch (sync_stat_type) {
      case 0:
      case 2:
        stat_value = avatar_stats_.hp / avatar_stats_.max_hp;
        break;
      case 1:
      case 3:
        stat_value = avatar_stats_.mp / avatar_stats_.max_mp;
        break;
      case 4:
        stat_value = double(avatar_.experience_this_level) / double(avatar_.experience_to_next_level);
        break;
    }

    // Limit to [0, 1)
    stat_value.int_value |= (stat_value.wholePart() != 0) ? 0x0000FFFF : 0x00000000;
    stat_value = FixedPoint::maxfast<16>(stat_value, 0x00000000);

    // Pack into 7 bits
    sync.stat_value = static_cast<unsigned>(stat_value.int_value >> (16 - 7));
  }

  sendAsyncPacket(NetMsg_FastSync::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    sync));

  // push the pker once every 2 seconds
  if (sync_id % ((int)(2.0/Network::FAST_SYNC_PERIOD)) == 0) {
    addAttackerToHistory(0,NULL);
  }
}




//----[  giveItem  ]-----------------------------------------------------------
bool Client::giveItem(ItemPointer* item) {
  return items_.giveItem(item, NULL);
}





//----[  giveItemOrDropOnGround  ]---------------------------------------------
void Client::giveItemOrDropOnGround(Evidyon::ItemPointer* item_pointer) {
  if (!items_.giveItem(item_pointer, NULL)) {
    // couldn't be completely given--drop the rest on ground!
    ActorInstance* my_actor = actor_.dereference();
    if (!my_actor) goto DEALLOCATE_ITEM;
    todo("karl","Inventory Full string should be pulled into netmsg");
    my_actor->say("Inventory Full");
    int x, y;
    WorldRegion* region;
    my_actor->getStrictRegion(&x, &y, &region);
    if (!region) goto DEALLOCATE_ITEM;
    region->addItemOnGround(true, x, y, *item_pointer);
  }

  return;

DEALLOCATE_ITEM: // something went wrong
  GlobalItemManager::singleton()->releaseItem(item_pointer);
}




//----[  getItems  ]-----------------------------------------------------------
ClientItemsInterface* Client::getItems() {
  return &items_;
}




//----[  sendFullInventoryUpdate  ]--------------------------------------------
void Client::sendFullInventoryUpdate() {
  NetworkPacket* packet;
  if (items_.updateInventory(&packet)) {
    sendAsyncPacket(packet);
  }
}





//----[  moveTo  ]-------------------------------------------------------------
void Client::moveTo(World::MapIndex map_index, float x, float y) {
  if (state_ != LOGGED_INTO_CHARACTER) return;
  ActorInstance* actor = actor_.dereference();
  CONFIRM(actor) else return;
  actor->changeMap(GlobalWorldManager::singleton()->getMapFromIndex(map_index), x, y);
}




//----[  loggedIntoAvatar  ]---------------------------------------------------
bool Client::loggedIntoAvatar(const std::string& reduced_name) const {
  if (state_ != LOGGED_INTO_CHARACTER) return false;
  std::string my_reduced_name;
  Evidyon::UniqueAvatarNameChecker::reduceName(avatar_.name, &my_reduced_name);
  return my_reduced_name.compare(reduced_name) == 0;
}




//----[  handleNetworkPacket  ]------------------------------------------------
void Client::handleNetworkPacket(NetworkPacket* packet) {
  switch(state_) {
    case CONNECTED_VERSIONCONFIRMED:
    case CONNECTED:       handleNetworkPacketWhenConnected(packet); break;
    case LOGGED_INTO_ACCOUNT:   handleNetworkPacketWhenLoggedIntoAccount(packet); break;
    case ENTERING_WORLD:    handleNetworkPacketWhenEnteringWorld(packet); break;
    case LOGGED_INTO_CHARACTER: handleNetworkPacketWhenLoggedIntoCharacter(packet); break;

    default:
      DEBUG_WARNING("Client in an invalid state received a packet");
      break;
  }
}



//----[  sendPacket  ]---------------------------------------------------------
void Client::sendPacket(NetworkPacket* packet) {
  CONFIRM(packet) else return;
  GlobalNetworkManager::sendPacket(this, packet);
  GlobalNetworkManager::handleOrphanedPacket(packet);
}



//----[  sendAsyncPacket  ]----------------------------------------------------
void Client::sendAsyncPacket(NetworkPacket* packet) {
  CONFIRM(packet) else return;
  GlobalNetworkManager::sendAsyncPacket(this, packet);
  GlobalNetworkManager::handleOrphanedPacket(packet);
}



  
//----[  sendAsyncPacket_UpdateWorldRegion  ]----------------------------------
void Client::sendAsyncPacket_UpdateWorldRegion(World::RegionID new_region_id) {
  using namespace Evidyon::Network::Packet::Server;
  sendAsyncPacket(Encode::updateLocalWorldRegion(new_region_id));
}


//----[  sendAsyncPacket_ChangeMap  ]------------------------------------------
void Client::sendAsyncPacket_ChangeMap(World::MapIndex new_map_id) {
  using namespace Evidyon::Network::Packet::Server;
  sendAsyncPacket(Encode::changeMap(new_map_id));
}







//----[  getClient  ]----------------------------------------------------------
Client* Client::getClient() const {
  return const_cast<Client*>(this);
}




//----[  getName  ]------------------------------------------------------------
const char* Client::getName() {
  return avatar_.name;
}



//----[  tellName  ]-----------------------------------------------------------
void Client::tellName(Client* client) {
  client->sendAsyncPacket(buildTellNamePacket());
}



//----[  syncToDatabase  ]-----------------------------------------------------
void Client::syncToDatabase(GlobalDatabaseManager* db) {
  commitToDatabase(db);
  if (state_ == LOGGED_INTO_CHARACTER) {
    CONFIRM(items_.commit());
  }
}




//----[  onSafeZone  ]---------------------------------------------------------
bool Client::onSafeZone() const {
  return getLocalZoneRules() == Zone::ZONERULES_TOWN;
}



//----[  getLocalZoneRules  ]--------------------------------------------------
Zone::ZoneRules Client::getLocalZoneRules() const {
  if (isAdmin()) {
    // admin chars are always "on the safe zone"
    return Zone::ZONERULES_TOWN;
  }

  const ActorInstance* actor = actor_.dereference();
  if (!actor) {
    todo("karl","some sort of error; actor should always be valid");
    return Zone::ZONERULES_NORMAL;
  }

  const Server::MapLocationInfo* location = actor->getMapLocationInfo();
  if (!location) {
    todo("karl","some sort of error; location should always be valid (but this has been hit in the past!)");
    return Zone::ZONERULES_NORMAL;
  }
  Zone::ZoneRules rules = location->zone_rules;
  Number x, z;
  actor->getPosition(&x, &z);
  Server::GeosidInstance* geosid =
    Server::GlobalGeosidManager::singleton()->getGeosidIfInRange(
      actor->getMapLocationInfo()->nearby_geosid,
      actor->getMapIndex(),
      x,
      z);
  if (geosid) rules = geosid->zoneRules();

  return rules;
}



//----[  zero  ]---------------------------------------------------------------
void Client::zero() {
  state_ = INVALID;
  peer_ = NULL;
  account_id_ = 0;
  character_id_ = 0;
  items_.zero();
  item_trader_.cancelTrade();
  memset(attacker_history_, 0, sizeof(attacker_history_));
  memset(active_quests_, 0, sizeof(active_quests_));
  update_actor_description_ = false;
  update_enchantment_icons_ = false;
  update_visible_enchantments_ = false;
  update_name_ = false;
  update_geosid_effects_ = false;
  update_client_avatar_stats_ = false;
  memset(portal_marks_, 0, sizeof(portal_marks_));
  guild_ = NULL;
  is_admin_ = false;
  hide_helmet_ = false;
}




//----[  commitToDatabase  ]---------------------------------------------------
void Client::commitToDatabase(GlobalDatabaseManager* database) {
  if (state_ != LOGGED_INTO_CHARACTER) return;
  ActorInstance* actor = actor_.dereference();
  assert(actor);
  if (!actor) return;
  float x, y;
  World::MapHash map_index = World::INVALID_MAP_INDEX;
  actor->getPosition(&x,&y);
  WorldRegion* local_region = actor->getRegion();
  if (local_region) {
    map_index = local_region->getMap()->getIndex();
  }
  database->commitCharacterData(account_id_,
                                character_id_,
                                x,
                                y,
                               &avatar_,
                                avatar_stats_.hp.wholePart(),
                                avatar_stats_.mp.wholePart(),
                                is_admin_,
                                map_index,
                                portal_marks_,
                                hide_helmet_);

  // log this client in a snapshot
  log_snapshot(character_id_,
               account_id_,
               x,
               y,
               map_index,
               &avatar_,
               &avatar_stats_);
}





//----[  handleNetworkPacketWhenConnected  ]-----------------------------------
void Client::handleNetworkPacketWhenConnected(NetworkPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  // The packet stored here during processing is sent to the user, if it is valid, at the end of this method
  ENetPacket* reply_packet = 0;

  // Get the message from the packet
  Message msg;
  decodeMessageType(packet, &msg);


  // Handle the message
  switch (msg) {
  case NETMSG_TELLCLIENTVERSIONINFO: NetMsg_TellClientVersionInfo(packet, &reply_packet); break;
  case NETMSG_CREATEACCOUNT: NetMsg_CreateAccount(packet, &reply_packet); break;
  case NETMSG_ACCESSACCOUNT: NetMsg_AccessAccount(packet, &reply_packet); break;

    default:
      DEBUG_WARNING("Received invalid packet");
      break;
  }

  // If the reply packet is valid, send it to the connected peer
  if (reply_packet != NULL) sendPacket(reply_packet);
}


void Client::handleNetworkPacketWhenLoggedIntoAccount(NetworkPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  // The packet stored here during processing is sent to the user, if it is valid, at the end of this method
  ENetPacket* reply_packet = 0;

  // Get the message from the packet
  Message msg;
  decodeMessageType(packet, &msg);

  GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();

  // Handle the message
  switch(msg) {
    case NETMSG_CREATECHARACTER: NetMsg_CreateCharacter(database, packet, &reply_packet); break;
    case NETMSG_DELETECHARACTER: NetMsg_DeleteCharacter(database, packet, &reply_packet); break;
    case NETMSG_ASKACCOUNTINFO: NetMsg_AskAccountInfo(database, packet, &reply_packet); break;
    case NETMSG_ASKCHARACTERNAME: NetMsg_AskCharacterName(database, packet, &reply_packet); break;
    case NETMSG_ENTERWORLD: NetMsg_EnterWorld(database, packet, &reply_packet); break;
    case NETMSG_LOGOUTACCOUNT: NetMsg_LogoutAccount(database, packet, &reply_packet); break;

    default:
      DEBUG_WARNING("Received invalid packet; type=%i from 0x%x", msg, this);
      break;
  }

  // If the reply packet is valid, send it to the connected peer
  if (reply_packet != NULL) sendPacket(reply_packet);
}



void Client::handleNetworkPacketWhenEnteringWorld(NetworkPacket* packet) {

  ActorInstance* actor = actor_.dereference();
  CONFIRM(actor) else { leaveWorld(); return; }

  state_ = LOGGED_INTO_CHARACTER;
  handleNetworkPacketWhenLoggedIntoCharacter(packet);
}



void Client::handleNetworkPacketWhenLoggedIntoCharacter(NetworkPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  ActorInstance* actor = actor_.dereference();
  CONFIRM(actor) else { leaveWorld(); return; }

  // The packet stored here during processing is sent to the user, if it is valid, at the end of this method
  ENetPacket* reply_packet = 0;

  // Get the message from the packet
  Message msg;
  decodeMessageType(packet, &msg);

  if (isAdmin() &&
      msg >= NETMSG_ADMIN_FIRSTMESSAGE) {
    using namespace ::Evidyon::Network::Packet;
    using namespace ::Evidyon::Network::Packet::Server;
    switch (msg) {

      case NETMSG_ADMIN_CREATEITEM: {
        int type;
        if (!Decode::admin_createItem(packet, &type)) break;
        ItemPointer item_ptr;
        GlobalItemManager::singleton()->acquireNewItem(1, type, &item_ptr);
        if (!giveItem(&item_ptr)) {
          GlobalItemManager::singleton()->releaseItem(&item_ptr);
        } else {
          sendFullInventoryUpdate();
          sendPacket(Encode::admin_serverMessage("Done"));
        }
      } break;

      case NETMSG_ADMIN_SQLQUERY: {
        std::string query;
        if (!Decode::admin_sqlQuery(packet, &query)) break;
        GlobalDatabaseManager* db = GlobalDatabaseManager::singleton();
        sqlite3_stmt* stmt = NULL;
        if (!db->adminQueryOpen(query, &stmt)) {
          db->adminQueryGetError(&query);
          reply_packet = Encode::admin_serverMessage(query);
        } else {
          int records = 0;
          while (db->adminQueryStep(stmt, &query)) {
            ++records;
            sendPacket(Encode::admin_serverMessage(query));
          }
          { // send back a result summary
            char records_text[16];
            query = "Done.  ";
            query.append(_itoa(records, records_text, 10));
            query.append(" record(s)");
            sendPacket(Encode::admin_serverMessage(query));
          }
          db->adminQueryClose(&stmt);
        }
      } break;

      case NETMSG_ADMIN_SIMPLECOMMAND: {
        AdminSimpleCommand command;
        if (!Decode::admin_simpleCommand(packet, &command)) break;
        switch (command) {
        case ADMINSIMPLECOMMAND_BACKUP:
          sendPacket(Encode::admin_serverMessage("starting backup"));
          GlobalDatabaseManager::singleton()->startBackup();
          break;
        case ADMINSIMPLECOMMAND_TURNOFF:
          Evidyon::running = false;
          break;
        case ADMINSIMPLECOMMAND_ONLINE: {
          GlobalClientManager::singleton()->admin_tellOnline(this);
          } break;
        case ADMINSIMPLECOMMAND_COMMIT: {
          items_.commit();
          sendFullInventoryUpdate();
          storageOpen();
          } break;
        case ADMINSIMPLECOMMAND_RESPAWNALL: {
          GlobalClientManager::singleton()->respawnAll();
          } break;
        case ADMINSIMPLECOMMAND_UNITTEST_DUPEITEM: {
            ItemPointer item_pointer;
            if (items_.getInventoryItem(0, NULL, &item_pointer)) {
              if (items_.addToInventory(item_pointer, NULL)) {
                char buffer[512];
                sprintf_s(buffer, 512, "duped item %lld", item_pointer.dereferenceAssumingValid()->getUniqueID());
                sendPacket(Encode::admin_serverMessage(buffer));
                sendFullInventoryUpdate();
              }
            }
          } break;
        case ADMINSIMPLECOMMAND_RESOURCES: {
          char buffer[2048];
          GlobalItemManager::singleton()->printDebugStats(buffer, 2048);
          sendPacket(Encode::admin_serverMessage(buffer));
          GlobalClientManager::singleton()->printDebugStats(buffer, 2048);
          sendPacket(Encode::admin_serverMessage(buffer));
          GlobalActorManager::singleton()->printDebugStats(buffer, 2048);
          sendPacket(Encode::admin_serverMessage(buffer));
          GlobalWorldManager::singleton()->printDebugStats(buffer, 2048);
          sendPacket(Encode::admin_serverMessage(buffer));
          } break;
        case ADMINSIMPLECOMMAND_RESETATTRIBUTES: {
          GlobalDatabaseManager::singleton()->avatarReassignAllAttributes();
          sendPacket(Encode::admin_serverMessage("Done"));
          } break;
        case ADMINSIMPLECOMMAND_GODMODE: {
          //avatar_stats_.hp = 50000;
          //avatar_stats_.mp = 50000;
          //changeGeonite(+5000);
          //avatar_.money += 100000;
          //

          //{ // update the spell availability based on the current level
          //  // and preserving any additional spells granted by items
          //  GlobalGamefileManager* gamefile_manager =
          //      GlobalGamefileManager::singleton();
          //  for (int i = 0; i < Avatar::NUMBER_OF_AVATAR_CLASSES; ++i) {
          //    avatar_.spell_availability.unionWith(
          //        gamefile_manager->getClassSpellAvailability(
          //          Avatar::AvatarClass(i),
          //          Evidyon::Avatar::HIGHEST_AVATAR_LEVEL));
          //  }
          //}

          //sendSpellAvailabilityUpdate();
          //sendPacket(Encode::admin_serverMessage("Done"));

          //combineUpdateClientAvatarStatsFlag(true);
          //syncCurrency();

          } break;

        case ADMINSIMPLECOMMAND_QUERYCHARACTERSTATISTICS: {
          GlobalDatabaseManager* db = GlobalDatabaseManager::singleton();
          for (int class_type = 0; class_type < Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES; ++class_type) {
            for (int level = Evidyon::Avatar::HIGHEST_AVATAR_LEVEL; level > 0; --level) {
              std::string stats_text;
              db->adminQueryCharacterStatistics(class_type, level, &stats_text);
              char buffer[512];
              sprintf_s(buffer, 512, "%3i - %s", level, stats_text.c_str());
              sendPacket(Encode::admin_serverMessage(buffer));
            }
            sendPacket(Encode::admin_serverMessage(Evidyon::Avatar::AvatarClassString((Evidyon::Avatar::AvatarClass)class_type)));
          }
          } break;

        case ADMINSIMPLECOMMAND_STRESSTESTS: {
          // touch a 9x9 area of world regions
          int rx = actor->getRegion()->getRegionX();
          int max_rx = rx + 30; rx -= 30;
          int ry = actor->getRegion()->getRegionY();
          int max_ry = ry + 30; ry -= 30;
          Map* map = actor->getMap();
          for (; rx < max_rx; ++rx) {
            for (; ry < max_ry; ++ry) {
              map->acquireBorderedRegionFromPoint((rx + 0.5f) * World::REGION_SIZE,
                                                  (ry + 0.5f) * World::REGION_SIZE);
            }
          }

          {
            static const int SQRT_NUMBER_OF_STRESS_ITEMS = 10;
            int x, y;
            float fx, fy;
            actor->getPosition(&fx, &fy);
            x = fx - SQRT_NUMBER_OF_STRESS_ITEMS / 2;
            int max_x = x + SQRT_NUMBER_OF_STRESS_ITEMS / 2;
            y = fy - SQRT_NUMBER_OF_STRESS_ITEMS / 2;
            GlobalItemManager* item_manager = GlobalItemManager::singleton();
            const unsigned int number_of_item_types = GlobalItemManager::singleton()->getNumberOfItemTypes();
            for (int i = 0; i < SQRT_NUMBER_OF_STRESS_ITEMS*SQRT_NUMBER_OF_STRESS_ITEMS; ++i) {
              if (x > max_x) { ++y; x -= SQRT_NUMBER_OF_STRESS_ITEMS; }
              ItemPointer item;
              item_manager->acquireNewItem(1, i%number_of_item_types, &item);
              WorldRegion* region = map->acquireRegionFromPoint(x, y);
              items_.giveItem(&item, NULL);
              items_.dropItemOnGround(region, x, y, 0);
              ++x;
            }
          }
          } break;

        case ADMINSIMPLECOMMAND_REPACKITEMS: {
          GlobalDatabaseManager::singleton()->itemRepackAllIDs();
          sendPacket(Encode::admin_serverMessage("Repacked item IDs"));
          } break;

        case ADMINSIMPLECOMMAND_UNITTEST_DROPONDEATH: {
          float fx, fy;
          actor->getPosition(&fx, &fy);
          items_.dropItemsOnDeath(getNumberOfEquippedItemsToDropOnDeath(),
                                  actor->getMap(),
                                  int(fx),
                                  int(fy));
          } break;

        case ADMINSIMPLECOMMAND_RESETALLPKS: {
          GlobalDatabaseManager::singleton()->avatarResetAllPKs(Evidyon::Time::hour_counter);
          char reply_message[128];
          sprintf_s(reply_message, 128, "Reset all PK counts to hour %lu", Evidyon::Time::hour_counter);
          sendPacket(Encode::admin_serverMessage(reply_message));
          } break;
        }
      } break;

      case NETMSG_ADMIN_SETALIGNMENT: {
        int alignment;
        if (!NetMsg_Admin_SetAlignment::readPacket(packet, &alignment)) break;
        setAlignmentValue(alignment);
        } break;

      //case NETMSG_ADMIN_UPLOADREPORTS: {
      //} break;
    }
  } else {

    // Handle the message
    switch(msg) {
    case NETMSG_LEAVEWORLD:     NetMsg_LeaveWorld(actor, packet, &reply_packet); break;
    case NETMSG_REQUEST_MOVE_STREAMING: NetMsg_RequestMoveStreaming(actor, packet, &reply_packet);  break;
    case NETMSG_CAST_SPELL:     NetMsg_CastSpell(actor, packet, &reply_packet); break;
    case NETMSG_ASKACTORFULLDESCRIPTIONS: NetMsg_AskActorFullDescriptions(actor, packet, &reply_packet); break;
    case NETMSG_REQUEST_MOVE:   NetMsg_RequestMove(actor, packet, &reply_packet); break;
    case NETMSG_DROPINVENTORYITEMS:   NetMsg_DropInventoryItems(actor, packet, &reply_packet); break;
    case NETMSG_DROP_CURRENCY:    NetMsg_Drop_Currency(actor, packet, &reply_packet); break;
    case NETMSG_USE_ITEM:       NetMsg_UseItem(actor, packet, &reply_packet);  break;
    case NETMSG_REQUEST_SPEAK:  NetMsg_Request_Speak(actor, packet, &reply_packet);  break;
    case NETMSG_STORAGE_WITHDRAW: NetMsg_StorageWithdraw(actor, packet, &reply_packet); break;
    case NETMSG_STORAGE_DEPOSIT:  NetMsg_StorageDeposit(actor, packet, &reply_packet); break;
    //case NETMSG_GEOSID_SACRIFICE: NetMsg_GeosidSacrifice(actor, packet, &reply_packet); break;
    case NETMSG_TRADE_ADDITEM:    NetMsg_TradeAddItem(actor, packet, &reply_packet); break;
    case NETMSG_TRADE_SETCURRENCY:NetMsg_Trade_SetCurrency(actor, packet, &reply_packet); break;
    case NETMSG_CHANGEEQUIPPEDSTATE:  NetMsg_ChangeEquippedState(actor, packet, &reply_packet);  break;
    case NETMSG_ASKACTORNAME:     NetMsg_AskActorName(actor, packet, &reply_packet); break;
    case NETMSG_ASKMYAVATARSTATS: NetMsg_AskMyAvatarStats(actor, packet, &reply_packet); break;
    case NETMSG_STORAGE_OPEN:     NetMsg_StorageOpen(actor, packet, &reply_packet); break;
    case NETMSG_TRADE_REMOVEITEM: NetMsg_TradeRemoveItem(actor, packet, &reply_packet); break;
    case NETMSG_TRADE_ACCEPT:     NetMsg_TradeAccept(actor, packet, &reply_packet); break;
    case NETMSG_TRADE_WITHDRAW:   NetMsg_TradeWithdraw(actor, packet, &reply_packet); break;
    case NETMSG_TRADE_CANCEL:     NetMsg_TradeCancel(actor, packet, &reply_packet); break;
    case NETMSG_SPLITSTACKEDITEM: NetMsg_SplitStackedItem(actor, packet, &reply_packet); break;
    case NETMSG_STOPACTION:       NetMsg_StopAction(actor, packet, &reply_packet); break;

    case NETMSG_GUILD_CREATE: NetMsg_Guild_Create(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_LEAD: NetMsg_Guild_Lead(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_JOIN: NetMsg_Guild_Join(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_LEAVE: NetMsg_Guild_Leave(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_PROMOTE: NetMsg_Guild_Promote(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_DEMOTE: NetMsg_Guild_Demote(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_CHAT: NetMsg_Guild_Chat(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_KICK: NetMsg_Guild_Kick(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_MEMBERS: NetMsg_Guild_Members(actor, packet, &reply_packet); break;

    case NETMSG_BAZAAR_OPEN:            NetMsg_Bazaar_Open(actor, packet, &reply_packet); break;
    case NETMSG_BAZAAR_CREATELISTING:   NetMsg_Bazaar_CreateListing(actor, packet, &reply_packet); break;
    case NETMSG_BAZAAR_RETRACTLISTING:  NetMsg_Bazaar_RetractListing(actor, packet, &reply_packet); break;
    case NETMSG_BAZAAR_REDEEMLISTING:   NetMsg_Bazaar_RedeemListing(actor, packet, &reply_packet); break;
    case NETMSG_BAZAAR_PURCHASELISTING: NetMsg_Bazaar_PurchaseListing(actor, packet, &reply_packet); break;
    case NETMSG_BAZAAR_GETLISTINGS:     NetMsg_Bazaar_GetListings(actor, packet, &reply_packet); break;

    case NETMSG_QUEST_TRIGGER: NetMsg_Quest_Trigger(actor, packet, &reply_packet); break;
    case NETMSG_QUEST_OFFER_RESPONSE: NetMsg_Quest_OfferResponse(actor, packet, &reply_packet); break;
    case NETMSG_QUEST_GIVEUP: NetMsg_Quest_GiveUp(actor, packet, &reply_packet); break;

    case NETMSG_GUILD_ASKGEOSIDS: NetMsg_Guild_AskGeosids(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_SETHOME: NetMsg_Guild_SetHome(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_HOME: NetMsg_Guild_Home(actor, packet, &reply_packet); break;
    case NETMSG_GUILD_ABANDON: NetMsg_Guild_Abandon(actor, packet, &reply_packet); break;
    case NETMSG_GEOSID_DROPSACRIFICE: NetMsg_Geosid_DropSacrifice(actor, packet, &reply_packet); break;
    case NETMSG_GEOSID_BURST: NetMsg_Geosid_Burst(actor, packet, &reply_packet); break;
    case NETMSG_GEOSID_BURSTATTACKGEOSID: NetMsg_Geosid_BurstAttackGeosid(actor, packet, &reply_packet); break;
    case NETMSG_GEOSID_BURSTSPELL: NetMsg_Geosid_BurstSpell(actor, packet, &reply_packet); break;
    case NETMSG_GEOSID_BURSTUSEGEOSID: NetMsg_Geosid_BurstUseGeosid(actor, packet, &reply_packet); break;
    case NETMSG_GEOSID_GETOWNER: NetMsg_Geosid_GetOwner(actor, packet, &reply_packet); break;

    case NETMSG_PARTY_CHAT: NetMsg_Party_Chat(actor, packet, &reply_packet); break;
    case NETMSG_PARTY_LEAD: NetMsg_Party_Lead(actor, packet, &reply_packet); break;
    case NETMSG_PARTY_JOIN: NetMsg_Party_Join(actor, packet, &reply_packet); break;
    case NETMSG_PARTY_LEAVE: NetMsg_Party_Leave(actor, packet, &reply_packet); break;
    case NETMSG_PARTY_KICK: NetMsg_Party_Kick(actor, packet, &reply_packet); break;
    case NETMSG_PARTY_CLOSE: NetMsg_Party_Close(actor, packet, &reply_packet); break;

    case NETMSG_RESURRECT_STANDUP: NetMsg_Resurrect_StandUp(actor, packet, &reply_packet); break;
    case NETMSG_RESURRECT_RESPAWN: NetMsg_Resurrect_Respawn(actor, packet, &reply_packet); break;

    case NETMSG_MARK: NetMsg_Mark(actor, packet, &reply_packet); break;
    case NETMSG_AUTOTARGET_ALL: NetMsg_AutoTarget_All(actor, packet, &reply_packet); break;
    case NETMSG_AUTOTARGET_MONSTERS: NetMsg_AutoTarget_Monsters(actor, packet, &reply_packet); break;

    default:
      DEBUG_WARNING("Received invalid packet");
      break;
    }
  }

  // If the reply packet is valid, send it to the connected peer
  if (reply_packet != 0) sendPacket(reply_packet);
}




//----[  leaveWorld  ]---------------------------------------------------------
void Client::leaveWorld() {
  using namespace ::Evidyon::Network::Packet::Server;
  if (state_ != LOGGED_INTO_CHARACTER) return;
  if (guild_) {
    Server::GlobalGuildManager* guild_manager
      = Server::GlobalGuildManager::singleton();
    guild_manager->guildMemberLoggedOff(guild_, character_id_, this);
    guild_ = NULL;
  }
  item_trader_.cancelTrade();

  // Log this client out of the world
  sendPacket(Encode::leaveWorldConfirmed());
  GlobalClientManager::singleton()->leftWorld(this);

  // Free player-interaction data.  This must be done before the
  // actor is released so that player-controlled effects (portals, projectiles,
  // etc.) can all be destroyed.
  player_.release();

  // Save the client's player information to the database.  This must be done
  // before the actor is erased, because it needs the actor's current world
  // coordinates.
  commitToDatabase(GlobalDatabaseManager::singleton());
  items_.checkIn();

  // Erase the actor
  ActorInstance* actor = actor_.dereference();
  assert(actor);
  if (actor != NULL) {
    actor->broadcastDestroyActorPacket();
    GlobalActorManager* actor_manager = GlobalActorManager::singleton();
    WorldRegion* local_region = actor->getRegion();
    actor_manager->releaseActor(&actor_); // will cause the actor to leave
    local_region->leaveWorld(this); // Remove client from the world
  }

  // Log out of the world and retreat to account screen
  state_ = LOGGED_INTO_ACCOUNT;
  character_id_ = 0;
}





//----[  logoutAccount  ]------------------------------------------------------
void Client::logoutAccount() {
  if (state_ != LOGGED_INTO_ACCOUNT) return;

  log_account_logout(account_id_);

  active_accounts_.erase(account_id_);

  // Log out of the account
  state_ = CONNECTED;
  account_id_ = 0;
}




//----[  changeHP  ]-----------------------------------------------------------
Number Client::changeHP(ActorInstance* source, bool magical, Number amount) {

  // can't affect admins
  if (isAdmin()) return 0;

  ActorInstance* actor = actor_.dereference();
  CONFIRM(actor) else return 0;

  if (!magical) actor->setBloodFlag();

  // Prevent damage from a client in the safe zone and to
  // a client in the safe zone, as well as from members of
  // our group.
  Client* source_client = NULL;
  if (amount < 0) {

    if (source != NULL) {
      // group members can't damage us
      if (source->inSameGroupAs(actor)) return 0;
      source_client = source->getController()->getClient();
    }

    // Protect non-evil players from taking damage
    // while they are on the safe zone
    if (!isEvil() && onSafeZone()) return 0;

    // Handle PVP.  First, make sure PVP is enabled, then check
    // safe zone stuff, then do initiatior damage reduction
    // algorithm and record combat attacks to track PKs.
    if (source_client != NULL) {

      // Non-evil players are protected from being killed by
      // players on the safe zone.
      if (!isEvil() &&
          source_client->onSafeZone()) {
        return 0;
      }

      // Guild members can't damage each other
      if (((guild_ != NULL) && source_client->guild_ == guild_)) {
        return 0;
      }

      // PVP-adjust the damage
      if (!source_client->isInIntroductoryLevels()) {
        static const Avatar::AvatarLevel OFFSET = 5;
        double adjustment
          = double(getLevel() + OFFSET)
              /double(source_client->getLevel() + OFFSET);
        amount = Number(amount.toFloat() * adjustment);
      }

      // If this player has not attacked the client doing damage,
      // the other client is considered the aggressor and his or her
      // damage is reduced until this client fights back.
      if (!source_client->attackedBy(this)) {

        // Low level characters cannot be pked unless they attack
        if (isInIntroductoryLevels()) return 0;

        // Low level characters cannot be killed unless they attack
        //if (!isHighEnoughLevelToBePKed()) return 0;

        //// this client is not being aggressive, so reduce damage
        //int sum_of_levels = avatar_.level +
        //                    source_client->avatar_.level;
        //amount *= float(avatar_.level) / float(sum_of_levels);

        // This client has been attacked by the agressive other client.  The
        // other client should lose alignment when the fight is over.
        addAttackerToHistory(source_client->getLoggedIntoCharacterID(),
                             source_client);
      } else {
        // This client has been attacked, but has also done damage to the
        // attacker.  If the source client was listed as an attacker before
        // now, it will have a valid character_id entry in the list and the
        // character ID will continue to propagate.  Otherwise, this method
        // lists the source client with a 0 ID so that the source client
        // isn't penalized if he or she kills this client.
        addAttackerToHistory(getAggressorClientCharacterID(source_client),
                             source_client);
      }
    }

    int hp_change;
    if (source != NULL) {
      /*if (source_client != NULL) {
        // pvp damage; reduce armor's effect
        todo("karl", "ensure that pvp armor reduction is appropriate");
        hp_change
          = Design::AvatarDamageTaken(
              avatar_.level,
              avatar_.class_,
              magical,
              avatar_stats_.armor.wholePart() / 2,
              amount.wholePart());
      } else {*/
        // monster damage
        hp_change
          = Design::AvatarDamageTaken(
              avatar_.level,
              avatar_.class_,
              magical,
              avatar_stats_.armor.wholePart(),
              amount.wholePart());
      //}
    } else {
      // don't prevent damage if this has no source
      hp_change
        = amount.wholePart();
    }
    avatar_stats_.hp += hp_change;

    if (!magical) {
      actor->setBloodFlag();
      actor->setAction(Actor::ACTORACTION_GOT_HIT, 300);
    }

  } else {

    // The HP change is positive.  Only gain HP up to the max.
    Number hp_after = avatar_stats_.hp + amount;
    avatar_stats_.hp = FixedPoint::minfast(hp_after, avatar_stats_.max_hp);

  }

  // Return the (possibly adjusted) change in HP
  return amount;
}






//----[  changeMP  ]-----------------------------------------------------------
void Client::changeMP(Number amount) {
//  if (amount > 0) { DEBUG_INFO("mp added:  %f", amount.toFloat());}
  Number mp = avatar_stats_.mp + amount;
  mp = FixedPoint::minfast(mp, avatar_stats_.max_mp);
  static const Number zero(0);
  avatar_stats_.mp = FixedPoint::maxfast(mp, zero);
}


//----[  validateSpellParameters  ]--------------------------------------------
bool Client::validateSpellParameters(ActorInstance* client_actor,
                                     float x,
                                     float y,
                                     ActorInstance* target_actor) {
  return ((target_actor == NULL) ||
      ((client_actor->distanceToSq(target_actor) < MAX_VISIBLE_DISTANCE_SQ)) &&
      (client_actor->distanceToSq(x, y) < MAX_VISIBLE_DISTANCE_SQ));
}




//----[  validTargetOfEnemyAt  ]-----------------------------------------------
bool Client::validTargetOfEnemyAt(float x, float y) const {
  const ActorInstance* actor = actor_.dereference();
  return (actor != NULL) &&
         (actor->isAlive()) &&
         (actor->distanceToSq(x, y) < MAX_VISIBLE_DISTANCE_SQ);
}



//----[  addRedFlag  ]---------------------------------------------------------
void Client::addRedFlag(const char* file, unsigned int line) {
  //do nothing atm
  //StatisticsLogger::
  log_formatted("[RED FLAG]  %s(%lu)", file, line);
  //DEBUG_INFO("WARNING:  client red flag at \"%s\"(%lu)", file, line);
}



//----[  updateClientAvatarStats  ]--------------------------------------------
void Client::updateClientAvatarStats() {
  Avatar::AvatarExperience base_level_experience =
    GlobalGamefileManager::singleton()->getExperienceToAchieveLevel(avatar_.level);

  using namespace Evidyon::Network::Packet;
  NetMsg_TellYourAvatarStats_Data data;
  data.attribute_values[0] = avatar_.attribute_values[0];
  data.attribute_values[1] = avatar_.attribute_values[1];
  data.attribute_values[2] = avatar_.attribute_values[2];
  data.attribute_values[3] = avatar_.attribute_values[3];
  data.attribute_values[4] = avatar_.attribute_values[4];
  data.available_attribute_points = avatar_.available_attribute_points;
  //data.max_hp_boosted = avatar_stats_.max_hp != avatar_.equipment.max_hp;
  //data.max_hp_boosted = avatar_stats_.max_mp != avatar_.equipment.max_mp;
  data.hp = avatar_stats_.hp.wholePart();
  data.mp = avatar_stats_.mp.wholePart();
  data.max_hp = avatar_stats_.max_hp.wholePart();
  data.hp_regen = avatar_stats_.hp_regen.wholePart();
  data.max_mp = avatar_stats_.max_mp.wholePart();
  data.mp_regen = avatar_stats_.mp_regen.wholePart();
  data.defense = avatar_stats_.armor.wholePart();

  if (guild_) {
    float ghp, gmp, gxp;
    guild_->getGeosidBonuses(&ghp, &gmp, &gxp);
    data.guild_max_hp = unsigned char(ghp * 100.0-100.0);
    data.guild_max_mp = unsigned char(gmp * 100.0-100.0);
    data.guild_xp = unsigned char(gxp * 100.0-100.0);
  } else {
    data.guild_max_hp = 0;
    data.guild_max_mp = 0;
    data.guild_xp = 0;
  }

  data.race = unsigned char(avatar_.race);
  data.class_ = unsigned char(avatar_.class_);
  data.gender = unsigned char(avatar_.gender);
  data.alignment = avatar_.alignment_value;
  data.current_xp = base_level_experience + avatar_.experience_this_level;
  data.next_level_xp = base_level_experience + avatar_.experience_to_next_level;
  data.penalty_xp = avatar_.penalty_experience;
  data.level = avatar_.level;
                                              
  update_client_avatar_stats_ = false;
  sendAsyncPacket(
    NetMsg_TellYourAvatarStats::writePacket(
      ENET_PACKET_FLAG_RELIABLE,&data));
}





//----[  storageOpen  ]--------------------------------------------------------
void Client::storageOpen() {
  NetworkPacket* packet;
  items_.refreshStorage();
  if (items_.updateStorage(&packet)) {
    sendAsyncPacket(packet);
  }
}




//----[  storageDeposit  ]-----------------------------------------------------
void Client::storageDeposit(int inventory_index) {
  items_.depositInventoryItemInStorage(inventory_index);

  todo("karl","update storage more efficiently on deposit; this sends the whole "\
              "inventory and storage");
  sendFullInventoryUpdate();
  storageOpen();
}




//----[  storageWithdraw  ]----------------------------------------------------
void Client::storageWithdraw(int storage_index) {
  items_.withdrawStorageItemToInventory(storage_index);

  todo("karl","update storage more efficiently on withdraw; this sends the whole "\
              "inventory and storage");
  sendFullInventoryUpdate();
  storageOpen();
}


//
////----[  changeGeonite  ]------------------------------------------------------
//bool Client::changeGeonite(int amount) {
//  int new_geonite = avatar_stats_.geonite + amount;
//  if (amount < 0 && new_geonite < 0) return false;
//  avatar_stats_.geonite = new_geonite;
//  NetworkPacket* update_packet = Evidyon::Network::Packet::Server::Encode::geosidTellGeonite(new_geonite);
//  sendAsyncPacket(update_packet);
//  return true;
//}





//----[  openActorGroup  ]-----------------------------------------------------
void Client::openActorGroup() {
  ActorInstance* client_actor = actor_.dereference();
  CONFIRM(client_actor) else return;
  ActorGroup* group = client_actor->getGroup();
  if (group == NULL) {
    ActorGroup* group = ActorGroup::createGroup(client_actor); // groups default to open
  } else {
    group->setGroupOpen(true);
  }
}




//----[  closeActorGroup  ]----------------------------------------------------
void Client::closeActorGroup() {
  ActorInstance* client_actor = actor_.dereference();
  CONFIRM(client_actor) else return;
  ActorGroup* group = client_actor->getGroup();
  if (group == NULL) return;
  group->setGroupOpen(false);
}




//----[  leaveActorGroup  ]----------------------------------------------------
void Client::leaveActorGroup() {
  ActorInstance* client_actor = actor_.dereference();
  CONFIRM(client_actor) else return;
  client_actor->setGroup(NULL);
}




//----[  joinActorGroup  ]-----------------------------------------------------
bool Client::joinActorGroup(ActorInstance* actor) {
  ActorInstance* client_actor = actor_.dereference();
  CONFIRM(client_actor) else return false;
  ActorGroup* group = actor->getGroup();
  if (group == NULL) return false;
  return client_actor->setGroup(group);
}



//----[  restoreHPMP  ]--------------------------------------------------------
void Client::restoreHPMP() {
  {
    const Number max = avatar_stats_.max_hp;
    if (avatar_stats_.hp < max) {
      avatar_stats_.hp = max;
    }
  }


  {
    const Number max = avatar_stats_.max_mp;
    if (avatar_stats_.mp < max) {
      avatar_stats_.mp = max;
    }
  }
}



//----[ bazaar_tellOwnListedItems ]--------------------------------------------
void Client::bazaar_tellOwnListedItems() {
  using namespace Evidyon::Network::Packet;
  GlobalBazaarManager* bazaar = GlobalDatabaseManager::singleton()->bazaar();
  ItemInBazaar buffer[MAX_ITEMS_PER_BAZAAR_CATEGORY];
  size_t number_of_listings =
    bazaar->getOwnListings(account_id_,
                           buffer,
                           MAX_ITEMS_PER_BAZAAR_CATEGORY);
  sendPacket(NetMsg_Bazaar_TellItems::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      CATEGORY_OWNITEMS,
     &CopiedArray<ItemInBazaar>(buffer, number_of_listings)));
}





//----[ bazaar_tellCategoryListedItems ]---------------------------------------
void Client::bazaar_tellCategoryListedItems(BazaarCategory category) {
  using namespace Evidyon::Network::Packet;
  GlobalBazaarManager* bazaar = GlobalDatabaseManager::singleton()->bazaar();
  const ItemInBazaar* listings;
  size_t number_of_listings;
  if (bazaar->getListings(category,
                          &listings,
                          &number_of_listings)) {
    sendPacket(NetMsg_Bazaar_TellItems::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        category,
       &CopiedArray<ItemInBazaar>(listings, number_of_listings)));
  }
}





//----[ syncCurrency ]---------------------------------------------------------
void Client::syncCurrency() {
  using namespace Evidyon::Network::Packet;
  sendPacket(NetMsg_TellCurrency::writePacket(ENET_PACKET_FLAG_RELIABLE, avatar_.money));
}




//----[  addAttackerToHistory  ]-----------------------------------------------
void Client::addAttackerToHistory(unsigned int character_id, Client* source_client) {

  // Be sure this client can defend themselves
  if (source_client && player_.setAttackPlayers(true)) {
    ActorInstance* my_actor = actor_.dereference();
    assert(my_actor);
    // this code duplicated from NetMsg_AutoTarget_All
    if (my_actor) my_actor->say("<Attacking All Targets>");
  }

  // Copy down
  memmove(attacker_history_,
          attacker_history_ + 1,
          sizeof(AttackerHistory) * (CLIENT_ATTACKER_HISTORY_LENGTH - 1));

  // Add to end
#define END CLIENT_ATTACKER_HISTORY_LENGTH - 1
  attacker_history_[END].character_id = character_id;
  attacker_history_[END].source_client = source_client;
#undef END
}



//----[  onAvatarDeath  ]------------------------------------------------------
void Client::onAvatarDeath(ActorInstance* my_actor,
                           bool drop_items,
                           bool dont_drop_equipment) {

  ++active_quests_[0].death_count;
  ++active_quests_[1].death_count;
  ++active_quests_[2].death_count;

  // Remove all enchantments and buffs
  avatar_enchantments_.clear();
  avatar_stats_.recalculate_timer.trigger();

  // Force update
  todo("karl","broadcasting an updated description packet should not be "\
              "necessary since dropping items changes the equipped items, "\
              "and that sets a flag which is checked by the Player, which "\
              "triggers an update packet");
  my_actor->broadcastUpdateDescriptionPacket();


  {
    // If this player is within a geosid area, don't penalize the attackers
    const Server::MapLocationInfo* location
      = my_actor->getMapLocationInfo();
    if (!location) {
      todo("karl","some sort of error; location should always "\
                  "be valid (but this has been hit in the past!)");
    } else {
      Zone::ZoneRules rules = location->zone_rules;
      Number x, z;
      my_actor->getPosition(&x, &z);
      Server::GeosidInstance* geosid =
        Server::GlobalGeosidManager::singleton()->getGeosidIfInRange(
          my_actor->getMapLocationInfo()->nearby_geosid,
          my_actor->getMapIndex(),
          x,
          z);
      if (geosid != NULL && geosid->isGuildOwnable()) {
        return;
      }
    }
  }

  // drop items
  if (drop_items &&
      (!isInIntroductoryLevels()) &&
      Zone::ZoneRules_DropItemsOnDeath(mostFavorableZoneRules())) {
    WorldRegion* region = my_actor->getRegion();
    Map* map = NULL;
    if ((region != NULL) && ((map = region->getMap()) != NULL)) {
      int x, y;
      my_actor->getMapLocation(&x, &y);

      //if (isHighEnoughLevelToDropAnyItems())
      {
        int equipped_items_to_drop
          = dont_drop_equipment
              ? 0
              : getNumberOfEquippedItemsToDropOnDeath();
        items_.dropItemsOnDeath(equipped_items_to_drop,
                                map,
                                x,
                                y);
      }
    }
  }


  // Save the client's state since this is an important event
  commitToDatabase(GlobalDatabaseManager::singleton());


  // Change the killers' alignments if this character was not evil
  if (!isEvil()) {
    for (int i = 0; i < CLIENT_ATTACKER_HISTORY_LENGTH; ++i) {
      Client* client = attacker_history_[i].source_client;
      if (client != NULL) {
        // reset this client anywhere it appears in the list
        attacker_history_[1].source_client = NULL;
        switch (i) {
          case 0: if (attacker_history_[1].source_client == client) attacker_history_[1].source_client = NULL;
          case 1: if (attacker_history_[2].source_client == client) attacker_history_[2].source_client = NULL;
          case 2: if (attacker_history_[3].source_client == client) attacker_history_[3].source_client = NULL;
          case 3: if (attacker_history_[4].source_client == client) attacker_history_[4].source_client = NULL;
          case 4: if (attacker_history_[5].source_client == client) attacker_history_[5].source_client = NULL;
        }
        // penalize the pker (match character ID to be sure it's the right client)
        if (attacker_history_[i].character_id == client->getLoggedIntoCharacterID()) {
          client->onPK();
          ActorInstance* actor = client->actor_.dereference();
          ActorGroup* group = actor ? actor->getGroup() : NULL;
          if (group) group->partyMemberPKed(actor, client, this);
        }
      }
    }
  }

  // The attacker history should already be cleared
  //memset(attacker_history_, 0, sizeof(attacker_history_));
}



//----[  setAlignmentValue  ]--------------------------------------------------
// Defines a number -100 to +100 that defines the avatar's alignment.  This
// method will send an update packet to the client.
void Client::setAlignmentValue(int alignment) {
  Actor::ActorAlignment previous_alignment_class = getAlignmentClass();
  avatar_.alignment_value = alignment;

  // update the actor's alignment
  ActorInstance* actor = actor_.dereference();
  if (actor) {
    switch (getAlignmentClass()) {
    case Actor::ALIGNMENT_ADMIN:
    case Actor::ALIGNMENT_GUARDIAN:
    case Actor::ALIGNMENT_CHAMPION:
    case Actor::ALIGNMENT_GOOD:
      actor->setMemberOfFaction(ActorInstance::GOOD);
      break;
    case Actor::ALIGNMENT_NEUTRAL:
      actor->setMemberOfFaction(ActorInstance::NEUTRAL);
      break;
    case Actor::ALIGNMENT_EVIL:
    case Actor::ALIGNMENT_DESTROYER:
    case Actor::ALIGNMENT_DREADLORD:
      actor->setMemberOfFaction(ActorInstance::EVIL);
      break;
    }
  }

  // update description if the alignment class changed
  combineUpdateActorDescriptionFlag(
    getAlignmentClass() != previous_alignment_class);
}




//----[  onPK  ]---------------------------------------------------------------
void Client::onPK() {

  {
    // If this player is within a geosid area, don't penalize for pking
    ActorInstance* my_actor = actor_.dereference();
    const Server::MapLocationInfo* location
      = my_actor->getMapLocationInfo();
    if (!location) {
      todo("karl","some sort of error; location should always "\
                  "be valid (but this has been hit in the past!)");
    } else {
      Zone::ZoneRules rules = location->zone_rules;
      Number x, z;
      my_actor->getPosition(&x, &z);
      Server::GeosidInstance* geosid =
        Server::GlobalGeosidManager::singleton()->getGeosidIfInRange(
          my_actor->getMapLocationInfo()->nearby_geosid,
          my_actor->getMapIndex(),
          x,
          z);
      if (geosid != NULL && geosid->isGuildOwnable()) {
        return;
      }
    }
  }

  if (hoursSinceLastPK() > 20) {
    avatar_.kill_count_at_last_pk = 0;
  }

  // Add to pk count for quests
  active_quests_[0].pk_count++;
  active_quests_[1].pk_count++;
  active_quests_[2].pk_count++;

  int kill_count = avatar_.kill_count_at_last_pk++;
  int alignment_value = avatar_.alignment_value - kill_count;

  // prevent players from becoming Dreadlords through PKs alone
  if (alignment_value < -99) alignment_value = -99;

  // update this player's alignment
  setAlignmentValue(alignment_value);

  // store the last time the player killed someone
  if (kill_count > 1) {
    avatar_.last_pk_hour = Evidyon::Time::hour_counter;
  }
}




//----[  getAlignmentClass  ]--------------------------------------------------
Actor::ActorAlignment Client::getAlignmentClass() const {
  if (isAdmin()) return Actor::ALIGNMENT_ADMIN;
  int alignment_value = avatar_.alignment_value;
  if (alignment_value >= 100) {
    return Actor::ALIGNMENT_GUARDIAN;
  } else if (alignment_value >= 75) {
    return Actor::ALIGNMENT_CHAMPION;
  } else if (alignment_value >= 25) {
    return Actor::ALIGNMENT_GOOD;
  } else if (alignment_value > -25) {
    return Actor::ALIGNMENT_NEUTRAL;
  } else if (alignment_value > -75) {
    return Actor::ALIGNMENT_EVIL;
  } else if (alignment_value > -100) {
    return Actor::ALIGNMENT_DESTROYER;
  } else /*if (alignment_value <= -100)*/ {
    return Actor::ALIGNMENT_DREADLORD;
  }
}



//----[  getNumberOfEquippedItemsToDropOnDeath  ]------------------------------
int Client::getNumberOfEquippedItemsToDropOnDeath() const {
  const int PERCENT_GROUP[] = {
    0, 0, 0, 0, // 40%
    1, 1, 1,    // 30%
    2, 2,       // 20%
    3,          // 10%
  };

  // per-alignment, per-percent group
  static const int ALL = AVATAR_INVENTORY_SIZE;
  static const int ITEMS_TO_DROP[8][4] = {
    0, 0, 0, 0,         // guardian drops nothing
    0, 1, 0, 0,         // champion - 30% 1 item, 70% 0 items
    0, 1, 1, 2,         // good - 50% 1 item, 40% 0 items, 10% 2 items
    1, 0, 1, 1,         // neutral - 60% 1 item, 30% 0 items, 10% 2 items
    1, 1, 2, 0,         // evil - 70% 1 item, 20% 2 items, 10% 0 items
    1, 2, 1, 3,         // destroyer
    ALL, ALL, ALL, ALL, // dreadlord drops everything
    0, 0, 0, 0,         // admins drop nothing
  };

  return Zone::ZoneRules_DropItemsOnDeath(getLocalZoneRules())
    ? ITEMS_TO_DROP[getAlignmentClass()][PERCENT_GROUP[rand()%10]] : 0;
}


//----[  hoursSinceLastPK  ]---------------------------------------------------
unsigned int Client::hoursSinceLastPK() const {
  unsigned int hour_counter = Evidyon::Time::hour_counter;
  unsigned int last_pk_hour = avatar_.last_pk_hour;

  // hour_counter should always be larger, but this prevents any problems
  return (hour_counter > last_pk_hour) ? hour_counter - last_pk_hour : 0;
}



//----[  fillDescription  ]----------------------------------------------------
void Client::fillDescription(Evidyon::Actor::ActorDescription* description) {
  description->profile_index = unsigned(avatar_stats_.actor_profile);
  description->form = unsigned(avatar_stats_.skinned_mesh_form);
  description->equipment_based_profile_specialization
    = avatar_stats_.specialization;
  description->alignment = unsigned(getAlignmentClass());
  description->enchantments =
    GlobalGamefileManager::singleton()
      ->packEnchantments(avatar_enchantments_.enchantments);
  bool hide_helmet = hide_helmet_;
  for (int i = 0; i < Evidyon::Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
    Evidyon::Scenery::SceneryIndex equipped = avatar_stats_.equipped[i];
    if ((i == Evidyon::Actor::ACTOREQUIPMENTSLOT_HELMET &&
         (hide_helmet || equipped == Evidyon::Scenery::INVALID_SCENERY_INDEX))) {
        description->equipment_scenery[i] =
          GlobalGamefileManager::singleton()->getHair(avatar_.race,
                                                      avatar_.gender,
                                                      character_id_);
    } else {
      description->equipment_scenery[i] = avatar_stats_.equipped[i];
    }
  }
}




//----[  broadcastUpdateDescriptionPacket  ]-----------------------------------
void Client::broadcastUpdateDescriptionPacket() {
  ActorInstance* actor = actor_.dereference();
  assert(actor);
  if (actor) {
    actor->broadcastUpdateDescriptionPacket();
    update_actor_description_ = false;
    update_visible_enchantments_ = false;
  }
}


//----[  respawn  ]------------------------------------------------------------
void Client::respawn(bool restore_hpmp, bool penalize_xp) {
  ActorInstance* actor = actor_.dereference();
  assert(actor);
  if (penalize_xp &&
      !isInIntroductoryLevels() &&
      Zone::ZoneRules_PenalizeXPOnDeath(mostFavorableZoneRules())) {
    // check penalty *before* respawning
    // move 50% toward maximum penalty
    Avatar::AvatarExperience max_penalty_xp = avatar_.experience_to_next_level * PERCENT_XP_TO_PENALIZE_ON_DEATH;
    avatar_.penalty_experience +=
      (max_penalty_xp - avatar_.penalty_experience) / 2;
  }
  if (actor) {
    GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
    Evidyon::World::MapIndex map;
    float x, z;
    gamefile_manager->getPlayerRespawnLocation(isEvil(), &map, &x, &z);
    actor->changeMap(GlobalWorldManager::singleton()->getMapFromIndex(map), x, z);
    actor->setAction(Actor::ACTORACTION_NONE, 0);
    actor->setMovingFlag(false);
  }
  if (restore_hpmp) restoreHPMP();
}



//----[  getPortalMark  ]------------------------------------------------------
void Client::getPortalMark(unsigned int mark,
                           Map** map,
                           Number* x,
                           Number* z) {
  using namespace Evidyon::Server;
  mark = mark < 0 ? 0 : mark;
  mark = mark >= ClientPortalMark::NUMBER_PER_CLIENT
    ? ClientPortalMark::NUMBER_PER_CLIENT - 1 : mark;
  *map = GlobalWorldManager::singleton()
            ->getMapFromIndex(portal_marks_[mark].map_index);
  *x = portal_marks_[mark].x;
  *z = portal_marks_[mark].z;
}



//----[  changePortalMarkToChaos  ]--------------------------------------------
void Client::changePortalMarkToChaos(unsigned int mark,
                                     Map** map,
                                     Number* x,
                                     Number* z) {
  using namespace Evidyon::Server;
  mark = mark < 0 ? 0 : mark;
  mark = mark >= ClientPortalMark::NUMBER_PER_CLIENT
    ? ClientPortalMark::NUMBER_PER_CLIENT - 1 : mark;
  ActorInstance* actor = actor_.dereference();
  assert(actor);
  if (!actor) return;
  WorldRegion* actor_region = actor->getRegion();
  assert(actor_region);
  // for now, just reset to the spawn point since chaos is no longer well defined
  float portal_x, portal_z;
  GlobalGamefileManager::singleton()->getPlayerRespawnLocation(
    isEvil(),
    &portal_marks_[mark].map_index,
    &portal_x,
    &portal_z);
  portal_marks_[mark].x = portal_x;
  portal_marks_[mark].z = portal_z;
  //Number mark_x, mark_z;
  //actor_region->getMap()->getRandomChaosLocation(&mark_x,
  //                                               &mark_z);
  //portal_marks_[mark].map_index = actor_region->getMap()->getIndex();
  //portal_marks_[mark].x = mark_x;
  //portal_marks_[mark].z = mark_z;

  *map
    = GlobalWorldManager::singleton()->getMapFromIndex(portal_marks_[mark].map_index);;
  *x = portal_x;
  *z = portal_z;
}




//----[  buildTellNamePacket  ]------------------------------------------------
NetworkPacket* Client::buildTellNamePacket() {
  confirm(state_ == LOGGED_INTO_CHARACTER) else return NULL;
  ActorInstance* actor = actor_.dereferenceAssumingValid();
  Actor::ActorID actor_id = actor->getIDNumber();

  NetworkPacket* packet = NULL;
  if (guild_) {
    packet
      = guild_->getTellMemberActorNamePacket(character_id_, this, actor_id);
  } else {
    using namespace Evidyon::Network::Packet;
    size_t name_length = strlen(avatar_.name);
    packet = NetMsg_TellActorName::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      actor_id,
      &CopiedArray<char>(avatar_.name, name_length));
  }

  return packet;
}

}
