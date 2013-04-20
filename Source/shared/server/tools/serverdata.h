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
#ifndef __SHARED_SERVER_TOOLS_SERVERDATA_H__
#define __SHARED_SERVER_TOOLS_SERVERDATA_H__
#pragma once

#include "shared/world/tools/maplocationreference.h"
#include "shared/scenery/tools/scenery.h"
#include "shared/scenery/sceneryindex.h"

namespace Evidyon {
namespace Server {
namespace Tools {


  
//----[  ServerData  ]---------------------------------------------------------
class ServerData : public dc::dcResource<ServerData> {
public:
  static std::string staticTypeName() { return "ServerData"; }
public:
  ServerData();

  // for compilation
  World::Tools::MapLocationReference* getGoodSpawnPoint() { return &good_spawn_point_; }
  World::Tools::MapLocationReference* getEvilSpawnPoint() { return &evil_spawn_point_; }
  Scenery::SceneryIndex getGoldScenery() { return gold_scenery_.getReferencedResourceIndex(); }

private:
  World::Tools::MapLocationReference good_spawn_point_, evil_spawn_point_;
  Evidyon::Tools::SceneryReference gold_scenery_;
};




}
}
}

#endif