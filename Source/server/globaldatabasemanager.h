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
#ifndef __GLOBALDATABASEMANAGER_H__
#define __GLOBALDATABASEMANAGER_H__
#pragma once


#include <dcx/asynchronoussisothreaddatacontroller>
#include <string>

#include "shared/actor/evidyon_actor.h"
#include "itemidtype.h"
#include "uniqueavatarnamechecker.h"
#include "serverstorage.h"

#include "shared/client-server/bazaar.h"

#include "globalbazaarmanager.h"
#include "shared/world/mapindex.h"
#include "shared/world/maphash.h"
#include "server/guild/guildid.h"
#include "server/avatar/avatarid.h"
#include "common/number.h"
#include "shared/geosid/geosidhashid.h"
#include "shared/geosid/geosidindex.h"
#include "shared/geosid/geonite.h"

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef void *HANDLE;

namespace Evidyon {

struct AvatarAttributes;
struct AvatarInventoryItem;

class Map;

namespace Server {
struct AvatarInstance;
struct ClientPortalMark;
class GuildInstance;
}

namespace Bazaar {
typedef __int64 ListingID;

// Contains the information needed to display a complete item
struct ItemProperties {
  unsigned int type;
  unsigned int quantity;
};

namespace Server {

struct SellerListing {
  Bazaar::ListingID listing_id;
  ItemProperties item;
  bool open; // true -> not sold, false -> has been purchased for $$
  unsigned int price;
  int offers;
};

struct OpenListing {
  Bazaar::ListingID listing_id;
  ItemProperties item;
  unsigned int price;
  int offers;
};

struct BuyerOfferElement {
  ItemProperties item;
};

struct ListingForBuyerOffer {
  Bazaar::ListingID listing_id;
  ItemProperties item;
  unsigned int price;

  enum Status {
    OFFER_UNPROCESSED,
    OFFER_REJECTED,
    OFFER_ACCEPTED,
  } offer_status;

  // How many items the buyer offered for this listing
  int number_of_items_offered;
};

}
}

namespace Quests {
namespace Server {
struct QuestParameters;
}
}



//----[  GlobalDatabaseManager  ]----------------------------------------------
class GlobalDatabaseManager {
public:
  enum DBResult
  {
    DBR_SUCCESS,
    DBR_ALREADYEXISTS,
    DBR_DOESNTEXIST,
    DBR_INCORRECTPASSWORD,
    DBR_OVERLIMIT,
    DBR_INTERNALERROR,
  };

public:
  static GlobalDatabaseManager* singleton();


public:

  GlobalDatabaseManager();
  ~GlobalDatabaseManager();

  bool create(const char* configurationFilename);
  void destroy();

  /**
   * Determines whether or not the provided account number is valid
   */
  static bool isValidAccountNumber(unsigned int id);

  /**
   * Tries to create an account with the given settings
   * @param password The password of the account
   * @param account_id The ID number of the account that was created
   * @return What happened during the process:
   *      DBR_SUCCESS - the account was created successfully
   *      DBR_INTERNALERROR
   */
  DBResult createAccount(const char* password, unsigned int* id);

  /**
   * Attempts to access the account with the provided ID number
   * @param id The ID number of the account to access
   * @param password The password with which to compare to the account's password
   * @return One of:
   *      DBR_SUCCESS - the password matches the one for the account
   *      DBR_DOESNTEXIST - no account with the given ID number exists
   *      DBR_INCORRECTPASSWORD - the password doesn't match the one for the account
   *      DBR_INTERNALERROR
   */
  DBResult accessAccount(unsigned int id, const char* password);

  /**
   * Gets a list of all of the characters that are linked to the account with the given ID.  The
   * provided list should have at least MAX_AVATARS_PER_ACCOUNT elements.
   * @param account_id The account for which to query for characters
   * @param characterIDArray The destination array into which to put character IDs
   * @param numberOfCharacters The destination variable for how many characters were returned.  If
   *      there is an error during the query (not due to invalid parameters) this will be set
   *      to zero.
   * @return One of:
   *      DBR_SUCCESS - the character list was retrieved successfully
   *      DBR_DOESNTEXIST - no account for the given ID number exists
   *      DBR_INTERNALERROR
   */
  DBResult getCharactersOnAccount(unsigned int account_id, unsigned int* characterIDArray, size_t* numberOfCharacters);

