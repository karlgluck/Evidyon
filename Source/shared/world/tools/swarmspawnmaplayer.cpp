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
#include "swarmspawnmaplayer.h"
#include "maplocationdata.h"
#include "shared/actor/tools/actorprofile.h"
#include <assert.h>
#include "common/randf.h"


namespace Evidyon {
namespace World {
namespace Tools {



dc::dcClass<SwarmSpawnMapLayer>::Implements<MapLayer> implements_map_layer_;

  
//----[  SwarmSpawnMapLayer  ]-------------------------------------------------
SwarmSpawnMapLayer::SwarmSpawnMapLayer() {
  member("Mask Area", &mask_area_);
  member("Swarm Spawns", &swarm_spawns_);
  member("Clear Existing", &clear_existing_);
  member("Number to Add", &number_to_add_);
  number_to_add_.setValue(1);
}



//----[  SwarmSpawnMapLayerCompliationFunctionoid  ]---------------------------
class SwarmSpawnMapLayerCompliationFunctionoid
  : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    int location_level = location->this_layer.spawn_intended_level;
    int number_of_spawns
      = clear_existing ? 0 : location->this_layer.number_of_spawns;
    int next_spawn_index = number_of_spawns;
    int number_added = 0;
    int random_offset = number_of_spawn_data > 0 ? (rand()%number_of_spawn_data) : 0;
    for (int i = 0; i < number_of_spawn_data; ++i) {
      int index = (i + random_offset) % number_of_spawn_data; // mixes up the order
      if (randf() > data[index].chance_of_appearing) continue;
      if (location_level < data[index].lowest_level ||
          location_level > data[index].highest_level) continue;
      location->this_layer.spawn[next_spawn_index]
        = data[index].lifeform_ai[rand()%SwarmSpawn::Data::NUMBER_OF_LIFEFORM_AI];
      if (number_of_spawns < MapLocationData::NUMBER_OF_SPAWNS) {
        ++number_of_spawns;
        next_spawn_index = number_of_spawns;
      } else {
        next_spawn_index
          = (next_spawn_index + 1) % MapLocationData::NUMBER_OF_SPAWNS;
      }
      if (++number_added >= number_to_add) break;
    }
    location->this_layer.number_of_spawns = number_of_spawns;
    return true;
  }

  // the buffer is filled with up to this many lifeforms
  static const int MAXIMUM_NUMBER_OF_SPAWN_DATA = 32;

  bool clear_existing;
  SwarmSpawn::Data data[MAXIMUM_NUMBER_OF_SPAWN_DATA];
  size_t number_of_spawn_data;
  int number_to_add;
};



//----[  compile  ]------------------------------------------------------------
void SwarmSpawnMapLayer::compile(
    const RECT* area,
    MapCompilationContext* compilation_context) {
  SwarmSpawnMapLayerCompliationFunctionoid functionoid;
  functionoid.clear_existing = clear_existing_.getValue();
  functionoid.number_to_add = number_to_add_.getValue();

  const dc::dcGenericResource::Array& swarm_spawns
    = swarm_spawns_.getMembers();
  functionoid.number_of_spawn_data = swarm_spawns.size();
  typedef dc::dcList<SwarmSpawn>::Element Element;
  for (int i = 0; i < functionoid.number_of_spawn_data; ++i) {
    Element* element = (Element*)(swarm_spawns.at(i));
    assert(element);
    element->compile(&functionoid.data[i]);
  }
  mask_area_.processMaskLocations(getOwner()->getIndex(),
                                  area,
                                  compilation_context,
                                  &functionoid);
}



//----[  setArea  ]------------------------------------------------------------
void SwarmSpawnMapLayer::setArea(
    dc::dcList<MapMask>::Element* map_mask,
    D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}




}
}
}