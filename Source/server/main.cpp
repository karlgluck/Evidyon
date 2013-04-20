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
#include "globalactormanager.h"
#include "globalclientmanager.h"
#include "globalnetworkmanager.h"
#include "globalworldmanager.h"
#include "globaldatabasemanager.h"
#include "globalgamefilemanager.h"

#include "globalitemmanager.h"
#include "globalquestmanager.h"
#include "server/lifeformai/globallifeformaimanager.h"
#include "server/treasure/globaltreasuremanager.h"
#include "server/guild/globalguildmanager.h"
#include "shared/evidyon_network.h"
#include <dc/debug>
#include <dcx/win32>
#include <dcx/inifilereader>
#include <dcx/inifilewriter>
#include <dcx/asyncdebugout>
#include "server/geosid/globalgeosidmanager.h"
#include "server/globallogger.h"

#include "time.h"


#include "main.h"

#include <dcx/dcxscheduler.h>
#include <dc/filestream>
#include "clientconfig.h"

#include "shared/evidyon_version.h"

#include "shared/time/timermanager.h"

#include "common/fracture/todo.h"


// i removed NDEBUG from the release-mode definition so that asserts would go

namespace Evidyon {

bool running = true;

unsigned long restart_counter;



//----[  Unique64BitGenerator  ]-----------------------------------------------
Unique64BitGenerator::Unique64BitGenerator() {
  next_lower_part_ = 0;
}



//----[  next  ]---------------------------------------------------------------
void Unique64BitGenerator::next(void* eight_bytes) {
 *(static_cast<unsigned __int64*>(eight_bytes)) =
    (static_cast<unsigned __int64>(MAJOR_VERSION) << (32+24)) |         // |-------
    (static_cast<unsigned __int64>(MINOR_VERSION) << (32+16)) |         // -|------
    (static_cast<unsigned __int64>(Evidyon::restart_counter) << (32)) | // --||----
    (static_cast<unsigned __int64>(++next_lower_part_));                // ----||||
}


}





//----[  ConsoleHandler  ]-----------------------------------------------------
BOOL WINAPI ConsoleHandler(DWORD CtrlType)
{
  if (CtrlType == CTRL_SHUTDOWN_EVENT ||
      CtrlType == CTRL_CLOSE_EVENT)
      Evidyon::running = false;

  return TRUE;
}



