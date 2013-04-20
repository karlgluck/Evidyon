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
#ifndef __CLIENT_CLIENTMAPDATA_H__
#define __CLIENT_CLIENTMAPDATA_H__
#pragma once

#include "shared/world/maprendererdatasource.h"
#include "shared/gamefile/clientgamefilemaplocationdata.h"
#include "shared/zone/zoneindex.h"

namespace Evidyon {
namespace Zone {
struct Zone;
}
}

namespace Evidyon {
namespace Client {



//----[  ClientMapData  ]------------------------------------------------------
class ClientMapData : public World::MapRendererDataSource {
  struct Location {
    GameFile::ClientGameFileMapLocationData* data;
  };

public:
  ClientMapData();
  ~ClientMapData();

  // Creates an internal copy of the location table, mapping each location's
  // index to a unique location.  This method returns 'false' if it fails.
  // No internal copy of location_table is made.
  bool create(size_t width,
              size_t height,
              const void* location_table,
              size_t location_byte_size,
              const GameFile::ClientGameFileMapLocationData* unique_locations,
              size_t number_of_unique_locations);

  // Frees memory
  void destroy();

  // Retrieves data from a map location
  Evidyon::Zone::ZoneIndex getZone(int x, int z) const;
  Evidyon::World::Navigability getNavigability(int x, int z) const;

  // Returns a value between 0 and 1 indicating how many of the currently
  // visible tiles belong to a chaos zone.
  float getChaosLevel(int x,
                      int z,
                      const Zone::Zone* zones) const;

  // Returns 'true' if the client should change zones, and fills *new_zone
  // with that zone's index.
  bool changeZone(int x,
                  int z,
                  Zone::ZoneIndex current_zone,
                  Zone::ZoneIndex* new_zone);


// implement MapRendererDataSource
public:
  virtual void updateMapTileData(int x,
                                 int y,
                                 Evidyon::World::LocationVisualData* data);

private:
  const GameFile::ClientGameFileMapLocationData* atOrDefault(int x, int z) const;
  void zero();

private:
  GameFile::ClientGameFileMapLocationData default_location_;
  GameFile::ClientGameFileMapLocationData* unique_locations_;
  size_t number_of_unique_locations_;
  size_t width_, height_;
  Location* locations_;
};

}
}

#endif