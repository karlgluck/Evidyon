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
#ifndef __SERVER_GUILD_GUILDINSTANCE_H__
#define __SERVER_GUILD_GUILDINSTANCE_H__
#pragma once


#include "common/kg/kg.h"
#include "shared/guild/guildlimits.h"
#include "server/guild/guildid.h"
#include "shared/avatar/avatarlimits.h"
#include "server/avatar/avatarid.h"
#include "shared/networkdecl.h"
#include "shared/actor/actorid.h"
#include "shared/time/timer.h"
#include "common/number.h"
#include "shared/geosid/geosidindex.h"
#include "shared/geosid/geonite.h"

namespace Evidyon {
class GlobalDatabaseManager;
class Client;
class Map;
namespace Server {
enum GeosidAttack;
class GeosidInstance;
class GlobalGeosidManager;
}
}

namespace Evidyon {
namespace Server {




  
//----[  GuildInstance  ]------------------------------------------------------
class GuildInstance {
  struct Member {
    AvatarID avatar_id;
    kg::CharBuffer<Guild::MAX_NAME_LENGTH> name;
    bool leader;
    bool online;
  };

public:

  // Initialize a new guild.  The founder's name is updated to reflect the
  // leadership position.
  void create(GuildID id,
              const char* guild_name,
              AvatarID founder_avatar,
              Client* founder_client);

  // Bring this guild's info in from the database
  bool retrieve(GuildID id,
                GlobalDatabaseManager* database,
                GlobalGeosidManager* geosid_manager);

  // Saves a guild's information
  bool store(GlobalDatabaseManager* database);

  // Guild member logged on.  If the avatar is not a member, 'false' is
  // returned.
  bool loggedOn(AvatarID avatar_id, Client* client);
  void loggedOff(AvatarID avatar_id, Client* client);
  bool areAnyMembersOnline() const;
  bool hasAnyMembers() const;
  GuildID getGuildID() const;

  // Tests to see whether or not the avatar is a member of this guild
  bool isMember(AvatarID avatar_id);

  // Puts this guild's name and the avatar's status in the guild into
  // the name of an avatar.
  NetworkPacket* getTellMemberActorNamePacket(AvatarID avatar_id,
                                              Client* online_client,
                                              Actor::ActorID actor_id);

  // Removes 'kicked' from the guild if the kicker is the guild founder, or
  // if the kicker is a leader and kicked is a non-leader, non-founder.  All
  // guild members will receive a message indicating what happened if this
  // method succeeds.
  // If the client is online, it will be notified to update its name.
  bool kick(AvatarID kicker, AvatarID kicked);

  // Removes 'avatar' from the guild.  If the founder leaves and there are
  // no leaders, this guild is essentially disabled since nobody can invite
  // new members.
  // If the client is online, it will be notified to update its name.
  void leave(AvatarID avatar);
  
  // Makes 'promoted' a leader if 'promoter' is the founder.  This will cause
  // the promoted client to broadcast a name update.
  bool promote(AvatarID promoter, AvatarID promoted);

  // Removes 'demoted' as a leader if 'demoter' is the founder.  This will
  // cause the demoted client to broadcast a name update.
  bool demote(AvatarID demoter, AvatarID demoted);

  // New member wants to join.  Checks to make sure 'leader' is a leader,
  // then checks capacity.  If added, added as non-leader/founder and a message
  // is sent to all guild members that the follower joined.  The follower client
  // must always be valid.
  bool lead(AvatarID leader, AvatarID follower, Client* follower_client);

  // Sets the rally point (the "home") for this guild.  Only leaders can do this.
  bool setHome(AvatarID avatar_id,
               Map* map,
               Number x,
               Number z);

  // When a player rallies/goes home, this method is eventually invoked to obtain
  // the home coordinates.
  bool goHome(Map** map, Number* x, Number* z);

  // Give up control of the given geosid voluntarily
  bool abandonGeosid(Geosid::GeosidIndex geosid_index, AvatarID avatar_id);

