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
#include "globaldatabasemanager.h"
#include <dcx/win32>
#include <time.h>     // For getting formatted time
#include <math.h>
#include <direct.h>
#include "../common/sqlite/sqlite3.h"
#include <dc/debug>
#include "globalgamefilemanager.h"
#include "shared/server-editor/server_gamefile.h"
#include "shared/avatar/evidyon_avatarinventory.h"

#include "globalitemmanager.h"
#include "item.h"
#include "../common/safemacros.h"
#include "randint32.h"
#include "shared/evidyon_accounts.h"
#include "databaseinventoryitem.h"
#include "avatarinventoryitem.h"
#include "server/avatar/avatarinstance.h"
#include <direct.h>
#include "common/md5/md5wrapper.h"


#include "common/fracture/todo.h"

#include <map>
#include <list>

#include <wininet.h>  // used to let us upload the database to the internet
#pragma comment(lib, "wininet.lib")


#include "globalquestmanager.h" // for QuestParameters structure
#include "globalworldmanager.h" // translate map hash
#include "server/client/clientportalmark.h"
#include "server/guild/guildinstance.h"
#include "server/guild/globalguildmanager.h"
#include "server/geosid/globalgeosidmanager.h"
#include "server/globalclientmanager.h"

#include "common/kg/gid/sqlitepreparedstmt.h"
// used when cleaning up prepared stmts
kg::gid::SQLitePreparedStmt* kg::gid::SQLitePreparedStmt::first = 0;

#include "server/globallogger.h"

namespace Evidyon {

#define DEF_DATABASE_NAME   "evidyon"
#define DEF_DATABASE_EXT    "sqldb"

static const char* DATABASE_NAME = DEF_DATABASE_NAME;
static const char* DATABASE_FILE_NAME = DEF_DATABASE_NAME "." DEF_DATABASE_EXT;

#undef DEF_DATABASE_NAME
#undef DEF_DATABASE_EXT

//DWORD WINAPI DiskDBThread(void* pointer)
//{
//  GlobalDatabaseManager* db = (GlobalDatabaseManager*)pointer;
//  db->executeDiskDBThread();
//  return S_OK;
//}



namespace DatabaseColumns {

enum CharacterColumn {
  ACCOUNT_ID,
  CHARACTER_ID,
  NAME,
  WORLD_X,
  WORLD_Y,
  GENDER,
  RACE,
  CLASS,
  LEVEL,
  HP_MAX,
  HP_CURRENT,
  MP_MAX,
  MP_CURRENT,
  EXP_CURRENT,
  STR,
  AGI,
  CON,
  INT,
  WIS,
  ABILITY_PTS,
  MONEY,
  MAP_HASH,
  IS_ADMIN,
  KILL_COUNT_AT_LAST_PK,
  LAST_PK_HOUR,
  ALIGNMENT_VALUE,
  PENALTY_XP,
  TOTAL_MONSTERS_KILLED,
  HIDE_HELMET,
  NUMBER_OF_CHARACTER_COLUMNS,
};

}



DWORD WINAPI GlobalDatabaseManager_RunBackup(void* void_db) {
  GlobalDatabaseManager* db = (GlobalDatabaseManager*)void_db;
  db->executeBackup();
  Server::GlobalLogger::singleton()->executeBackup();
  return S_OK;
}

GlobalDatabaseManager* GlobalDatabaseManager::singleton_ = NULL;

GlobalDatabaseManager* GlobalDatabaseManager::singleton() {
  return singleton_;
}


//-----------------------------------------------------------------------------
// Name: GlobalDatabaseManager
// Desc: 
//-----------------------------------------------------------------------------
GlobalDatabaseManager::GlobalDatabaseManager() {
  myDB = 0; // Reset the database connection pointer

  // Setting this to 0 when the server starts makes the server back up the database when it
  // starts, regardless of anything else.  This is usually a good idea.
  myLastBackupTime = 0;

  todo("karl","convert old-style SQLite database queries into new statment-based versions");


  myNextCharacterID = 0;

  backup_thread_ = NULL;
}



//-----------------------------------------------------------------------------
// Name: GlobalDatabaseManager
// Desc: 
//-----------------------------------------------------------------------------
GlobalDatabaseManager::~GlobalDatabaseManager() {

  // Make sure the database connection closed
  if (APP_WARNING(myDB != 0)("Management database was still open when the server closed")) {
    destroy();
  }

}



//-----------------------------------------------------------------------------
// Name: open
// Desc: 
//-----------------------------------------------------------------------------
bool GlobalDatabaseManager::create(const char* configurationFilename) {

  std::string sqldbFileNameString  = DATABASE_FILE_NAME,
       oldSQLDBFileNameString  = "";
  const char* sqldbFileName = sqldbFileNameString.c_str();
  bool blankDB = true;

  // Move the existing database file to a backup file name
  {
    // Generate the file name to which to move the database
    char file[256], mask[256];
    _mkdir("./backups/");
    sprintf_s(mask, sizeof(mask), "./backups/%s - %%s.sqldb", DATABASE_NAME);

    // Find the current time
    tm * loc;
    time_t ltime;
    time(&ltime);
  	  loc = localtime(&ltime);

    // Print the date and time
    char dateTime[32];
	    asctime_s(dateTime, sizeof(dateTime), loc);
    dateTime[strlen(dateTime)-1] = '\0';

    // Create the file
    sprintf_s(file, sizeof(file), mask, dateTime);

    // Remove invalid characters
    for (int i = 0; file[i] != '\0'; ++i) {
      if (file[i] == ':')
        file[i] = '\'';
    }

    // Copy to this file
    if (CopyFile(sqldbFileNameString.c_str(), file, FALSE))
    {
      oldSQLDBFileNameString = file;
      blankDB = false;
    }
  }

  // Read the configuration file and generate the data formats table
  {
    // Open the database for this execution of the server (it shouldn't exit right now)
    sqlite3_open(":memory:", &myDB);

    // Make sure that the databases were opened correctly
    CONFIRM(myDB) else { destroy(); return false; }

    // Result codes
    int rc;
    char* error_message;

    // stores a list of index tables to generate
    std::string current_table_name;
    typedef std::list<std::pair<std::string,std::string>> IndicesToCreate;
    IndicesToCreate indices_to_create;

    // Start writing to the database
    CONFIRM(SQLITE_OK == (rc = sqlite3_executeQuery("BEGIN", &error_message)));

    // Create a table that holds the structure of this database
    rc = sqlite3_executeQuery("CREATE TABLE meta_structure (table_name VARCHAR, field_name VARCHAR, field_type VARCHAR, import_default VARCHAR)", &error_message);

    // Read the database structure definition file and use it to (1) fill the meta_structure
    // table with entries and (2) create tables in this database
    FILE* pf = 0;
    CONFIRM(0 == fopen_s(&pf, configurationFilename, "rt")) else { destroy(); return false; }
    bool fileEnded = 0 != feof(pf);
    bool readFirstTableName = false;
    bool readFirstField = false;
    char tableSQL[2048];
    char completeTableSQL[2048];
    char metaStructureInsertSQLMask[1024];

    while(!fileEnded) {
      // Character arrays used to read data from the database configuration file
      char line[512];     line[0] = '\0';
      char label[128];    label[0] = '\0';
      char type[128];     type[0] = '\0';
      char importValue[256];  importValue[0] = '\0';
      bool completedTableSQL = false;

      // Read each line of the config file
      fscanf_s(pf, " %[^\n\r]", line, sizeof(line));
      sscanf_s(line, "%s %[^;];%[^\n\r]",
               label, sizeof(label),
               type, sizeof(type),
               importValue, sizeof(importValue));

      // If the table's SQL is complete (i.e. we've started reading a different table), copy the statement over
      if (readFirstTableName && (type[0] == '\0'))
      {
        strcpy_s(completeTableSQL, sizeof(completeTableSQL), tableSQL);
        completedTableSQL = true;
      }

      // If this is a table header, create table information
      if (type[0] == '\0')
      {
        // Save the table name so that indices can be created in it
        current_table_name = label;

        // This string builds the table-creation SQL
        sqlite3_snprintf(sizeof(tableSQL), tableSQL, "CREATE TABLE %Q (", label);

        // Create the insert mask for meta_structure entries related to this table
        sqlite3_snprintf(sizeof(metaStructureInsertSQLMask), metaStructureInsertSQLMask, "INSERT INTO meta_structure VALUES (%Q,%%Q,%%Q,%%Q)", label);

        // Reset the flag that tells the app whether or not it's read the first field entry
        // (this is used to control commas)
        readFirstField = false;
      }
      else
      {
        char sql[1024];

        // Check to see if the field's name begins with a *.  If it does, the
        // database should create an index on this field since it will be
        // often used in searches.
        if (label[0] == '*') {
          std::string field = &label[1];
          indices_to_create.push_back(IndicesToCreate::value_type(current_table_name, field));
          strcpy_s(label, 128, field.c_str());  // erase the *
        }

        // Add this field name to the table SQL
        sprintf_s(sql, sizeof(sql), "%s%s %s", readFirstField ? ", " : "", label, type);
        strcat_s(tableSQL, sizeof(tableSQL), sql);
        readFirstField = true;

        // Employ the insert mask to generate an SQL statement to insert the field format into the meta table.
        sqlite3_snprintf(sizeof(sql), sql, metaStructureInsertSQLMask, label, type, importValue);

        // Insert into our table_structure table
        rc = sqlite3_executeQuery(sql, &error_message);
        if (APP_ERROR(rc != SQLITE_OK)("Problem inserting table entry into meta_structure table:  %s (sql=%s)", error_message, sql))
        {
          fclose(pf);
          destroy();
          return false;
        }
      }

      // If the file ends, we're done reading
      if (fileEnded = (0 != feof(pf)))
      {
        strcpy_s(completeTableSQL, sizeof(completeTableSQL), tableSQL);
        completedTableSQL = true;
      }

      // If the file is finished or this type is not a database, add the current
      // table entry to the 
      if (completedTableSQL)
      {
        // Terminate the table-creation SQL
        strcat_s(completeTableSQL, sizeof(completeTableSQL), ")");

        // Execute this SQL
        rc = sqlite3_executeQuery(completeTableSQL, &error_message);
        if (APP_ERROR(rc != SQLITE_OK)("Problem creating table:  %s (sql=%s)", error_message, completeTableSQL))
        {
          fclose(pf);
          destroy();
          return false;
        }
      }

      // We've now read the name of the first table
      readFirstTableName = true;
    }

    // Generate the indices
    for (IndicesToCreate::iterator i = indices_to_create.begin(); i != indices_to_create.end(); ++i) {
      const char* table = i->first.c_str();
      const char* field = i->second.c_str();
      char sql[1024];
      sqlite3_snprintf(1024,
                       sql,
                       "CREATE INDEX index_%s_%s ON %s (%s)",
                       table,
                       field,
                       table,
                       field);

      // Execute this SQL
      rc = sqlite3_executeQuery(sql, &error_message);
      CONFIRM(rc == SQLITE_OK) else {
        DEBUG_ERROR("Problem creating table index:  %s (sql=%s)",
                    error_message,
                    completeTableSQL);
        fclose(pf);
        destroy();
        return false;
      }
    }

    // Close the configuration file
    fclose(pf);
  }

  // Perform import if a previous database exists
  if (!blankDB)
  {
    // Open the previous database
    sqlite3* previousDB;
    sqlite3_open(oldSQLDBFileNameString.c_str(), &previousDB);

    char sql[1024];

    // Query for each of the tables in the new database
    sqlite3_snprintf(sizeof(sql), sql, "SELECT DISTINCT table_name FROM meta_structure");
    sqlite3_stmt* newTables;
    sqlite3_prepare_v2(myDB, sql, sizeof(sql), &newTables, 0);

    // Iterate through all of the results
    while(SQLITE_ROW == sqlite3_step(newTables)) {

      // Query for fields of this table in the new and old databases
      sqlite3_stmt* oldTableFields;
      sqlite3_stmt* newTableFields;
      const char* tableName = (const char*)sqlite3_column_text(newTables, 0);
      sqlite3_snprintf(sizeof(sql), sql, "SELECT field_name, field_type, import_default FROM meta_structure WHERE table_name=%Q", tableName);
      sqlite3_prepare_v2(previousDB, sql, sizeof(sql), &oldTableFields, 0);
      sqlite3_prepare_v2(myDB, sql, sizeof(sql), &newTableFields, 0);

      struct Field {
        std::string field_name;
        std::string field_type;
        std::string import_default;
      };

      // Add all of the fields from the databases into a list
      typedef std::map<std::string,Field> FieldTable;
      FieldTable newFields;
      while (SQLITE_ROW == sqlite3_step(newTableFields)) {

        // Add this to the new fields
        Field field;
        field.field_name   = (const char*)sqlite3_column_text(newTableFields, 0);
        field.field_type   = (const char*)sqlite3_column_text(newTableFields, 1);
        field.import_default = (const char*)sqlite3_column_text(newTableFields, 2);
        newFields.insert(FieldTable::value_type(field.field_name, field));
      }

      // As long as this entry exists in the new database, add it
      FieldTable oldFields;
      while (SQLITE_ROW == sqlite3_step(oldTableFields)) {

        // Add this to the new fields
        Field field;
        field.field_name   = (const char*)sqlite3_column_text(oldTableFields, 0);
        field.field_type   = (const char*)sqlite3_column_text(oldTableFields, 1);
        field.import_default = (const char*)sqlite3_column_text(oldTableFields, 2);
        oldFields.insert(FieldTable::value_type(field.field_name, field));
      }

      // Finalize the queries
      sqlite3_finalize(newTableFields);
      sqlite3_finalize(oldTableFields);

      // Remove any fields that exist only in the old database
      for (FieldTable::iterator i = oldFields.begin(); i != oldFields.end();) {

        if (newFields.find(i->first) == newFields.end())
        {
          FieldTable::iterator next = i; next++;
          oldFields.erase(i);
          i = next;
        } else {
          ++i;
        }
      }

      // Removes any fields that exist in the old database (leaving only newly created ones)
      for (FieldTable::iterator i = newFields.begin(); i != newFields.end();) {

        if (oldFields.find(i->first) != oldFields.end()) {
          FieldTable::iterator next = i; next++;
          newFields.erase(i);
          i = next;
        } else {
          ++i;
        }
      }

      // Query for old fields
      strcpy_s(sql, sizeof(sql), "SELECT ");
      for (FieldTable::iterator i = oldFields.begin(); i != oldFields.end(); ++i)
        strcat_s(sql, sizeof(sql), (std::string((i == oldFields.begin()) ? "" : ",") + i->first).c_str());
      strcat_s(sql, sizeof(sql), " FROM ");
      strcat_s(sql, sizeof(sql), tableName);
      strcat_s(sql, sizeof(sql), " WHERE 1");
      sqlite3_stmt* oldFieldValues;
      sqlite3_prepare_v2(previousDB, sql, sizeof(sql), &oldFieldValues, 0);

      // If we couldn't prepare the old fields values statement, warn about this.  It's
      // likely, however, that we are creating a new database
      if (APP_WARNING(!oldFieldValues)("Couldn't query for old fields from '%s'; this is OK if it's a new table", tableName))
        continue;

      // Build an insert statement to transform the results into 
      char insertStatementFields[1024];
      char insertStatementValues[1024];
      sqlite3_snprintf(sizeof(insertStatementFields), insertStatementFields, "INSERT INTO %s (", tableName);
      sqlite3_snprintf(sizeof(insertStatementValues), insertStatementValues, ") VALUES (");

      // Add the default values first, so we can assign them directly
      bool insertedFieldsFirst = false, insertedValuesFirst = false;
      for (FieldTable::iterator i = newFields.begin(); i != newFields.end(); ++i) {
        strcat_s(insertStatementFields,
               (std::string(insertedFieldsFirst ? ", " : "") + i->first).c_str());
        strcat_s(insertStatementValues,
               (std::string(insertedValuesFirst ? ", " : "") + i->second.import_default).c_str());
        insertedFieldsFirst = true;
        insertedValuesFirst = true;
      }

      // Add the field names that will be pulled from the old database
      for (FieldTable::iterator i = oldFields.begin(); i != oldFields.end(); ++i)
      {
        strcat_s(insertStatementFields, sizeof(insertStatementFields), (std::string(insertedFieldsFirst ? "," : "") + i->first).c_str());
        insertedFieldsFirst = true;
      }

      // Add the default values and such
      strcat_s(insertStatementFields, sizeof(insertStatementFields), insertStatementValues);

      // Query the old database table and insert into the new database for each element
      bool insertedValuesFirstInTemplate = insertedValuesFirst;
      while(SQLITE_ROW == sqlite3_step(oldFieldValues))
      {
        // Copy the old string
        strcpy_s(sql, sizeof(sql), insertStatementFields);
        insertedValuesFirst = insertedValuesFirstInTemplate;

        // Add each field values
        int index = 0;
        for (FieldTable::iterator i = oldFields.begin(); i != oldFields.end(); ++i, ++index)
        {
          if (insertedValuesFirst)
            strcat_s(sql, sizeof(sql), ", ");
          char value[256];
          sqlite3_snprintf(sizeof(value), value, "%Q", (const char*)sqlite3_column_text(oldFieldValues, index));
          strcat_s(sql, sizeof(sql), value);
          insertedValuesFirst = true;
        }

        // Complete the statement
        strcat_s(sql, sizeof(sql), ")");

        // Execute the insertion
        char* error_message;
        //int rc = sqlite3_exec(myDB, sql, 0, 0, &error_message);
        int rc = sqlite3_executeQuery(sql, &error_message);
        if (APP_ERROR(rc != SQLITE_OK)("Couldn't insert into DB:  %s  (sql=%s)", error_message, sql)) {
          todo("karl", "clean up the database");
          return false;
        }
      }

      // Close the query
      sqlite3_finalize(oldFieldValues);
    }
    sqlite3_finalize(newTables);

    // Close the old database
    sqlite3_close(previousDB);
  }

  { // Create a statement to pick the maximum character ID
    sqlite3_stmt* stmt;
    const char query[] = "SELECT MAX(character_id) FROM characters";
    sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
    int rc = sqlite3_step(stmt);

    // Make sure this succeeded
    if (!APP_WARNING(rc != SQLITE_ROW)("Unable to get maximum character ID: unless this is the first time the server is being run, new characters may fail to be created"))
    {
      myNextCharacterID = sqlite3_column_int(stmt,0);
    }
    else
      myNextCharacterID = 0;

    DEBUG_INFO("%lu characters have been created on the server", myNextCharacterID);

    // Erase the statement structure memory
    sqlite3_finalize(stmt);
  }

  { // import all character names into the unique name function
    sqlite3_stmt* stmt;
    const char query[] = "SELECT name FROM characters WHERE account_id != 0";
    sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      std::string name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
      unique_avatar_name_checker_.addName(name);
    }
    sqlite3_finalize(stmt);

    { // add guild names
      const char guild_query[] = "SELECT name FROM guilds WHERE 1";
      sqlite3_prepare_v2(myDB, guild_query, sizeof(guild_query), &stmt, 0);
      while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        unique_avatar_name_checker_.addName(name);
      }
      sqlite3_finalize(stmt);
    }

