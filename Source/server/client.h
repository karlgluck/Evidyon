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
#ifndef __SERVER_CLIENT_H__
#define __SERVER_CLIENT_H__
#pragma once


#include "shared/networkdecl.h"

#include "shared/avatar/evidyon_avatarinventory.h"
#include "actorcontroller.h"
#include "actorpointer.h"

#include "avatarinventoryitem.h"

#include "shared/currencytype.h"

#include "shared/actor/actorid.h"
#include "serverstorage.h"
#include "clientitemsinterface.h"
#include "itempointer.h"

#include "clienttoclientitemtrade.h"
#include "shared/quest/quest-server.h"

#include "globalquestmanager.h"

#include <set>

#include "shared/actor/actorprofileindex.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/world/mapindex.h"

#include "server/player/player.h"
#include "server/player/playerrequest.h"
#include "server/avatar/avatarenchantments.h"
#include "server/avatar/avatarinstance.h"
#include "server/avatar/avatarstats.h"
#include "shared/evidyon_world.h"
#include "server/client/clientportalmark.h"
#include "shared/world/mapindex.h"
#include "shared/zone/zonerules.h"

namespace Evidyon {

class GlobalDatabaseManager;
class ActorInstance;
struct SpellDescription;
enum AdminSimpleCommand;
namespace Server {
struct MapLocationInfo;
class GuildInstance;
}
namespace Actor {
struct ActorDescription;
}
namespace Magic {
struct Magic_Buff;
}
}


namespace Evidyon {

static const int CLIENT_ATTACKER_HISTORY_LENGTH = 6;




//----[  Client  ]-------------------------------------------------------------
// Terminology:
//  Client - all data/meta-data relating to the player, his/her terminal, the
//           network connection.
//  Player - data that interfaces between the person behind the remote client
//           and the Avatar as a life-form.  Interaction that occurs with menus
//           such as the inventory, storage, bazaar, geosids, etc. belongs in
//           the Client (even if it is based on the Player's state)
//  Avatar - the character that the Player controls
class Client : public ActorController {
  enum State {
    INVALID,
    CONNECTED,
    CONNECTED_VERSIONCONFIRMED,
    LOGGED_INTO_ACCOUNT,
    ENTERING_WORLD, // between account and character states, waiting for client
    LOGGED_INTO_CHARACTER,
  };


public:
  Client();

  void acquire(NetworkPeer* peer);
  void release();

  void sendDisconnectionPacket();

  // Pre-Player::update()
  //  free play time expired? message, + set grace period
  //  grace period expired? idle log off timer expired? -> leave world
  // Post-Player::update()
  //  send inventory / storage updates
  //  if Player::leavingWorld(), log the player out.
  void update(double time, double time_since_last_update);

  void sendSlowSync(float time_since_last_sync);
  void sendFastSync(float time_since_last_sync, int sync_id);

  void handleNetworkPacket(NetworkPacket* packet);

  void sendPacket(NetworkPacket* packet);
  void sendAsyncPacket(NetworkPacket* packet);

  void sendAsyncPacket_UpdateWorldRegion(World::RegionID new_region_id);
  void sendAsyncPacket_ChangeMap(World::MapIndex new_map_id);

  int getLevel();
  void giveMonsterEXP(Avatar::AvatarLevel monster_intended_level,
                      Avatar::AvatarExperience amount);
  void shareMonsterEXP(Avatar::AvatarLevel monster_intended_level,
                       Avatar::AvatarLevel sum_of_party_level_squares,
                       Avatar::AvatarExperience amount);
  void changeEXP(Avatar::AvatarExperience amount);
  void changeMoney(int amount);
  virtual Number changeHP(ActorInstance* source, bool magical, Number amount);
  void changeMP(Number amount);

  // Takes the internal flag and OR-s it with the input flag.  These flags
  // trigger various network messages to be sent.
  void combineUpdateActorDescriptionFlag(bool flag);
  void combineUpdateEnchantmentIconsFlag(bool flag);
  void combineUpdateVisibleEnchantmentsFlag(bool flag);
  void combineUpdateNameFlag(bool flag);
  void combineUpdateGeosidEffectsFlag(bool flag);
  void combineUpdateClientAvatarStatsFlag(bool flag);


  // Adds an item to this client's inventory.  If this method returns 'true',
  // the item was added successfully.
  bool giveItem(ItemPointer* item);