namespace Evidyon {

static const char* CONFIG_FILE_NAME = "config.ini";

struct GlobalContext {
  Evidyon::GlobalNetworkManager network;
  Evidyon::GlobalDatabaseManager database;
  Evidyon::GlobalClientManager clients;
  Evidyon::GlobalGamefileManager gamefile;
  Evidyon::GlobalActorManager actors;
  Evidyon::GlobalWorldManager world;
  Server::GlobalLifeformAIManager lifeform_ai;
  Evidyon::GlobalItemManager items;
  Evidyon::Quests::Server::GlobalQuestManager quests;
  Evidyon::Server::GlobalTreasureManager treasure;
  Evidyon::Server::GlobalGuildManager guild;
  Evidyon::Server::GlobalGeosidManager geosid;
  Evidyon::Server::GlobalLogger logger;
};




//----[  startup  ]------------------------------------------------------------
bool startup(GlobalContext& server) {

  dcx::dcxIniFileReader ini_file_reader;
  ini_file_reader.read(CONFIG_FILE_NAME);

  size_t epic_counter = ini_file_reader.getSetting("Treasure", "EpicCounter", 99999);

  // the logger uses a database named "forest"!  get it?  ha!
  CONFIRM(server.logger.create(ini_file_reader.getSetting("Settings", "logfile", "forest.sqldb")));

//  CONFIRM(server.statistics_logger.create()) else return false;
  CONFIRM(server.network.create(Evidyon::GlobalClientManager::MAX_CLIENTS,
                                Evidyon::Network::SERVER_PORT)) else return false;
  CONFIRM(server.items.create()) else return false;
  CONFIRM(server.actors.create()) else return false;
  CONFIRM(server.lifeform_ai.create(&server.actors)) else return false;
  server.treasure.create(epic_counter, &server.items);
  CONFIRM(server.database.create(ini_file_reader.getSetting("Settings","database","database.cfg"))) else return false;
  server.geosid.create(&server.database);
  CONFIRM(server.gamefile.create(ini_file_reader.getSetting("Settings","media","evidyon.gsvr"),
                                 server.database.getUniqueAvatarNameChecker(),
                                &server.items,
                                &server.actors,
                                &server.world,
                                &server.lifeform_ai,
                                &server.quests,
                                &server.treasure,
                                &server.geosid)) else return false;
  CONFIRM(server.clients.create()) else return false;
  server.guild.create(&server.database, &server.geosid);
  SetConsoleCtrlHandler(ConsoleHandler, TRUE);

  // Increment the restart counter
  static const char* VERSION_SECTION = "Server Data - " VERSION_TEXT;
  unsigned int restart_counter = 1 +
    ini_file_reader.getSetting(VERSION_SECTION,
                               "restart_count",
                               0); // default value
  { // rewrite the .ini file
    dcx::dcxIniFileWriter ini_file_writer;
    ini_file_writer.scan(&ini_file_reader);
    ini_file_writer.setSetting(VERSION_SECTION,
                               "restart_count",
                              (int)restart_counter);
    dc::dcFileStream ini_fs;
    ini_fs.open(CONFIG_FILE_NAME, STREAM_OVERWRITE|STREAM_TEXT);
    ini_file_writer.write(&ini_fs);
    ini_fs.close();
  }

  // Sync the current hour
  Time::updateMinuteHourCounters();

  // Finished starting up
  Evidyon::restart_counter = restart_counter;
  printf("started : execution #%lu", restart_counter);

  // Success
  return true;
}




//----[  shutdown  ]-----------------------------------------------------------
void shutdown(GlobalContext& server) {
  DEBUG_INFO("disconnecting clients...");
  server.clients.disconnectAll();
  DEBUG_INFO("shutting down...");
  SetConsoleCtrlHandler(ConsoleHandler, FALSE);
  {
    dcx::dcxIniFileReader ini_file_reader;
    ini_file_reader.read(CONFIG_FILE_NAME);
    dcx::dcxIniFileWriter writer;
    writer.scan(&ini_file_reader);
    size_t epic_counter;
    server.treasure.destroy(&epic_counter);
    writer.setSetting("Treasure", "EpicCounter", (int)epic_counter);
    writer.write(CONFIG_FILE_NAME);
  }
  server.geosid.store();
  server.geosid.destroy();
  server.guild.store();
  server.guild.destroy();
  server.quests.destroy();
  server.lifeform_ai.destroy();
  server.actors.destroy();
  server.world.destroy();
  server.items.destroy();
  server.gamefile.destroy();
  server.clients.destroy();
  server.database.destroy();
  server.network.destroy();
  server.logger.destroy();
  DEBUG_ATEXIT(); // Display debugging messages
  Sleep(1000);
}

}


void writeTodoReport(const char* report) {
  FILE* fp = NULL;
  fp = fopen("todo-report.txt", "w+t");
  if (fp) {
    fwrite(report, 1, strlen(report), fp);
    fclose(fp);
  }
  OutputDebugString(report);
}

#include <direct.h>
#include "shared/time/timer.h"

extern const char* LICENSE_TEXT;