  /**
   * 
   */
  //DBResult changeAccountPassword(unsigned int id, const char* oldPassword, const char* newPassword);

  /**
   * Tries to create a character with the provided attributes
   * @param account_id The ID of the account with which to associate the character
   * @param name The name of the character to create
   * @param character_id Destination field for the ID number of the character that was created
   * @return One of:
   *      DBR_SUCCEESS - the character was created and linked to the account successfully
   *      DBR_OVERLIMIT - there are too many characters on this account to make another one
   *      DBR_ALREADYEXISTS - there is already a character with that name
   *      DBR_INTERNALERROR - something went horribly wrong
   */
  DBResult createCharacter(unsigned int account_id,
                           const char* name,
                           unsigned int* character_id,
                           float worldX,
                           float worldY,
                           World::MapIndex map,
                           bool male, int chRace, int chClass, int level, int attributes[5]);

  /**
   * Unlinks a character from the account.  When the user deletes a character from their account, it is
   * simply removed from being linked into the account--it isn't deleted.
   * @param account_id The ID number of the account from which to remove the character
   * @param character_id The character to remove from the account
   * @return One of:
   *      DBR_SUCCESS - the character was removed successfully
   *      DBR_DOESNTEXIT - either the account doesn't exit or the character isn't on this account
   *      DBR_INTERNALERROR
   */
  DBResult removeCharacterFromAccount(unsigned int account_id, unsigned int character_id);

  /**
   * Gets the amount of play time that an account has free.  The daysLeft is calculated absolutely,
   * so it is negative if the account is out of time.
   * @param account_id The ID number of the account for which to retrieve the time left
   * @param daysLeft Output variable for the number of days left on this account
   * @return One of:
   *      DBR_SUCCESS - the account time was retrieved successfully
   *      DBR_DOESNTEXIST - the requested account ID doesn't exist
   *      DBR_INTERNALERROR
   */
  DBResult getAccountTimeLeft(unsigned int account_id, float* daysLeft);

  /**
   * Gets the name of the character with the given ID
   */
  DBResult getCharacterName(unsigned int account_id, unsigned int character_id, char* nameBuffer, size_t nameBufferLength);

  /**
   * Gets the full description of a character
   */
  DBResult getCharacterDesc(unsigned int account_id,
                            unsigned int character_id,
                            char* name_buffer,
                            size_t name_buffer_length,
                            ActorTypeIndex* actor_type,
                            size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT],
                            int* map_index,
                            float* world_x,
                            float* world_y);

  /**
   * Gets information about a character.  The character must exist within the account, or this
   * method fails.
   */
  DBResult getCharacterData(unsigned int account_id,
                            unsigned int character_id,
                            float* x,
                            float* y,
                            Server::AvatarInstance* avatar,
                            int* hp,
                            int* mp,
                            bool* is_admin,
                            World::MapIndex* map_index,
                            Evidyon::Server::ClientPortalMark* portal_marks,
                            bool* hide_helmet);

  // Saves data about an avatar.  Be sure that the hashes for the portals have been
  // correctly set before calling this method.
  DBResult commitCharacterData(unsigned int account_id,
                               unsigned int character_id,
                               float x,
                               float y,
                               Server::AvatarInstance* avatar,
                               int hp,
                               int mp,
                               bool is_admin,
                               World::MapIndex map_index,
                               const Evidyon::Server::ClientPortalMark* portal_marks,
                               bool hide_helmet);

  DBResult renameCharacter(const char* old_name, const char* new_name);
  DBResult findAccountFromName(const char* name, unsigned int* number);

  //----------------------------------------------------------------------------------------------------
  // Avatars
  //----------------------------------------------------------------------------------------------------
  // Makes all players reassign their avatars' attributes starting at level 1.
  void avatarReassignAllAttributes();
  
  // Sets the last pk hour for all avatars to be the current hour.  Also resets the kill counts.
  void avatarResetAllPKs(unsigned int current_hour);