  // Adds the item to the client's inventory.  If it couldn't be completely
  // added, this method will drop the remaining quantity on the ground.  If
  // the actor doesn't exist or something goes wrong, the item is deallocated.
  // After invoking this method, the caller must not try to access the item
  // since what has happened to it isn't clearly defined.
  void giveItemOrDropOnGround(ItemPointer* item_pointer);

  // Looks through the given inventory indices and compares them with other
  // entries in the inventory to see if they can be stacked.
  // Sends an update to the client defining the state of all affected indices.
  void syncCurrency();

  ClientItemsInterface* getItems();
  void sendFullInventoryUpdate();
  void sendSpellAvailabilityUpdate();
  void sendBuffIconsUpdate();
  void broadcastNameUpdate();
  void updateGeosidEffects();

  // Gets information about the avatar the player is controlling
  const Server::AvatarStats* getAvatarStats() const;
  const Server::AvatarInstance* getAvatarInstance() const;

  // Brings this player back from the dead!
  void resurrect();

  // Adds the buff to this client's avatar.  If the caster is not
  // an avatar, just pass NULL and the effect will be applied without any
  // state-based bonuses.
  // This method returns whether or not it was affected by the buff.
  bool applyBuff(const Magic::Magic_Buff* buff,
                 const Server::AvatarInstance* caster_avatar);

  // Returns whether or not this client is able to begin a trade with the
  // client provided in the parameter.  For this method to return true,
  // both the other client != this, the other client's player must be in
  // the "finding trade" state and the actors of the clients must be
  // fairly close to each other.
  bool canBeginTradeWith(Client* other_client) const;

  // Initiates a trade with another client
  void beginTrade(Client* other_client);
  void cancelTrade();

  // Returns true if this client is in the leading-guild state, the other client
  // != this and the clients are close to each other.
  bool canJoinGuildOf(Client* other_client) const;

  // Adds this client to the guild of the leader.  If the leader is trying to
  // start a guild, this will cause the guild to be created.
  // If the player is trying to mess with us and a non-leader types "lead" and
  // another person detects and tries to join, this method is a no-op and
  // cleans up gracefully.
  bool joinGuild(Client* leader);

  // Changes the guild that this client is in.  Should be called by guild instance
  // and guild manager only.  This will cause the name of the actor to be updated.
  void setGuild(Evidyon::Server::GuildInstance* guild);
  Evidyon::Server::GuildInstance* getGuild();

  void logSnapshot();

// questing commands
public:

  // Determines whether or not the client meets the requirements to engage
  // in the given quest.
  Quests::QuestQualification checkQualificationForQuest(unsigned int quest_index);

  // Sends a packet to the client that brings up a quest proposal dialog.  The
  // included information (npc, npc quest index and validation number) must be
  // returned back to the server.
  void sendQuestOffer(unsigned int quest_index,
                      Evidyon::Actor::ActorID npc_actor_id,
                      int npc_quest_index,
                      unsigned int validation_number,
                      const void* /*unimplemented: encryption_key*/);

  // Starts this client on a new quest.  If the quest has a cost, this method
  // will return 'false' if the client can't pay--otherwise, the cost is
  // extracted, the quest is added to the saved set and the quest-begin action
  // is executed.
  bool startOnQuest(unsigned int quest_index);

  // Takes a quest's gamefile-index as a parameter and determines if this
  // client is engaged in that quest.  If it is, the method returns 'true'
  // and sets *local_quest_index to an internal value that is passed to
  // other quest-related Client methods.
  bool isOnQuest(unsigned int quest_index,
                 unsigned int* local_quest_index);

  // Obtains the time remaining on a given quest based on its local quest
  // index.  See isOnQuest.
  void getQuestTime(unsigned int local_quest_index,
                    int* minutes_elapsed,
                    int* minutes_remaining);

  // Polls the quest's parameters to see if this client's status as
  // regards the quest has changed.  If something happens (success/failure)
  // the client will apply the QuestAction accordingly.
  Quests::QuestStatus checkAndApplyQuestStatus(unsigned int local_quest_index);

  // This method is called when this client kills a monster so that its type
  // can be registered with the quest engine.
  void onKilledMonster(int level, unsigned int type);


private:

  // Obtains a pointer to an empty quest-parameters slot.  If a slot could
  // be found, the method returns true.  empty_slot can be NULL.
  bool getEmptyActiveQuest(Quests::Server::QuestParameters** empty_slot);

