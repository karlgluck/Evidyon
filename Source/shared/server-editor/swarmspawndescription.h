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
#ifndef __SWARMSPAWNDESCRIPTION_H__
#define __SWARMSPAWNDESCRIPTION_H__
#pragma once



namespace Evidyon {

// Always <= 32 because there are only 32 bits in the spawn location mask
static const unsigned int MAX_SWARM_SPAWNS_PER_WORLD_REGION = 32;

struct CompiledRegionSwarmSpawn {
  int ai_lifeform_type;

  // This is based on the average level of monsters in this region.  It has
  // N paired bits (N <= 16) in the high and low WORDs set to 0.  The random
  // treasure generation value is ANDed with this mask before checking to
  // see how many bits match.
  // By setting N higher, treasure is more more valuable on average.
  unsigned int treasure_randomizer_mask;

  // count from 0->activate period, then:
  //  if spawn counter >= 0, decrement spawn counter by granularity
  //  if spawn counter < 0
  //      spawn a monster
  //      spawn counter++
  //      if spawn counter == 0, spawn counter = spawn_counter_reset_value
  unsigned int spawn_counter_activate_period;
  unsigned int spawn_counter_granularity;
  unsigned int spawn_counter_reset_value;

  // bit set to 1 if this swarm type can create an actor at
  // the corresponding location
  unsigned int spawn_location_mask;
};


struct CompiledSwarmSpawnLocationInfo {
  int x, y; // location at which to spawn
  int level;// level of monster to spawn here
};


extern const float SWARM_SPAWN_PERIOD;

}




#endif

