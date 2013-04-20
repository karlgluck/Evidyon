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
#include "swarmspawntargetlevelmaplayer.h"
#include "maplocationdata.h"


namespace Evidyon {
namespace World {
namespace Tools {



dc::dcClass<SwarmSpawnTargetLevelMapLayer>::Implements<MapLayer>
  SwarmSpawnTargetLevelMapLayer::implements_map_layer_;



//----[  SwarmSpawnTargetLevelMapLayer  ]--------------------------------------
SwarmSpawnTargetLevelMapLayer::SwarmSpawnTargetLevelMapLayer() {
  member("Mask Area", &mask_area_);
  member("Target Level", &intended_level_);
}




//----[  SwarmSpawnTargetLevelMapLayerCompilationFunctionoid  ]----------------
class SwarmSpawnTargetLevelMapLayerCompilationFunctionoid
    : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    location->this_layer.spawn_intended_level = intended_level;
    return true;
  }
  int intended_level;
};


//----[  compile  ]------------------------------------------------------------
void SwarmSpawnTargetLevelMapLayer::compile(const RECT* area,
                            MapCompilationContext* compilation_context) {
  SwarmSpawnTargetLevelMapLayerCompilationFunctionoid functionoid;
  functionoid.intended_level = intended_level_.getValue();
  mask_area_.processMaskLocations(getOwner()->getIndex(),
                                  area,
                                  compilation_context,
                                  &functionoid);
}


//----[  setArea  ]------------------------------------------------------------
void SwarmSpawnTargetLevelMapLayer::setArea(
    dc::dcList<MapMask>::Element* map_mask,
    D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}



}
}
}

