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
#ifndef __MAPLOCATIONDATA_H__
#define __MAPLOCATIONDATA_H__
#pragma once



#include "shared/world/locationvisualdata.h"
#include "maplayerblendingtype.h"
#include "shared/world/navigability.h"
#include "shared/lifeformai/lifeformaiindex.h"
#include "shared/trigger/triggerindex.h"
#include "shared/zone/zoneindex.h"

namespace Evidyon {
namespace World {
namespace Tools {


//----[  MapLocationData  ]----------------------------------------------------
struct MapLocationData {
  enum SpawnType {
    SPAWN_SWARM,
    SPAWN_INHABITANT,
    INVALID_SPAWN_TYPE,
  };

  // How many swarm spawn types there are per location
  static const int NUMBER_OF_SPAWNS = 8;

  // During construction, this_layer is copied to last_layer after every layer
  // completes its modifications.  This prevents layers that base their behavior
  // on previous layers from inadvertently messing themselves up.
  struct {

    // Information about this location
    LocationVisualData visual_data;
    Navigability navigability;
    LifeformAI::LifeformAIIndex spawn[NUMBER_OF_SPAWNS];
    int number_of_spawns;
    SpawnType spawn_type;
    int spawn_intended_level; // monsters are spawned for avatars of this level
    Evidyon::Zone::ZoneIndex zone; // defines global properties of this area
    Evidyon::Trigger::TriggerIndex trigger;
    
    // Stores data about the last layer to change this location.  This is used
    // by blending layers.
    struct {
      size_t map_layer_index;
    } last_modified_by;

  } last_layer, this_layer;

  // During construction, these are initialized to the north/east/south/west
  // neighbors of this location on the map to facilitate blending operations.
  // These pointers are ALWAYS valid--even on the edges of the map.  Edges will
  // all point to a default location that is entirely filled with invalid
  // attributes.  For the default location, all of these pointers just point
  // back to the structure itself.
  const MapLocationData* neighbors_nesw[4];

  // Returns the blending type that should be applied to this location, if any.
  // This method checks the square of 9 locations centered on this one to see
  // if they match one of the defined blending types.
  MapLayerBlendingType getLayerBlendingType(size_t inner_map_layer_index,
                                            size_t outer_map_layer_index);
};

}
}
}

#endif