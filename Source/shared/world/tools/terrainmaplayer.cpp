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
#include "terrainmaplayer.h"
#include "maplocationdata.h"


namespace Evidyon {
namespace World {
namespace Tools {


dc::dcClass<TerrainMapLayer>::Implements<MapLayer> TerrainMapLayer::implements_map_layer_;

  
//----[  TerrainMapLayer  ]------------------------------------------------------
TerrainMapLayer::TerrainMapLayer() {
  member("Mask Area", &mask_area_);
  member("Randomize Orientation", &randomize_orientation_);
  member("Terrain Textures", &textures_);
}






//----[  TerrainMapLayerCompliationFunctionoid  ]------------------------------
class TerrainMapLayerCompliationFunctionoid : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    location->this_layer.visual_data.terrain_texture =
      terrain_textures[rand()%number_of_terrain_textures];
    location->this_layer.visual_data.terrain_texture_rotation =
      randomize_orientation ? rand()%4 : 0;
    return true;
  }
  bool randomize_orientation;
  Texture::TextureIndex* terrain_textures;
  size_t number_of_terrain_textures;
};



//----[  compile  ]------------------------------------------------------------
void TerrainMapLayer::compile(const RECT* area,
                            MapCompilationContext* compilation_context) {
  const dc::dcGenericResource::Array& textures = textures_.getMembers();
  if (textures.empty()) return;

  Texture::TextureIndex* terrain_textures
      = new Texture::TextureIndex[textures.size()];
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = textures.begin(); i != textures.end(); ++i, ++index) {
    const Evidyon::Tools::TextureReference* reference = 
      static_cast<const Evidyon::Tools::TextureReference*>(*i);
    terrain_textures[index] = reference->getReferencedResourceIndex();
  }

  TerrainMapLayerCompliationFunctionoid functionoid;
  functionoid.randomize_orientation = randomize_orientation_.getValue();
  functionoid.terrain_textures = terrain_textures;
  functionoid.number_of_terrain_textures = textures.size();
  mask_area_.processMaskLocations(getOwner()->getIndex(),
                                  area,
                                  compilation_context,
                                  &functionoid);

  delete[] terrain_textures;
}



//----[  brushPreview  ]-------------------------------------------------------
void TerrainMapLayer::brushPreview(MapMask* mask,
                                   D3DCOLOR color,
                                   LocationVisualData* visual_data) {
  if (mask_area_.references(mask, color)) {
    const dc::dcGenericResource::Array& textures = textures_.getMembers();
    if (textures.empty()) return;
    const Evidyon::Tools::TextureReference* reference = 
      static_cast<const Evidyon::Tools::TextureReference*>(textures.front());
    visual_data->terrain_texture = reference->getReferencedResourceIndex();
  }
}



//----[  setArea  ]------------------------------------------------------------
void TerrainMapLayer::setArea(dc::dcList<MapMask>::Element* map_mask,
                              D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}




}
}
}