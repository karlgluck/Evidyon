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
#ifndef __COMPLETE_EDITOR_GAMEFILE_SERVERGAMEFILEMAPREGIONDATA_H__
#define __COMPLETE_EDITOR_GAMEFILE_SERVERGAMEFILEMAPREGIONDATA_H__
#pragma once


#include "shared/avatar/avatarlevel.h"
#include "shared/lifeformai/lifeformaiindex.h"
#include "shared/lifeformai/spawnpointindex.h"


namespace Evidyon {
namespace GameFile {

//----[  ServerGameFileMapRegionData  ]----------------------------------------
struct ServerGameFileMapRegionData {
  static const int NUMBER_OF_SWARM_SPAWNS = 16;
  static const int NUMBER_OF_SPAWN_LOCATIONS = 32;
  static const int NUMBER_OF_INHABITANTS = 8;

  // The whole swarm spawn array is filled with data.  If there are not
  // enough types to fill the array, entries are repeated.
  struct SwarmSpawn {
    LifeformAI::LifeformAIIndex lifeform_ai;
  } swarm_spawn[NUMBER_OF_SWARM_SPAWNS];

  struct SpawnLocation {
    Avatar::AvatarLevel intended_level;
    LifeformAI::SpawnPointIndex spawn_point;
    unsigned int swarms_mask;
    unsigned char x_subcoordinate, y_subcoordinate;
  } spawn_locations[NUMBER_OF_SPAWN_LOCATIONS];

  struct Inhabitant {
    LifeformAI::SpawnPointIndex spawn_point;
    LifeformAI::LifeformAIIndex lifeform_ai;
    unsigned char x_subcoordinate, y_subcoordinate;
  } inhabitants[NUMBER_OF_INHABITANTS];
};


}
}

#endif