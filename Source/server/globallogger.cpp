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
#include "globallogger.h"
#include "common/sqlite/sqlite3.h"
#include "server/avatar/avatarinstance.h"
#include "server/avatar/avatarstats.h"
#include "common/kg/gid/sqlitepreparedstmt.h"
#include "server/time.h"
#include "globalgamefilemanager.h"
#include "server/item.h"
#include "shared/item/itemserverdescription.h"
#include <stdarg.h>
#include <string.h>
#include <windows.h> // for OutputDebugString

namespace Evidyon {
namespace Server {

/*
characters
  character_id
  account_id
  name
  race
  class
  gender
  creation_minute

account_activity
  minute
  account_id
  type
  parameter

snapshots
  minute_counter
  character_id
  x
  z
  map
  level
  experience
  penalty_xp
  guild_id
  money
  geonite
  alignment
  str
  agi
  con
  int
  wis
  hp
  hp_regen
  max_hp
  mp
  mp_regen
  max mp
  armor
  total_monsters_killed

chat
  character_id
  x
  z
  text

party_chat
  character_id
  party_id
  text

guild_chat
  character_id
  guild_id
  text

trades
  trade_id (primary key, autoincrement)
  minute_of_trade
  a_character_id
  a_gold
  b_character_id
  b_gold

trade_items_a
  trade_id (crossreferenced)
  item_name

trade_items_b
  trade_id (crossreferenced)
  item_name

bazaar_purchases
  character_id
  item_name
  price

log
  minute
  text

*/


//----[  GlobalLogger  ]-------------------------------------------------------
GlobalLogger::GlobalLogger() {
  db_ = NULL;
}



//----[  create  ]-------------------------------------------------------------
bool GlobalLogger::create(const char* disk_database_file) {
  disk_database_file_ = disk_database_file;

  sqlite3_open(":memory:", &db_);
  if (!db_) return false;
  executeBackup(false); // false = from-disk

  int rc = SQLITE_OK;
  char* error_msg = NULL;
#define evidyon_db_exec(sql)\
    if (rc != SQLITE_OK && rc != SQLITE_DONE) goto ERROR_LBL;\
    rc = sqlite3_exec(db_, sql, NULL, NULL, &error_msg);

  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS characters ("\
                    "character_id INTEGER PRIMARY KEY,"\
                    "account_id INTEGER UNSIGNED,"\
                    "name VARCHAR,"\
                    "race VARCHAR,"\
                    "class VARCHAR,"\
                    "gender VARCHAR,"\
                    "creation_minute INTEGER UNSIGNED)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS account_activity("\
                    "minute INTEGER UNSIGNED,"\
                    "account_id INTEGER UNSIGNED,"\
                    "type VARCHAR,"\
                    "parameter VARCHAR)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS snapshots("\
                    "minute_counter INTEGER UNSIGNED,"\
                    "character_id INTEGER_UNSIGNED,"\
                    "x REAL,"\
                    "z REAL,"\
                    "map INTEGER UNSIGNED,"\
                    "level INTEGER UNSIGNED,"\
                    "experience BIGINT,"\
                    "penalty_xp BIGINT,"\
                    "guild_id INTEGER UNSIGNED,"\
                    "money INTEGER UNSIGNED,"\
                    "geonite INTEGER UNSIGNED,"\
                    "alignment INTEGER UNSIGNED,"\
                    "str INTEGER UNSIGNED,"\
                    "agi INTEGER UNSIGNED,"\
                    "con INTEGER UNSIGNED,"\
                    "int INTEGER UNSIGNED,"\
                    "wis INTEGER UNSIGNED,"\
                    "hp INTEGER UNSIGNED,"\
                    "hp_regen INTEGER UNSIGNED,"\
                    "max_hp INTEGER UNSIGNED,"\
                    "mp INTEGER UNSIGNED,"\
                    "mp_regen INTEGER UNSIGNED,"\
                    "max_mp INTEGER UNSIGNED,"\
                    "armor INTEGER UNSIGNED,"\
                    "total_monsters_killed INTEGER UNSIGNED)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS chat("\
                    "character_id INTEGER UNSIGNED,"\
                    "x REAL,"\
                    "z REAL,"\
                    "text VARCHAR)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS party_chat("\
                    "character_id INTEGER UNSIGNED,"\
                    "party_id BIGINT,"\
                    "text VARCHAR)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS guild_chat("\
                    "character_id INTEGER UNSIGNED,"\
                    "guild_id INTEGER UNSIGNED,"\
                    "text VARCHAR)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS trades("\
                    "trade_id INTEGER PRIMARY KEY,"\
                    "minute_of_trade INTEGER UNSIGNED,"\
                    "a_character_id INTEGER UNSIGNED,"\
                    "a_gold INTEGER_UNSIGNED,"\
                    "b_character_id INTEGER UNSIGNED,"\
                    "b_gold INTEGER UNSIGNED)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS trade_items_a("\
                    "trade_id INTEGER UNSIGNED,"\
                    "item_name VARCHAR)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS trade_items_b("\
                    "trade_id INTEGER UNSIGNED,"\
                    "item_name VARCHAR)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS bazaar_purchases("\
                    "character_id INTEGER UNSIGNED,"\
                    "item_name VARCHAR,"\
                    "price INTEGER UNSIGNED)");
  evidyon_db_exec(  "CREATE TABLE IF NOT EXISTS log("\
                    "minute INTEGER UNSIGNED,"\
                    "text VARCHAR)");

  return true;

ERROR_LBL:
  OutputDebugString("\n\n====================== Database Error ===================\n");
  OutputDebugString(error_msg);
  OutputDebugString("\n=========================================================\n\n");
  sqlite3_free(error_msg);
  return false;
}



//----[  destroy  ]------------------------------------------------------------
void GlobalLogger::destroy() {
  if (db_) {
    sqlite3_close(db_);
    db_ = NULL;
  }
}



//----[  createAvatar  ]-------------------------------------------------------
void GlobalLogger::createAvatar(unsigned int account_id,
                                unsigned int character_id,
                                const char* name,
                                Avatar::AvatarRace race,
                                Avatar::AvatarClass class_,
                                Avatar::AvatarGender gender) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO characters VALUES (?,?,?,?,?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, character_id);
  sqlite3_bind_int(insert.stmt, 2, account_id);
  sqlite3_bind_text(insert.stmt, 3, name, strlen(name), SQLITE_STATIC);
  sqlite3_bind_text(insert.stmt, 4,
    Avatar::AvatarRaceString(race),
    strlen(Avatar::AvatarRaceString(race)), SQLITE_STATIC);
  sqlite3_bind_text(insert.stmt, 5,
    Avatar::AvatarClassString(class_),
    strlen(Avatar::AvatarClassString(class_)), SQLITE_STATIC);
  sqlite3_bind_text(insert.stmt, 6,
    Avatar::AvatarGenderString(gender),
    strlen(Avatar::AvatarGenderString(gender)), SQLITE_STATIC);
  sqlite3_bind_int(insert.stmt, 7, Time::minute_counter);
  insert.stepReset();
}


//----[  accountLogin  ]-------------------------------------------------------
void GlobalLogger::accountLogin(unsigned int account_id) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO account_activity VALUES (?,?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, Time::minute_counter);
  sqlite3_bind_int(insert.stmt, 2, account_id);
  static const char* TEXT = "login";
  sqlite3_bind_text(insert.stmt, 3, TEXT, strlen(TEXT), SQLITE_STATIC);
  sqlite3_bind_text(insert.stmt, 4, "", 0, SQLITE_STATIC);
  insert.stepReset();
}


