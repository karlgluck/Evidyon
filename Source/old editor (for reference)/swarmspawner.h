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
#ifndef __SWARMSPAWNER_H__
#define __SWARMSPAWNER_H__
#pragma once


#include "mapcolormask.h"
#include "swarmspawn.h"
#include "spawndensity.h"
#include "spawnlevel.h"
#include <d3dx9.h>

namespace Evidyon {

struct CompiledRegionSwarmSpawn;
struct CompiledSwarmSpawnLocationInfo;
extern const float SWARM_SPAWN_PERIOD;

}

class SwarmSpawner : public dc::dcResource<SwarmSpawner> {
public:
  SwarmSpawner();
  bool compileForServer(LPDIRECT3DDEVICE9 d3d_device,
                        int map_regions_wide,
                        int map_regions_high,
                        int* monster_level_rarity_boost,
                        dc::dcStreamOut* stream);

public:
  static std::string staticTypeName();

private:
  dc::dcList<MapColorMask<SwarmSpawn>> spawn_;
  MapColorMask<SpawnLevel> levels_;
  MapColorMask<SpawnDensity> density_;
};


#endif