  //----------------------------------------------------------------------------------------------------
  // Items
  //----------------------------------------------------------------------------------------------------
  // Called when a client has become the owner of an item.  Will replace any
  // current entry in the database.
  void itemChangeOwner(ItemIDType item_id,
                       unsigned int account_id,
                       unsigned int character_id,
                       unsigned int session_id);
  void itemConfirmOwner(ItemIDType item_id,
                        unsigned int account_id,
                        unsigned int character_id,
                        unsigned int session_id);

  // Removes the item's entry in the database so that it can't be duplicated accidentally
  void itemOnAvatarErased(ItemIDType item_id);

  // Reads ID #s from all items in the database, and gives them new, incremental IDs starting at ID# 0.
  //   441502930 -> 0
  //  5019238507 -> 1
  //   990783092 -> 2
  //         ... -> ...
  // Any active items will not be properly repacked (their ID#s will not change when re-committed).
  void itemRepackAllIDs();


  //----------------------------------------------------------------------------------------------------
  //  Inventory
  //----------------------------------------------------------------------------------------------------
  bool inventoryCheckOut(unsigned int account_id,
                         unsigned int character_id,
                         unsigned int session_id,
                         AvatarInventoryItem* inventory,
                         unsigned int* items_returned_in_inventory);
  bool inventoryCheckIn(unsigned int account_id,
                        unsigned int character_id,
                        unsigned int session_id,
                        AvatarInventoryItem* inventory,
                        unsigned int items_in_inventory);

  //----------------------------------------------------------------------------------------------------
  //  Storage
  //----------------------------------------------------------------------------------------------------
  bool storageCheckOut(unsigned int account_id,
                       unsigned int character_id,
                       unsigned int session_id,
                       ServerStorageItem* storage,
                       unsigned int* items_returned_in_storage);
  bool storageCheckIn(unsigned int account_id,
                      unsigned int character_id,
                      unsigned int session_id,
                      ServerStorageItem* storage,
                      unsigned int items_in_storage);







  //----------------------------------------------------------------------------------------------------
  //  Bazaar
  //----------------------------------------------------------------------------------------------------

  // Called every few minutes to update the cached listings in the bazaar
  void bazaar_rotateListings();

  // This will automatically remove the given item id from the active items table
  bool bazaar_addListing(BazaarListingID listing_id,
                         unsigned int category,
                         CurrencyType price,
                         ItemIDType item_id,
                         unsigned int owner_account_id,
                         unsigned int permanent_storage_hash,
                         unsigned int quantity);
  bool bazaar_getListing(BazaarListingID listing_id,
                         unsigned int* seller_account_id,
                         unsigned int* category,
                         ItemIDType* item_id,
                         ItemInBazaar* listing);
  void bazaar_getCategoryListings(unsigned int category,
                                  unsigned int lower,
                                  unsigned int upper,
                                  ItemInBazaar cache[MAX_ITEMS_PER_BAZAAR_CATEGORY],
                                  size_t* number_of_listings);
  unsigned int bazaar_getNumberOfListedItems(unsigned int seller_account_id);
  unsigned int bazaar_getNumberOfItemsInCategory(unsigned int category);
  void bazaar_removeListing(BazaarListingID listing_id);

  void bazaar_insertSale(unsigned int seller_account_id,
                         const ItemInBazaar* sold_listing);
  bool bazaar_removeSale(BazaarListingID listing_id,
                         unsigned int seller_account_id,
                         CurrencyType* profit);

  // This method will return both active listings and sales for the given account
  size_t bazaar_getOwnListings(unsigned int seller_account_id,
                               ItemInBazaar* buffer,
                               size_t buffer_size);



  DBResult commitCharacterMoney(unsigned int account_id, unsigned int character_id, unsigned int money);






  //----------------------------------------------------------------------------------------------------
  //  Quests
  //----------------------------------------------------------------------------------------------------

  // Returns 'true' and fills the termination time database parameters for
  // the given quest if an entry for it exists in the database.  Otherwise,
  // the method returns 'false' and the parameters are unchanged.
  bool questQueryTerminationTimes(unsigned int character_id,
                                  unsigned int quest_hash,
                                  int* minute_terminated_success,
                                  int* minute_terminated_failure);

