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
#include "geosidinstance.h"
#include "server/globaldatabasemanager.h"
#include "server/geosid/globalgeosidmanager.h"
#include "server/guild/guildinstance.h"
#include "shared/geosid/geosidlimits.h"
#include "server/actor.h"
#include "server/geosid/geosidattack.h"
#include "shared/client-server/packets.h"
#include "server/globalworldmanager.h"
#include "server/worldregion.h"
#include "server/client.h"

#include "server/globalitemmanager.h"
#include "common/randint32.h"
#include "server/event/eventidgenerator.h"

#include "common/fracture/todo.h"

namespace Evidyon {
namespace Server {



static const Time::Milliseconds GEOSID_ATTACK_DELAY_MS = 30000;
static const Number MAX_GEOSID_VISIBLE_RANGE_SQ = 16.0f * 16.0f;

//----[  init  ]---------------------------------------------------------------
void GeosidInstance::init(const Geosid::GeosidServerDescription* description,
                          GuildID owner_guild,
                          Geosid::Geonite geonite) {
  zero();
  memcpy(&description_, description, sizeof(*description));
  owner_ = owner_guild;
  if (geonite <= 0) geonite = description_.geonite_to_activate;
  geonite_ = geonite;
}





//----[  store  ]--------------------------------------------------------------
void GeosidInstance::store(GlobalDatabaseManager* database) {
  database->geosidSet(description_.id, owner_, geonite_);
}







//----[  setOwnerInstance  ]---------------------------------------------------
void GeosidInstance::setOwnerInstance(GuildInstance* instance) {
  assert(instance->getGuildID() == owner_);
  owner_instance_ = instance;
}




//----[  nullifyOwnerInstance  ]-----------------------------------------------
void GeosidInstance::nullifyOwnerInstance(GuildInstance* instance) {
  if (owner_instance_ == instance) {
    owner_instance_ = NULL;
  }
}





//----[  removeOwner  ]--------------------------------------------------------
void GeosidInstance::removeOwner() {
  owner_ = INVALID_GUILD_ID;
  owner_instance_ = NULL;
  guardianRemoveOwner(); // forcibly update the geosid's state...hackish
}



//----[  getOwnerID  ]---------------------------------------------------------
GuildID GeosidInstance::getOwnerID() const {
  return owner_;
}

//----[  getOwnerInstance  ]---------------------------------------------------
GuildInstance* GeosidInstance::getOwnerInstance() {
  confirm(!owner_instance_ || owner_instance_->getGuildID() == owner_) else {
    owner_instance_ = NULL;
    owner_ = Server::INVALID_GUILD_ID;
  }
  return owner_instance_;
}




//----[  hasOwner  ]-----------------------------------------------------------
bool GeosidInstance::hasOwner() const {
  return owner_ != Server::INVALID_GUILD_ID;
}



//----[  getGeonite  ]---------------------------------------------------------
Geosid::Geonite GeosidInstance::getGeonite() const {
  return geonite_;
}



//----[  changeGeonite  ]------------------------------------------------------
void GeosidInstance::changeGeonite(Geosid::Geonite geonite,
                                   WorldRegion* region) {
  geonite_ += geonite;
  if (geonite_ < 0) {
    // activate!

    // drop items
    if (description_.number_of_items > 0) {
      Item::ItemIndex item_index
        = description_.items[rand_uint32()%description_.number_of_items];
      for (int i = 0; i < 4; ++i) {
        static const int DISTANCE = 3;
        int x = Number(description_.x).wholePart(),
            z = Number(description_.z).wholePart();
        switch (i) {
          case 0:            z -= DISTANCE;            break;
          case 1:            x += DISTANCE;            break;
          case 2:            z += DISTANCE;            break;
          case 3:            x -= DISTANCE;            break;
        }
        ItemPointer new_item;
        if (GlobalItemManager::singleton()->acquireNewItem(
              1,
              item_index,
              &new_item)) {
          WorldRegion* drop_region = region->getMap()->getRegionFromPoint(x, z);
          if (!drop_region ||
              !drop_region->addItemOnGround(
                true,
                x,
                z,
                new_item)) {
            GlobalItemManager::singleton()->releaseItem(&new_item);
          } else {
            drop_region->broadcastUpdateItemsOnLocation(x, z);
          }
        }
      }
    }

    if (description_.number_of_portals > 0) {
      active_portal_index_ = rand_uint32() % description_.number_of_portals;
      //// portal everyone already in the area
      //Evidyon::ActorInstance* actors[16];
      //int actors = region->findActorsNearby(
      //  Number(description_.x).toFloat(),
      //  Number(description_.z).toFloat(),
      //  3.0f*3.0f,
      //  actors,
      //  16);
      //for (int i = 0; i < actors; ++i) {
      //}
      portal_open_timer_.disable();
      portal_close_timer_.disable();
      portal_open_timer_.enable(1000);
      portal_close_timer_.enable(description_.activation_event_duration);
    }

    // create the activation effect
    region->broadcastEvent_GenerateTargetedSpecialFX(
      GenerateEventID(),
      description_.activation_event,
      Number(description_.x),
      Number(description_.z),
      Number(description_.x),
      Number(description_.z),
      NULL,
      0);

    // reset geonite
    geonite_ += description_.geonite_to_activate;

    // prevent multiple activations from a single sacrifice
    if (geonite_ < 1) geonite_ = 1;
  }
  using namespace Evidyon::Network::Packet;
  region->broadcastOrphanedNetworkPacket(
    NetMsg_Geosid_TellGeonite::writePacket(
      ENET_PACKET_FLAG_UNSEQUENCED,
      description_.index,
      geonite_));
}


//----[  getGeosidIndex  ]-----------------------------------------------------
Geosid::GeosidIndex GeosidInstance::getGeosidIndex() const {
  return description_.index;
}



//----[  canBeSeenBy  ]--------------------------------------------------------
bool GeosidInstance::canBeSeenBy(ActorInstance* actor) const {
  return (actor->getMapIndex() == description_.map) &&
    actor->distanceToSq(Number(description_.x),
                        Number(description_.z)) < MAX_GEOSID_VISIBLE_RANGE_SQ;
}



//----[  isWithinRange  ]------------------------------------------------------
bool GeosidInstance::isWithinRange(World::MapIndex map,
                                   Number x,
                                   Number z) const {
  return (description_.map == map) &&
         (FixedPoint::square(Number(description_.x) - x) +
          FixedPoint::square(Number(description_.z) - z))
              < Number(Geosid::GEOSID_RANGE_SQ);
}


//----[  isWithinRange  ]------------------------------------------------------
bool GeosidInstance::isWithinRange(const ActorInstance* actor) const {
  return (actor->getMapIndex() == description_.map) &&
         (actor->distanceToSq(Number(description_.x), Number(description_.z))
                    < Number(Geosid::GEOSID_RANGE_SQ));
}



//----[  isGuildOwnable  ]-----------------------------------------------------
bool GeosidInstance::isGuildOwnable() const {
  return description_.is_white_geosid == false;
}




//----[  getBurstMagic  ]------------------------------------------------------
bool GeosidInstance::getBurstMagic(Client* client,
                                   ActorInstance* client_actor,
                                   Evidyon::Magic::MagicIndex* magic_index,
                                   Geosid::Geonite* geonite_cost) {
  assert(magic_index);
  assert(geonite_cost);
  Server::GuildInstance* guild = client->getGuild();
  if (isWithinRange(client_actor) &&
      (description_.is_white_geosid ||
        (guild && guild->getGuildID() == owner_))) {
    assert(description_.is_white_geosid || (owner_instance_ == guild));
    *magic_index = description_.burst_magic;
    *geonite_cost = description_.burst_cost;
    return true;
  } else {
    return false;
  }
}




//----[  isOwner  ]------------------------------------------------------------
bool GeosidInstance::isOwner(GuildID guild) const {
  return owner_ == guild;
}



//----[  isOwner  ]------------------------------------------------------------
bool GeosidInstance::isOwner(GuildInstance* guild) const {
  return guild != NULL && owner_instance_ != NULL && (guild == owner_instance_);
}




//----[  attack  ]-------------------------------------------------------------
bool GeosidInstance::attack(GuildInstance* guild,
                            Client* client,
                            GeosidAttack* attack_type,
                            Time::Milliseconds* attack_timer_delay) {
  return false;
  // this is the old way of doing things; now you just kill the guardian
  //assert(guild);
  //assert(attack_type);
  //assert(attack_timer_delay);
  //if (description_.is_white_geosid ||
  //    !client->changeGeonite(-description_.burst_cost)) return false;
  //if (owner_ == Server::INVALID_GUILD_ID) {
  //  *attack_type = GEOSIDATTACK_ESTABLISHOWNERSHIP;
  //} else {
  //  if (owner_instance_) owner_instance_->geosidUnderAttack(this, guild);
  //  *attack_type = GEOSIDATTACK_REMOVEOWNER;
  //}
  //*attack_timer_delay = GEOSID_ATTACK_DELAY_MS;
  //return true;
}




//----[  burstAttackTimerExpired  ]--------------------------------------------
void GeosidInstance::burstAttackTimerExpired(GuildInstance* guild,
                                             GeosidAttack attack_type) {
  assert(guild);
  if (owner_instance_ == guild) {
    assert(owner_ == guild->getGuildID());
    return;
  }
  
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet = NULL;
  if (!hasOwner()) {
    if (attack_type == GEOSIDATTACK_ESTABLISHOWNERSHIP) {
      guild->gainedGeosid(this);
      owner_instance_ = guild;
      owner_ = guild->getGuildID();
      packet = guild->getChangeGeosidOwnerPacket(description_.index,
                                                 geonite_);
    }
  } else {
    if (attack_type == GEOSIDATTACK_REMOVEOWNER) {
      if (owner_instance_) owner_instance_->lostGeosid(this);
      owner_instance_ = NULL;
      owner_ = Server::INVALID_GUILD_ID;
      packet = NetMsg_Geosid_ChangeOwner::writePacket(
        ENET_PACKET_FLAG_RELIABLE,
        description_.index,
        &CopiedArray<char>(NULL, 0));
    }
  }

  
  if (packet) {
    WorldRegion* region = getLocalRegion();
    if (region) {
      region->broadcastOrphanedNetworkPacket(packet);
    } else {
      enet_packet_destroy(packet);
    }


    // since the geosid changed, be sure its new owner is synced in the db
    store(GlobalDatabaseManager::singleton());
  }
}




//----[  guardianUnderAttack  ]------------------------------------------------
void GeosidInstance::guardianUnderAttack(Evidyon::Client* attacker) {
  GuildInstance* current_owners = getOwnerInstance();
  GuildInstance* attacker_guild = attacker->getGuild();
  if (current_owners) {
    if (attacker_guild) {
      // inform the owners that this geosid is under attack
      current_owners->geosidUnderAttack(this, attacker_guild);
    } else {
      // inform of the client attack
      todo("karl","inform client of attack on geosid by non-guild-member");
    }
  }
}



//----[  guardianRemoveOwner  ]------------------------------------------------
void GeosidInstance::guardianRemoveOwner() {
  if (!hasOwner()) return;

  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet = NULL;
  if (owner_instance_) owner_instance_->lostGeosid(this);
  owner_instance_ = NULL;
  owner_ = Server::INVALID_GUILD_ID;
  packet = NetMsg_Geosid_ChangeOwner::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    description_.index,
    &CopiedArray<char>(NULL, 0));