//----[  accountLoginFailed  ]-------------------------------------------------
void GlobalLogger::accountLoginFailed(unsigned int account_id, const char* failed_password) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO account_activity VALUES (?,?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, Time::minute_counter);
  sqlite3_bind_int(insert.stmt, 2, account_id);
  static const char* TEXT = "failed login";
  sqlite3_bind_text(insert.stmt, 3, TEXT, strlen(TEXT), SQLITE_STATIC);
  sqlite3_bind_text(insert.stmt, 4, failed_password, strlen(failed_password), SQLITE_STATIC);
  insert.stepReset();
}


//----[  accountLogout  ]------------------------------------------------------
void GlobalLogger::accountLogout(unsigned int account_id) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO account_activity VALUES (?,?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, Time::minute_counter);
  sqlite3_bind_int(insert.stmt, 2, account_id);
  static const char* TEXT = "logout";
  sqlite3_bind_text(insert.stmt, 3, TEXT, strlen(TEXT), SQLITE_STATIC);
  insert.stepReset();
}


//----[  accountDeleteCharacter  ]---------------------------------------------
void GlobalLogger::accountDeleteCharacter(unsigned int account_id,
                                          unsigned int character_id) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO account_activity VALUES (?,?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, Time::minute_counter);
  sqlite3_bind_int(insert.stmt, 2, account_id);
  static const char* TEXT = "delete character";
  sqlite3_bind_text(insert.stmt, 3, TEXT, strlen(TEXT), SQLITE_STATIC);
  sqlite3_bind_int(insert.stmt, 4, character_id);
  insert.stepReset();
}


