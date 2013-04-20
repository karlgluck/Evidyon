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
#include "fluidmaplayer.h"
#include "maplocationdata.h"

namespace Evidyon {
namespace World {
namespace Tools {



dc::dcClass<FluidMapLayer>::Implements<MapLayer> FluidMapLayer::implements_map_layer_;

  
//----[  FluidMapLayer  ]------------------------------------------------------
FluidMapLayer::FluidMapLayer() {
  member("Mask Area", &mask_area_);
  member("Fluid Texture", &texture_);
}




//----[  FluidMapLayerCompliationFunctionoid  ]--------------------------------
class FluidMapLayerCompliationFunctionoid : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    location->this_layer.visual_data.fluid_texture = fluid_texture;
    return true;
  }
  Texture::TextureIndex fluid_texture;
};



//----[  compile  ]------------------------------------------------------------
void FluidMapLayer::compile(const RECT* area,
                            MapCompilationContext* compilation_context) {
  FluidMapLayerCompliationFunctionoid functionoid;
  functionoid.fluid_texture = texture_.getReferencedResourceIndex();
  mask_area_.processMaskLocations(getOwner()->getIndex(),
                                  area,
                                  compilation_context,
                                  &functionoid);
}



//----[  brushPreview  ]-------------------------------------------------------
void FluidMapLayer::brushPreview(MapMask* mask,
                                 D3DCOLOR color,
                                 LocationVisualData* visual_data) {
  if (mask_area_.references(mask, color)) {
    visual_data->fluid_texture = texture_.getReferencedResourceIndex();
  }
}



//----[  setArea  ]------------------------------------------------------------
void FluidMapLayer::setArea(dc::dcList<MapMask>::Element* map_mask,
                            D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}





}
}
}