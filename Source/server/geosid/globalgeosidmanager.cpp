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
#include "globalgeosidmanager.h"
#include "server/globaldatabasemanager.h"
#include "server/guild/guildinstance.h"
#include "shared/client-server/packets.h"

namespace Evidyon {
namespace Server {






//----[  GlobalGeosidManager  ]------------------------------------------------
GlobalGeosidManager::GlobalGeosidManager() {
  zero();
}




//----[  create  ]-------------------------------------------------------------
void GlobalGeosidManager::create(GlobalDatabaseManager* database) {
  database_ = database;
}





//----[  destroy  ]------------------------------------------------------------
void GlobalGeosidManager::destroy() {
  database_ = NULL;
  geosids_.free();
}





//----[  store  ]--------------------------------------------------------------
bool GlobalGeosidManager::store() {
  return geosids_.forAllElements(this, &GlobalGeosidManager::storeGeosidInstance);
}






//----[  initNumberOfGeosids  ]------------------------------------------------
void GlobalGeosidManager::initNumberOfGeosids(size_t count) {
  geosids_.allocate(count);
}




//----[  initGeosid  ]---------------------------------------------------------
void GlobalGeosidManager::initGeosid(
    const Geosid::GeosidServerDescription* description) {
  assert(database_);
  translator_.addIndexHashPair(description->index, description->id);
  GeosidInstance& instance = geosids_.add();
  Server::GuildID owner_guild;
  Geosid::Geonite geonite;
  database_->getGeosidData(description->id, &owner_guild, &geonite);
  instance.init(description, owner_guild, geonite);
}




//----[  getNumberOfGeosids  ]-------------------------------------------------
size_t GlobalGeosidManager::getNumberOfGeosids() const {
  return geosids_.count;
}



//----[  getGeosidByHash  ]----------------------------------------------------
GeosidInstance* GlobalGeosidManager::getGeosidByHash(Geosid::GeosidHashID geosid_hash) {
  Geosid::GeosidIndex index;
  if (!translator_.findIndex(geosid_hash, &index)) return NULL;
  return geosids_.get(index);
}




//----[  getGeosidByIndex  ]---------------------------------------------------
GeosidInstance* GlobalGeosidManager::getGeosidByIndex(Geosid::GeosidIndex geosid_index) {
  return geosids_.get(geosid_index);
}





//----[  getGeosidIfInRange  ]-------------------------------------------------
GeosidInstance* GlobalGeosidManager::getGeosidIfInRange(
    Geosid::GeosidIndex geosid,
    World::MapIndex map,
    Number x,
    Number z) {
  GeosidInstance* instance = getGeosidByIndex(geosid);
  return (instance && instance->isWithinRange(map, x, z)) ? instance : NULL;
}







//----[  guildInstanceRetrieved  ]---------------------------------------------
void GlobalGeosidManager::guildInstanceRetrieved(GuildInstance* guild) {
  GuildID guild_id = guild->getGuildID();
  for (size_t i = 0; i < geosids_.count; ++i) {
    if (geosids_[i].isOwner(guild_id)) {
      if (guild->addRetrievedGeosid(&geosids_[i])) {
        geosids_[i].setOwnerInstance(guild);
      } else {
        geosids_[i].removeOwner();
      }
    }
  }
}






//----[  guildInstanceDestroyed  ]---------------------------------------------
void GlobalGeosidManager::guildInstanceDestroyed(GuildInstance* guild) {
  for (size_t i = 0; i < geosids_.count; ++i) {
    geosids_[i].nullifyOwnerInstance(guild);
  }
}




//----[  getTellGeosidOwnerDataPacket  ]-------------------------------------------
NetworkPacket* GlobalGeosidManager::getTellGeosidOwnerDataPacket(
    Geosid::GeosidIndex geosid_index,
    ActorInstance* requesting_actor) {
  GeosidInstance* geosid;
  if (!geosids_.get(geosid_index, &geosid)) return NULL;
  if (!geosid->canBeSeenBy(requesting_actor)) return NULL;

  return getTellGeosidOwnerDataPacket(geosid);
}



//----[  getTellGeosidOwnerDataPacket  ]---------------------------------------
NetworkPacket* GlobalGeosidManager::getTellGeosidOwnerDataPacket(
    GeosidInstance* geosid) {
  GuildInstance* owner = geosid->getOwnerInstance();
  if (owner) {
    return owner->getTellGeosidOwnerDataPacket(geosid->getGeosidIndex(),
                                               geosid->getGeonite());
  } else {
    // the owner guild is not online; get the name from the database
    kg::CharBuffer<Guild::MAX_NAME_LENGTH> guild;
    database_->guildGetName(geosid->getOwnerID(),
                            guild.characters,
                            guild.CAPACITY);

    using namespace Evidyon::Network::Packet;
    return NetMsg_Geosid_TellOwnerData::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      geosid->getGeosidIndex(),
      &CopiedArray<char>(guild.characters, strlen(guild.characters)),
      geosid->getGeonite());
  }
}



//----[  zero  ]---------------------------------------------------------------
void GlobalGeosidManager::zero() {
  database_ = NULL;
  geosids_.size = 0;
  geosids_.count = 0;  
}





//----[  storeGeosidInstance  ]------------------------------------------------
bool GlobalGeosidManager::storeGeosidInstance(GeosidInstance* instance) {
  assert(database_);
  instance->store(database_);
  return true;
}





}
}

