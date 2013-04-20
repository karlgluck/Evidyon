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
#include "mapmaskcolordata.h"


namespace Evidyon {
namespace World {
namespace Tools {



  
//----[  MapMaskColorData  ]---------------------------------------------------
MapMaskColorData::MapMaskColorData() {
  member("Navigability", &navigability_);
  member("Color", &color_);
  member("Monster Swarm - Minimum Level", &monster_swarm_level_min_);
  member("Monster Swarm - Maximum Level", &monster_swarm_level_max_);
  member("Monster Swarms", &monster_swarms_);
  member("Zone", &zone_);
  navigability_.setValue(NAVIGABILITY_INVALID);
  monster_swarm_level_min_.setValue(-1);
  monster_swarm_level_max_.setValue(-1);
}




//----[  getColor  ]-----------------------------------------------------------
dcx::dcxD3DColor* MapMaskColorData::getColor() {
  return &color_;
}



//----[  getD3DColor  ]--------------------------------------------------------
D3DCOLOR MapMaskColorData::getD3DColor() const {
  return color_.getD3DColor();
}



//----[  getNavigability  ]----------------------------------------------------
bool MapMaskColorData::getNavigability(Navigability* navigability) const {
  *navigability = navigability_.getValue();
  return (*navigability) != NAVIGABILITY_INVALID;
}




//----[  getSwarmLevel  ]------------------------------------------------------
bool MapMaskColorData::getSwarmLevel(int* min_level, int* level_range) const {
  int values[] = {
    monster_swarm_level_min_.getValue(),
    monster_swarm_level_max_.getValue(),
  };
  if (values[0] < 1 || values[1] < values[0]) return false;
  *min_level = values[0];
  *level_range = 1 + values[1] - values[0];
  return true;
}




//----[  getSwarms  ]----------------------------------------------------------
size_t MapMaskColorData::getSwarms(SwarmSpawn::Data* swarm_spawns, size_t size) const {
  const dc::dcGenericResource::Array& members = monster_swarms_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  size_t index = 0;
  for (i = members.begin(); i != members.end() && index < size; ++i, ++index) {
    dc::dcList<SwarmSpawn>::Element* element
      = (dc::dcList<SwarmSpawn>::Element*)(*i);
    element->compile(&swarm_spawns[index]);
  }
  return index;
}


//----[  getZone  ]------------------------------------------------------------
bool MapMaskColorData::getZone(Zone::ZoneIndex* zone) const {
  *zone = zone_.getReferencedResourceIndex();
  return (*zone) != Zone::INVALID_ZONE_INDEX;
}



}
}
}

