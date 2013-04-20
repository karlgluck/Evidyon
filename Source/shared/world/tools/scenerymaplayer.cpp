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
#include "scenerymaplayer.h"
#include "maplocationdata.h"
#include "shared/scenery/sceneryindex.h"


namespace Evidyon {
namespace World {
namespace Tools {


dc::dcClass<SceneryMapLayer>::Implements<MapLayer> SceneryMapLayer::implements_map_layer_;

  
//----[  SceneryMapLayer  ]------------------------------------------------------
SceneryMapLayer::SceneryMapLayer() {
  member("Mask Area", &mask_area_);
  member("Scenery", &scenery_);
  member("Tiles Per Placement", &density_);
  member("Navigability for Placed Locations", &navigability_);
}




//----[  SceneryMapLayerCompliationFunctionoid  ]--------------------------------
class SceneryMapLayerCompliationFunctionoid : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    if (density == 1 || (rand()%density) == 0) {
      size_t scenery_index =
        number_of_scenery_indices == 1 ? 0 : (rand()%number_of_scenery_indices);
      location->this_layer.visual_data.scenery = scenery_indices[scenery_index];
      location->this_layer.navigability = navigability;
      return true;
    } else {
      return false;
    }
  }
  Scenery::SceneryIndex* scenery_indices;
  size_t number_of_scenery_indices;
  unsigned int density;
  World::Navigability navigability;
};



//----[  compile  ]------------------------------------------------------------
void SceneryMapLayer::compile(const RECT* area,
                            MapCompilationContext* compilation_context) {
  if (density_.getValue() < 1) density_.setValue(1);

  const dc::dcGenericResource::Array& scenery = scenery_.getMembers();
  if (scenery.empty()) return;

  Scenery::SceneryIndex* scenery_indices
      = new Scenery::SceneryIndex[scenery.size()];
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = scenery.begin(); i != scenery.end(); ++i, ++index) {
    const Evidyon::Tools::SceneryReference* reference = 
      static_cast<const Evidyon::Tools::SceneryReference*>(*i);
    scenery_indices[index] = reference->getReferencedResourceIndex();
  }
  SceneryMapLayerCompliationFunctionoid functionoid;
  functionoid.scenery_indices = scenery_indices;
  functionoid.number_of_scenery_indices = scenery.size();
  functionoid.density = density_.getValue();
  functionoid.navigability = navigability_.getValue();
  mask_area_.processMaskLocations(getIndex(),
                                  area,
                                  compilation_context,
                                  &functionoid);
  delete[] scenery_indices;
}


//----[  brushPreview  ]-------------------------------------------------------
void SceneryMapLayer::brushPreview(MapMask* mask,
                                   D3DCOLOR color,
                                   LocationVisualData* visual_data) {
  if (density_.getValue() < 1) density_.setValue(1);
  if (mask_area_.references(mask, color) &&
      (rand()%density_.getValue()) == 0) {
    const dc::dcGenericResource::Array& scenery = scenery_.getMembers();
    if (!scenery.empty()) {
      const Evidyon::Tools::SceneryReference* scenery_reference = 
        (const Evidyon::Tools::SceneryReference*)(scenery.at(rand()%scenery.size()));
      visual_data->scenery = scenery_reference->getReferencedResourceIndex();
    }
  }
}


//----[  setArea  ]------------------------------------------------------------
void SceneryMapLayer::setArea(dc::dcList<MapMask>::Element* map_mask,
                              D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}


}
}
}