    // nobody can use these reserved names
    unique_avatar_name_checker_.addName("evidyon");
    unique_avatar_name_checker_.addName("geosid");
    unique_avatar_name_checker_.addName("genoite");
    unique_avatar_name_checker_.addName("karl");
    unique_avatar_name_checker_.addName("karlgluck");
    unique_avatar_name_checker_.addName("evidyon");
  }

  { // remove all active items, since they're left over from the last restart
    CONFIRM(SQLITE_OK == sqlite3_executeQuery("DELETE FROM 'active_items' WHERE 1", NULL));
  }

  // Commit changes to the database
  CONFIRM(SQLITE_OK == (sqlite3_executeQuery("COMMIT", 0)));


  singleton_ = this;

  CONFIRM(bazaar_.create(this)) else {
    destroy();
    return false;
  }

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name: destroy
// Desc: 
//-----------------------------------------------------------------------------
void GlobalDatabaseManager::destroy() {

  singleton_ = NULL;

  if (backupInProgress()) { // wait for the current backup to complete
    SetThreadPriority(backup_thread_, THREAD_PRIORITY_HIGHEST);
    while (WAIT_TIMEOUT == WaitForSingleObject(backup_thread_, INFINITE)) {
      printf("Waiting for in-progress backup to finish...");
    }
  }

  bazaar_.destroy();

  // make absolutely sure the database is completely backed up by doing it again
  executeBackup();

  kg::gid::SQLitePreparedStmt::finalizeAll();

  if (myDB != 0) {
    sqlite3_close(myDB);
    myDB = 0;
  }
}



//-----------------------------------------------------------------------------
// Name: isValidAccountNumber
// Desc: Determines whether or not the provided account number is valid
//-----------------------------------------------------------------------------
bool GlobalDatabaseManager::isValidAccountNumber(unsigned int id)
{
  return !(id < 1000000 || id > 9999999);
}



//-----------------------------------------------------------------------------
// Name: createAccount
// Desc: Tries to create an account with the given settings
//-----------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::createAccount(const char* password, unsigned int* id)
{
  // These variables are used during SQL queries
  int rc;
  char* error_message;

  // This is the ID number of the created account
  unsigned int idNumber;

  // Calculate the expiration date for this new account
  __int64 expirationDate = 0;
  {
    time_t timeIn14Days;
    time(&timeIn14Days);
    timeIn14Days += 60 * 60 * 24 * 14;
    expirationDate = (__int64)timeIn14Days;
  }

  // Build the SQL query.  The %Q option inserts a string from the parameter list in which every
  // "'" character is doubled, and the entire phrase is surrounded by single-quotes
  do
  {
    // Create a random ID number with 7 digits
    char idNumberString[8];
    idNumberString[0] = '1' + rand()%9; // 1st char is digit 1-9 (never 0!)
    idNumberString[1] = '\0';
    for (int i = 1; i < 7; ++i)
    {
      char number[2];
      sprintf_s(number, sizeof(number), "%i", rand()%10);
      strcat_s(idNumberString, sizeof(idNumberString), number);
    }

    // Get the account ID number
    idNumber = (unsigned int)atoi(idNumberString);

    // Make sure the number is in the right range
    if (!isValidAccountNumber(idNumber))
      continue;

    // Print the integer
    char expirationDateString[64];
    sprintf_s(expirationDateString, sizeof(expirationDateString), "%I64d", expirationDate);

    // Salt the password with the account number & "secret" string
    char combined_password[128];
    sprintf_s(combined_password, 128, "%s%s%s", idNumberString, password, "evidyon");
    md5wrapper wrapper;
    std::string password_str = wrapper.getHashFromString(combined_password);
    password = password_str.c_str();

    // Build the query
    char query[512];
    sqlite3_snprintf(sizeof(query),
                     query,
                     "INSERT INTO accounts (account_id,password,time_expiration_date) VALUES (%lu,%Q,%s)",
                     idNumber,
                     password,
                     expirationDateString);

    // Execute the query
    rc = sqlite3_executeQuery(query, &error_message);

    // If we get a constraint error, that means we didn't generate a unique account number.  Just
    // run the generation routine again.
    if (rc == SQLITE_CONSTRAINT) continue; 

    // Any other errors in the database are fatal
    if (APP_ERROR(rc != SQLITE_OK)("Couldn't execute query (%s) during createAccount:  %s", query, error_message))
      return DBR_INTERNALERROR;

  } while(0 == sqlite3_total_changes(myDB));

  // Save the account's ID
  *id = idNumber;

  // Success!
  return DBR_SUCCESS;
}




