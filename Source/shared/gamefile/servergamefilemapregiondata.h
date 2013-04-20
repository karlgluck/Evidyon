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