int main(int argc, char* argv[]) {
  printf("%s",LICENSE_TEXT);

  Evidyon::GlobalContext server;
  if (!Evidyon::startup(server)) {
    Evidyon::shutdown(server);
    return -1;
  }

  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

  typedef dcx::dcxWin32Clock Clock;
  double last_update_time = Clock::getAccurateSystemTime();
  Evidyon::Time::TimerManager timer_manager(last_update_time);

  srand(Clock::getSystemTime());


  enum MainLoopPeriodTasks {
    UPDATE_CLIENTS,
    UPDATE_LIFEFORM_AI,
    UPDATE_GUILDS, // this should NOT BE HERE
    UPDATE_MEDIUMTASK_SCHEDULER,
    UPDATE_COUNT,
  };

  enum MediumTasks {
    TASK_UPDATE_WORLD,
    TASK_CONSOLIDATE_REGIONS,
    TASK_ROTATE_BAZAAR_LISTINGS,
    TASK_UPDATE_MINUTE_COUNTER,
    TASK_LONGTASK_SCHEDULER,
    NUMBER_OF_MEDIUM_TASKS,
  };

  enum LongTasks {
    TASK_BACKUP_DATABASE,
    NUMBER_OF_LONG_TASKS,
  };

  double current_time = Clock::getAccurateSystemTime();
  dcx::dcxSchedulerB<double, UPDATE_COUNT> update_scheduler(current_time, Evidyon::Time::MAIN_LOOP_PERIOD);
  dcx::dcxSchedulerB<double, NUMBER_OF_MEDIUM_TASKS> medium_task_scheduler(current_time, 15.0);
  dcx::dcxSchedulerB<double, Evidyon::ClientConfig::SYNC_GROUPS> slow_sync_scheduler(current_time, Evidyon::Network::SLOW_SYNC_PERIOD);
  dcx::dcxSchedulerB<double, Evidyon::ClientConfig::SYNC_GROUPS> fast_sync_scheduler(current_time, Evidyon::Network::FAST_SYNC_PERIOD);

  Evidyon::Time::Timer<Evidyon::Time::RESOLUTION_60000_MS> backup_timer;
  backup_timer.enable();
  backup_timer.set(10000);

  unsigned int consolidate_incrementer = 0;
  
  while (Evidyon::running) {
    current_time = Clock::getAccurateSystemTime();
    timer_manager.update(current_time);

    Evidyon::Time::current_frame = current_time;
    double time_since_last_executed;
    switch (update_scheduler.poll(current_time, &time_since_last_executed)) {
      case UPDATE_CLIENTS:      server.clients.update(current_time, time_since_last_executed); break;
      case UPDATE_LIFEFORM_AI:  server.lifeform_ai.update(current_time, time_since_last_executed); break;
      case UPDATE_GUILDS:       server.guild.updateGuildsAttackingGeosids(); break;
      case UPDATE_MEDIUMTASK_SCHEDULER:
        switch (medium_task_scheduler.poll(current_time, &time_since_last_executed)) {
          case TASK_UPDATE_WORLD:        server.world.update(current_time); break;
          case TASK_CONSOLIDATE_REGIONS: server.world.consolidateRegions(consolidate_incrementer++); break;
          case TASK_ROTATE_BAZAAR_LISTINGS: server.database.bazaar_rotateListings(); break;
          case TASK_UPDATE_MINUTE_COUNTER: Evidyon::Time::updateMinuteHourCounters(); break;
          case TASK_LONGTASK_SCHEDULER:
            if (backup_timer.poll()) {
              backup_timer.set(60000 * 10); // backup every 10 minutes
              server.database.startBackup();
            } break;
        }
      break;
    }

    // network updates must occur AS FAST AS POSSIBLE
    server.network.update(&server.clients);

    unsigned int group = slow_sync_scheduler.poll(current_time, &time_since_last_executed);
    if (group < Evidyon::ClientConfig::SYNC_GROUPS) server.clients.slowSync(group, time_since_last_executed);
    group = fast_sync_scheduler.poll(current_time, &time_since_last_executed);
    if (group < Evidyon::ClientConfig::SYNC_GROUPS) {
      server.clients.fastSync(group, time_since_last_executed);
    }

    // network updates must occur AS FAST AS POSSIBLE
    server.network.update(&server.clients);
  }



  Evidyon::shutdown(server);

  todoReport(writeTodoReport);

  return 0;
}