  // Writes the data for a single quest into the database.  Replaces any
  // existing information.
  bool questWrite(unsigned int character_id,
                  const Quests::Server::QuestParameters* parameters);

  // Reads data for all of a character's active quests from the database.
  // This method returns the number of quests read.  Before calling this
  // method, the quests_array should be reset to a default state.
  int questReadActive(unsigned int character_id,
                      Quests::Server::QuestParameters* quests_array,
                      size_t quests_array_size);




  //----------------------------------------------------------------------------------------------------
  //  Guilds
  //----------------------------------------------------------------------------------------------------
  Server::GuildID getHighestGuildID();
  bool guildRetrieve(Server::GuildID id,
                     char* guild_name,
                     size_t name_length,
                     Server::AvatarID* founder_id,
                     Map** home_map,
                     Number* home_x,
                     Number* home_z,
                     Server::GuildInstance* instance);

  // Removes this guild from the database
  void guildErase(Server::GuildID guild_id);

  // Clears the guild entirely from the database (if it exists) and defines
  // its name and the founder's id number.  This will NOT add the founder
  // as a member (call guildMemberSet)
  void guildSet(Server::GuildID guild_id,
                const char* guild_name,
                Server::AvatarID founder_id,
                Map* home_map,
                Number home_x,
                Number home_z);

  // Adds the member to the guild.  Will not add a member twice, so this
  // should only be called after calling guildSet.
  void guildMemberSet(Server::GuildID guild_id,
                      Server::AvatarID member_id,
                      bool leader);

  void guildGetName(Server::GuildID guild_id,
                    char* guild_name,
                    size_t max_length);

  //----------------------------------------------------------------------------------------------------
  //  Geosids
  //----------------------------------------------------------------------------------------------------

  // Returns the owner of this geosid, or INVALID_GUILD_ID if none exists.
  void getGeosidData(Geosid::GeosidHashID geosid,
                     Server::GuildID* owner_guild,
                     Geosid::Geonite* geonite);
  void geosidSet(Geosid::GeosidHashID id,
                 Server::GuildID owner,
                 Geosid::Geonite geonite);


  int sqlite3_executeQuery(const char* query, char** errorMessage);


  // Used to add names of monsters, races/classes, etc. to the reserved name list.  This method
  // is used to provide the structure to the gamefile manager when the server starts.
  UniqueAvatarNameChecker* getUniqueAvatarNameChecker();


  // Generates a query that can be stepped at the request of an administrator
  bool adminQueryOpen(const std::string& query, sqlite3_stmt** stmt_out);
  void adminQueryGetError(std::string* error);
  bool adminQueryStep(sqlite3_stmt* stmt, std::string* query_result);
  void adminQueryClose(sqlite3_stmt** stmt);

  // Gets information about average characters at this level
  void adminQueryCharacterStatistics(int class_type, int level, std::string* query_result);

  GlobalBazaarManager* bazaar() { return &bazaar_; }

public:
  // Performs an asynchronous backup of this database to the hard disk.  This
  // method will do nothing if a backup is already in progress.
  void startBackup();

  // Finds out whether or not a backup thread is active
  bool backupInProgress();

  // Performs a complete backup of the in-memory database to the hard disk in a way
  // that at any point, the server can recover from the previous backup until this
  // one is completed.  After executing, if the server is unexpectedly shut off,
  // it will recover the correct active state of the database.   This is a blocking
  // method with incremental sleeps to allow concurrent transactions.
  // It is safe to call this method from the main thread, but it can take an
  // unbounded amount of time to return, so it is more efficient to use startBackup
  // to spawn a thread that runs this method exclusively.
  void executeBackup();


private:

  /// Database connection to the in-memory database.
  sqlite3* myDB;

  /// The last time at which the database was backed up
  unsigned long myLastBackupTime;

  /// The next available character ID
  unsigned int myNextCharacterID;

  /// Makes sure that characters don't have the same name
  UniqueAvatarNameChecker unique_avatar_name_checker_;

  /// Non-NULL if a database backup is in progress
  HANDLE backup_thread_;

  /// Bazaar
  GlobalBazaarManager bazaar_;

private:
  static GlobalDatabaseManager* singleton_;
};

}


#endif