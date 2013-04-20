#if 0
#include <windows.h>
#include <memory>
#include <math.h>
#include "common/fixedpoint/fixedpoint.h"
#include <assert.h>
#include "common/number.h"
#include "common/randint32.h"
#define DC_ENABLE_DEBUGGING
#include <dc/debug>
#include <dcx/win32>
#include <d3dx9math.h>
#include <dcx/bitmask>
#include "common/sqlite/sqlite3.h"
#include "common/randf.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "dcx_d.lib")

/*http://www.sqlite.org/rtree.html

each entity has a unique id in the database and its corresponding size-range

CREATE VIRTUAL TABLE entity_tree USING rtree(id, min_x, max_x, min_z, max_z)
REPLACE INTO entity_tree VALUES (?,?,?,?,?)

CREATE TABLE entities (id INTEGER PRIMARY KEY, x REAL, z REAL, entity_ptr BLOB)
INSERT INTO entities VALUES (?,?,?,?)

// save a pointer to the entity in the database
sqlite3_bind_blob(entity_insert, 4, &entity_pointer, SQLITE_TRANSIENT);

*/

int collider_comparisons;
Number collider_x, collider_z, collider_rsq;
void collisionFunctionForSqlite(sqlite3_context* context, int dunno, sqlite3_value** values) {
  Number x(sqlite3_value_double(values[0])), z(sqlite3_value_double(values[1]));
  Number distance_sq = FixedPoint::square(collider_x - x)
                     + FixedPoint::square(collider_z - z);
  sqlite3_result_int(context,
                     distance_sq < collider_rsq ? 1 : 0);
  ++collider_comparisons;
}

//----[  WinMain  ]------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  sqlite3* db;
  sqlite3_open(":memory:", &db);
  char* errmsg;

  // create the data structure
  char sql[2048];
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE VIRTUAL TABLE entity_tree USING rtree(id, min_x, max_x, min_z, max_z)");
  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    return 0;
  }
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE TABLE entities (id INTEGER PRIMARY KEY, x REAL, z REAL, entity_ptr BLOB)");
  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    return 0;
  }

  sqlite3_create_function(db,
                          "collision",
                          2,
                          SQLITE_ANY,
                          NULL, // user data ptr
                          collisionFunctionForSqlite,
                          NULL, NULL); // not an aggregating function

  static const size_t ENTITIES = 1024;
  static const size_t SIZE = 4096;


  enum Statement {
    STATEMENT_UPDATE_BOUNDARIES,
    STATEMENT_INSERT_ENTITY,
    STATEMENT_UPDATE_ENTITY,
    STATEMENT_QUERY_OVERLAP,
    STATEMENT_QUERY_COLLISION,
    NUMBER_OF_STATEMENTS,
  };

  struct Entity {
    int id;
    Number x, z;
    Number collision_rad_sq;
    Number visible_rad_sq;
    Number radius;
  };

  const char* statements_text[NUMBER_OF_STATEMENTS] = {
    "REPLACE INTO entity_tree VALUES (?,?,?,?,?)",
    "INSERT INTO entities VALUES (?,?,?,?)", // id, x, z, entity_ptr (blob)
    "UPDATE entities SET x=?, z=? WHERE id=?", // x, z, id
    "SELECT id FROM entity_tree WHERE id>? AND max_x>=? AND min_x<=? AND max_z>=? AND min_z<=?", // min/max x then min/max z
    // searcher's id, min x, max x, min z, max z
    "SELECT entities.id, entity_ptr FROM entities,entity_tree WHERE entities.id=entity_tree.id AND entity_tree.id>? AND collision(entities.x, entities.z) AND max_x>=? AND min_x<=? AND max_z>=? AND min_z<=?"
  };
  sqlite3_stmt* statements[NUMBER_OF_STATEMENTS];

  for (int i = 0; i <NUMBER_OF_STATEMENTS; ++i) {
    int rc;
    if (SQLITE_OK != (rc=sqlite3_prepare_v2(db,
                                        statements_text[i],
                                        strlen(statements_text[i]),
                                        &statements[i],
                                        NULL))) {

      DEBUG_INFO("%i - %s", i, sqlite3_errmsg(db));
    sqlite3_close(db);
      return false;
    }
  }

  Entity* entities = new Entity[ENTITIES];
  for (int i = 0; i < ENTITIES; ++i) {
    entities[i].id = i;
    entities[i].x = randf() * SIZE;
    entities[i].z = randf() * SIZE;
    float f = 0.5 + randf() * 1.0;
    entities[i].radius = f;
    entities[i].collision_rad_sq = f*f;
    f = 1.5f*randf() + f;
    entities[i].visible_rad_sq = f*f;

    sqlite3_bind_double(statements[STATEMENT_INSERT_ENTITY], 1, i);
    sqlite3_bind_double(statements[STATEMENT_INSERT_ENTITY], 2, entities[i].x.toFloat());
    sqlite3_bind_double(statements[STATEMENT_INSERT_ENTITY], 3, entities[i].z.toFloat());
    sqlite3_bind_blob(statements[STATEMENT_INSERT_ENTITY], 4, &entities[i], 4, SQLITE_TRANSIENT);
    sqlite3_step(statements[STATEMENT_INSERT_ENTITY]);
    sqlite3_reset(statements[STATEMENT_INSERT_ENTITY]);

    sqlite3_bind_int(statements[STATEMENT_UPDATE_BOUNDARIES], 1, entities[i].id);
    sqlite3_bind_double(statements[STATEMENT_UPDATE_BOUNDARIES], 2, (entities[i].x - entities[i].radius).toFloat());
    sqlite3_bind_double(statements[STATEMENT_UPDATE_BOUNDARIES], 3, (entities[i].x + entities[i].radius).toFloat());
    sqlite3_bind_double(statements[STATEMENT_UPDATE_BOUNDARIES], 4, (entities[i].z - entities[i].radius).toFloat());
    sqlite3_bind_double(statements[STATEMENT_UPDATE_BOUNDARIES], 5, (entities[i].z + entities[i].radius).toFloat());
    sqlite3_step(statements[STATEMENT_UPDATE_BOUNDARIES]);
    sqlite3_reset(statements[STATEMENT_UPDATE_BOUNDARIES]);
  }

  dcx::dcxWin32Clock clock;
  unsigned int counter = 0;
  double starter = clock.getAccurateSystemTime();
  double ender = starter + 5.0;
  double time=ender;
  unsigned int visible_tally = 0;
      collider_comparisons = 0;