//----[  snapshot  ]-----------------------------------------------------------
void GlobalLogger::snapshot(unsigned int character_id,
                            unsigned int account_id,
                            float x,
                            float z,
                            unsigned int map,
                            const AvatarInstance* avatar,
                            const AvatarStats* stats) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO snapshots VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, Time::minute_counter);
  sqlite3_bind_int(insert.stmt, 2, character_id);
  sqlite3_bind_double(insert.stmt, 3, x);
  sqlite3_bind_double(insert.stmt, 4, z);
  sqlite3_bind_int(insert.stmt, 5, map);
  sqlite3_bind_int(insert.stmt, 6, avatar->level);
  sqlite3_bind_int64(insert.stmt, 7,
    avatar->experience_this_level +
    GlobalGamefileManager::singleton()->getExperienceToAchieveLevel(avatar->level));
  sqlite3_bind_int64(insert.stmt, 8, avatar->penalty_experience);
  sqlite3_bind_int(insert.stmt, 9, avatar->guild);
  sqlite3_bind_int(insert.stmt, 10, avatar->money);
  sqlite3_bind_int(insert.stmt, 11, -1);
  sqlite3_bind_int(insert.stmt, 12, avatar->alignment_value);
  sqlite3_bind_int(insert.stmt, 13, avatar->attribute_values[Avatar::ATTRIBUTE_STRENGTH]);
  sqlite3_bind_int(insert.stmt, 14, avatar->attribute_values[Avatar::ATTRIBUTE_AGILITY]);
  sqlite3_bind_int(insert.stmt, 15, avatar->attribute_values[Avatar::ATTRIBUTE_CONSTITUTION]);
  sqlite3_bind_int(insert.stmt, 16, avatar->attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE]);
  sqlite3_bind_int(insert.stmt, 17, avatar->attribute_values[Avatar::ATTRIBUTE_WISDOM]);
  sqlite3_bind_int(insert.stmt, 18, stats->hp.wholePart());
  sqlite3_bind_int(insert.stmt, 19, stats->hp_regen.wholePart());
  sqlite3_bind_int(insert.stmt, 20, stats->max_hp.wholePart());
  sqlite3_bind_int(insert.stmt, 21, stats->mp.wholePart());
  sqlite3_bind_int(insert.stmt, 22, stats->mp_regen.wholePart());
  sqlite3_bind_int(insert.stmt, 23, stats->max_mp.wholePart());
  sqlite3_bind_int(insert.stmt, 24, stats->armor.wholePart());
  sqlite3_bind_int(insert.stmt, 25, avatar->total_monsters_killed);
  insert.stepReset();
}



//----[  chat  ]---------------------------------------------------------------
void GlobalLogger::chat(unsigned int character_id,
                        float x,
                        float z,
                        const char* text,
                        size_t text_len) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO chat VALUES (?,?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, character_id);
  sqlite3_bind_double(insert.stmt, 2, x);
  sqlite3_bind_double(insert.stmt, 3, z);
  sqlite3_bind_text(insert.stmt, 4, text, text_len, SQLITE_STATIC);
  insert.stepReset();
}




//----[  partyChat  ]----------------------------------------------------------
void GlobalLogger::partyChat(unsigned int character_id,
                             unsigned __int64 party_id,
                             const char* text,
                             size_t text_len) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO party_chat VALUES (?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, character_id);
  sqlite3_bind_int64(insert.stmt, 2, party_id);
  sqlite3_bind_text(insert.stmt, 3, text, text_len, SQLITE_STATIC);
  insert.stepReset();
}




//----[  guildChat  ]----------------------------------------------------------
void GlobalLogger::guildChat(unsigned int character_id,
                             unsigned int guild_id,
                             const char* text,
                             size_t text_len) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO guild_chat VALUES (?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, character_id);
  sqlite3_bind_int(insert.stmt, 2, guild_id);
  sqlite3_bind_text(insert.stmt, 3, text, text_len, SQLITE_STATIC);
  insert.stepReset();
}

