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
#include "terrainheightmaplayer.h"
#include "maplocationdata.h"


namespace Evidyon {
namespace World {
namespace Tools {



dc::dcClass<TerrainHeightMapLayer>::Implements<MapLayer>
  TerrainHeightMapLayer:: implements_map_layer_;



  
//----[  TerrainHeightMapLayer  ]------------------------------------------------------
TerrainHeightMapLayer::TerrainHeightMapLayer() {
  member("Mask Area", &mask_area_);
  member("Terrain Height", &height_);
}




//----[  TerrainHeightMapLayerCompliationFunctionoid  ]--------------------------------
class TerrainHeightMapLayerCompliationFunctionoid : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    location->this_layer.visual_data.terrain_height = height;
    return true;
  }
  float height;
};



//----[  compile  ]------------------------------------------------------------
void TerrainHeightMapLayer::compile(const RECT* area,
                            MapCompilationContext* compilation_context) {
  TerrainHeightMapLayerCompliationFunctionoid functionoid;
  functionoid.height = height_.getValue();
  mask_area_.processMaskLocations(getOwner()->getIndex(),
                                  area,
                                  compilation_context,
                                  &functionoid);
}



//----[  brushPreview  ]-------------------------------------------------------
void TerrainHeightMapLayer::brushPreview(MapMask* mask,
                                         D3DCOLOR color,
                                         LocationVisualData* visual_data) {
  if (mask_area_.references(mask, color)) {
    visual_data->terrain_height = height_.getValue();
  }
}


//----[  setArea  ]------------------------------------------------------------
void TerrainHeightMapLayer::setArea(dc::dcList<MapMask>::Element* map_mask,
                                    D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}


}
}
}