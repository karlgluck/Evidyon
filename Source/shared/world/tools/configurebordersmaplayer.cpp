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
#include "configurebordersmaplayer.h"
#include "shared/trigger/trigger.h"
#include "shared/world/tools/maplocationdata.h"
#include "shared/world/tools/mapcompilationcontext.h"


namespace Evidyon {
namespace World {
namespace Tools {




dc::dcClass<ConfigureBordersMapLayer>::Implements<MapLayer> ConfigureBordersMapLayer::implements_map_layer_;

  
//----[  ConfigureBordersMapLayer  ]------------------------------------------------------
ConfigureBordersMapLayer::ConfigureBordersMapLayer() {
  member("Border Zone", &border_zone_);
  member("Mask Area", &mask_area_);
  member("Border Effect Scenery", &chaos_border_effect_scenery_);
  Evidyon::Tools::SceneryReference chaos_border_effect_scenery_;
}




//----[  ConfigureBordersMapLayerCompliationPreprocessorFunctionoid  ]---------
class ConfigureBordersMapLayerCompliationPreprocessorFunctionoid
    : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    // just touch this map element so that the layer index is set
    location->this_layer.zone = zone;
    location->this_layer.navigability = NAVIGABILITY_IMPASSABLE;
    return true;
  }
  Zone::ZoneIndex zone;
};


//----[  ConfigureBordersMapLayerCompliationFunctionoid  ]---------------------
class ConfigureBordersMapLayerCompliationFunctionoid
    : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    // count neighbors not in this layer
    float average_height = 0.0f;
    int number_nearby_not_in_layer = 0;
#define CHECK(a)\
    number_nearby_not_in_layer +=\
      (location->neighbors_nesw[a]\
        ->this_layer.last_modified_by.map_layer_index == map_layer_index) ? 0 : 1;
#define CHECK2(a,b)\
    number_nearby_not_in_layer +=\
      (location->neighbors_nesw[a]\
        ->neighbors_nesw[b]\
        ->this_layer.last_modified_by.map_layer_index == map_layer_index) ? 0 : 1;

    CHECK(0);
    CHECK(1);
    CHECK(2);
    CHECK(3);
    CHECK2(0,3);
    CHECK2(0,1);
    CHECK2(2,1);
    CHECK2(2,3);

    if (number_nearby_not_in_layer == 0) {
      location->this_layer.visual_data.occluding = true;
    } else {
      location->this_layer.visual_data.scenery = scenery;
    }

    return true;
  }
  size_t map_layer_index;
  Scenery::SceneryIndex scenery;
};


//----[  compile  ]------------------------------------------------------------
void ConfigureBordersMapLayer::compile(const RECT* area,
                                       MapCompilationContext* compilation_context) {
  size_t this_layer_index = getOwner()->getIndex();

  // This is necessary to make the borders work
  compilation_context
    ->default_location
     .this_layer
     .last_modified_by
     .map_layer_index = this_layer_index;

  // first, touch everything we're going to change with the preprocessor
  ConfigureBordersMapLayerCompliationPreprocessorFunctionoid preprocessor;
  preprocessor.zone = border_zone_.getReferencedResourceIndex();
  mask_area_.processMaskLocations(this_layer_index,
                                  area,
                                  compilation_context,
                                  &preprocessor);

  // next, apply the effect
  ConfigureBordersMapLayerCompliationFunctionoid compiler;
  compiler.map_layer_index = this_layer_index;
  compiler.scenery = chaos_border_effect_scenery_.getReferencedResourceIndex();
  mask_area_.processMaskLocations(this_layer_index,
                                  area,
                                  compilation_context,
                                  &compiler);
}



//----[  ConfigureBordersMapLayer  ]-------------------------------------------
void ConfigureBordersMapLayer::brushPreview(
    MapMask* mask,
    D3DCOLOR color,
    LocationVisualData* visual_data) {
  if (mask_area_.references(mask, color)) {
    visual_data->scenery = chaos_border_effect_scenery_.getReferencedResourceIndex();
  }
}



//----[  setArea  ]------------------------------------------------------------
void ConfigureBordersMapLayer::setArea(dc::dcList<MapMask>::Element* map_mask,
                                       D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}


}
}
}

