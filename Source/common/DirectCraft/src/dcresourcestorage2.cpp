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
#include "../dc/dcresourcestorage2.h"
#include "common/sqlite/sqlite3.h"
#include <queue>
#include <algorithm>

namespace dc {



  
//----[  dcResourceStorage2  ]-----------------------------------------------
dcResourceStorage2::dcResourceStorage2() {
  storage_ = NULL;
}


//----[  open  ]---------------------------------------------------------------
bool dcResourceStorage2::openFile(const char* file_name,
                                  const char* segment) {
  sqlite3_open(file_name, &storage_);
  if (!storage_) return false;
  segment_ = segment;
  char sql[2048];

  // create the data structure
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE TABLE IF NOT EXISTS %s ("\
                   "record_id INTEGER PRIMARY KEY,"\
                   "owner_path VARCHAR,"\
                   "owner_record_id INTEGER,"\
                   "resource_type VARCHAR,"\
                   "member_name VARCHAR,"\
                   "value VARCHAR,"
                   "index_hint INTEGER"\
                   ")",
                   segment);
  char* errmsg;
  if (SQLITE_OK != sqlite3_exec(storage_, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    close();
    return false;
  }
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE INDEX IF NOT EXISTS "\
                   "index_%s_owner_record_id "\
                   "ON %s (owner_record_id)",
                   segment,
                   segment);
  if (SQLITE_OK != sqlite3_exec(storage_, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    close();
    return false;
  }
  sqlite3_exec(storage_, "PRAGMA case_sensitive_like=ON;", NULL, NULL, NULL);
  sqlite3_exec(storage_, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
  sqlite3_exec(storage_, "PRAGMA count_changes=OFF;", NULL, NULL, NULL);


  return true;
}


//----[  close  ]--------------------------------------------------------------
void dcResourceStorage2::close() {
  if (storage_) {
    sqlite3_close(storage_);
    storage_ = NULL;
  }
}

  
//----[  load  ]-------------------------------------------------------------
bool dcResourceStorage2::load(const char* owner_path,
                              dcGenericResource* resource,
                              bool recursive) {

  const char null_string[] = { '\0' };
  if (!owner_path) owner_path = null_string;
  typedef std::queue<std::pair<dcGenericResource*,__int64>> LoadingQueue;
  LoadingQueue loading_queue;
  {
    std::string resource_path = owner_path;
    const char PATH_SEPARATOR_STRING[] =
      { dc::dcGenericResource::PATH_SEPARATOR, '\0' };
    resource_path.append(PATH_SEPARATOR_STRING);
    resource_path.append(resource->getName());
    __int64 resource_record_id;
    getRecordID(resource_path.c_str(), &resource_record_id);
    loading_queue.push(LoadingQueue::value_type(resource,resource_record_id));
  }

  char buffer[2048];
  sqlite3_snprintf(2048,
                   buffer,
                   "SELECT record_id, resource_type, member_name, value, index_hint "\
                   "FROM %Q "\
                   "WHERE owner_record_id=?",
                   segment_.c_str());
  sqlite3_stmt* stmt = NULL;
  if (SQLITE_OK != sqlite3_prepare_v2(storage_,
                                      buffer,
                                      sizeof(buffer),
                                      &stmt,
                                      NULL)) {
    return false;
  }

  // Remove the owner of this resource so the paths don't get confused
  dcGenericResource* resource_owner = resource->leave();

  sprintf_s(buffer,
            2048,
            "%s%c",
            owner_path, 
            dc::dcGenericResource::PATH_SEPARATOR); 
  size_t buffer_bytes_left = 2048;
  size_t bytes_used = strlen(buffer);
  buffer_bytes_left -= bytes_used;
  char* buffer_write_location = buffer + bytes_used;

  do {
    dcGenericResource* loading_resource = loading_queue.front().first;
    __int64 resource_record_id = loading_queue.front().second;
    //size_t consumed = loading_resource->getPathString(buffer_write_location,
    //                                                  buffer_bytes_left);
    if (SQLITE_OK != sqlite3_bind_int64(stmt,
                                        1,
                                        resource_record_id)) {
      resource->rejoin(resource_owner);
      sqlite3_finalize(stmt);
      return false;
    }

    while (SQLITE_ROW == sqlite3_step(stmt)) {
      dcGenericResource* member =
        loading_resource->acquireMember((const char*)sqlite3_column_text(stmt, 2),
                                        (const char*)sqlite3_column_text(stmt, 1),
                                        sqlite3_column_int(stmt, 4));
      if (member) {
        member->interpret((const char*)sqlite3_column_text(stmt, 3));
        if (recursive) {
          loading_queue.push(LoadingQueue::value_type(member,
                                                      sqlite3_column_int64(stmt, 0)));
        }
      }
    }

    // Done loading this resource
    sqlite3_reset(stmt);
    loading_queue.pop();
  } while (!loading_queue.empty());
  sqlite3_finalize(stmt);
  resource->rejoin(resource_owner);
  return true;
}



//----[  save  ]---------------------------------------------------------------
bool dcResourceStorage2::save(const char* owner_path,
                              dcGenericResource* resource) {

  // Remove the owner of this resource so the paths don't get confused
  dcGenericResource* resource_owner = resource->leave();

  // Fix the owner path so that the root is correctly defined as '/'
  const char null_string[] = { '\0' };
  if (!owner_path) owner_path = null_string;

  // Initiate the transaction
  if (!begin(owner_path, resource->getName().c_str())) return false;

  struct Record {
    dcGenericResource* resource;
    int index_hint;
    __int64 owner_record_id;
  };

  typedef std::list<Record> ResourcesToSave;
  ResourcesToSave resources_to_save;

  { // Obtain the record ID of the resource into which we are saving.
    // Resource 0 is the root, but -1 indicates that the owner was not
    // found in the database.  Fail on this condition.
    Record new_record;
    new_record.resource = resource;
    new_record.index_hint = 0;
    getRecordID(owner_path, &new_record.owner_record_id);
    CONFIRM(new_record.owner_record_id >= 0) else {
      resource->rejoin(resource_owner);
      end(false);
      return false;
    }
    resources_to_save.push_back(new_record);
  }

  std::string value_string;
  do {
    Record& record = resources_to_save.back();
    dcGenericResource* resource_being_saved = record.resource;
    dcGenericResource* owner = resource_being_saved->getOwner();

    value_string.clear();
    resource_being_saved->serialize(&value_string);
    char* sql = NULL;
    if (owner) {
      sql = sqlite3_mprintf("REPLACE INTO %Q "\
                            "(record_id, owner_path, owner_record_id, "\
                            "resource_type, member_name, "\
                            "value, index_hint) "\
                            "VALUES "\
                            "(NULL, '%q%c%q', %lld, %Q, %Q, %Q, %i)",
                            segment_.c_str(),
                            owner_path,
                            dc::dcGenericResource::PATH_SEPARATOR,
                            owner->getPathString().c_str(),
                            record.owner_record_id,
                            resource_being_saved->getTypeName().c_str(),
                            resource_being_saved->getName().c_str(),
                            value_string.c_str(),
                            record.index_hint);
    } else {
      sql = sqlite3_mprintf("REPLACE INTO %Q "\
                            "(record_id, owner_path, owner_record_id, "\
                            "resource_type, member_name, "\
                            "value, index_hint) "\
                            "VALUES "\
                            "(NULL, '%q', %lld, %Q, %Q, %Q, %i)",
                            segment_.c_str(),
                            owner_path,
                            record.owner_record_id,
                            resource_being_saved->getTypeName().c_str(),
                            resource_being_saved->getName().c_str(),
                            value_string.c_str(),
                            record.index_hint);
    }
    bool written = SQLITE_OK == sqlite3_exec(storage_, sql, NULL, 0, NULL);
    sqlite3_free(sql);

    // If we couldn't save this resource, undo the transaction and exit
    if (!written) {
      resource->rejoin(resource_owner);
      end(false);
      return false;
    }

    // Add all resource children to the saving queue
    __int64 record_id = sqlite3_last_insert_rowid(storage_);
    dcGenericResource::Array members = resource_being_saved->getMembers();
    dcGenericResource::Array::iterator i = members.begin(), end = members.end();
    Record new_record;
    new_record.owner_record_id = record_id;
    for (int index = 0; i != end; ++i, ++index) {
      new_record.resource = *i;
      new_record.index_hint = index;
      resources_to_save.insert(resources_to_save.begin(), new_record);
    }
    resources_to_save.pop_back();
  } while (!resources_to_save.empty());
  sqlite3_exec(storage_, "VACUUM", NULL, 0, NULL);
  resource->rejoin(resource_owner);
  end(true);
  return true;
}




//----[  clear  ]--------------------------------------------------------------
void dcResourceStorage2::clear() {
  begin("","");
  end(true);
}



//----[  begin  ]--------------------------------------------------------------
bool dcResourceStorage2::begin(const char* owner_path,
                               const char* member_name) {
  if (SQLITE_OK != sqlite3_exec(storage_, "BEGIN", NULL, NULL, NULL))
    return false;

  // Erase all resources whose parent string contains the name of this resource.
  char* sql = sqlite3_mprintf(
     "DELETE FROM %Q "\
     "WHERE owner_path LIKE '%q%c%q%%' OR (owner_path=%Q AND member_name=%Q)",
     segment_.c_str(),
     owner_path,
     dc::dcGenericResource::PATH_SEPARATOR,
     member_name,
     owner_path,
     member_name);
  bool succeeded = SQLITE_OK == sqlite3_exec(storage_, sql, NULL, 0, NULL);
  sqlite3_free(sql);
  return succeeded;
}





//----[  end  ]----------------------------------------------------------------
void dcResourceStorage2::end(bool commit) {
  sqlite3_exec(storage_, commit ? "COMMIT" : "ROLLBACK", NULL, NULL, NULL);
}




//----[  getRecordID  ]--------------------------------------------------------
void dcResourceStorage2::getRecordID(const char* resource_path,
                                     __int64* record_id) {

  // Do a quick test to see if this is the root object
  if (!resource_path || *resource_path == '\0') {
    *record_id = __int64(0);
    return;
  }

  // split the resource path into its owner's path and the member name
  std::string resource_owner_path = resource_path;
  int last_separator = -1;
  const char* resource_member_name = resource_path;
  while (*resource_member_name != '\0') {
    if (*resource_member_name == dc::dcGenericResource::PATH_SEPARATOR) {
      last_separator = resource_member_name - resource_path;
    }
    ++resource_member_name;
  }
  if (last_separator >= 0) {
    resource_owner_path.erase(last_separator);
    resource_member_name = &resource_path[last_separator+1];
  } else {
    resource_member_name = resource_path;
  }

  char buffer[2048];
  sqlite3_snprintf(2048,
                   buffer,
                   "SELECT record_id "\
                   "FROM %Q "\
                   "WHERE owner_path=%Q AND member_name=%Q",
                   segment_.c_str(),
                   resource_owner_path.c_str(),
                   resource_member_name);
  sqlite3_stmt* stmt = NULL;
  if (SQLITE_OK != sqlite3_prepare_v2(storage_,
                                      buffer,
                                      sizeof(buffer),
                                      &stmt,
                                      NULL)) {
    *record_id = __int64(-1);
  }
  if (SQLITE_ROW == sqlite3_step(stmt)) {
    *record_id = sqlite3_column_int64(stmt, 0);
  } else {
    *record_id = __int64(-1);
  }
  sqlite3_finalize(stmt);
}


}