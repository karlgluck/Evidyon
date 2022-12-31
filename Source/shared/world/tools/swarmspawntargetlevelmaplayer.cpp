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