  // If the quest hash is invalid, this method returns 'invalid_hash_return'
  // and doesn't change minutes_since_completed.  Otherwise, it checks to see
  // if the client has successfully completed the quest more recently than
  // the client has failed at it.  If this is the case, if the parameter
  // *minutes_since_completed is valid, it is set to the number of minutes
  // since it was last successfully completed.  The method then returns true.
  bool hasSucceededAtQuestIfValid(bool invalid_hash_return,
                                  Quests::Server::QuestHash quest_hash,
                                  int* minutes_since_completed);

  // If the quest hash is invalid, this method returns false.  Otherwise,
  // it finds the time since the quest was last attempted.  If it has not
  // been attempted (ending either in success or failure), the method
  // returns false.
  // minutes_since_attempted must not be NULL.
  bool hasAttemptedQuestIfValid(Quests::Server::QuestHash quest_hash,
                                int* minutes_since_attempted);

  // Returns the times at which the client last completed the given quest.  If
  // the quest has not been attempted, zeroes are returned.
  void getQuestTerminationTimes(Quests::Server::QuestHash quest_hash,
                                int* minute_terminated_success,
                                int* minute_terminated_failure);

  // Runs the quest action on this client
  void applyQuestAction(const Quests::Server::QuestAction* action);

  // Terminates the given quest.  This is called either when the parameters are
  // checked and the quest has been succeeded or failed, or when the client
  // voluntarily elects to give up on a quest (fail immediately).
  void endQuest(unsigned int local_quest_index,
                bool succeeded,
                bool over_time_limit);

  // Writes the parameters for quests that this player is engaged in
  // to the database.  This replaces any information currently stored
  // about the same quest.
  void saveQuestsToDatabase(GlobalDatabaseManager* db);

  // Reads the data for the quests this player is actively engaged in
  // from the database.  Activity is defined as having termination
  // times (success and failure) lower than the start time.
  // This method doesn't update the client, and will overwrite any quests
  // currently in the buffer without checking to see if they are valid.
  void loadQuestsFromDatabase(GlobalDatabaseManager* db);

  // Updates the client with the quests that the client is currently
  // engaged in.
  void syncClientQuests();


private:
  static const int MAX_SIMULTANEOUS_QUESTS = 3;
  Quests::Server::QuestParameters active_quests_[MAX_SIMULTANEOUS_QUESTS];


// administrative commands
public:
  void moveTo(World::MapIndex map_index, float x, float y);
  bool loggedIntoAvatar(const std::string& reduced_name) const;


// ActorController
// Virtual method implementations
public:
  virtual Client* getClient() const;
  const char* getName();  // Gets the base name of this client

  // Sends a full name update to the target; this includes the guild-membership
  // tag if the client is part of a guild.
  virtual void tellName(Client* client);

public:
  inline NetworkPeer* getPeer();
  inline bool isValid() const;
  inline unsigned int getLoggedIntoAccountID() const;
  inline unsigned int getLoggedIntoCharacterID() const;
  inline const ActorPointer* getActor() const;
  inline bool hasCurrency(CurrencyType amount) const;
  inline void giveCurrency(CurrencyType amount);
  // returns '0' if the amount can't be taken from this client
  inline CurrencyType takeCurrency(CurrencyType amount);


public:
  bool validTargetOfEnemyAt(float x, float y) const; // used by enemies searching for targets

  // Writes this client's information to the database to make sure everything
  // is synchronized.  This is done for all online clients before a backup
  // of the database is created.
  void syncToDatabase(GlobalDatabaseManager* db);

  // This character is only on a safe zone if it is not evil
  bool onSafeZone() const;
  Zone::ZoneRules getLocalZoneRules() const;

  // Returns 'true' if this client's alignment class is Dreadlord, Champion or
  // Evil.  If a character is evil, safe zones do not work for this character.
  inline bool isEvil() const;

  // Returns 'true' if this client contains source_client in the attackers list
  inline bool attackedBy(const Client* source_client) const;

  // Called when this client kills another client's avatar.  Called internally
  // and by the actor's group when this player is in a group that PKs someone.
  void onPK();


private:
  void zero();

  void commitToDatabase(GlobalDatabaseManager* database);// client

  void leaveWorld();//client
  void logoutAccount();//client

  //// Applies enchantment and item effects to fill out the avatar's current
  //// state (see AvatarAttributes::Enchantable).
  //void buildCurrentAvatarAttributes(double time);//player

  // Called when casting a spell that the client wants to use to make sure
  // that the parameters don't violate any basic rules:  the biggest one is
  // that you can't target beyond your visible radius.
  // client_actor MUST be valid
  bool validateSpellParameters(ActorInstance* client_actor,
                               float x,
                               float y,
                               ActorInstance* target_actor);//player

