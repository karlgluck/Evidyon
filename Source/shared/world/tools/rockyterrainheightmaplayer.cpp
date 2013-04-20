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
#include "rockyterrainheightmaplayer.h"
#include "maplocationdata.h"
#include "common/randf.h"


namespace Evidyon {
namespace World {
namespace Tools {



dc::dcClass<RockyTerrainHeightMapLayer>::Implements<MapLayer> RockyTerrainHeightMapLayer::implements_map_layer_;

  
//----[  RockyTerrainHeightMapLayer  ]-----------------------------------------
RockyTerrainHeightMapLayer::RockyTerrainHeightMapLayer() {
  member("Mask Area", &mask_area_);
  member("Rocky Terrain Height", &height_);
}




//----[  RockyTerrainHeightMapLayerCompliationPreprocessorFunctionoid  ]-------
class RockyTerrainHeightMapLayerCompliationPreprocessorFunctionoid
    : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    // just touch this map element so that the layer index is set
    return true;
  }
};




//----[  RockyTerrainHeightMapLayerCompliationFunctionoid  ]-------------------
class RockyTerrainHeightMapLayerCompliationFunctionoid
    : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    // count neighbors not in this layer
    float average_height = 0.0f;
    int number_nearby_not_in_layer = 0;
#define CHECK(a)\
    average_height += location->neighbors_nesw[a]\
          ->last_layer.visual_data.terrain_height;\
    number_nearby_not_in_layer +=\
      (location->neighbors_nesw[a]\
        ->this_layer.last_modified_by.map_layer_index == map_layer_index) ? 0 : 1;
#define CHECK2(a,b)\
    average_height += location->neighbors_nesw[a]\
          ->neighbors_nesw[b]\
          ->last_layer.visual_data.terrain_height;\
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

    static const float height_modifications[8] = {
      2.0f / 2.0f,
      2.0f / 3.0f,
      2.0f / 4.0f,
      2.0f / 5.0f,
      2.0f / 5.0f,
      2.0f / 5.0f,
      2.0f / 6.0f,
      2.0f / 6.0f,
    };

    average_height = 0.0f; // TODO: this creates problems for some reason!
    float range = height - average_height;
    range *= randf() * height_modifications[number_nearby_not_in_layer];

    location->this_layer.visual_data.terrain_height = range + average_height;

    return true;
  }
  float height;
  size_t map_layer_index;
};



//----[  compile  ]------------------------------------------------------------
void RockyTerrainHeightMapLayer::compile(
    const RECT* area,
    MapCompilationContext* compilation_context) {

  size_t my_map_layer_index = getOwner()->getIndex();

  // first, touch everything we're going to change with the preprocessor
  RockyTerrainHeightMapLayerCompliationPreprocessorFunctionoid preprocessor;
  mask_area_.processMaskLocations(my_map_layer_index,
                                  area,
                                  compilation_context,
                                  &preprocessor);

  // go back and do the mountain-height calculations
  RockyTerrainHeightMapLayerCompliationFunctionoid functionoid;
  functionoid.height = height_.getValue();
  functionoid.map_layer_index = my_map_layer_index;
  mask_area_.processMaskLocations(my_map_layer_index,
                                  area,
                                  compilation_context,
                                  &functionoid);
}


//----[  brushPreview  ]-------------------------------------------------------
void RockyTerrainHeightMapLayer::brushPreview(
    MapMask* mask,
    D3DCOLOR color,
    LocationVisualData* visual_data) {
  if (mask_area_.references(mask, color)) {
    visual_data->terrain_height = randf() * height_.getValue();
  }
}


//----[  setArea  ]------------------------------------------------------------
void RockyTerrainHeightMapLayer::setArea(dc::dcList<MapMask>::Element* map_mask,
                                         D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}



}
}
}