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

