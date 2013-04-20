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
#include "clientfacade.h"
#include "stdafx.h"
#include "client/evidyonclient.h"
#include "common/isometriccamera.h"
#include "shared/geosid/geosidclientdescription.h"
#include "shared/geosid/geosidlimits.h"

namespace Evidyon {
namespace Client {

struct GeosidFinder {

  bool isClickable(Geosid::GeosidClientDescription* geosid) {
    is_white_geosid = true; // sacrifice to all geosids //geosid->type == Geosid::GEOSID_TOWN_WHITE;
    index = geosid->index;
    float dx = geosid->x - x;
    float dz = geosid->z - z;
    return (map == geosid->map) && (dx*dx+dz*dz) < 1.5f;
  }

  bool findClickable(EvidyonClient* client, Geosid::GeosidIndex* geosid, bool* is_white) {
    map = client->getCurrentMap();
    if (client
          ->getGeosids()
          ->forEachUntilTrue(this, &GeosidFinder::isClickable)) {
      assert(geosid);
      if (geosid) *geosid = index;
      if (is_white) *is_white = is_white_geosid;
      return true;
    } else {
      return false;
    }
  }

  bool isInside(Geosid::GeosidClientDescription* geosid) {
    is_white_geosid = geosid->type == Geosid::GEOSID_TOWN_WHITE;
    index = geosid->index;
    float dx = geosid->x - x;
    float dz = geosid->z - z;
    return (map == geosid->map) && (dx*dx+dz*dz) < Geosid::GEOSID_RANGE_SQ;
  }

  bool findInside(EvidyonClient* client, Geosid::GeosidIndex* geosid, bool* is_white) {
    map = client->getCurrentMap();
    if (client
          ->getGeosids()
          ->forEachUntilTrue(this, &GeosidFinder::isInside)) {
      if (geosid) *geosid = index;
      if (is_white) *is_white = true; // all are "white" (sacrifice)
      return true;
    } else {
      return false;
    }
  }
  Geosid::GeosidIndex index;
  bool is_white_geosid;
  World::MapIndex map;
  float x, z;
};
  
//----[  ClientFacade  ]-------------------------------------------------------
ClientFacade::ClientFacade(EvidyonClient* client) 
  : client_(client) {
}



//----[  screenPtToWorldPt  ]--------------------------------------------------
void ClientFacade::screenPtToWorldPt(float screen_x,
                                     float screen_y,
                                     float* world_x,
                                     float* world_z) {
  client_->getCamera()->getWorldLocation(screen_x, screen_y, world_x, world_z);
}




//----[  findClickableGeosid  ]------------------------------------------------
bool ClientFacade::findClickableGeosid(float world_x,
                                       float world_z,
                                       Geosid::GeosidIndex* geosid,
                                       bool* is_white) {
  GeosidFinder finder;
  finder.x = world_x;
  finder.z = world_z;
  return finder.findClickable(client_, geosid, is_white);
}



//----[  isClientInsideGeosid  ]-----------------------------------------------
bool ClientFacade::isClientInsideGeosid(Geosid::GeosidIndex* geosid, bool* is_white) {
  Actor::Client::ClientActorInstance* actor = client_->getClientActor();
  confirm(actor) else return false;
  GeosidFinder finder;
  actor->getPosition(&finder.x, &finder.z);
  return finder.findInside(client_, geosid, is_white);
}



//----[  isVisible  ]----------------------------------------------------------
bool ClientFacade::isVisible(World::MapIndex map,
                             float world_x,
                             float world_z) const {
  Actor::Client::ClientActorInstance* actor = client_->getClientActor();
  confirm(actor) else return false;
  float x, z;
  actor->getPosition(&x, &z);
  x -= world_x;
  z -= world_z;

  return (map == client_->getCurrentMap()) &&
          ((x*x + z*z) < World::VISIBLE_RADIUS_SQ);
}



//----[  getGeosidByName  ]----------------------------------------------------
bool ClientFacade::getGeosidByName(const char* name, Geosid::GeosidIndex* geosid) {
  struct Matcher {
    bool matches(Geosid::GeosidClientDescription* description) {
      *index = description->index;
      return 0 == stricmp(description->name, name);
    }
    const char* name;
    Geosid::GeosidIndex* index;
  };
  Matcher matcher;
  matcher.name = name;
  matcher.index = geosid;
  return client_->getGeosids()->forEachUntilTrue(&matcher, &Matcher::matches);
}

}
}