//-----------------------------------------------------------------------------
// Name: accessAccount
// Desc: Attempts to access the account with the provided ID number
//-----------------------------------------------------------------------------
GlobalDatabaseManager::DBResult
GlobalDatabaseManager::accessAccount(unsigned int id, const char* password) {

  // Fail automatically for bad account numbers
  if (!isValidAccountNumber(id)) return DBR_DOESNTEXIST;

  // Build the SQL query
  char query[128];
  sqlite3_stmt* stmt;
  sqlite3_snprintf(sizeof(query), query, "SELECT password FROM accounts WHERE account_id=%lu", id);
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);

  // Execute the query
  switch(sqlite3_step(stmt))
  {
    case SQLITE_ROW:
    {
      // Compare the password
      DBResult retval = DBR_INCORRECTPASSWORD;
      const char* passwordFromDatabase = (const char*)sqlite3_column_text(stmt, 0);
      if ((passwordFromDatabase != 0) &&
        (0 == strcmp(passwordFromDatabase, password)))
        retval = DBR_SUCCESS;

      // Erase the query
      sqlite3_finalize(stmt);

      // Return the result
      return retval;
    }

    default:
      return DBR_DOESNTEXIST;
  }
}



//-----------------------------------------------------------------------------
// Name: getCharactersOnAccount
// Desc: Gets a list of all of the characters that are linked to the account with the given ID
//-----------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::getCharactersOnAccount(
  unsigned int account_id, unsigned int* characterIDArray, size_t* numberOfCharacters)
{
  // Fail for bad account numbers
  if (!isValidAccountNumber(account_id)) return DBR_DOESNTEXIST;

  // Build the SQL query
  char query[128];
  sqlite3_snprintf(sizeof(query), query, "SELECT character_id FROM characters WHERE account_id=%lu", account_id);

  // Create a statement
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);

  // Stores the number of characters in the account
  size_t characters = 0;

  // Reset the output parameter
  *numberOfCharacters = 0;

  // Iterate through all of the results
  int rc;
  while (SQLITE_ROW == (rc = sqlite3_step(stmt))) {
    // Make sure that we haven't obtained too many results
    if (!APP_ERROR(characters >= MAX_AVATARS_PER_ACCOUNT)("Account %lu has too many characters!", account_id))
    {
      // Get the value of the character's ID from the query
      sscanf_s((char*)sqlite3_column_text(stmt, 0), "%lu", &characterIDArray[characters]);

      // Increment the number of characters
      characters++;
    }
  }

  sqlite3_finalize(stmt);

  if (SQLITE_DONE != rc) {
    // If we get here, something went wrong with the query
    *numberOfCharacters = 0;
    return DBR_INTERNALERROR;
  }

  // Finalize the statement
  *numberOfCharacters = characters;

  // Success
  return DBR_SUCCESS;
}




//-----------------------------------------------------------------------------
// Name: createCharacter
// Desc: Tries to create a character with the provided attributes
//-----------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::createCharacter(
    unsigned int account_id,
    const char* name,
    unsigned int* character_id,
    float worldX,
    float worldY,
    World::MapIndex map,
    bool male,
    int chRace,
    int chClass,
    int level,
    int attributes[5]) {

  // Fail for bad account numbers
  if (!isValidAccountNumber(account_id)) return DBR_DOESNTEXIST;

  // Make sure the name doesn't already exist
  if (!unique_avatar_name_checker_.isNameAvailable(std::string(name))) return DBR_ALREADYEXISTS;

  // Get the next available character ID
  unsigned int newCharacterID = ++myNextCharacterID;

  // Build the SQL query
  char query[2048];
  strcpy_s(query, sizeof(query), "INSERT INTO characters (account_id, character_id, name, map_hash, world_x, world_y, gender, race, class, "\
                   "level, hp_current, mp_current, exp_current, str, agi, con, "\
                   "int, wis, penalty_xp, alignment_value) VALUES (");

  {
    char sql[256];
    sqlite3_snprintf(sizeof(sql), sql, "%lu,%lu,%Q", account_id, newCharacterID, name);
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%lu,%.02f,%.02f,%i", map, worldX, worldY, (male ? 0 : 1));
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%i,%i,%i", chRace, chClass, level);
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",99,99,0"); // hp, mp, last one is EXP--note, hp/mp are limited at login (and no lvl 1 char has 99 hp or mp)
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%i,%i,%i,%i,%i", attributes[0], attributes[1], attributes[2], attributes[3], attributes[4]);
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%i,25)", 0); // default alignment for new chars is 25
    strcat_s(query, sizeof(query), sql);
  }

  //DEBUG_INFO("query:  %s", query);

  int rc = sqlite3_executeQuery(query, 0);

  if (rc == SQLITE_CONSTRAINT) return DBR_ALREADYEXISTS;
  if (rc != SQLITE_OK && rc != SQLITE_DONE) return DBR_INTERNALERROR;
  *character_id = newCharacterID;

 // // commit initial stats
 // for (int attribute_index = 0; attribute_index < 5; ++attribute_index) {
 //   unsigned int inc = attributes[attribute_index] - 10;
 //   sqlite3_snprintf(sizeof(query),
 //                    query,
 //                    "INSERT INTO avatar_attribute_increments "\
 //                    "(character_id, level_minus_one, attribute, amount)"\
 //                    "VALUES (%lu, 0, %lu, %lu)",
 //                    newCharacterID,
 //                    attribute_index,
 //                    inc);
 //   ASSERT(SQLITE_OK == sqlite3_executeQuery(query, NULL));
 //} 

  return DBR_SUCCESS;
}






//-----------------------------------------------------------------------------
// Name: removeCharacterFromAccount
// Desc: Unlinks a character from the account
//-----------------------------------------------------------------------------
GlobalDatabaseManager::DBResult
GlobalDatabaseManager::removeCharacterFromAccount(unsigned int account_id,
                                                  unsigned int character_id)
{
  // Fail for bad account numbers
  if (!isValidAccountNumber(account_id)) return DBR_DOESNTEXIST;

  // These variables are used during SQL queries
  int rc;
  char* error_message;

  char name_buffer[512];
  if (DBR_SUCCESS != getCharacterName(account_id, character_id, name_buffer, sizeof(name_buffer))) {
    return DBR_INTERNALERROR;
  }

  std::string deleted_name = name_buffer;
  deleted_name.append(" [deleted from ");

  { // append the source account
    char account_number[16];
    sprintf_s(account_number, sizeof(account_number), "%lu", account_id);
    deleted_name.append(account_number);
    deleted_name.append(" ");
  }

  { // Append the current date and time to the name to make it unique
    tm * loc;
    time_t ltime;
    time(&ltime);
	  loc = localtime(&ltime);
    char dateTime[32];
	  asctime_s(dateTime, sizeof(dateTime), loc);
    dateTime[strlen(dateTime)] = '\0';
    deleted_name.append(dateTime);
    deleted_name.append("]");
  }

  // Build the SQL query
  char query[1024];
  sqlite3_snprintf(sizeof(query),
           query,
           "UPDATE characters SET name=%Q, account_id=0 WHERE account_id=%lu AND character_id=%lu",
           deleted_name.c_str(),
           account_id,
           character_id);

  // Execute the query
  rc = sqlite3_executeQuery(query, &error_message);

  // Check to make sure the operation completed successfully
  if (APP_ERROR(rc != SQLITE_OK)("Query (%s) during removeCharacterFromAccount failed:  %s", query, error_message))
    return DBR_INTERNALERROR;

  // Success
  return DBR_SUCCESS;
}




//-----------------------------------------------------------------------------
// Name: GlobalDatabaseManager_getAccountTimeLeft_callback
// Desc: 
//-----------------------------------------------------------------------------
int GlobalDatabaseManager_getAccountTimeLeft_callback(void* timeExpirationDateVoid, int argc, char** argv, char** cols)
{
  __int64 timeExpirationDate;
  sscanf_s(argv[0], "%I64d", &timeExpirationDate);

  // Calculate the difference in times
  *((time_t*)timeExpirationDateVoid) = (time_t)timeExpirationDate;

  // Exit
  return 0;
}




//-----------------------------------------------------------------------------
// Name: getAccountTimeLeft
// Desc: Gets the amount of play time that an account has free
//-----------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::getAccountTimeLeft(unsigned int account_id, float* daysLeft)
{
  // Check parameter
  if (APP_ERROR(!daysLeft)("Invalid daysLeft parameter to getAccountTimeLeft")) return DBR_INTERNALERROR;

  // Fail quickly for bad account numbers
  if (!isValidAccountNumber(account_id)) return DBR_DOESNTEXIST;

  // These variables are used during SQL queries
  int rc;
  char* error_message;

  // Build the SQL query
  char query[128];
  sqlite3_snprintf(sizeof(query), query,
    "SELECT time_expiration_date FROM accounts WHERE account_id=%lu", account_id);

  // The time value stored in the database
  time_t timeExpirationDate = 0;

  // Execute the query
  rc = sqlite3_exec(myDB, query, GlobalDatabaseManager_getAccountTimeLeft_callback, &timeExpirationDate, &error_message);

  // Check to make sure the operation completed successfully
  if (APP_ERROR(rc != SQLITE_OK)("Query (%s) during getAccountTimeLeft failed:  %s", query, error_message))
    return DBR_INTERNALERROR;

  // Check to make sure the time exists
  if (timeExpirationDate == 0)
    return DBR_DOESNTEXIST;

  // Calculate how many days are left
  double doubleDaysLeft = difftime(timeExpirationDate, time(0)) / (60.0 * 60.0 * 24.0);

  // Assign to the lower-capacity output parameter
  if (doubleDaysLeft < -9999.0)    *daysLeft = -9999.0f;
  else if (doubleDaysLeft > 99999.0) *daysLeft = 99999.0f;
  else                *daysLeft = (float)doubleDaysLeft;

  *daysLeft = 14.0f;

  // Success
  return DBR_SUCCESS;

}



//-----------------------------------------------------------------------------
// Name: getCharacterName
// Desc: Gets the name of the character with the given ID
//-----------------------------------------------------------------------------
GlobalDatabaseManager::DBResult
GlobalDatabaseManager::getCharacterName(unsigned int account_id,
                                        unsigned int character_id,
                                        char* nameBuffer,
                                        size_t nameBufferLength)
{
  if (APP_ERROR(!nameBuffer)("Invalid parameter to getCharacterName")) return DBR_INTERNALERROR;

  // Build the SQL query
  char query[128];
  sqlite3_snprintf(sizeof(query), query,
    "SELECT name FROM characters WHERE account_id=%lu AND character_id=%lu", account_id, character_id);

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);

  if (sqlite3_step(stmt) == SQLITE_ROW)
  {
    // Get the value of the character's name from the query
    strncpy_s(nameBuffer,
              nameBufferLength,
              (char*)sqlite3_column_text(stmt, 0),
              nameBufferLength - 1);
    sqlite3_finalize(stmt);
    return DBR_SUCCESS;
  }
  else
  {
    // If we get here, something went wrong with the query
    nameBuffer[0] = 0;
    sqlite3_finalize(stmt);
    return DBR_DOESNTEXIST;
  }
}


//-----------------------------------------------------------------------------
GlobalDatabaseManager::DBResult
GlobalDatabaseManager::getCharacterDesc(
    unsigned int account_id,
    unsigned int character_id,
    char* name_buffer,
    size_t name_buffer_length,
    ActorTypeIndex* actor_type,
    size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT],
    int* map_index,
    float* world_x,
    float* world_y) {

  CONFIRM(name_buffer && name_buffer_length && actor_type && visible_equipment && world_x && world_y)
    else return DBR_DOESNTEXIST;

  // Get world info
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
  "SELECT world_x, world_y, race, gender, name FROM characters WHERE account_id=%lu AND character_id=%lu",
  account_id, character_id);
  sqlite3_stmt* statement;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &statement, 0);
  if (sqlite3_step(statement) != SQLITE_ROW) {
    sqlite3_finalize(statement);
    return DBR_DOESNTEXIST;
  }

  *world_x = (float)sqlite3_column_double(statement, 0);
  *world_y = (float)sqlite3_column_double(statement, 1);
  *actor_type = GlobalGamefileManager::singleton()->getCharacterActorTypeIndex(
      sqlite3_column_int(statement, 2),  // race index
      sqlite3_column_int(statement, 3)); // gender index
  strncpy_s(name_buffer,
            Avatar::MAX_AVATAR_NAME_LENGTH,
            (const char*)sqlite3_column_text(statement, 4),
            Avatar::MAX_AVATAR_NAME_LENGTH - 1);
  *map_index = -1; // no longer used

  sqlite3_finalize(statement);

  // Erase all members
  memset(visible_equipment, 0xFFFFFFFF, sizeof(size_t) * ACTORATTACHMENTPOINT_COUNT);


  return DBR_SUCCESS;
}