const char* LICENSE_TEXT =
  "\n" \
  "common\\mtrand\\mtrand.h  (Mersenne Twister)\n" \
  "// mtrand.h\n" \
  "// C++ include file for MT19937, with initialization improved 2002/1/26.\n" \
  "// Coded by Takuji Nishimura and Makoto Matsumoto.\n" \
  "// Ported to C++ by Jasper Bedaux 2003/1/1 (see http://www.bedaux.net/mtrand/).\n" \
  "// The generators returning floating point numbers are based on\n" \
  "// a version by Isaku Wada, 2002/01/09\n" \
  "//\n" \
  "// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,\n" \
  "// All rights reserved.\n" \
  "//\n" \
  "// Redistribution and use in source and binary forms, with or without\n" \
  "// modification, are permitted provided that the following conditions\n" \
  "// are met:\n" \
  "//\n" \
  "// 1. Redistributions of source code must retain the above copyright\n" \
  "//  notice, this list of conditions and the following disclaimer.\n" \
  "//\n" \
  "// 2. Redistributions in binary form must reproduce the above copyright\n" \
  "//  notice, this list of conditions and the following disclaimer in the\n" \
  "//  documentation and/or other materials provided with the distribution.\n" \
  "//\n" \
  "// 3. The names of its contributors may not be used to endorse or promote\n" \
  "//  products derived from this software without specific prior written\n" \
  "//  permission.\n" \
  "//\n" \
  "// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n" \
  "// \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n" \
  "// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n" \
  "// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR\n" \
  "// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n" \
  "// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n" \
  "// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\n" \
  "// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\n" \
  "// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n" \
  "// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n" \
  "// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n" \
  "//\n" \
  "// Any feedback is very welcome.\n" \
  "// http://www.math.keio.ac.jp/matumoto/emt.html\n" \
  "// email: matumoto@math.keio.ac.jp\n" \
  "//\n" \
  "// Feedback about the C++ port should be sent to Jasper Bedaux,\n" \
  "// see http://www.bedaux.net/mtrand/ for e-mail address and info.\n" \
  "\n" \
  "\n" \
  "\n" \
  "common\\fixedpoint\\fixedpoint.h - fixed-point math library\n" \
  "/*\n" \
  "Copyright (c) 2007, Markus Trenkwalder\n" \
  "\n" \
  "Portions taken from the Vicent 3D rendering library\n" \
  "Copyright (c) 2004, David Blythe, Hans Martin Will\n" \
  "\n" \
  "All rights reserved.\n" \
  "\n" \
  "Redistribution and use in source and binary forms, with or without \n" \
  "modification, are permitted provided that the following conditions are met:\n" \
  "\n" \
  "* Redistributions of source code must retain the above copyright notice, \n" \
  "  this list of conditions and the following disclaimer.\n" \
  "\n" \
  "* Redistributions in binary form must reproduce the above copyright notice,\n" \
  "  this list of conditions and the following disclaimer in the documentation \n" \
  "  and/or other materials provided with the distribution.\n" \
  "\n" \
  "* Neither the name of the library's copyright owner nor the names of its \n" \
  "  contributors may be used to endorse or promote products derived from this \n" \
  "  software without specific prior written permission.\n" \
  "\n" \
  "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n" \
  "\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n" \
  "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n" \
  "A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR\n" \
  "CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n" \
  "EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n" \
  "PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\n" \
  "PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\n" \
  "LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n" \
  "NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n" \
  "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n" \
  "*/\n" \
  "\n" \
  "\n" \
  "//---------------------------------------------------------------------------//\n" \
  "//  Evidyon, a 3d multiplayer online role-playing game                       //\n" \
  "//  Copyright (c) 2008, 2009, 2010 Karl Gluck                                //\n" \
  "//                                                                           //\n" \
  "//  Evidyon is free software: you can redistribute it and/or modify          //\n" \
  "//  it under the terms of the GNU General Public License as published by     //\n" \
  "//  the Free Software Foundation, either version 3 of the License, or        //\n" \
  "//  (at your option) any later version.                                      //\n" \
  "//                                                                           //\n" \
  "//  Evidyon is distributed in the hope that it will be useful,               //\n" \
  "//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //\n" \
  "//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //\n" \
  "//  GNU General Public License for more details.                             //\n" \
  "//                                                                           //\n" \
  "//  You should have received a copy of the GNU General Public License        //\n" \
  "//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //\n" \
  "//                                                                           //\n" \
  "//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //\n" \
  "//---------------------------------------------------------------------------//\n";