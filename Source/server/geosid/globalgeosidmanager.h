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
#ifndef __SERVER_GEOSID_GLOBALGEOSIDMANAGER_H__
#define __SERVER_GEOSID_GLOBALGEOSIDMANAGER_H__
#pragma once

#include "geosidinstance.h"
#include "shared/geosid/geosidhashid.h"
#include "shared/geosid/geosidindex.h"
#include "shared/world/mapindex.h"
#include "common/kg/kg.h"
#include "common/kg/singleton.h"
#include "shared/networkdecl.h"

namespace Evidyon {
class ActorInstance;
namespace Server {
class GuildInstance;
}
}


namespace Evidyon {
namespace Server {


//----[  GlobalGeosidManager  ]------------------------------------------------
class GlobalGeosidManager : public kg::Singleton<GlobalGeosidManager> {
public:
  GlobalGeosidManager();

  // Initializes this manager
  void create(GlobalDatabaseManager* database);
  
  // Frees internal memory
  void destroy();

  // Updates all geosid information in the database
  bool store();

  // Used to load geosids from the game file.  The manager must be created
  // first.
  void initNumberOfGeosids(size_t count);
  void initGeosid(const Geosid::GeosidServerDescription* description);
  size_t getNumberOfGeosids() const;

  // Obtain the geosid with the given hash type
  GeosidInstance* getGeosidByHash(Geosid::GeosidHashID geosid_hash);
  GeosidInstance* getGeosidByIndex(Geosid::GeosidIndex geosid_index);
  
  // Obtains the geosid at the given index if the player is within range of it
  GeosidInstance* getGeosidIfInRange(Geosid::GeosidIndex geosid,
                                     World::MapIndex map,
                                     Number x,
                                     Number z);

  // When the first member of a guild logs on, this method is called to add
  // the instance pointer to all of the active geosids that the guild owns,
  // and add all of the geosids that guild owns to the instance.
  void guildInstanceRetrieved(GuildInstance* guild);

  // Invoke this method when the last member of a guild logs off so that all
  // geosids can remove references to the guild instance.
  void guildInstanceDestroyed(GuildInstance* guild);

  NetworkPacket* getTellGeosidOwnerDataPacket(Geosid::GeosidIndex geosid_index,
                                              ActorInstance* requesting_actor);

  NetworkPacket* getTellGeosidOwnerDataPacket(GeosidInstance* geosid);

private:
  void zero();

  // Writes the instance to the database
  bool storeGeosidInstance(GeosidInstance* instance);

private:
  GlobalDatabaseManager* database_;
  kg::AllocatedArray<GeosidInstance> geosids_;
  kg::HashToIndexTranslator<Geosid::GeosidHashID,Geosid::GeosidIndex> translator_;
};



}
}

#endif