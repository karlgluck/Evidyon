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