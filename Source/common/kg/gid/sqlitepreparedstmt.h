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
#ifndef __COMMON_KG_GID_SQLITEPREPAREDSTMT_H__
#define __COMMON_KG_GID_SQLITEPREPAREDSTMT_H__
#pragma once

#include "../kg.h"
#include "../assert.h"

namespace kg {
namespace gid {


  
//----[  SQLitePreparedStmt  ]-------------------------------------------------
// Stores a pre-parsed query to improve the speed of database access.  Each
// prepared statement is created as a static object inside whatever method
// uses that statement.  Thus, the first time it is executed the statement is
// prepared; after that, the method need only call stepReset in order to
// execute that statement.
// When the program closes, calling finalizeAll will clean up everything
// associated with prepared statements by traversing a static linked list.
class SQLitePreparedStmt {
public:
  SQLitePreparedStmt(sqlite3* db, const char* query) {
    next = first;
    if (next) next->to_this = &next;
    first = this;
    to_this = &first;
    int rc = sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);
    if (rc != SQLITE_OK) {
      const char* error = sqlite3_errmsg(db);
      confirm_failed(error);
    }
  }
  ~SQLitePreparedStmt() {
    assert(!stmt);
    if (next) next->to_this = to_this;
    *to_this = next;
  }

  static void finalizeAll() {
    SQLitePreparedStmt* next = first;
    while (next) {
      sqlite3_finalize(next->stmt);
      next->stmt = 0;
      next = next->next;
    }
  }

  void stepReset() {
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
  }

  sqlite3_stmt* stmt;

private:
  SQLitePreparedStmt** to_this;
  SQLitePreparedStmt* next;
  static SQLitePreparedStmt* first;
};

}
}


#endif