  Map* map;
  if (packet &&
      (map = GlobalWorldManager::singleton()->getMapFromIndex(description_.map))) {
    WorldRegion* region
      = map->getRegionFromPoint(Number(description_.x).wholePart(),
                                Number(description_.z).wholePart());
    if (region) {
      region->broadcastOrphanedNetworkPacket(packet);
    }

    // since the geosid changed, be sure its new owner is synced in the db
    store(GlobalDatabaseManager::singleton());
  }
}




//----[  guardianSetOwner  ]---------------------------------------------------
void GeosidInstance::guardianSetOwner(GuildInstance* guild) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet = NULL;
  owner_instance_ = guild;
  owner_ = guild->getGuildID();
  packet = guild->getChangeGeosidOwnerPacket(description_.index,
                                             geonite_);
  guild->gainedGeosid(this);

  Map* map;
  if (packet &&
      (map = GlobalWorldManager::singleton()->getMapFromIndex(description_.map))) {
    WorldRegion* region
      = map->getRegionFromPoint(Number(description_.x).wholePart(),
                                Number(description_.z).wholePart());
    if (region) {
      region->broadcastOrphanedNetworkPacket(packet);
    }

    // since the geosid changed, be sure its new owner is synced in the db
    store(GlobalDatabaseManager::singleton());
  }
}



