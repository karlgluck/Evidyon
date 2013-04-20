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
#ifndef __SERVER_GUILD_GLOBALGUILDMANAGER_H__
#define __SERVER_GUILD_GLOBALGUILDMANAGER_H__
#pragma once


#include "common/kg/kg.h"
#include "server/avatar/avatarid.h"
#include "server/guild/guildid.h"

namespace Evidyon {
class UniqueAvatarNameChecker;
class GlobalDatabaseManager;
class Client;
namespace Server {
class GlobalGeosidManager;
class GuildInstance;
}
}

namespace Evidyon {
namespace Server {



  
//----[  GlobalGuildManager  ]-------------------------------------------------
class GlobalGuildManager {
public:

  // Sets the database reference
	void create(GlobalDatabaseManager* database,
              GlobalGeosidManager* geosid_manager);

  // Erases memory
	void destroy();

  // Saves all guild information to the database
	bool store();
	
	// Establishes a new guild.  If the name is not unique, NULL is returned.
	GuildInstance* createGuild(AvatarID founder_avatar,
                             Client* founder_client,
	                           const char* guild_name);

  // Removes the avatar from the guild.  If the guild has no members, it is
  // erased.
  void leaveGuild(GuildInstance* guild, AvatarID avatar_id);

  // Returns the active guild instance for the given guild.  If this
  // actor is no longer a member of the guild or the guild doesn't exist,
  // the return value is NULL.	                           
	GuildInstance* guildMemberLoggedOn(GuildID guild,
	                                   AvatarID avatar_id,
                                     Client* client);

  // Removes the client from the guild's active table and erases the active
  // guild instance if it was the last member to leave.
	void guildMemberLoggedOff(GuildInstance* guild,
                            AvatarID avatar_id,
                            Client* client);

  // This guild has just started attacking a new geosid
  void addGuildAttackingGeosid(GuildInstance* guild);

  // Checks the state of each guild that is attacking a geosid, removing
  // it from the attackers list if the return flag indicates it is finished.
  void updateGuildsAttackingGeosids();

private:

  // Reads a guild's information from the database
  bool retrieveInstance(GuildID id, GuildInstance** instance);

  // Saves a guild's information to the database
  bool storeInstance(GuildID id, GuildInstance* instance);  

  // Removes a guild from the guilds map, deallocates the instance memory and
  // cleans up external references (such as those in the geosids).
  void eraseInstance(GuildInstance* instance);

private:
  GlobalDatabaseManager* database_;
  kg::IDMap<GuildID, GuildInstance> guilds_;
  GlobalGeosidManager* geosid_manager_;
  kg::AllocatedArray<GuildInstance*> guilds_attacking_geosids_;

public:
  static GlobalGuildManager* singleton();
private:
  static GlobalGuildManager* singleton_;
};


}
}

#endif