//----[  getCharacterData  ]---------------------------------------------------
GlobalDatabaseManager::DBResult
GlobalDatabaseManager::getCharacterData(unsigned int account_id,
                                        unsigned int character_id,
                                        float* x,
                                        float* y,
                                        Server::AvatarInstance* avatar,
                                        int* hp,
                                        int* mp,
                                        bool* is_admin,
                                        World::MapIndex* map_index,
                                        Evidyon::Server::ClientPortalMark* portal_marks,
                                        bool* hide_helmet) {
  static kg::gid::SQLitePreparedStmt select_character_data(
    myDB,
    "SELECT characters.*, guild_members.guild_id "\
    "FROM characters LEFT JOIN guild_members "\
    "ON characters.character_id=guild_members.character_id "\
    "WHERE characters.account_id=? AND characters.character_id=? "\
    "LIMIT 1");
  sqlite3_bind_int(select_character_data.stmt, 1, account_id);
  sqlite3_bind_int(select_character_data.stmt, 2, character_id);

  bool succeeded = false;
  if (SQLITE_ROW == sqlite3_step(select_character_data.stmt)) {
    succeeded = true;
    using namespace DatabaseColumns;
    enum GuildColumn {
      GUILD_ID = NUMBER_OF_CHARACTER_COLUMNS,
    };


    // Fill out information using data from the database
    *x = (float)sqlite3_column_double(select_character_data.stmt, WORLD_X);
    *y = (float)sqlite3_column_double(select_character_data.stmt, WORLD_Y);
    avatar->gender
      = (Evidyon::Avatar::AvatarGender)sqlite3_column_int(select_character_data.stmt, GENDER);
    avatar->race
      = (Evidyon::Avatar::AvatarRace)sqlite3_column_int(select_character_data.stmt, RACE);
    avatar->class_
      = (Evidyon::Avatar::AvatarClass)sqlite3_column_int(select_character_data.stmt, CLASS);
    avatar->level = sqlite3_column_int(select_character_data.stmt, LEVEL);
    *hp = sqlite3_column_int(select_character_data.stmt, HP_CURRENT);
    *mp = sqlite3_column_int(select_character_data.stmt, MP_CURRENT);
    avatar->experience_this_level = (Avatar::AvatarExperience)sqlite3_column_int64(select_character_data.stmt, EXP_CURRENT);
    avatar->attribute_values[Avatar::ATTRIBUTE_STRENGTH]     = sqlite3_column_int(select_character_data.stmt, STR);
    avatar->attribute_values[Avatar::ATTRIBUTE_AGILITY]      = sqlite3_column_int(select_character_data.stmt, AGI);
    avatar->attribute_values[Avatar::ATTRIBUTE_CONSTITUTION] = sqlite3_column_int(select_character_data.stmt, CON);
    avatar->attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE] = sqlite3_column_int(select_character_data.stmt, INT);
    avatar->attribute_values[Avatar::ATTRIBUTE_WISDOM]       = sqlite3_column_int(select_character_data.stmt, WIS);
    strncpy_s(avatar->name,
              Avatar::MAX_AVATAR_NAME_LENGTH,
              (const char*)sqlite3_column_text(select_character_data.stmt, NAME),
              Avatar::MAX_AVATAR_NAME_LENGTH-1);
    avatar->available_attribute_points = sqlite3_column_int(select_character_data.stmt, ABILITY_PTS);
    avatar->money = sqlite3_column_int(select_character_data.stmt, MONEY);
    World::MapHash map_hash = sqlite3_column_int(select_character_data.stmt, MAP_HASH);
    *map_index = GlobalWorldManager::singleton()->getMapIndexFromHash(map_hash);
    *is_admin = 1 == sqlite3_column_int(select_character_data.stmt, IS_ADMIN);
    avatar->kill_count_at_last_pk = sqlite3_column_int(select_character_data.stmt, KILL_COUNT_AT_LAST_PK);
    avatar->last_pk_hour = sqlite3_column_int(select_character_data.stmt, LAST_PK_HOUR);
    avatar->alignment_value = sqlite3_column_int(select_character_data.stmt, ALIGNMENT_VALUE);
    avatar->penalty_experience = (Avatar::AvatarExperience)sqlite3_column_int64(select_character_data.stmt, PENALTY_XP);
    avatar->total_monsters_killed = sqlite3_column_int(select_character_data.stmt, TOTAL_MONSTERS_KILLED);
    *hide_helmet = sqlite3_column_int(select_character_data.stmt, HIDE_HELMET);

    GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
    avatar->experience_to_next_level =
      gamefile_manager->getLevelUpExperience(avatar->level);
    
    avatar->guild = sqlite3_column_int(select_character_data.stmt, GUILD_ID);
    if (avatar->guild == 0) {
      avatar->guild = Server::INVALID_GUILD_ID;
    }
  }

  sqlite3_reset(select_character_data.stmt);

  if (!succeeded) return DBR_DOESNTEXIST;

  char query[1024];
  sqlite3_stmt* stmt;
  sqlite3_snprintf(sizeof(query),
                   query,
                   "SELECT mark_number, map_hash, x, z "\
                   "FROM marks "\
                   "WHERE character_id=%lu",
                   character_id);
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  GlobalWorldManager* world_manager = GlobalWorldManager::singleton();
  for (int i = 0; i < Evidyon::Server::ClientPortalMark::NUMBER_PER_CLIENT; ++i) {
    portal_marks[i].map_index = World::INVALID_MAP_INDEX;
  }
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int mark = sqlite3_column_int(stmt, 0);
    if (mark < 0 ||
        mark >= Evidyon::Server::ClientPortalMark::NUMBER_PER_CLIENT) continue;
    portal_marks[mark].map_index =
      world_manager->getMapIndexFromHash(sqlite3_column_int(stmt, 1));
    portal_marks[mark].x = Number(sqlite3_column_double(stmt, 2));
    portal_marks[mark].z = Number(sqlite3_column_double(stmt, 3));
  }
  sqlite3_finalize(stmt);

  // Success
  return DBR_SUCCESS;

}






//----[  commitCharacterData  ]------------------------------------------------
GlobalDatabaseManager::DBResult
GlobalDatabaseManager::commitCharacterData(unsigned int account_id,
                                           unsigned int character_id,
                                           float x,
                                           float y,
                                           Server::AvatarInstance* avatar,
                                           int hp,
                                           int mp,
                                           bool is_admin,
                                           World::MapIndex map_index,
                                           const Evidyon::Server::ClientPortalMark* portal_marks,
                                           bool hide_helmet)
{
  // Fail for bad account numbers
  if (!isValidAccountNumber(account_id)) return DBR_DOESNTEXIST;

  char* error_message;
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery("BEGIN", &error_message))("db error: %s", error_message);

  static kg::gid::SQLitePreparedStmt update_character_data(
    myDB,
    "REPLACE INTO characters VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

  GlobalWorldManager* world_manager = GlobalWorldManager::singleton();

  using namespace DatabaseColumns;
  sqlite3_bind_int(update_character_data.stmt, 1+ACCOUNT_ID, account_id);
  sqlite3_bind_int(update_character_data.stmt, 1+CHARACTER_ID, character_id);
  sqlite3_bind_text(update_character_data.stmt, 1+NAME, avatar->name, strlen(avatar->name), SQLITE_STATIC);
  sqlite3_bind_double(update_character_data.stmt, 1+WORLD_X, x);
  sqlite3_bind_double(update_character_data.stmt, 1+WORLD_Y, y);
  sqlite3_bind_int(update_character_data.stmt, 1+GENDER, avatar->gender);
  sqlite3_bind_int(update_character_data.stmt, 1+RACE, avatar->race);
  sqlite3_bind_int(update_character_data.stmt, 1+CLASS, avatar->class_);
  sqlite3_bind_int(update_character_data.stmt, 1+LEVEL, avatar->level);
  sqlite3_bind_int(update_character_data.stmt, 1+HP_MAX, avatar->max_hp.wholePart());
  sqlite3_bind_int(update_character_data.stmt, 1+HP_CURRENT, hp);
  sqlite3_bind_int(update_character_data.stmt, 1+MP_MAX, avatar->max_mp.wholePart());
  sqlite3_bind_int(update_character_data.stmt, 1+MP_CURRENT, mp);
  sqlite3_bind_int64(update_character_data.stmt, 1+EXP_CURRENT, avatar->experience_this_level);
  sqlite3_bind_int(update_character_data.stmt, 1+STR, avatar->attribute_values[Avatar::ATTRIBUTE_STRENGTH]);
  sqlite3_bind_int(update_character_data.stmt, 1+AGI, avatar->attribute_values[Avatar::ATTRIBUTE_AGILITY]);
  sqlite3_bind_int(update_character_data.stmt, 1+CON, avatar->attribute_values[Avatar::ATTRIBUTE_CONSTITUTION]);
  sqlite3_bind_int(update_character_data.stmt, 1+INT, avatar->attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE]);
  sqlite3_bind_int(update_character_data.stmt, 1+WIS, avatar->attribute_values[Avatar::ATTRIBUTE_WISDOM]);
  sqlite3_bind_int(update_character_data.stmt, 1+ABILITY_PTS, avatar->available_attribute_points);
  sqlite3_bind_int(update_character_data.stmt, 1+MONEY, avatar->money);
  sqlite3_bind_int(update_character_data.stmt, 1+MAP_HASH, world_manager->getMapHashFromIndex(map_index));
  sqlite3_bind_int(update_character_data.stmt, 1+IS_ADMIN, is_admin ? 1 : 0);
  sqlite3_bind_int(update_character_data.stmt, 1+KILL_COUNT_AT_LAST_PK, avatar->kill_count_at_last_pk);
  sqlite3_bind_int(update_character_data.stmt, 1+LAST_PK_HOUR, avatar->last_pk_hour);
  sqlite3_bind_int(update_character_data.stmt, 1+ALIGNMENT_VALUE, avatar->alignment_value);
  sqlite3_bind_int64(update_character_data.stmt, 1+PENALTY_XP, avatar->penalty_experience);
  sqlite3_bind_int(update_character_data.stmt, 1+TOTAL_MONSTERS_KILLED, avatar->total_monsters_killed);
  sqlite3_bind_int(update_character_data.stmt, 1+HIDE_HELMET, hide_helmet);

  update_character_data.stepReset();


  // Build the SQL query
  char query[2048];
  // Save all of the portal marks
  sqlite3_snprintf(sizeof(query), query,
    "DELETE FROM marks WHERE character_id=%lu", character_id);
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("db error: %s", error_message);

  for (int i = 0; i < Server::ClientPortalMark::NUMBER_PER_CLIENT; ++i) {
    World::MapHash map_hash = world_manager->getMapHashFromIndex(portal_marks[i].map_index);
  sqlite3_snprintf(sizeof(query), query,
    "INSERT INTO marks (character_id, mark_number, map_hash, x, z)"\
    "VALUES (%lu, %i, %lu, %.02f, %.02f)",
    character_id,
    i,
    map_hash,
    portal_marks[i].x.toFloat(),
    portal_marks[i].z.toFloat());
    APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("db error: %s", error_message);
  }


  APP_ERROR(SQLITE_OK != sqlite3_executeQuery("COMMIT", &error_message))("db error: %s", error_message);


  return DBR_SUCCESS;
}