//----[  getOwnershipBenefits  ]-----------------------------------------------
bool GeosidInstance::getOwnershipBenefits(GuildID guild,
                                          float* bonus_hp,
                                          float* bonus_mp,
                                          float* bonus_xp) {
  if (guild == owner_) {
    *bonus_hp = description_.hp_bonus;
    *bonus_mp = description_.mp_bonus;
    *bonus_xp = description_.xp_bonus;
    return true;
  } else {
    return false;
  }
}




//----[  canSacrificeItems  ]--------------------------------------------------
bool GeosidInstance::canSacrificeItems(Client* client) {
  GuildInstance* guild = client->getGuild();
  return description_.is_white_geosid || (getOwnerInstance() == guild && guild);
}



//----[  zoneRules  ]----------------------------------------------------------
Zone::ZoneRules GeosidInstance::zoneRules() const {
  return description_.is_white_geosid ? Zone::ZONERULES_TOWN : Zone::ZONERULES_WILD_GEOSID;
}



//----[  onEnterCheckPortal  ]-------------------------------------------------
bool GeosidInstance::onEnterCheckPortal(Client* client, ActorInstance* actor) {
  if (active_portal_index_ >=
      Geosid::GeosidServerDescription::MAX_PORTALS) return false;

  if (portal_close_timer_.expired()) {
    portal_open_timer_.disable();
    portal_close_timer_.disable();
    active_portal_index_ = -1;
    return false;
  }
  if (portal_open_timer_.expired()) {
    actor->changeMap(
      GlobalWorldManager::singleton()->getMapFromIndex(
        description_.portals[active_portal_index_].map),
      1.0f * description_.portals[active_portal_index_].x + 0.5f,
      1.0f * description_.portals[active_portal_index_].z + 0.5f);
  }

  // fall through to 'true' even if the portal isn't open yet so that an attack
  // isn't initiated while a portal is opening
  return true;
}



//----[  zero  ]---------------------------------------------------------------
void GeosidInstance::zero() {
  memset(&description_, 0xFFFFFFFF, sizeof(description_));
  owner_ = INVALID_GUILD_ID;
  owner_instance_ = NULL;
  under_attack_timer_.reset();
  active_portal_index_ = -1;
}




//----[  getLocation  ]--------------------------------------------------------
void GeosidInstance::getLocation(Number* x, Number* z) const {
  *x = description_.x;
  *z = description_.z;
}



//----[  getLocalRegion  ]-----------------------------------------------------
WorldRegion* GeosidInstance::getLocalRegion() {
  Map* map;
  map = GlobalWorldManager::singleton()->getMapFromIndex(description_.map);
  return map->getRegionFromPoint(Number(description_.x).wholePart(),
                                 Number(description_.z).wholePart());
};




}
}

