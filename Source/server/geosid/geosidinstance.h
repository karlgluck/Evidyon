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
#ifndef __SERVER_GEOSID_GEOSIDINSTANCE_H__
#define __SERVER_GEOSID_GEOSIDINSTANCE_H__
#pragma once

#include "shared/geosid/geosidserverdescription.h"
#include "server/guild/guildid.h"
#include "common/number.h"
#include "shared/time/timer.h"
#include "shared/world/mapindex.h"
#include "shared/zone/zonerules.h"

namespace Evidyon {
class GlobalDatabaseManager;
class Client;
class ActorInstance;
class WorldRegion;
namespace Server {
enum GeosidAttack;
class GuildInstance;
}
}

namespace Evidyon {
namespace Server {



//----[  GeosidInstance  ]-----------------------------------------------------
class GeosidInstance {
public:
  // no constructor - used in array

  // Called when the server is starting up.  This overwrites any internal
  // information without checks and resets the class's other internal values.
  void init(const Geosid::GeosidServerDescription* description,
            GuildID owner_guild,
            Geosid::Geonite geonite);
  
  // Called when the server is shutting down or doing a backup
  void store(GlobalDatabaseManager* database);

  // Changes the owner instance.  This method will NOT update the owner id--it
  // is intended only to be called when the owning guild is instantiated or
  // released.
  void setOwnerInstance(GuildInstance* instance);

  // Sets the owner to NULL (but does not change the ownership ID) if the
  // given instance matches the internal instance.  This is done when the
  // instance is being erased.
  void nullifyOwnerInstance(GuildInstance* instance);

  // Erases the owner's ID and the owner instance
  void removeOwner();

  // Returns the owner of this geosid
  GuildID getOwnerID() const;
  GuildInstance* getOwnerInstance();
  bool hasOwner() const;

  Geosid::Geonite getGeonite() const;
  void changeGeonite(Geosid::Geonite geonite,
                     WorldRegion* region);

  // Obtains the geosid's ID
  Geosid::GeosidIndex getGeosidIndex() const;
  Geosid::GeosidHashID getGeosidHashID() const;

  // Is it valid for the given actor to interact with this geosid?
  bool canBeSeenBy(ActorInstance* actor) const;

  // Returns whether or not the given coordinates are in the range of this
  // geosid.
  bool isWithinRange(World::MapIndex map, Number x, Number z) const;
  bool isWithinRange(const ActorInstance* actor) const;
  bool isGuildOwnable() const;

  // Returns the burst magic for this class if the client's actor is within
  // the area of the geosid and the geosid is white or the client is a member
  // of the guild that owns this geosid.
  bool getBurstMagic(Client* client,
                     ActorInstance* client_actor,
                     Evidyon::Magic::MagicIndex* magic_index,
                     Geosid::Geonite* geonite_cost);

  // Queries whether or not the given guild owns this geosid
  bool isOwner(GuildID guild) const;
  bool isOwner(GuildInstance* guild) const;

  // Called when a guild begins an attack on this geosid.  If this is a
  // white geosid, the return value is 'false', indicating that the
  // geosid cannot be attacked.
  bool attack(GuildInstance* guild,
              Client* client,
              GeosidAttack* attack_type,
              Time::Milliseconds* attack_timer_delay);

  // Called when the client detects that its geosid attack timer has expired.
  // This method will only apply the attack type if the geosid is in the
  // correct state to do so--this prevents a client from being able to take
  // over a geosid in 1/2 the time if two people burst on the same geosid
  // within seconds of each other, and both are able to stay in range.
  // This method assumes that the client has checked the conditions of erasing
  // the burst attack (leaving the geosid's range).
  void burstAttackTimerExpired(GuildInstance* guild,
                               GeosidAttack attack_type);

  void guardianUnderAttack(Client* attacker);
  void guardianRemoveOwner();
  void guardianSetOwner(GuildInstance* guild);

  // Client uses burst ability of this geosid in the area of a geosid.  This
  // method should use geonite from the client and return magic only if the
  // cost could be paid and the client's guild is the owner of this geosid,
  // or the geosid does not need to be owned (white geosid).
//  bool burst(Client* client,
//             GuildID guild_id,
//             ActorInstance* client_actor,
//             const Magic::Magic** geosid_magic);

  // Returns 'true' and fills the bonus values if the guild is the owner guild.
  bool getOwnershipBenefits(GuildID guild,
                            float* bonus_hp,
                            float* bonus_mp,
                            float* bonus_xp);

  bool canSacrificeItems(Client* client);

  // Obtains the local zone rules that apply to clients in this area
  Zone::ZoneRules zoneRules() const;

  // Invoked by the geosid activation trigger when the client steps into
  // the area of a geosid.
  bool onEnterCheckPortal(Client* client, ActorInstance* actor);

  void getLocation(Number* x, Number* z) const;
  WorldRegion* getLocalRegion();

private:
  void zero();

private:
  Geosid::GeosidServerDescription description_;

  // Which guild owns this geosid?  If this is INVALID_GUILD_ID, there is no
  // current owner.
  GuildID owner_;

  // The instance of the guild that owns this geosid.  If this is not valid,
  // no members of the owning guild are online or the geosid is not owned.
  GuildInstance* owner_instance_;

  // The first time the instance is under attack, until the timer goes off,
  // the under attack message send to the owner guild won't be triggered
  // again.
  Time::Timer<Time::RESOLUTION_60000_MS> under_attack_timer_;

  // How much geonite this geosid has left before it creates its special effect
  Geosid::Geonite geonite_;

  // Control the activation of this geosid's portal
  Time::Timer<Time::RESOLUTION_500_MS> portal_open_timer_, portal_close_timer_;
  size_t active_portal_index_;
};



}
}

#endif