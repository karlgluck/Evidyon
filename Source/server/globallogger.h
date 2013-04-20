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
#ifndef __SERVER_GLOBALLOGGER_H__
#define __SERVER_GLOBALLOGGER_H__
#pragma once

#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatargender.h"
#include "common/kg/singleton.h"
#include "server/itempointer.h"
#include <string>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

namespace Evidyon {
namespace Server {
struct AvatarInstance;
struct AvatarStats;
}
}


#define log_create_avatar Evidyon::Server::GlobalLogger::singleton()->createAvatar
#define log_account_login Evidyon::Server::GlobalLogger::singleton()->accountLogin
#define log_account_login_failed Evidyon::Server::GlobalLogger::singleton()->accountLoginFailed
#define log_account_logout Evidyon::Server::GlobalLogger::singleton()->accountLogout
#define log_delete_character Evidyon::Server::GlobalLogger::singleton()->accountDeleteCharacter
#define log_snapshot Evidyon::Server::GlobalLogger::singleton()->snapshot
#define log_chat Evidyon::Server::GlobalLogger::singleton()->chat
#define log_party_chat Evidyon::Server::GlobalLogger::singleton()->partyChat
#define log_guild_chat Evidyon::Server::GlobalLogger::singleton()->guildChat
#define log_trade Evidyon::Server::GlobalLogger::singleton()->trade
#define log_bazaar_purchase Evidyon::Server::GlobalLogger::singleton()->bazaarPurchase
#define log_formatted  Evidyon::Server::GlobalLogger::singleton()->logFormattedText

namespace Evidyon {
namespace Server {


  
//----[  GlobalLogger  ]-------------------------------------------------------
class GlobalLogger : public kg::Singleton<GlobalLogger> {
public:
  GlobalLogger();
  bool create(const char* disk_database_file);
  void destroy();

  void createAvatar(unsigned int account_id,
                    unsigned int character_id,
                    const char* name,
                    Avatar::AvatarRace race,
                    Avatar::AvatarClass class_,
                    Avatar::AvatarGender gender);

  void accountLogin(unsigned int account_id);
  void accountLoginFailed(unsigned int account_id,
                          const char* failed_password);
  void accountLogout(unsigned int account_id);
  void accountDeleteCharacter(unsigned int account_id,
                              unsigned int character_id);

  void snapshot(unsigned int character_id,
                unsigned int account_id,
                float x,
                float z,
                unsigned int map,
                const AvatarInstance* avatar,
                const AvatarStats* stats);

  void chat(unsigned int character_id,
            float x,
            float z,
            const char* text,
            size_t text_len);
  void partyChat(unsigned int character_id,
                 unsigned __int64 party_id,
                 const char* text,
                 size_t text_len);
  void guildChat(unsigned int character_id,
                 unsigned int guild_id,
                 const char* text,
                 size_t text_len);

  void trade(unsigned int a_character_id,
             ItemPointer* a_items,
             size_t number_of_a_items,
             unsigned int a_gold,
             unsigned int b_character_id,
             ItemPointer* b_items,
             size_t number_of_b_items,
             unsigned int b_gold);

  void bazaarPurchase(unsigned int character_id,
                      const char* item_name,
                      unsigned int price);

  void logFormattedText(const char* format, ...);

public:

  // Saves this database over the on-disk copy.  This method is called from the
  // GlobalDatabaseManager's backup thread.  This will block periodically to
  // allow concurrent transactions.
  // If to_disk is false, the backup reads from disk into memory
  void executeBackup(bool to_disk = true);

private:
  sqlite3* db_;
  std::string disk_database_file_;
};





}
}

#endif