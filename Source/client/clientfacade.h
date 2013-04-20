//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __CLIENT_CLIENTFACADE_H__
#define __CLIENT_CLIENTFACADE_H__
#pragma once

#include "common/kg/singleton.h"
#include "shared/geosid/geosidindex.h"
#include "shared/world/mapindex.h"

class EvidyonClient;

namespace Evidyon {
namespace Client {
  
//----[  ClientFacade  ]-------------------------------------------------------
// Provides a global interface to the Evidyon client that can be used anywhere
// in the program to access important, context-sensitive functionality without
// needing pointers to every friggin' thing in the game.
//  ex. action targeter needs to know if the client is near a geosid,
//      which actors are near a clicked point and what the world-point is for
//      a given screen coordinate.
class ClientFacade : public kg::Singleton<ClientFacade> {
public:
  ClientFacade(EvidyonClient* client);

  void screenPtToWorldPt(float screen_x, float screen_y,
                         float* world_x, float* world_z);

  bool findClickableGeosid(float world_x,
                           float world_z,
                           Geosid::GeosidIndex* geosid,
                           bool* is_white);

  bool isClientInsideGeosid(Geosid::GeosidIndex* geosid, bool* is_white);

  bool isVisible(World::MapIndex, float world_x, float world_z) const;

  bool getGeosidByName(const char* name, Geosid::GeosidIndex* geosid);

private:
  EvidyonClient* client_;
};

}
}

#endif