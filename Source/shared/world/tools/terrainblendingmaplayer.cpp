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
#include "terrainblendingmaplayer.h"
#include "maplocationdata.h"
#include "mapcompilationcontext.h"


namespace Evidyon {
namespace World {
namespace Tools {


dc::dcClass<TerrainBlendingMapLayer>::Implements<MapLayer>
  TerrainBlendingMapLayer::implements_map_layer_;

  

//----[  TerrainBlendingMapLayer  ]--------------------------------------------
TerrainBlendingMapLayer::TerrainBlendingMapLayer() {
  member("Inner Layer", &inner_);
  member("Outer Layer", &outer_);
  member("Create Variations by Rotating Elements?",
         &create_variations_by_rotation_);

  for (int i = 0; i < NUMBER_OF_MAP_LAYER_BLENDING_TYPES; ++i) {
    std::string name = "Blending Type:  ";
    name.append(MapLayerBlendingTypeString(MapLayerBlendingType(i)));
    member(name, &blends_[i]);
  }
}




typedef std::pair<Texture::TextureIndex,int> Terrain;
typedef std::vector<Terrain> TerrainArray;



//----[  SetLocationBlendingType  ]--------------------------------------------
void SetLocationBlendingType(size_t my_layer_index,
                             size_t inner_map_layer_index,
                             size_t outer_map_layer_index,
                             const TerrainArray* blending_options,
                             MapLocationData* location) {
  MapLayerBlendingType blending_type
    = location->getLayerBlendingType(inner_map_layer_index,
                                     outer_map_layer_index);
  if (NUMBER_OF_MAP_LAYER_BLENDING_TYPES <= blending_type) return;
  const TerrainArray* terrain_options = &blending_options[blending_type];
  if (terrain_options->empty()) return;
  const Terrain& terrain
    = terrain_options->at(rand()%terrain_options->size());
  location->this_layer.visual_data.terrain_texture = terrain.first;
  location->this_layer.visual_data.terrain_texture_rotation = terrain.second;
}


//----[  compile  ]------------------------------------------------------------
void TerrainBlendingMapLayer::compile(const RECT* area,
                                      MapCompilationContext* compilation_context) {

  // First, get the references and be sure they're valid
  size_t inner_map_layer_index = inner_.getReferencedResourceIndex();
  size_t outer_map_layer_index = outer_.getReferencedResourceIndex();
  size_t my_layer_index = getOwner()->getIndex();
  if (inner_map_layer_index >= my_layer_index ||
      outer_map_layer_index >= my_layer_index) {
    return;
  }

  // Create all blending options
  TerrainArray blending_options[NUMBER_OF_MAP_LAYER_BLENDING_TYPES];
  if (create_variations_by_rotation_.getValue()) {
    for (int type = 0; type < NUMBER_OF_MAP_LAYER_BLENDING_TYPES; ++type) {
      const dc::dcGenericResource::Array& members = blends_[type].getMembers();
      dc::dcGenericResource::Array::const_iterator i;
      for (i = members.begin(); i != members.end(); ++i) {
        using namespace Evidyon::Tools;
        TextureReference* element = static_cast<TextureReference*>(*i);
        int group = (type / 4) * 4;
        size_t index = element->getReferencedResourceIndex();
        blending_options[group+(type+0)%4].push_back(Terrain(index,0));
        blending_options[group+(type+1)%4].push_back(Terrain(index,1));
        blending_options[group+(type+2)%4].push_back(Terrain(index,2));
        blending_options[group+(type+3)%4].push_back(Terrain(index,3));
      }
    }
  } else {
    for (int type = 0; type < NUMBER_OF_MAP_LAYER_BLENDING_TYPES; ++type) {
      const dc::dcGenericResource::Array& members = blends_[type].getMembers();
      dc::dcGenericResource::Array::const_iterator i;
      for (i = members.begin(); i != members.end(); ++i) {
        using namespace Evidyon::Tools;
        TextureReference* element = static_cast<TextureReference*>(*i);
        blending_options[type].push_back(Terrain(element->getReferencedResourceIndex(),0));
      }
    }
  }

  // If this is a full area, use an optimized method
  //unsigned int width = compilation_context->width;
  //unsigned int height = compilation_context->height;
  //if (area->left == 0 && area->top == 0 &&
  //    area->right == width && area->bottom == height) {
  //  unsigned int size = width * height;
  //  MapLocationData* location = compilation_context->locations;
  //  for (unsigned int i = 0; i < size; ++i, ++location) {
  //    SetLocationBlendingType(my_layer_index,
  //                            inner_map_layer_index,
  //                            outer_map_layer_index,
  //                            blending_options,
  //                            location);
  //  }
  //} else {
    // Scan over the specified area
    for (int y = area->top; y < area->bottom; ++y) {
      for (int x = area->left; x < area->right; ++x) {
        MapLocationData* location = compilation_context->atOrNull(x, y);
        if (location) {
          SetLocationBlendingType(my_layer_index,
                                  inner_map_layer_index,
                                  outer_map_layer_index,
                                  blending_options,
                                  location);
        }
      }
    }
  //}
}





}
}
}