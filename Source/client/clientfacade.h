//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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