  //void applySpecialFunctionOfCurrentLocation();

  //// Returns 'true' if the avatar's description needs to be updated.  Should
  //// ONLY be called by buildCurrentAvatarAttributes (it assumes some state
  //// variables that are set therein)
  //bool reequipEquipment(AvatarAttributes::Enchantable* avatar_attributes);//player

  // sends the client's current stats
  void updateClientAvatarStats();

  //// Rebuilds the avatar's HP/MP/speed and other attributes from those defined
  //// in by the game file.  This should be called after adding a stat point,
  //// gaining a level, etc.
  //// This method will invoke buildCurrentAvatarAttributes.
  //void rebuildBaseAvatarAttributes();//player


private:
  // These methods manage the client's storage.  They will send messages to
  // the client to update inventory and storage.
  void storageOpen();
  void storageDeposit(int inventory_index);
  void storageWithdraw(int storage_index);


private:
  // Adds an attacking client to this client's history.
  // Every time the character regenerates, ID # 0 is added to the history
  // so that old attackers (or accidental attackers) aren't penalized for
  // what new attackers do.
  // If the other attacker is not the aggressor, the character id is zero.
  void addAttackerToHistory(unsigned int character_id,
                            Client* source_client);
public://made public so it can be called by Player
  // Looks up players that are in this client's attacker list and does
  // alignment modification.  This will also drop items where the actor
  // is standing if the flag is set.  Times when items should not be
  // dropped include when the actor falls in a pit or is swimming in
  // water, since the items can't be recovered in these situations.
  // dont_drop_equipment is set when 'wishing' items have prevented the
  // equipment from dropping
  void onAvatarDeath(ActorInstance* my_actor,
                     bool drop_items,
                     bool dont_drop_equipment);


private:
  // Defines a number -100 to +100 that defines the avatar's alignment.  This
  // method will send an update packet to the client.
  void setAlignmentValue(int alignment);

  // Obtains the class of this avatar's alignment
  Actor::ActorAlignment getAlignmentClass() const;

  // The returned value is randomized based on the character's alignment
  int getNumberOfEquippedItemsToDropOnDeath() const;

  // Time since the client last killed another player
  unsigned int hoursSinceLastPK() const;

  // In the introductory levels, a player cannot be pked, does not drop any
  // items on death, and the damage he or she deals is not pvp-adjusted
  // In the non-introductory levels, a player's can be pked, drops items
  // on death and his or her damage is pvp-adjusted by:
  //  damage target takes
  //    = damage * (target's level + OFFSET) / (this level + OFFSET)
  inline bool isInIntroductoryLevels() const;

  //// Returns false if this player can't take damage from other players.
  //inline bool isHighEnoughLevelToBePKed() const;

  //// Returns false if this player should not drop equipped items at all
  //inline bool isHighEnoughLevelToDropEquipment() const;

  //// Returns false if this player can't drop inventory items or equipped items
  //inline bool isHighEnoughLevelToDropAnyItems() const;

  // Returns the character ID of the client paired with the provided structure
  // if it is in the attackers list; otherwise, returns 0.
  inline unsigned int getAggressorClientCharacterID(const Client* source_client) const;

  // A client cannot leave the world if he or she is engaged in combat.  This is
  // defined as the condition in which one of this client's attackers lists this
  // client as an attacker (i.e. both have damaged each other recently).
  inline bool inCombat() const;

  // Returns the safest zone in which an attacker of this client is standing.  This
  // is used to "absorb" this client into its attackers regions (this way, a player
  // standing in a geosid area can attack outside the area with spells, but others
  // will get geosid rules even if they aren't inside).
  // If the client is standing in a zone with more favorable rules than any
  // attacker, the client's own rules are returned.
  inline Zone::ZoneRules mostFavorableZoneRules() const;

private:
  void openActorGroup();
  void closeActorGroup();
  void leaveActorGroup();
  bool joinActorGroup(ActorInstance* actor);


private:

  // This client has attempted to do something invalid.  This method is called
  // every time the client passes information that fails sanity/validity checks
  // of what is expected from the client.  It can be used to identify potential
  // hackers.
  void addRedFlag(const char* file, unsigned int line);


private:
  void restoreHPMP();


private:
  void handleNetworkPacketWhenConnected(NetworkPacket* packet);
  void handleNetworkPacketWhenLoggedIntoAccount(NetworkPacket* packet);
  void handleNetworkPacketWhenEnteringWorld(NetworkPacket* packet);
  void handleNetworkPacketWhenLoggedIntoCharacter(NetworkPacket* packet);