//----[  renameCharacter  ]----------------------------------------------------
GlobalDatabaseManager::DBResult
GlobalDatabaseManager::renameCharacter(const char* old_name, const char* new_name) {
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
    "UPDATE characters SET name=%Q WHERE name=%Q", new_name, old_name);
  char* error_message;
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't change char name:  %s", error_message);
  return DBR_SUCCESS;
}



//----[  findAccountFromName  ]------------------------------------------------
GlobalDatabaseManager::DBResult
GlobalDatabaseManager::findAccountFromName(const char* name, unsigned int* number) {
  sqlite3_stmt* stmt;
  char query[2048];
  sqlite3_snprintf(sizeof(query), query,
    "SELECT account_id FROM characters WHERE name=%Q", name);
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  DBResult dbr = DBR_SUCCESS;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    *number = sqlite3_column_int(stmt, 0);
  } else {
    dbr = DBR_DOESNTEXIST;
  }
  sqlite3_finalize(stmt);
  return dbr;
}





//----[  avatarReassignAllAttributes  ]----------------------------------------
void GlobalDatabaseManager::avatarReassignAllAttributes() {
  ASSERT(SQLITE_OK == sqlite3_executeQuery("BEGIN", NULL));
  char query[2048];
  sqlite3_snprintf(sizeof(query),
                   query,
                   "DELETE FROM avatar_attribute_increments "\
                   "WHERE 1");
  ASSERT(SQLITE_OK == sqlite3_executeQuery(query, NULL));

  sqlite3_snprintf(sizeof(query), query,
    "SELECT character_id, level, str, agi, con, int, wis FROM characters WHERE 1");
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    unsigned int character_id = sqlite3_column_int(stmt, 0);
    unsigned int level = sqlite3_column_int(stmt, 1);
    unsigned int stats[5] = {
      sqlite3_column_int(stmt, 2), // str
      sqlite3_column_int(stmt, 3), // agi
      sqlite3_column_int(stmt, 4), // con
      sqlite3_column_int(stmt, 5), // int
      sqlite3_column_int(stmt, 6), // wis
    };

    // use maxs for humans
    unsigned int available_attribute_points = level-1 + 25;

    sqlite3_snprintf(sizeof(query),
                     query,
                     "UPDATE characters "\
                     "SET ability_pts=%lu, "\
                     "str=%lu, agi=%lu, con=%lu, int=%lu, wis=%lu, hp_max=0, mp_max=0 "\
                     "WHERE character_id=%lu",
                     available_attribute_points,
                     stats[0],
                     stats[1],
                     stats[2],
                     stats[3],
                     stats[4],
                     character_id);
    char * msg;
    ASSERT(SQLITE_OK == sqlite3_executeQuery(query, &msg)) else {
      DEBUG_INFO("query failed: %s", msg);
    }
  }
  sqlite3_finalize(stmt);
  ASSERT(SQLITE_OK == sqlite3_executeQuery("COMMIT", NULL));
}




//----[  avatarResetAllPKs  ]--------------------------------------------------
void GlobalDatabaseManager::avatarResetAllPKs(unsigned int current_hour) {
  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "UPDATE characters "\
                   "SET kill_count_at_last_pk=0, last_pk_hour=%lu WHERE 1",
                   current_hour);
  ASSERT(SQLITE_OK == sqlite3_executeQuery(sql, NULL));
}



//----[  itemChangeOwner  ]----------------------------------------------------
void GlobalDatabaseManager::itemChangeOwner(ItemIDType item_id,
                                            unsigned int account_id,
                                            unsigned int character_id,
                                            unsigned int session_id) {
  char sql[2048];
  sqlite3_snprintf(sizeof(sql), sql,
    "REPLACE INTO active_items (item_id, account_id, character_id, session_id) "\
    "VALUES (%lld, %lu, %lu, %lu)",
    item_id,
    account_id,
    character_id,
    session_id);
  ASSERT(SQLITE_OK == sqlite3_executeQuery(sql, NULL));
}





//----[  itemOnAvatarErased  ]-------------------------------------------------
void GlobalDatabaseManager::itemOnAvatarErased(ItemIDType item_id) {
  char sql[2048];
  sqlite3_snprintf(sizeof(sql), sql,
                   "DELETE FROM active_items WHERE item_id=%lld",
                   item_id);
  ASSERT(SQLITE_OK == sqlite3_executeQuery(sql, NULL));
}


//----[  itemRepackAllIDs  ]---------------------------------------------------
void GlobalDatabaseManager::itemRepackAllIDs() {
  typedef std::set<ItemIDType> ItemIDSet;
  ItemIDSet all_item_ids;

  // Read out all of the old ID numbers
  char sql[2048];
  sqlite3_snprintf(sizeof(sql), sql, "SELECT item_id FROM 'items' WHERE 1");
  sqlite3_stmt* stmt = NULL;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, 0);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    all_item_ids.insert((ItemIDType)sqlite3_column_int64(stmt, 0));
  }
  sqlite3_finalize(stmt);


  // Swap in the new ID numbers
  ItemIDType next_available_item_id = 1; // item 0 is special
  for (ItemIDSet::iterator i = all_item_ids.begin(); i != all_item_ids.end(); ++i) {
    do {  // find an item ID not already in the database
      ++next_available_item_id;
    } while (all_item_ids.find(next_available_item_id) != all_item_ids.end());

    sqlite3_snprintf(sizeof(sql),
                     sql,
                     "UPDATE items SET item_id=%lld WHERE item_id=%lld",
                     next_available_item_id,  // new ID number
                     *i);                     // old ID number
    CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, NULL));

    sqlite3_snprintf(sizeof(sql),
                     sql,
                     "UPDATE storage SET item_id=%lld WHERE item_id=%lld",
                     next_available_item_id,  // new ID number
                     *i);                     // old ID number
    CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, NULL));

    sqlite3_snprintf(sizeof(sql),
                     sql,
                     "UPDATE bazaar_open_listings SET item_id=%lld WHERE item_id=%lld",
                     next_available_item_id,  // new ID number
                     *i);                     // old ID number
    CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, NULL));
  }
}




//----------------------------------------------------------------------------------------------------
//  Inventory
//----------------------------------------------------------------------------------------------------
bool GlobalDatabaseManager::inventoryCheckOut(unsigned int account_id,
                                              unsigned int character_id,
                                              unsigned int session_id,
                                              AvatarInventoryItem* inventory,
                                              unsigned int* items_returned_in_inventory) {

  // Start a transaction
  char* error_message;
  CONFIRM(SQLITE_OK == sqlite3_executeQuery("BEGIN", &error_message)) else return false;

  char sql[2048];

  // Query the item database for this character's inventory
  sqlite3_snprintf(sizeof(sql), sql,
      "SELECT item_id, type_hash, quantity, equipped "\
      "FROM items "\
      "WHERE owner_character_id=%lu "\
      "ORDER BY equipped", character_id);
  sqlite3_stmt* stmt = NULL;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, 0);

  // Read all of the items out of the database
  GlobalItemManager* item_manager = GlobalItemManager::singleton();
  unsigned int inventory_index = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW) {

    DatabaseInventoryItem database_item;
    database_item.id = (ItemIDType)sqlite3_column_int64(stmt, 0); // unique id #

    { // search the active items table
      todo("karl", "templateized query");
      sqlite3_stmt* active_stmt = NULL;
      sqlite3_snprintf(sizeof(sql),
                       sql,
                       "SELECT account_id, character_id, session_id "\
                       "FROM active_items "\
                       "WHERE item_id=%lld",
                       database_item.id);
      sqlite3_prepare_v2(myDB, sql, sizeof(sql), &active_stmt, NULL);
      if (SQLITE_ROW == sqlite3_step(active_stmt)) {
        todo("karl", "check to see if its the same account/character, and give it back");
        DEBUG_ERROR("ItemInstance already checked out!!!!");
        continue;
      }
      sqlite3_finalize(active_stmt);
    }

    // fill in the rest of the item
    database_item.permanent_storage_hash = (unsigned)sqlite3_column_int(stmt, 1);
    database_item.quantity = sqlite3_column_int(stmt, 2);
    database_item.equipped = 0 > sqlite3_column_int(stmt, 3) ? true : false;

    // Translate this item
    if (item_manager->acquireAvatarItemForDatabaseItem(&database_item,
                                                       &inventory[inventory_index])) {
        ++inventory_index;

      if (inventory_index >= AVATAR_INVENTORY_SIZE) break;
    }
  }

  //// remove all active items
  //sqlite3_snprintf(sizeof(sql),
  //                 sql,
  //                 "DELETE FROM active_items "\
  //                 "WHERE account_id=%lu AND character_id=%lu",
  //                 account_id,
  //                 character_id);
  //CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message));

  for (unsigned int i = 0; i < inventory_index; ++i) {
    // "replace" is just for safety
    ItemInstance* item = inventory[i].item.dereference();
    ASSERT(item) else continue;
    sqlite3_snprintf(sizeof(sql),
                     sql,
                     "REPLACE INTO active_items "\
                     "(item_id, account_id, character_id, session_id) " \
                     "VALUES (%lld, %lu, %lu, %lu)",
                     item->getUniqueID(),
                     account_id,
                     character_id,
                     session_id);
    if (SQLITE_OK != sqlite3_executeQuery(sql, &error_message)) { // item is already checked out
      todo("karl", "check to see if its the same account/character, and give it back");
      DEBUG_ERROR("ItemInstance already checked out!!!!");
      continue;
    }
  }

  // Invalidate the rest of the entries in the inventory
  for (unsigned int i = inventory_index; i < AVATAR_INVENTORY_SIZE; ++i) {
    inventory[i].equipped  = false;
    inventory[i].stackable = false;
    inventory[i].item.reset();
  }

  *items_returned_in_inventory = inventory_index;

  // Finish the query and the transaction
  sqlite3_finalize(stmt);
  ASSERT(SQLITE_OK == sqlite3_executeQuery("COMMIT", &error_message)); // shouldn't ever fail

  return true;
}




//----[  inventoryCheckIn  ]---------------------------------------------------
bool GlobalDatabaseManager::inventoryCheckIn(unsigned int account_id,
                                             unsigned int character_id,
                                             unsigned int session_id,
                                             AvatarInventoryItem* inventory,
                                             unsigned int items_in_inventory) {

  // Start a transaction
  char* error_message;
  CONFIRM(SQLITE_OK == sqlite3_executeQuery("BEGIN", &error_message)) else return false;

  char sql[2048];
  sqlite3_stmt* stmt;

  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "DELETE FROM items WHERE owner_character_id=%lu",
                   character_id);
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(sql, &error_message))("Couldn't commit character:  %s", error_message);

  GlobalItemManager* item_manager = GlobalItemManager::singleton();

  for (unsigned int i = 0; i < items_in_inventory; ++i) {
    AvatarInventoryItem* inventory_item = &inventory[i];
    DatabaseInventoryItem database_item;
    {
      bool translated =
        item_manager->translateAvatarItemToDatabaseItem(inventory_item, &database_item);
      item_manager->releaseAvatarInventoryItem(inventory_item); // release the item
      CONFIRM(translated) else continue;
      inventory_item = NULL;
    }

    // make sure it's not in the active table,
    // or is in the table but matches the attributes of the committing class
    sqlite3_snprintf(sizeof(sql),
                     sql,
                     "SELECT account_id, character_id, session_id FROM 'active_items' "\
                     "WHERE item_id=%lld",
                     database_item.id,
                     account_id,
                     character_id,
                     session_id);
    sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
    bool commit_item = true;
    if (sqlite3_step(stmt) != SQLITE_ROW) {
      DEBUG_INFO("inactive item in inventory not committed to DB");
      commit_item = false;
    }

    if (commit_item) { // a row was returned
      unsigned int item_account_id   = unsigned int(sqlite3_column_int(stmt, 0));
      unsigned int item_character_id = unsigned int(sqlite3_column_int(stmt, 1));
      unsigned int item_session_id   = unsigned int(sqlite3_column_int(stmt, 2));
      if (item_account_id   != account_id ||
          item_character_id != character_id ||
          item_session_id   != session_id) {
        commit_item = false;
        DEBUG_ERROR("ItemInstance %lu not checked out correctly, so it's not being checked in");
      }
    }
    sqlite3_finalize(stmt);

    if (commit_item) {
      // save the item and remove from the active table
      sqlite3_snprintf(sizeof(sql),
                       sql,
                       "REPLACE INTO items (item_id, type_hash, quantity, equipped, owner_character_id) "\
                       "VALUES (%lld, %lu, %lu, %i, %lu)",
                       database_item.id,
                       database_item.permanent_storage_hash,
                       database_item.quantity,
                       database_item.equipped ? (i-AVATAR_INVENTORY_SIZE) : i,
                       character_id);
      CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
        DEBUG_INFO("%s: %s", sql, error_message);
      }
      sqlite3_snprintf(sizeof(sql),
                       sql,
                       "DELETE FROM active_items WHERE item_id=%lld", database_item.id);
      CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
        DEBUG_INFO("%s: %s", sql, error_message);
      }
    }
  }

  // Finalize
  ASSERT(SQLITE_OK == sqlite3_executeQuery("COMMIT", &error_message)); // shouldn't ever fail

  return true;
}