#define TIMETRIAL
#ifdef TIMETRIAL
  while ((time = clock.getAccurateSystemTime()) < ender) {
#else
  while (counter < 1) {
#endif
    for (int i = 0; i < ENTITIES; ++i) {
      collider_x = entities[i].x = randf()*SIZE;
      collider_z = entities[i].z = randf()*SIZE;
      collider_rsq = entities[i].collision_rad_sq;
    sqlite3_bind_int(statements[STATEMENT_UPDATE_ENTITY], 3, entities[i].id);
    sqlite3_bind_double(statements[STATEMENT_UPDATE_ENTITY], 1, collider_x.toFloat());
    sqlite3_bind_double(statements[STATEMENT_UPDATE_ENTITY], 2, collider_z.toFloat());
    sqlite3_step(statements[STATEMENT_UPDATE_ENTITY]);
    sqlite3_reset(statements[STATEMENT_UPDATE_ENTITY]);

      float minx = (entities[i].x - entities[i].radius).toFloat();
      float maxx = (entities[i].x + entities[i].radius).toFloat();
      float minz = (entities[i].x - entities[i].radius).toFloat();
      float maxz = (entities[i].x + entities[i].radius).toFloat();
    sqlite3_bind_int(statements[STATEMENT_UPDATE_BOUNDARIES], 1, entities[i].id);
    sqlite3_bind_double(statements[STATEMENT_UPDATE_BOUNDARIES], 2, minx);
    sqlite3_bind_double(statements[STATEMENT_UPDATE_BOUNDARIES], 3, maxx);
    sqlite3_bind_double(statements[STATEMENT_UPDATE_BOUNDARIES], 4, minz);
    sqlite3_bind_double(statements[STATEMENT_UPDATE_BOUNDARIES], 5, maxz);
    sqlite3_step(statements[STATEMENT_UPDATE_BOUNDARIES]);
    sqlite3_reset(statements[STATEMENT_UPDATE_BOUNDARIES]);
#if 0
      sqlite3_bind_int(statements[STATEMENT_QUERY_OVERLAP], 1, entities[i].id);
      sqlite3_bind_double(statements[STATEMENT_QUERY_OVERLAP], 2, (collider_x - entities[i].radius).toFloat());
      sqlite3_bind_double(statements[STATEMENT_QUERY_OVERLAP], 3, (collider_x + entities[i].radius).toFloat());
      sqlite3_bind_double(statements[STATEMENT_QUERY_OVERLAP], 4, (collider_z - entities[i].radius).toFloat());
      sqlite3_bind_double(statements[STATEMENT_QUERY_OVERLAP], 5, (collider_z + entities[i].radius).toFloat());
      while (SQLITE_ROW == sqlite3_step(statements[STATEMENT_QUERY_OVERLAP])) {
        Entity* entity = (Entity*)sqlite3_column_blob(statements[STATEMENT_QUERY_OVERLAP], 1);
        ++visible_tally;
#ifndef TIMETRIAL
        DEBUG_INFO("collision = %i with %i @ %.02f, %.02f",
          entities[i].id, sqlite3_column_int(statements[STATEMENT_QUERY_OVERLAP], 0),
          entity->x.toFloat(),
          entity->z.toFloat());
#endif
      }
      sqlite3_reset(statements[STATEMENT_QUERY_OVERLAP]);
#endif

      sqlite3_bind_int(statements[STATEMENT_QUERY_COLLISION], 1, entities[i].id);
      sqlite3_bind_double(statements[STATEMENT_QUERY_COLLISION], 2, (collider_x - entities[i].radius).toFloat());
      sqlite3_bind_double(statements[STATEMENT_QUERY_COLLISION], 3, (collider_x + entities[i].radius).toFloat());
      sqlite3_bind_double(statements[STATEMENT_QUERY_COLLISION], 4, (collider_z - entities[i].radius).toFloat());
      sqlite3_bind_double(statements[STATEMENT_QUERY_COLLISION], 5, (collider_z + entities[i].radius).toFloat());
      while (SQLITE_ROW == sqlite3_step(statements[STATEMENT_QUERY_COLLISION])) {
        Entity* entity = (Entity*)sqlite3_column_blob(statements[STATEMENT_QUERY_COLLISION], 1);
        ++visible_tally;
#ifndef TIMETRIAL
        DEBUG_INFO("collision = %i with %i @ %.02f, %.02f",
          entities[i].id, sqlite3_column_int(statements[STATEMENT_QUERY_COLLISION], 0),
          entity->x.toFloat(),
          entity->z.toFloat());
#endif
      }
      sqlite3_reset(statements[STATEMENT_QUERY_COLLISION]);

#ifndef TIMETRIAL
      DEBUG_INFO("comparisons = %i", collider_comparisons);
#endif
    }

    ++counter;
  }

  for (int i = 0; i <NUMBER_OF_STATEMENTS; ++i) {
    sqlite3_finalize(statements[i]);
  }
  sqlite3_close(db);
  delete[] entities;
  DEBUG_INFO("%f queries/second; %f visible/entity (comparisons=%i)", counter / (time-starter), visible_tally/double(counter), collider_comparisons);

#if 0
  char sql[2048];

  // create the data structure
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE TABLE IF NOT EXISTS objects ("\
                   "id INTEGER PRIMARY KEY,"\
                   "x INTEGER,"\
                   "z INTEGER"\
                   ")");
  char* errmsg;
  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    return 0;
  }
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE INDEX IF NOT EXISTS "\
                   "index_x "\
                   "ON objects (x)");
  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    sqlite3_close(db);
    return false;
  }
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE INDEX IF NOT EXISTS "\
                   "index_z "\
                   "ON objects (z)");
  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    sqlite3_close(db);
    return false;
  }
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE TABLE IF NOT EXISTS has_seen ("\
                   "a INTEGER,"\
                   "b INTEGER"\
                   ")");
  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    sqlite3_close(db);
    return false;
  }
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE INDEX IF NOT EXISTS "\
                   "index_a "\
                   "ON has_seen (a)");
  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    sqlite3_close(db);
    return false;
  }
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE INDEX IF NOT EXISTS "\
                   "index_b "\
                   "ON has_seen (b)");
  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    sqlite3_close(db);
    return false;
  }
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE TABLE IF NOT EXISTS results ("\
                   "id INTEGER PRIMARY KEY,"\
                   "x INTEGER,"\
                   "z INTEGER"\
                   ")");

  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
    DEBUG_INFO("%s", errmsg);
    return 0;
  }
  sqlite3_exec(db, "PRAGMA case_sensitive_like=ON;", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA count_changes=OFF;", NULL, NULL, NULL);

  static const int COUNT = 256;
  static const int SIZE = 64;
  // insert a bunch of random objects
  for (int i = 0; i < COUNT; ++i) {
    sqlite3_snprintf(2048,
                     sql,
                     "INSERT INTO objects (id, x, z) VALUES "\
                     "(%i, %i, %i)",
                     i,
                     rand_uint32()%SIZE,
                     rand_uint32()%SIZE);
    if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
      DEBUG_INFO("%s", errmsg);
    sqlite3_close(db);
      return false;
    }
  }

  enum Statement {
    STATEMENT_CURRENT_VISIBILITY_QUERY,
    STATEMENT_NEW_VISIBILITY_QUERY,
    STATEMENT_INSERT_HAS_SEEN,
    NUMBER_OF_STATEMENTS,
  };

  const char* statements_text[NUMBER_OF_STATEMENTS] = {
    "SELECT id, x, z FROM objects WHERE (x BETWEEN ? AND ?) AND (z BETWEEN ? AND ?)",
    "SELECT id FROM objects WHERE (x BETWEEN ? AND ?) AND (z BETWEEN ? AND ?) EXCEPT SELECT (a) FROM has_seen WHERE (b=?)",
    "INSERT INTO has_seen (a,b) VALUES (?,?)"
  };
  sqlite3_stmt* statements[NUMBER_OF_STATEMENTS];

  for (int i = 0; i <NUMBER_OF_STATEMENTS; ++i) {
    int rc;
    if (SQLITE_OK != (rc=sqlite3_prepare_v2(db,
                                        statements_text[i],
                                        strlen(statements_text[i]),
                                        &statements[i],
                                        NULL))) {

      DEBUG_INFO("%i-%s", i, sqlite3_errmsg(db));
    sqlite3_close(db);
      return false;
    }
  }

  dcx::dcxWin32Clock clock;
  unsigned int counter = 0;
  double starter = clock.getAccurateSystemTime();
  double ender = starter + 1.0;
  double time=ender;
  unsigned int visible_tally = 0;