  void NetMsg_TellClientVersionInfo(NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_CreateAccount(NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_AccessAccount(NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_CreateCharacter(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_DeleteCharacter(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_AskAccountInfo(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_AskCharacterName(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_EnterWorld(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_LogoutAccount(GlobalDatabaseManager* db, NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_LeaveWorld(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_AskActorFullDescriptions(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_RequestMove(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_RequestMoveStreaming(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_CastSpell(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_UseItem(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Request_Speak(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_ChangeEquippedState(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_DropInventoryItems(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Drop_Currency(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_AskActorName(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_AskMyAvatarStats(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_StorageOpen(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_StorageWithdraw(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_StorageDeposit(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_GeosidSacrifice(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_TradeAddItem(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Trade_SetCurrency(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_TradeRemoveItem(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_TradeAccept(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_TradeWithdraw(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_TradeCancel(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_SplitStackedItem(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_StopAction(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_Bazaar_Open(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Bazaar_CreateListing(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Bazaar_RetractListing(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Bazaar_RedeemListing(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Bazaar_PurchaseListing(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Bazaar_GetListings(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_Quest_Trigger(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Quest_OfferResponse(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Quest_GiveUp(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);

	void NetMsg_Guild_Create(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
	void NetMsg_Guild_Lead(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
	void NetMsg_Guild_Join(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
	void NetMsg_Guild_Leave(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
	void NetMsg_Guild_Promote(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
	void NetMsg_Guild_Demote(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
	void NetMsg_Guild_Chat(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
	void NetMsg_Guild_Kick(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
	void NetMsg_Guild_Members(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_Guild_AskGeosids(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Guild_SetHome(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Guild_Abandon(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Guild_Home(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Geosid_DropSacrifice(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Geosid_Burst(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Geosid_BurstAttackGeosid(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Geosid_BurstSpell(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Geosid_BurstUseGeosid(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Geosid_GetOwner(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_Party_Chat(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Party_Lead(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Party_Join(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Party_Leave(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Party_Kick(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Party_Close(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_Resurrect_StandUp(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Resurrect_Respawn(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);

  void NetMsg_AutoTarget_All(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_AutoTarget_Monsters(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);
  void NetMsg_Mark(ActorInstance* actor, NetworkPacket* packet, NetworkPacket** reply_packet);


  void bazaar_tellOwnListedItems();
  void bazaar_tellCategoryListedItems(BazaarCategory category);


private:
  State state_;
  NetworkPeer* peer_;
  unsigned int account_id_, character_id_;

  Time::Timer<Time::RESOLUTION_60000_MS> idle_logoff_timer_;
  Time::Timer<Time::RESOLUTION_60000_MS> daily_free_play_timer_;
  Time::Timer<Time::RESOLUTION_60000_MS> daily_free_play_grace_period_timer_;
  ActorPointer actor_;  // represents the avatar in the world
  Server::Player player_;       // interfaces with player of game -> avatar
  Server::PlayerRequest request_;
  Server::AvatarInstance avatar_;
  Server::AvatarEnchantments avatar_enchantments_;
  Server::AvatarStats avatar_stats_;
  ClientItemsInterface items_;
  ClientToClientItemTrade item_trader_;

  // Saves the ID numbers of the last few characters that attacked this avatar
  struct AttackerHistory {
    unsigned int character_id;
    Client* source_client;
  };
  AttackerHistory attacker_history_[CLIENT_ATTACKER_HISTORY_LENGTH];


private:
  static std::set<unsigned int> active_accounts_;

public:
  virtual void fillDescription(Evidyon::Actor::ActorDescription* description);
  void broadcastUpdateDescriptionPacket();
  void respawn(bool restore_hpmp, bool penalize_xp); // sends the player back home
  void getPortalMark(unsigned int mark, Map** map, Number* x, Number* z);
  void changePortalMarkToChaos(unsigned int mark,
                               Map** map,
                               Number* x,
                               Number* z);
  inline bool isAdmin() const;
private:
  NetworkPacket* buildTellNamePacket();

private:
  bool update_actor_description_;
  bool update_enchantment_icons_;
  bool update_visible_enchantments_;
  bool update_name_;
  bool update_geosid_effects_;
  bool update_client_avatar_stats_;
  Server::ClientPortalMark portal_marks_[Server::ClientPortalMark::NUMBER_PER_CLIENT];
  Server::GuildInstance* guild_;
  bool is_admin_, hide_helmet_;
};

#include "client_inline.h"


}


#endif