//----------------------------------------------------------------------------------------------------
//  Storage
//----------------------------------------------------------------------------------------------------
bool GlobalDatabaseManager::storageCheckOut(unsigned int account_id,
                                            unsigned int character_id,
                                            unsigned int session_id,
                                            ServerStorageItem* storage,
                                            unsigned int* items_returned_in_storage) {

  // Start a transaction
  char* error_message;
  CONFIRM(SQLITE_OK == sqlite3_executeQuery("BEGIN", &error_message)) else return false;

  char sql[2048];

  // Query the item database for this character's inventory
  sqlite3_snprintf(sizeof(sql), sql,
    "SELECT item_id, type_hash, quantity "\
    "FROM storage "\
    "WHERE account_id=%lu", account_id);
  sqlite3_stmt* stmt = NULL;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, 0);

  // Read all of the items out of the database
  GlobalItemManager* item_manager = GlobalItemManager::singleton();
  unsigned int index = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    storage[index].id = (ItemIDType)sqlite3_column_int64(stmt, 0); // unique id #

    { // search the active items table
      todo("karl", "use a templated query");
      sqlite3_stmt* active_stmt = NULL;
      sqlite3_snprintf(sizeof(sql),
                       sql,
                       "SELECT account_id, character_id, session_id "\
                       "FROM active_items "\
                       "WHERE item_id=%lld",
                       storage[index].id);
      sqlite3_prepare_v2(myDB, sql, sizeof(sql), &active_stmt, NULL);
      if (SQLITE_ROW == sqlite3_step(active_stmt)) {
        todo("karl", "check to see if its the same account/character and give it back");
        DEBUG_ERROR("ItemInstance already checked out!!!!");
        continue;
      }
      sqlite3_finalize(active_stmt);
    }

    // fill in the rest of the item
    CONFIRM(item_manager->translatePermanentStorageHashToItemType(sqlite3_column_int(stmt, 1),
                                                                 &storage[index].type)) else continue;
    storage[index].quantity = sqlite3_column_int(stmt, 2);
    ++index;
    if (index >= MAX_ITEMS_IN_STORAGE) break;
  }

  for (unsigned int i = 0; i < index; ++i) {
    // "replace" is just for safety
    sqlite3_snprintf(sizeof(sql),
                     sql,
                     "REPLACE INTO active_items "\
                     "(item_id, account_id, character_id, session_id) " \
                     "VALUES (%lld, %lu, %lu, %lu)",
                     storage[i].id,
                     account_id,
                     character_id,
                     session_id);
    if (SQLITE_OK != sqlite3_executeQuery(sql, &error_message)) { // item is already checked out
      todo("karl", "check to see if its the same account/character, and give it back");
      DEBUG_ERROR("ItemInstance already checked out!!!!");
      continue;
    }
  }

  // Invalidate the rest of the entries in the storage
  for (unsigned int i = index; i < MAX_ITEMS_IN_STORAGE; ++i) {
    storage[i].quantity = -1;
    storage[i].type = ~(unsigned int(0));
  }

  *items_returned_in_storage = index;

  // Finish the query and the transaction
  sqlite3_finalize(stmt);
  ASSERT(SQLITE_OK == sqlite3_executeQuery("COMMIT", &error_message)); // shouldn't ever fail

  return true;
}

bool GlobalDatabaseManager::storageCheckIn(unsigned int account_id,
                                           unsigned int character_id,
                                           unsigned int session_id,
                                           ServerStorageItem* storage,
                                           unsigned int items_in_storage) {

  // Start a transaction
  char* error_message;
  CONFIRM(SQLITE_OK == sqlite3_executeQuery("BEGIN", &error_message)) else return false;

  char sql[2048];
  sqlite3_stmt* stmt;

  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "DELETE FROM storage WHERE account_id=%lu",
                   account_id);
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(sql, &error_message))("Couldn't commit character:  %s", error_message);

  GlobalItemManager* item_manager = GlobalItemManager::singleton();

  for (unsigned int i = 0; i < items_in_storage; ++i) {
    ServerStorageItem* storage_item = &storage[i];
    ItemIDType item_id = storage_item->id;

    // make sure it's not in the active table,
    // or is in the table but matches the attributes of the committing class
    sqlite3_snprintf(sizeof(sql),
                     sql,
                     "SELECT account_id, character_id, session_id FROM 'active_items' "\
                     "WHERE item_id=%lld",
                     item_id,
                     account_id,
                     character_id,
                     session_id);
    sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
    bool commit_item = true;
    if (sqlite3_step(stmt) != SQLITE_ROW) {
      // this item isn't active
      DEBUG_INFO("Inactive item not committed to DB");
      commit_item = false;
    }

    if (commit_item) {
      unsigned int item_account_id   = unsigned int(sqlite3_column_int(stmt, 0));
      unsigned int item_character_id = unsigned int(sqlite3_column_int(stmt, 1));
      unsigned int item_session_id   = unsigned int(sqlite3_column_int(stmt, 2));
      if (item_account_id   != account_id ||
          item_character_id != character_id ||
          item_session_id   != session_id) {
        commit_item = false;
        DEBUG_ERROR("ItemInstance %lu not checked out correctly, so it's not being checked in");
      }
    }

    sqlite3_finalize(stmt);

    if (commit_item) {
      unsigned int hash;
      if (!item_manager->translateItemTypeToPermanentStorageHash(storage_item->type, &hash)) continue;

      // save the item and remove from the active table
      sqlite3_snprintf(sizeof(sql),
                       sql,
                       "REPLACE INTO storage (item_id, type_hash, quantity, account_id) "\
                       "VALUES (%lld, %lu, %i, %lu)",
                       item_id,
                       hash,
                       storage_item->quantity,
                       account_id);
      CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
        DEBUG_INFO("%s: %s", sql, error_message);
      }
      sqlite3_snprintf(sizeof(sql),
                       sql,
                       "DELETE FROM active_items WHERE item_id=%lld", item_id);
      CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
        DEBUG_INFO("%s: %s", sql, error_message);
      }
    }
  }

  // Finalize
  ASSERT(SQLITE_OK == sqlite3_executeQuery("COMMIT", &error_message)); // shouldn't ever fail

  return true;
}










void GlobalDatabaseManager::bazaar_rotateListings() {
  bazaar_.rotateListings();
}


bool GlobalDatabaseManager::bazaar_addListing(
    BazaarListingID listing_id,
    unsigned int category,
    CurrencyType price,
    ItemIDType item_id,
    unsigned int owner_account_id,
    unsigned int permanent_storage_hash,
    unsigned int quantity) {

  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "INSERT INTO bazaar_open_listings "\
                   "(listing_id, category, price, item_id, selling_account_id, type_hash, quantity) "\
                   "VALUES (%lld, %lu, %lu, %lld, %lu, %lu, %lu)",
                   listing_id,
                   category,
                   price,
                   item_id,
                   owner_account_id,
                   permanent_storage_hash,
                   quantity);

  // insert into the listings table
  char* error_message;
  CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
    DEBUG_INFO("%s: %s", sql, error_message);
    return false;
  }

  // Remove from the active items table
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "DELETE FROM active_items WHERE item_id=%lld", item_id);
  CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
    DEBUG_INFO("%s: %s", sql, error_message);
  }

  return true;
}




bool GlobalDatabaseManager::bazaar_getListing(
    BazaarListingID listing_id,
    unsigned int* seller_account_id,
    unsigned int* category,
    ItemIDType* item_id,
    ItemInBazaar* listing) {
  
  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "SELECT selling_account_id, category, item_id, listing_id, price, type_hash, quantity "\
                   "FROM bazaar_open_listings WHERE listing_id=%lld",
                   listing_id);

  bool return_value = false;

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
  if (sqlite3_step(stmt) == SQLITE_ROW) {

    // Get this item's type
    unsigned int hash = unsigned int(sqlite3_column_int(stmt, 5));
    unsigned int translated_type;
    if (GlobalItemManager::singleton()->translatePermanentStorageHashToItemType(hash, &translated_type)) {
      *seller_account_id  =    unsigned int(sqlite3_column_int(stmt, 0));
      *category           =    unsigned int(sqlite3_column_int(stmt, 1));
      *item_id            =      ItemIDType(sqlite3_column_int(stmt, 2));
      listing->listing_id = BazaarListingID(sqlite3_column_int64(stmt, 3));
      listing->price      =    unsigned int(sqlite3_column_int(stmt, 4));
      listing->type       =    unsigned short(translated_type);
      listing->quantity   =    unsigned short(sqlite3_column_int(stmt, 6));
      return_value = true;
    }
  }

  sqlite3_finalize(stmt);

  return return_value;
}




void GlobalDatabaseManager::bazaar_getCategoryListings(
    unsigned int category,
    unsigned int lower,
    unsigned int upper,
    ItemInBazaar cache[MAX_ITEMS_PER_BAZAAR_CATEGORY],
    size_t* number_of_listings) {

  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "SELECT listing_id, price, type_hash, quantity "\
                   "FROM bazaar_open_listings "\
                   "WHERE category=%lu ORDER BY -listing_id LIMIT %lu OFFSET %lu",
                   category,
                   upper - lower, lower);

  size_t returned_listings = 0;

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    ItemInBazaar* listing = &cache[returned_listings];

    // Get this item's type
    unsigned int translated_type;
    {
      unsigned int hash = unsigned int(sqlite3_column_int(stmt, 2));
      if (!GlobalItemManager::singleton()->
            translatePermanentStorageHashToItemType(hash, &translated_type)) continue;
    }

    ++returned_listings;

    listing->listing_id = BazaarListingID(sqlite3_column_int64(stmt, 0));
    listing->price      =    unsigned int(sqlite3_column_int(stmt, 1));
    listing->type       =    unsigned short(translated_type);
    listing->quantity   =    unsigned int(sqlite3_column_int(stmt, 3));
    listing->sold       = 0; // any open listing has, by definition, not been sold
  }

  sqlite3_finalize(stmt);

  *number_of_listings = returned_listings;
}



unsigned int GlobalDatabaseManager::bazaar_getNumberOfListedItems(unsigned int seller_account_id) {

  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "SELECT COUNT(listing_id)"\
                   "FROM bazaar_open_listings WHERE selling_account_id=%lu",
                   seller_account_id);

  unsigned int count = 0;

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    count = sqlite3_column_int(stmt, 0);
  }

  sqlite3_finalize(stmt);

  return count;
}


unsigned int GlobalDatabaseManager::bazaar_getNumberOfItemsInCategory(unsigned int category) {

  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "SELECT COUNT(listing_id)"\
                   "FROM bazaar_open_listings WHERE category=%lu",
                   category);

  unsigned int count = 0;

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    count = sqlite3_column_int(stmt, 0);
  }

  sqlite3_finalize(stmt);

  return count;
}


void GlobalDatabaseManager::bazaar_removeListing(BazaarListingID listing_id) {
  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "DELETE FROM bazaar_open_listings "\
                   "WHERE listing_id=%lld",
                   listing_id);

  // remove from the listings table
  char* error_message;
  CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
    DEBUG_INFO("%s: %s", sql, error_message);
  }
}


