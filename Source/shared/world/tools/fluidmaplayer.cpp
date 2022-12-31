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