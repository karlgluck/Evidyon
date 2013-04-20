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
#include "wallmaplayer.h"
#include "maplocationdata.h"


namespace Evidyon {
namespace World {
namespace Tools {


dc::dcClass<WallMapLayer>::Implements<MapLayer> WallMapLayer::implements_map_layer_;

  
//----[  WallMapLayer  ]-------------------------------------------------------
WallMapLayer::WallMapLayer() {
  member("Mask Area", &mask_area_);
  member("Wall Texture", &texture_);
  member("Wall Type", &wall_type_);
}

//----[  WallMapLayerCompliationFunctionoid  ]--------------------------------
class WallMapLayerCompliationFunctionoid : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    location->this_layer.visual_data.wall_texture = wall_texture;
    location->this_layer.visual_data.wall_type = wall_type;
    return true;
  }
  Texture::TextureIndex wall_texture;
  WallType wall_type;
};



//----[  compile  ]------------------------------------------------------------
void WallMapLayer::compile(const RECT* area,
                            MapCompilationContext* compilation_context) {
  WallMapLayerCompliationFunctionoid functionoid;
  functionoid.wall_texture = texture_.getReferencedResourceIndex();
  functionoid.wall_type = wall_type_.getValue();
  mask_area_.processMaskLocations(getOwner()->getIndex(),
                                  area,
                                  compilation_context,
                                  &functionoid);
}


//----[  brushPreview  ]-------------------------------------------------------
void WallMapLayer::brushPreview(MapMask* mask,
                                D3DCOLOR color,
                                LocationVisualData* visual_data) {
  if (mask_area_.references(mask, color)) {
    visual_data->wall_texture = texture_.getReferencedResourceIndex();
    visual_data->wall_type = wall_type_.getValue();
  }
}



//----[  setArea  ]------------------------------------------------------------
void WallMapLayer::setArea(dc::dcList<MapMask>::Element* map_mask,
                           D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}


}
}
}