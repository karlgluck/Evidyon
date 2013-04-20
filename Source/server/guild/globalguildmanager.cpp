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
#include "server/guild/globalguildmanager.h"
#include "server/globaldatabasemanager.h"
#include "common/null.h"
#include "server/guild/guildinstance.h"
#include "server/geosid/globalgeosidmanager.h"


namespace Evidyon {
namespace Server {




//----[  singleton_  ]---------------------------------------------------------
GlobalGuildManager* GlobalGuildManager::singleton_ = NULL;



//----[  singleton  ]----------------------------------------------------------
GlobalGuildManager* GlobalGuildManager::singleton() {
  assert(singleton_);
  return singleton_;
}



  
//----[  create  ]-------------------------------------------------------------
void GlobalGuildManager::create(GlobalDatabaseManager* database,
                                GlobalGeosidManager* geosid_manager) {
  database_ = database;
  geosid_manager_ = geosid_manager;
  guilds_attacking_geosids_.allocate(geosid_manager->getNumberOfGeosids());
  guilds_.setNextID(database->getHighestGuildID());
  assert(singleton_ == NULL);
  singleton_ = this;
}



//----[  destroy  ]------------------------------------------------------------
void GlobalGuildManager::destroy() {
  struct Destroyer {
    void operator()(GuildInstance* instance) { delete instance; }
  };
  Destroyer destroyer;
  guilds_attacking_geosids_.free();
  guilds_.clear(&destroyer);
  assert(!singleton_ || singleton_ == this);
  singleton_ = NULL;
  geosid_manager_ = NULL;
  database_ = NULL;
}



//----[  store  ]--------------------------------------------------------------
bool GlobalGuildManager::store() {
  return guilds_.forAllObjects(this, &GlobalGuildManager::storeInstance);
}





//----[  createGuild  ]--------------------------------------------------------
GuildInstance* GlobalGuildManager::createGuild(AvatarID founder_avatar,
                                               Client* founder_client,
                                               const char* guild_name) {
  assert(database_);
  assert(founder_client);
  assert(guild_name);

  // Is this name unique?
  UniqueAvatarNameChecker* names = database_->getUniqueAvatarNameChecker();
  if (!names->addIfAvailable(guild_name)) return false;

  // Create an instance
  GuildInstance* instance = new GuildInstance();
  GuildID id = guilds_.add(instance);

  instance->create(id, guild_name, founder_avatar, founder_client);
  return instance;
}




//----[  leaveGuild  ]---------------------------------------------------------
void GlobalGuildManager::leaveGuild(GuildInstance* guild, AvatarID avatar_id) {
  assert(database_);
  assert(guild);
  assert(guilds_.get(guild->getGuildID()) == guild);
  guild->leave(avatar_id);
  if (guild->hasAnyMembers()) return;
  // Remove this guild from the database.  The name cannot be reused until the
  // server is restarted, since the name is held in the UniqueAvatarNameChecker.
  database_->guildErase(guild->getGuildID());
  eraseInstance(guild);
}




//----[  guildMemberLoggedOn  ]------------------------------------------------
GuildInstance* GlobalGuildManager::guildMemberLoggedOn(GuildID guild,
                                                       AvatarID avatar_id,
                                                       Client* client) {
  assert(database_);
  assert(client);
  if (guild == INVALID_GUILD_ID) return NULL;
  confirm(database_) else return NULL;
  GuildInstance* instance = guilds_.get(guild);
  if (!instance && !retrieveInstance(guild, &instance)) return false;
  return instance->loggedOn(avatar_id, client) ? instance : NULL;
}




//----[  guildMemberLoggedOff  ]-----------------------------------------------
void GlobalGuildManager::guildMemberLoggedOff(GuildInstance* guild,
                                              AvatarID avatar_id,
                                              Client* client) {
  assert(database_);
  assert(guild);
  assert(client);
  guild->loggedOff(avatar_id, client);
  if (guild->areAnyMembersOnline()) return;

  guild->store(database_);
  eraseInstance(guild);
}



//----[  addGuildAttackingGeosid  ]--------------------------------------------
void GlobalGuildManager::addGuildAttackingGeosid(GuildInstance* guild) {
  assert(!guilds_attacking_geosids_.containsByEquality(guild));
  guilds_attacking_geosids_.add(guild);
}


//----[  updateGuildsAttackingGeosids  ]---------------------------------------
void GlobalGuildManager::updateGuildsAttackingGeosids() {
  for (size_t i = guilds_attacking_geosids_.count; i > 0;) {
    --i;
    GuildInstance* guild = guilds_attacking_geosids_[i];
    assert(guild);
    if (!guild->updateGeosidAttack()) {
      guilds_attacking_geosids_.swapErase(i, NULL);
    }
  }
}



//----[  retrieveInstance  ]---------------------------------------------------
bool GlobalGuildManager::retrieveInstance(GuildID id, GuildInstance** instance) {
  assert(guilds_.doesNotContain(id));
  assert(database_);
  assert(instance);
  GuildInstance* new_instance = new GuildInstance();
  if (!new_instance || !new_instance->retrieve(id, database_, geosid_manager_)) {
    assert(new_instance->hasAnyMembers());
    // no such guild exists
    delete new_instance;
    return false;
  }
  // load into memory
  if (!guilds_.insert(id, new_instance)) assert(false);
  *instance = new_instance;
  return true;
}





//----[  storeInstance  ]------------------------------------------------------
bool GlobalGuildManager::storeInstance(GuildID id, GuildInstance* instance) {
  assert(database_);
  assert(instance);
  return instance->store(database_);
}



//----[  eraseInstance  ]------------------------------------------------------
void GlobalGuildManager::eraseInstance(GuildInstance* instance) {
  geosid_manager_->guildInstanceDestroyed(instance);
  guilds_.erase(instance->getGuildID());
  size_t i = guilds_attacking_geosids_.linearFindByEquality(instance);
  if (i < guilds_attacking_geosids_.count) {
    guilds_attacking_geosids_.swapErase(i, NULL);
  }
  delete instance;
}


}
}