void GlobalDatabaseManager::bazaar_insertSale(
    unsigned int seller_account_id,
    const ItemInBazaar* sold_listing) {

  // translate the listing's hash
  unsigned int hash;
  CONFIRM(GlobalItemManager::singleton()->translateItemTypeToPermanentStorageHash(sold_listing->type, &hash)) else return;

  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "INSERT INTO bazaar_sold_listings "\
                   "(listing_id, profit, selling_account_id, type_hash, quantity) "\
                   "VALUES (%lld, %lu, %lu, %lu, %lu)",
                   sold_listing->listing_id,
                   sold_listing->price,
                   seller_account_id,
                   hash,
                   sold_listing->quantity);

  // add to the sales table
  char* error_message;
  CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
    DEBUG_INFO("%s: %s", sql, error_message);
  }

  // for safety, we could: DELETE FROM bazaar_open_listings WHERE listing_id=%lld
}


bool GlobalDatabaseManager::bazaar_removeSale(
    BazaarListingID listing_id,
    unsigned int seller_account_id,
    CurrencyType* profit) {

  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "SELECT profit "\
                   "FROM bazaar_sold_listings "\
                   "WHERE listing_id=%llu AND selling_account_id=%lu",
                   listing_id,
                   seller_account_id);

  bool return_value = false;

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    *profit = sqlite3_column_int(stmt, 0);
    return_value = true;
  }

  sqlite3_finalize(stmt);

  // if profit was obtained from the listing, remove the listing from the database
  if (return_value) {
    sqlite3_snprintf(sizeof(sql),
                     sql,
                     "DELETE "\
                     "FROM bazaar_sold_listings "\
                     "WHERE listing_id=%llu",
                     listing_id);

    // add to the sales table
    char* error_message;
    CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, &error_message)) else {
      DEBUG_INFO("%s: %s", sql, error_message);
    }

  }

  return return_value;
}

//----[ bazaar_getOwnListings ]------------------------------------------------
size_t GlobalDatabaseManager::bazaar_getOwnListings(
    unsigned int seller_account_id,
    ItemInBazaar* buffer,
    size_t buffer_size) {

  char sql[2048];

  size_t returned_listings = 0;
  sqlite3_stmt* stmt;


  for (int i = 0; i < 2; ++i) {

    if (i == 0) {
      // first, query sales
      sqlite3_snprintf(sizeof(sql),
                       sql,
                       "SELECT listing_id, profit, type_hash, quantity "\
                       "FROM bazaar_sold_listings "\
                       "WHERE selling_account_id=%lu ORDER BY -listing_id",
                       seller_account_id);
    } else {
      // second, query open listings
      sqlite3_snprintf(sizeof(sql),
                       sql,
                       "SELECT listing_id, price, type_hash, quantity "\
                       "FROM bazaar_open_listings "\
                       "WHERE selling_account_id=%lu ORDER BY -listing_id",
                       seller_account_id);
    }

    sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
    while ((returned_listings < buffer_size) &&
           sqlite3_step(stmt) == SQLITE_ROW) {
      ItemInBazaar* listing = &buffer[returned_listings];

      // Get this item's type
      unsigned int translated_type;
      {
        unsigned int hash = unsigned int(sqlite3_column_int(stmt, 2));
        if (!GlobalItemManager::singleton()->
              translatePermanentStorageHashToItemType(hash, &translated_type)) continue;
      }

      ++returned_listings;

      listing->listing_id = BazaarListingID(sqlite3_column_int64(stmt, 0));
      listing->price      =    unsigned int(sqlite3_column_int(stmt, 1));
      listing->type       =    unsigned short(translated_type);
      listing->quantity   =    unsigned short(sqlite3_column_int(stmt, 3));
      listing->sold       =    i == 0 ? 1 : 0;
    }

    sqlite3_finalize(stmt);
  }

  return returned_listings;
}








//----[  commitCharacterMoney  ]-----------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::commitCharacterMoney(unsigned int account_id, unsigned int character_id, unsigned int money) {
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
    "UPDATE characters SET money=%i WHERE account_id=%lu AND character_id=%lu",
    money, account_id, character_id);
  char* error_message;
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't commit character:  %s", error_message);
  return DBR_SUCCESS;
}





//----[  questQueryTerminationTimes  ]-----------------------------------------
bool GlobalDatabaseManager::questQueryTerminationTimes(
    unsigned int character_id,
    unsigned int quest_hash,
    int* minute_terminated_success,
    int* minute_terminated_failure) {
  char sql[1024];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "SELECT minute_terminated_success, minute_terminated_failure "\
                   "FROM quests "\
                   "WHERE (character_id=%lu AND quest_hash=%lu)",
                   character_id,
                   quest_hash);

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
  bool return_value = false;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    return_value = true;
    *minute_terminated_success = sqlite3_column_int(stmt, 0);
    *minute_terminated_failure = sqlite3_column_int(stmt, 1);
  }
  sqlite3_finalize(stmt);
  return return_value;
}




//----[  questWrite  ]---------------------------------------------------------
bool GlobalDatabaseManager::questWrite(
    unsigned int character_id,
    const Quests::Server::QuestParameters* parameters) {
  char sql[1024];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "DELETE FROM quests "\
                   "WHERE (character_id=%lu AND quest_hash=%lu)",
                   character_id,
                   parameters->quest->quest_permanent_hash);
  CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, NULL));
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "INSERT INTO quests "\
                   "(character_id,quest_hash,competition_id,"\
                    "minute_started,minute_terminated_success,"\
                    "minute_terminated_failure,item_id_at_start,"\
                    "qualified_kills,specific_kills,"\
                    "pk_count,death_count,has_changed_map) "\
                   "VALUES "\
                   "(%lu,%lu,%lu,%lu,%lu,%lu,%lld,%lu,%lu,%lu,%lu,%lu)",
                   character_id,
                   parameters->quest->quest_permanent_hash,
                   parameters->competition_id,
                   parameters->minute_started,
                   parameters->minute_terminated_success,
                   parameters->minute_terminated_failure,
                   parameters->item_id_when_started,
                   parameters->qualified_kills,
                   parameters->specific_kills,
                   parameters->pk_count,
                   parameters->death_count,
                   parameters->has_changed_map ? 1 : 0);
  CONFIRM(SQLITE_OK == sqlite3_executeQuery(sql, NULL));
  return true;
}




//----[  questReadActive  ]----------------------------------------------------
int GlobalDatabaseManager::questReadActive(
    unsigned int character_id,
    Quests::Server::QuestParameters* quests_array,
    size_t quests_array_size) {
  using namespace Quests::Server;
  char sql[1024];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "SELECT quest_hash,competition_id,"\
                    "minute_started,minute_terminated_success,"\
                    "minute_terminated_failure,item_id_at_start,"\
                    "qualified_kills,specific_kills,"\
                    "pk_count,death_count,has_changed_map "\
                   "FROM quests "\
                   "WHERE (character_id=%lu "\
                   "AND minute_started > minute_terminated_failure "\
                   "AND minute_started > minute_terminated_success)",
                   character_id);

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, sql, sizeof(sql), &stmt, NULL);
  int return_value = 0;
  GlobalQuestManager* quest_manager = GlobalQuestManager::singleton();
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const Quest* quest =
      quest_manager->translateToQuest(sqlite3_column_int(stmt, 0));
    if (!quest) {
      todo("karl", "erase quest from db");
      continue;
    }
    QuestParameters* parameters = &quests_array[return_value++];
    parameters->quest = quest;
    parameters->competition_id = sqlite3_column_int(stmt, 1);
    parameters->minute_started = sqlite3_column_int(stmt, 2);
    parameters->minute_terminated_success = sqlite3_column_int(stmt, 3);
    parameters->minute_terminated_failure = sqlite3_column_int(stmt, 4);
    parameters->item_id_when_started = sqlite3_column_int(stmt, 5);
    parameters->qualified_kills = sqlite3_column_int(stmt, 6);
    parameters->specific_kills  = sqlite3_column_int(stmt, 7);
    parameters->pk_count        = sqlite3_column_int(stmt, 8);
    parameters->death_count     = sqlite3_column_int(stmt, 9);
    parameters->has_changed_map = sqlite3_column_int(stmt, 10) == 1;
  }
  sqlite3_finalize(stmt);

  return return_value;
}




//----[  getHighestGuildID  ]--------------------------------------------------
Server::GuildID GlobalDatabaseManager::getHighestGuildID() {
  sqlite3_stmt* stmt;
  const char query[] = "SELECT MAX(guild_id) FROM guilds";
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  int rc = sqlite3_step(stmt);

  // Make sure this succeeded
  Server::GuildID guild_id
    = (rc == SQLITE_ROW) ? sqlite3_column_int(stmt,0) : 0;

  DEBUG_INFO("%lu guilds have been created on the server", guild_id);
  sqlite3_finalize(stmt);
  return guild_id;
}



//----[  guildRetrieve  ]------------------------------------------------------
bool GlobalDatabaseManager::guildRetrieve(Server::GuildID id,
                                          char* guild_name,
                                          size_t name_length,
                                          Server::AvatarID* founder_id,
                                          Map** home_map,
                                          Number* home_x,
                                          Number* home_z,
                                          Server::GuildInstance* instance) {
  static kg::gid::SQLitePreparedStmt select_guild_data(
    myDB,
    "SELECT * FROM guilds WHERE guild_id=?");
  sqlite3_bind_int(select_guild_data.stmt, 1, id);

  if (sqlite3_step(select_guild_data.stmt) == SQLITE_ROW) {
    strncpy_s(guild_name, name_length, (const char*)sqlite3_column_text(select_guild_data.stmt, 1), name_length-1);
    *founder_id = (unsigned int)sqlite3_column_int(select_guild_data.stmt, 2);
    *home_map = GlobalWorldManager::singleton()->getMapFromHash(
      (World::MapHash)sqlite3_column_int(select_guild_data.stmt, 3));
    *home_x = sqlite3_column_double(select_guild_data.stmt, 4);
    *home_z = sqlite3_column_double(select_guild_data.stmt, 5);
  } else {
    sqlite3_reset(select_guild_data.stmt);
    return false;
  }
  sqlite3_reset(select_guild_data.stmt);

  static kg::gid::SQLitePreparedStmt select_guild_members(
    myDB,
    "SELECT guild_members.character_id, guild_members.leader, characters.name "\
    "FROM (SELECT * FROM characters WHERE account_id<>0) as characters "\
    "LEFT JOIN guild_members "\
    "ON guild_members.character_id=characters.character_id "\
    "WHERE guild_id=?");
  sqlite3_bind_int(select_guild_members.stmt, 1, id);

  while(sqlite3_step(select_guild_members.stmt) == SQLITE_ROW) {
    instance->addRetrievedMember(
      sqlite3_column_int(select_guild_members.stmt, 0),
      (const char*)sqlite3_column_text(select_guild_members.stmt, 2),
      sqlite3_column_int(select_guild_members.stmt,1) == 1);
  }
  sqlite3_reset(select_guild_members.stmt);

  return true;
}


//----[  guildErase  ]---------------------------------------------------------
void GlobalDatabaseManager::guildErase(Server::GuildID guild_id) {
  guildSet(guild_id, NULL, Server::INVALID_AVATAR_ID, NULL, -1, -1);
}