#define TIMETRIAL
#ifdef TIMETRIAL
  while ((time = clock.getAccurateSystemTime()) < ender) {
#else
  while (counter < 64) {
#endif
    double x = counter;
    double z = counter;
    sqlite3_bind_int(statements[STATEMENT_NEW_VISIBILITY_QUERY], 1, x -8);
    sqlite3_bind_int(statements[STATEMENT_NEW_VISIBILITY_QUERY], 2, x +8);
    sqlite3_bind_int(statements[STATEMENT_NEW_VISIBILITY_QUERY], 3, z - 8);
    sqlite3_bind_int(statements[STATEMENT_NEW_VISIBILITY_QUERY], 4, z + 8);
    sqlite3_bind_int(statements[STATEMENT_NEW_VISIBILITY_QUERY], 5, COUNT);

    //sqlite3_bind_int(statements[STATEMENT_CURRENT_VISIBILITY_QUERY], 1, x -8);
    //sqlite3_bind_int(statements[STATEMENT_CURRENT_VISIBILITY_QUERY], 2, x +8);
    //sqlite3_bind_int(statements[STATEMENT_CURRENT_VISIBILITY_QUERY], 3, z - 8);
    //sqlite3_bind_int(statements[STATEMENT_CURRENT_VISIBILITY_QUERY], 4, z + 8);
#ifndef TIMETRIAL
    char buffer[512];
    sprintf_s(buffer, 512, "\n====   %f, %f   ============\n", x, z);
    OutputDebugString(buffer);
#endif
    while (sqlite3_step(statements[STATEMENT_NEW_VISIBILITY_QUERY]) == SQLITE_ROW) {
#ifndef TIMETRIAL
      char buffer[512];
      sprintf_s(buffer, 512, " %lu %8f, %8f\n", sqlite3_column_int(statements[STATEMENT_NEW_VISIBILITY_QUERY], 0), sqlite3_column_double(statements[STATEMENT_NEW_VISIBILITY_QUERY], 1), sqlite3_column_double(statements[STATEMENT_NEW_VISIBILITY_QUERY], 2));
      OutputDebugString(buffer);
#endif

      //sqlite3_bind_int(statements[STATEMENT_INSERT_HAS_SEEN], 1, sqlite3_column_int(statements[STATEMENT_NEW_VISIBILITY_QUERY],0));
      //sqlite3_bind_int(statements[STATEMENT_INSERT_HAS_SEEN], 2, COUNT);
      //sqlite3_step(statements[STATEMENT_INSERT_HAS_SEEN]);
      //sqlite3_reset(statements[STATEMENT_INSERT_HAS_SEEN]);
      visible_tally++;
    }
    sqlite3_reset(statements[STATEMENT_NEW_VISIBILITY_QUERY]);
    sqlite3_reset(statements[STATEMENT_CURRENT_VISIBILITY_QUERY]);
    ++counter;
  }

  for (int i = 0; i <NUMBER_OF_STATEMENTS; ++i) {
    sqlite3_finalize(statements[i]);
  }
  sqlite3_close(db);
  DEBUG_INFO("%f queries/second; %f visible/entity (tally=%i)", counter / (time-starter), visible_tally/double(counter), visible_tally);
#endif

//  sqlite3* db;
//  sqlite3_open(":memory:", &db);
//
//  char sql[2048];
//
//  // create the data structure
//  sqlite3_snprintf(2048,
//                   sql,
//                   "CREATE TABLE IF NOT EXISTS objects ("\
//                   "id INTEGER PRIMARY KEY,"\
//                   "x REAL,"\
//                   "z REAL"\
//                   ")");
//  char* errmsg;
//  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
//    DEBUG_INFO("%s", errmsg);
//    return 0;
//  }
//  sqlite3_snprintf(2048,
//                   sql,
//                   "CREATE INDEX IF NOT EXISTS "\
//                   "index_x "\
//                   "ON objects (x)");
//  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
//    DEBUG_INFO("%s", errmsg);
//    sqlite3_close(db);
//    return false;
//  }
//  sqlite3_snprintf(2048,
//                   sql,
//                   "CREATE INDEX IF NOT EXISTS "\
//                   "index_z "\
//                   "ON objects (z)");
//  if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
//    DEBUG_INFO("%s", errmsg);
//    sqlite3_close(db);
//    return false;
//  }
//  sqlite3_exec(db, "PRAGMA case_sensitive_like=ON;", NULL, NULL, NULL);
//  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
//  sqlite3_exec(db, "PRAGMA count_changes=OFF;", NULL, NULL, NULL);
//
//  static const int COUNT = 16000;
//  static const float SIZE = 2048;
//  // insert a bunch of random objects
//  for (int i = 0; i < COUNT; ++i) {
//    sqlite3_snprintf(2048,
//                     sql,
//                     "INSERT INTO objects (id, x, z) VALUES "\
//                     "(%i, %f, %f)",
//                     i,
//                     randf()*SIZE,
//                     randf()*SIZE);
//    if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) {
//      DEBUG_INFO("%s", errmsg);
//    sqlite3_close(db);
//      return false;
//    }
//  }
//
//  // create a query
//  sqlite3_snprintf(2048,
//                   sql,
//                   "SELECT id, x, z FROM objects "\
//                   "WHERE x>=? AND x<=? AND z>=? AND z<=?");
//  sqlite3_stmt* stmt = NULL;
//  if (SQLITE_OK != sqlite3_prepare_v2(db,
//                                      sql,
//                                      sizeof(sql),
//                                      &stmt,
//                                      NULL)) {
//    return false;
//  }
//
//  dcx::dcxWin32Clock clock;
//  unsigned int counter = 0;
//  double starter = clock.getAccurateSystemTime();
//  double ender = starter + 1;
//  double time;
//  unsigned int visible_tally = 0;
//  while ((time = clock.getAccurateSystemTime()) < ender) {
//  //while (counter < 64) {
//    double x = 32.0f;
//    double z = randf()*SIZE;
//    sqlite3_bind_double(stmt, 1, x - 8.0);
//    sqlite3_bind_double(stmt, 2, x + 8.0);
//    sqlite3_bind_double(stmt, 3, z - 8.0);
//    sqlite3_bind_double(stmt, 4, z + 8.0);
//    //char buffer[512];
//    //sprintf_s(buffer, 512, "\n====   %f, %f   ============\n", x, z);
//    //OutputDebugString(buffer);
//    while (sqlite3_step(stmt) == SQLITE_ROW) {
//      //char buffer[512];
//      //sprintf_s(buffer, 512, " %lu %8f, %8f\n", sqlite3_column_int(stmt, 0), sqlite3_column_double(stmt, 1), sqlite3_column_double(stmt, 2));
//      //OutputDebugString(buffer);
//      visible_tally++;
//    }
//    sqlite3_reset(stmt);
//    ++counter;
//  }
//  sqlite3_finalize(stmt);
//  sqlite3_close(db);
//  DEBUG_INFO("%f queries/second; %f visible/entity", counter / (time-starter), visible_tally/double(counter));
}



// some sort of filter class?
#endif