  // Returns the ID number of a guild member from its name or INVALID_AVATAR_ID
  // if the name doesn't match one of the members.
  AvatarID getGuildMemberID(const char* avatar_name) const;

  // Sends system-generated chat text to each of the members in this guild
  void sendMessage(const char* text);
  void sendMessage(const char* text, size_t length);

  // Sends player text to all online guild members
  void sendChat(const char* member_name, size_t name_length,
                const char* chat_text, size_t chat_length);

  // Builds a list of all of the members in this guild and their
  // online/offline status
  void membersText(char* output_buffer, size_t size);

  // Creates a packet with the ID numbers of all the geosids this guild owns
  NetworkPacket* tellGeosidsPacket();

  // This guild is starting an attack on the target geosid
  bool attackGeosid(GeosidInstance* geosid, Client* initiator);

  // Called periodically to check to see whether the conditions of the
  // geosid attack in progress are still valid.
  // Returns 'false' if this method no longer needs to be called.
  bool updateGeosidAttack();

  // This guild has become the owner of the target geosid
  // A message is always sent to online guild members indicating what happened"
  //    <<Guild>>  Your guild now owns Luna, a large red (+20% HP) geosid
  void gainedGeosid(GeosidInstance* geosid);

  // This guild no longer owns the geosid.  It should be removed from the
  // internal list, and a message broadcast to online members about the
  // loss.
  void lostGeosid(GeosidInstance* geosid);

  // The given geosid is under attack and should be defended!  Sends a message
  // to online clients about the attack.
  void geosidUnderAttack(GeosidInstance* geosid,
                         GuildInstance* attackers);

  // Does this guild have enough members to take on another geosid?
  bool canAcquireAnotherGeosid() const;

  NetworkPacket* getTellGeosidOwnerDataPacket(Geosid::GeosidIndex geosid,
                                              Geosid::Geonite geonite);
  NetworkPacket* getChangeGeosidOwnerPacket(Geosid::GeosidIndex geosid,
                                            Geosid::Geonite geonite);

public:

  // Put a member into this guild (no checks; called by the database during
  // retrieve()).
  void addRetrievedMember(AvatarID avatar_id,
                          const char* name,
                          bool leader);

  // Called when the instance is being created to add a geosid that this guild
  // currently owns.  This method is only called after all members are
  // retrieved.
  // 'false' is returned if the guild does not have enough members to support
  // another geosid.
  bool addRetrievedGeosid(GeosidInstance* geosid);

  // Return value is the index of the client in online_, if the client
  // that owns avatar_id was found.  Should only be called when
  // getMember(avatar_id)->online == true.
  size_t getOnlineMemberClient(AvatarID avatar_id, Client** client);

  void getGeosidBonuses(float* hp_multiplier,
                        float* mp_multiplier,
                        float* xp_multiplier);

private:

  // Removes the member from the guild.  Called by kick() or leave()
  void remove(Member* member);

  // the founder is always a leader
  bool isLeader(AvatarID avatar_id);

  // Returns NULL if the member could not be found
  Member* getMember(AvatarID id);

  // Sums up the effects of all of the geosids owned by the guild.
  void calculateGeosidEffects(bool trigger_client_updates);

  // Sends the packet to all online clients.  If the packet has no references,
  // it is deallocated.
  void broadcast(NetworkPacket* packet);

private:
  GuildID guild_id_;
  kg::CharBuffer<Guild::MAX_NAME_LENGTH> guild_name_;
  kg::Array<Client*, Guild::MAX_MEMBERS> online_;

  Map* home_map_;
  Number home_x_, home_z_;

  AvatarID founder_id_;
  int number_of_leaders_;
  kg::Array<Member, Guild::MAX_MEMBERS> members_;

  kg::Array<GeosidInstance*, Guild::MAX_GEOSIDS_OWNED> geosids_;
  float hp_multiplier_, mp_multiplier_, xp_multiplier_;

  Client* geosid_attack_initiator_;
  GeosidInstance* attacked_geosid_;
  GeosidAttack geosid_attack_type_;
  Time::Timer<Time::RESOLUTION_500_MS> geosid_attack_timer_;
};




}
}

#endif