//----[  guildSet  ]-----------------------------------------------------------
void GlobalDatabaseManager::guildSet(Server::GuildID guild_id,
                                     const char* guild_name,
                                     Server::AvatarID founder_id,
                                     Map* home_map,
                                     Number home_x,
                                     Number home_z) {
  static kg::gid::SQLitePreparedStmt delete_guild(
    myDB,
    "DELETE FROM guilds WHERE guild_id=?");
  sqlite3_bind_int(delete_guild.stmt, 1, guild_id);
  delete_guild.stepReset();

  static kg::gid::SQLitePreparedStmt delete_guild_members(
    myDB,
    "DELETE FROM guild_members WHERE guild_id=?");
  sqlite3_bind_int(delete_guild_members.stmt, 1, guild_id);
  delete_guild_members.stepReset();

  if (guild_name != NULL && founder_id != Server::INVALID_AVATAR_ID) {
    static kg::gid::SQLitePreparedStmt insert_guild(
      myDB,
      "INSERT INTO guilds VALUES (?,?,?,?,?,?)");
    sqlite3_bind_int(insert_guild.stmt, 1, guild_id);
    sqlite3_bind_text(insert_guild.stmt, 2, guild_name, strlen(guild_name), SQLITE_STATIC);
    sqlite3_bind_int(insert_guild.stmt, 3, founder_id);
    sqlite3_bind_int(insert_guild.stmt, 4, home_map ? home_map->getHash() : World::INVALID_MAP_HASH);
    sqlite3_bind_double(insert_guild.stmt, 5, home_x.toFloat());
    sqlite3_bind_double(insert_guild.stmt, 6, home_z.toFloat());
    insert_guild.stepReset();
  }
}




//----[  guildMemberSet  ]-----------------------------------------------------
void GlobalDatabaseManager::guildMemberSet(Server::GuildID guild_id,
                                           Server::AvatarID member_id,
                                           bool leader) {
  static kg::gid::SQLitePreparedStmt insert_guild_member(
    myDB,
    "REPLACE INTO guild_members VALUES (?,?,?)");
  sqlite3_bind_int(insert_guild_member.stmt, 1, guild_id);
  sqlite3_bind_int(insert_guild_member.stmt, 2, member_id);
  sqlite3_bind_int(insert_guild_member.stmt, 3, leader ? 1 : 0);
  insert_guild_member.stepReset();
}




//----[  guildGetName  ]-------------------------------------------------------
void GlobalDatabaseManager::guildGetName(Server::GuildID guild_id,
                                         char* guild_name,
                                         size_t max_length) {
  static kg::gid::SQLitePreparedStmt select_guild_name(
    myDB,
    "SELECT name FROM guilds WHERE guild_id=?");
  sqlite3_bind_int(select_guild_name.stmt, 1, guild_id);
  int rc = sqlite3_step(select_guild_name.stmt);
  if (SQLITE_ROW == rc || SQLITE_OK == rc) {
    const char* data
      = (const char*)sqlite3_column_text(select_guild_name.stmt, 0);
    strncpy_s(guild_name, max_length, data, max_length-1);
  } else {
    *guild_name = '\0';
  }
  sqlite3_reset(select_guild_name.stmt);  
}




//----[  getGeosidData  ]------------------------------------------------------
void GlobalDatabaseManager::getGeosidData(Geosid::GeosidHashID geosid,
                                          Server::GuildID* owner_guild,
                                          Geosid::Geonite* geonite) {
  static kg::gid::SQLitePreparedStmt select_geosid_owner(
    myDB,
    "SELECT owner_guild_id, geonite FROM geosids WHERE geosid_hash=?");
  sqlite3_bind_int(select_geosid_owner.stmt, 1, geosid);
  Server::GuildID owner = Server::INVALID_GUILD_ID;
  Geosid::Geonite geonite_left = -1;
  if (SQLITE_ROW == sqlite3_step(select_geosid_owner.stmt)) {
    owner = (Server::GuildID)sqlite3_column_int(select_geosid_owner.stmt, 0);
    geonite_left = (Server::GuildID)sqlite3_column_int(select_geosid_owner.stmt, 1);
  }
  sqlite3_reset(select_geosid_owner.stmt);
  *owner_guild = owner;
  *geonite = geonite_left;
}




//----[  geosidSet  ]----------------------------------------------------------
void GlobalDatabaseManager::geosidSet(Geosid::GeosidHashID id,
                                      Server::GuildID owner,
                                      Geosid::Geonite geonite) {
  static kg::gid::SQLitePreparedStmt insert_geosid(
    myDB,
    "REPLACE INTO geosids VALUES (?,?,?)");
  sqlite3_bind_int(insert_geosid.stmt, 1, id);
  sqlite3_bind_int(insert_geosid.stmt, 2, owner);
  sqlite3_bind_int(insert_geosid.stmt, 3, geonite);
  insert_geosid.stepReset();
}





//----[  sqlite3_executeQuery  ]-----------------------------------------------
int GlobalDatabaseManager::sqlite3_executeQuery(const char* query, char** error_message) {
  int rc = sqlite3_exec(myDB, query, 0, 0, NULL);
  if (error_message) *error_message = (char*)sqlite3_errmsg(myDB);// "kwg-disabled";
  return rc == SQLITE_DONE ? SQLITE_OK : rc; // merge done -> OK
}








//----[  getUniqueAvatarNameChecker  ]-----------------------------------------
UniqueAvatarNameChecker* GlobalDatabaseManager::getUniqueAvatarNameChecker() {
  return &unique_avatar_name_checker_;
}



//----[  adminQueryOpen  ]-----------------------------------------------------
bool GlobalDatabaseManager::adminQueryOpen(const std::string& query,
                                           sqlite3_stmt** stmt_out) {
  sqlite3_stmt* stmt;
  if (SQLITE_OK == sqlite3_prepare_v2(myDB,
                                      query.c_str(),
                                      query.length() + 1,
                                      &stmt,
                                      NULL)) {
    *stmt_out = stmt;
    return true;
  }
  return false;
}



//----[  adminQueryGetError  ]-------------------------------------------------
void GlobalDatabaseManager::adminQueryGetError(std::string* error) {
  error->assign(sqlite3_errmsg(myDB));
}



//----[  adminQueryStep  ]-----------------------------------------------------
bool GlobalDatabaseManager::adminQueryStep(sqlite3_stmt* stmt, std::string* query_result) {
  query_result->clear();
  if (SQLITE_ROW == sqlite3_step(stmt)) {
    int cols = sqlite3_column_count(stmt);
    for (int i = 0; i < cols; ++i) {
      if (i > 0) query_result->append(",");
      query_result->append((const char*)sqlite3_column_text(stmt, i));
    }
    return true;
  }

  return false;
}



//----[  adminQueryClose  ]----------------------------------------------------
void GlobalDatabaseManager::adminQueryClose(sqlite3_stmt** stmt) {
  if (!stmt) return;
  sqlite3_stmt* internal_stmt = *stmt;
  if (!internal_stmt) return;
  sqlite3_finalize(internal_stmt);
  *stmt = NULL;
}



//----[  adminQueryCharacterStatistics  ]--------------------------------------
void GlobalDatabaseManager::adminQueryCharacterStatistics(int class_type, int level, std::string* query_result) {
  sqlite3_stmt* stmt;
  char sql[2048];
  sqlite3_snprintf(sizeof(sql),
                   sql,
                   "SELECT hp_max, mp_max " \
                   "FROM 'characters' "\
                   "WHERE level=%lu AND class=%lu",
                   level,
                   class_type);
  sqlite3_prepare_v2(myDB,
                     sql,
                     sizeof(sql),
                    &stmt,
                     NULL);
  __int64 total_hp = 0, total_mp = 0;
  unsigned int rows = 0;
  while (SQLITE_ROW == sqlite3_step(stmt)) {
    unsigned int hp = sqlite3_column_int(stmt, 0);
    unsigned int mp = sqlite3_column_int(stmt, 1);
    if (hp == 0 || mp == 0) continue;
    ++rows;
    total_hp += hp;
    total_mp += mp;
  }
  sqlite3_finalize(stmt);
  if (rows == 0) {
    query_result->assign("(no data)");
  } else {
    sprintf_s(sql, sizeof(sql), "%6i HP, %6i MP", total_hp / rows, total_mp / rows);
    query_result->assign(sql);
  }
}




//----[  startBackup  ]--------------------------------------------------------
void GlobalDatabaseManager::startBackup() {
  if (backupInProgress()) return;
  GlobalClientManager::singleton()->commitAllToDatabase(this);
  confirm(Server::GlobalGuildManager::singleton()->store());
  confirm(Server::GlobalGeosidManager::singleton()->store());
  backup_thread_ = CreateThread(NULL,
                                0,
                                GlobalDatabaseManager_RunBackup,
                                this,
                                THREAD_PRIORITY_NORMAL,
                                NULL);
}




//----[  backupInProgress  ]---------------------------------------------------
bool GlobalDatabaseManager::backupInProgress() {
  // should be the only check we need
  if (backup_thread_ == NULL) return false;

  // this does a safety check--did the thread crash without resetting the member?
  if (THREAD_PRIORITY_ERROR_RETURN == GetThreadPriority(backup_thread_)) {
    backup_thread_ = NULL;
    return false;
  }

  // thread is valid and active
  return true;
}





//----[  executeBackup  ]------------------------------------------------------
void GlobalDatabaseManager::executeBackup() {
  CONFIRM(myDB) else {
    printf("executeBackup() - no database");
    return;
  }

  char filename[MAX_PATH];
#if 0
  { // make a backup folder:  /backups/date/hour
    // and create a filename that fits in there
    SYSTEMTIME st;
    GetSystemTime(&st);
    char folder[MAX_PATH];
    char temp[MAX_PATH];
    folder[0] = '\0';
    for (int i = 0; i < 3; ++i) {
      char* subdirectory = temp;
      switch (i) {
      case 0: subdirectory = "backups"; break;
      case 1: GetDateFormatA(LOCALE_USER_DEFAULT,
                             0,
                             &st,
                             "yyyy-MM-dd",
                             temp,
                             MAX_PATH);
              strcpy_s(filename, MAX_PATH, temp);
              break;
      case 2: sprintf_s(temp,
                        MAX_PATH,
                        "%lu",
                        st.wHour); break;
      }

      // Create the folder
      strcat_s(folder, MAX_PATH, subdirectory);
      _mkdir(folder);
      strcat_s(folder, "/");
    }

    // build the file name (without path)
    sprintf_s(temp,
              MAX_PATH,
              "%s at %lu'%lu'%lu.sqldb",
              filename,
              st.wHour,
              st.wMinute,
              st.wSecond);

    // construct the full file name (with path)
    strcpy_s(filename, MAX_PATH, folder);
    strcat_s(filename, MAX_PATH, temp);

    // delete the file if it already exists
    if (GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES) {
      BOOL deleted = DeleteFile(filename);
    }
  }
#endif
  
  { // make a backup folder:  "backups/"
    // and create a filename that fits in there like "backups/2009-12-04.sqldb"
    SYSTEMTIME st;
    GetSystemTime(&st);
    _mkdir("backups");

    char temp[MAX_PATH];
    GetDateFormatA(LOCALE_USER_DEFAULT,
                   0,
                   &st,
                   "yyyy-MM-dd",
                   temp,
                   MAX_PATH);
    sprintf_s(filename, MAX_PATH, "backups/%s.sqldb", temp);

    // delete the file if it already exists
    if (GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES) {
      DeleteFile(filename);
    }
  }

  // Open the database file
  sqlite3* disk_db;
  int rc = sqlite3_open(filename, &disk_db);
  if (rc == SQLITE_OK) {
    sqlite3_backup* backup = sqlite3_backup_init(disk_db, "main", myDB, "main");
    if (backup) {
      sqlite3_backup_step(backup, -1);  // -1 => # of pages = everything
      sqlite3_backup_finish(backup);
    }

    // get the error code (if there was one)
    rc = sqlite3_errcode(disk_db);
  }

  // free the disk DB connection
  sqlite3_close(disk_db);

  if (rc == SQLITE_OK || rc == SQLITE_DONE) {
    // the backup succeeded, so copy the backup file to the one
    // that will be loaded next time the server restarts
    CopyFile(filename, DATABASE_FILE_NAME, FALSE);
    printf("\ncopying '%s' to '%s'...", filename, DATABASE_FILE_NAME);
  } else {
    printf("\nBackup failed!  sqlite3 error code = %i", rc);
  }

  printf("\nbackup to %s complete!\n\n", filename);

  // remove reference to this database
  backup_thread_ = NULL;
}



}