//----[  trade  ]--------------------------------------------------------------
void GlobalLogger::trade(unsigned int a_character_id,
                         ItemPointer* a_items,
                         size_t number_of_a_items,
                         unsigned int a_gold,
                         unsigned int b_character_id,
                         ItemPointer* b_items,
                         size_t number_of_b_items,
                         unsigned int b_gold) {
  static kg::gid::SQLitePreparedStmt insert_trade(
    db_,
    "INSERT INTO trades (minute_of_trade,a_character_id,a_gold,b_character_id,b_gold) VALUES (?,?,?,?,?)");
  sqlite3_bind_int(insert_trade.stmt, 1, Time::minute_counter);
  sqlite3_bind_int(insert_trade.stmt, 2, a_character_id);
  sqlite3_bind_int(insert_trade.stmt, 3, a_gold);
  sqlite3_bind_int(insert_trade.stmt, 4, b_character_id);
  sqlite3_bind_int(insert_trade.stmt, 5, b_gold);
  insert_trade.stepReset();

  __int64 trade_id = sqlite3_last_insert_rowid(db_);

  static kg::gid::SQLitePreparedStmt insert_a_item(
    db_,
    "INSERT INTO trade_items_a VALUES (?,?)");
  for (size_t i = 0; i < number_of_a_items; ++i) {
    ItemInstance* item = a_items[i].dereferenceAssumingValid();
    const char* name = item->getDescription()->true_name;
    sqlite3_bind_int64(insert_a_item.stmt, 1, trade_id);
    sqlite3_bind_text(insert_a_item.stmt, 2, name, strlen(name), SQLITE_STATIC);
    insert_a_item.stepReset();
  }

  static kg::gid::SQLitePreparedStmt insert_b_item(
    db_,
    "INSERT INTO trade_items_b VALUES (?,?)");
  for (size_t i = 0; i < number_of_b_items; ++i) {
    ItemInstance* item = b_items[i].dereferenceAssumingValid();
    const char* name = item->getDescription()->true_name;
    sqlite3_bind_int64(insert_b_item.stmt, 1, trade_id);
    sqlite3_bind_text(insert_b_item.stmt, 2, name, strlen(name), SQLITE_STATIC);
    insert_b_item.stepReset();
  }
}



//----[  bazaarPurchase  ]-----------------------------------------------------
void GlobalLogger::bazaarPurchase(unsigned int character_id,
                                  const char* item_name,
                                  unsigned int price) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO bazaar_purchases VALUES (?,?,?)");
  sqlite3_bind_int(insert.stmt, 1, character_id);
  sqlite3_bind_text(insert.stmt, 2, item_name, strlen(item_name), SQLITE_STATIC);
  sqlite3_bind_int(insert.stmt, 3, price);
  insert.stepReset();
}




//----[  logFormattedText  ]---------------------------------------------------
void GlobalLogger::logFormattedText(const char* format, ...) {
  static kg::gid::SQLitePreparedStmt insert(
    db_,
    "INSERT INTO log VALUES (?,?)");
  sqlite3_bind_int(insert.stmt, 1, Time::minute_counter);

  va_list args;
  va_start(args, format);
  char buffer[1024];
  _vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer), format, args);
  buffer[sizeof(buffer)-1] = '\0';
  va_end(args);

  sqlite3_bind_text(insert.stmt, 4, buffer, strlen(buffer), SQLITE_STATIC);
  insert.stepReset();
}



//----[  executeBackup  ]------------------------------------------------------
void GlobalLogger::executeBackup(bool to_disk) {

  // This disables logging, which can cause server lag issues when the
  // disk drive is slow.
  return;

  // Open the database file
  sqlite3* disk_db;
  int rc = sqlite3_open_v2(disk_database_file_.c_str(), &disk_db, SQLITE_OPEN_CREATE|SQLITE_OPEN_READWRITE, NULL);
  if (rc == SQLITE_OK) {
    sqlite3_backup* backup
      = to_disk ? sqlite3_backup_init(disk_db, "main", db_, "main")
                : sqlite3_backup_init(db_, "main", disk_db, "main");
    if (backup) {
      static const int PAGES_PER_STEP = 5;
      static const int SLEEP_MILLIS_PER_STEP = 250;
      
      do {
        rc = sqlite3_backup_step(backup, PAGES_PER_STEP);
        if (to_disk) sqlite3_sleep(SLEEP_MILLIS_PER_STEP);
      } while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

      sqlite3_backup_finish(backup);
    }

    // get the error code (if there was one)
    rc = sqlite3_errcode(disk_db);
  }

  // free the disk DB connection
  sqlite3_close(disk_db);
}



}
}

