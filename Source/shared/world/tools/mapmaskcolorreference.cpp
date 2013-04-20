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
#include "mapmaskcolorreference.h"
#include "maplocationdata.h"
#include "mapcompilationcontext.h"
#include <assert.h>


namespace Evidyon {
namespace World {
namespace Tools {



  
//----[  MapMaskColorReference  ]----------------------------------------------
MapMaskColorReference::MapMaskColorReference() {
  member("Mask", &mask_);
  member("Color", &color_);
}








//----[  processMaskLocations  ]-----------------------------------------------
void MapMaskColorReference::processMaskLocations(
    size_t map_layer_index,
    const RECT* area,
    MapCompilationContext* compilation_context,
    MapMaskLocationFunctionoid* functionoid) {
  assert(area);
  MapMask* map_mask = mask_.getReferencedResource();
  if (!map_mask) return;

  int x, y;
  D3DCOLOR mask_color = color_.getD3DColor();
  if (map_mask->getFirstLocationInArea(mask_color, area, &x, &y)) {
    do {
      //if (area->left <= x && area->top <= y &&
      //    area->right > x && area->bottom > y) {
        MapLocationData* location = compilation_context->atOrNull(x,y);
        assert(location);
        if (functionoid->process(location)) {
          location->this_layer.last_modified_by.map_layer_index = map_layer_index;
        }
      //}
    } while (map_mask->getNextLocationInArea(mask_color, x, y, area, &x, &y));
  }
}






//----[  toString  ]-----------------------------------------------------------
std::string MapMaskColorReference::toString() {
  MapMask* map_mask = mask_.getReferencedResource();
  if (!map_mask) return color_.toString();
  const MapMaskColorData* color = map_mask->colorData(color_.getD3DColor());
  return (map_mask->getName() + " [") + (color ? color->getName() : "") + "] ";
}




//----[  getReferencedMaskListElement  ]---------------------------------------
dc::dcList<MapMask>::Element* MapMaskColorReference::getReferencedMaskListElement() {
  return mask_.getReferencedResource();
}



//----[  getD3DColorValue  ]---------------------------------------------------
D3DCOLOR MapMaskColorReference::getD3DColorValue() const {
  return color_.getD3DColor();
}


//----[  references  ]---------------------------------------------------------
bool MapMaskColorReference::references(MapMask* mask) const {
  return (mask_.getReferencedResource() == mask);
}


//----[  references  ]---------------------------------------------------------
bool MapMaskColorReference::references(MapMask* mask, D3DCOLOR color) const {
  return (color_.getD3DColor() == color) &&
         (mask_.getReferencedResource() == mask);
}


//----[  set  ]----------------------------------------------------------------
void MapMaskColorReference::set(dc::dcList<MapMask>::Element* map_mask,
                                D3DCOLOR color) {
  mask_.setReferencedResource(map_mask);
  color_.setD3DColor(color);
}


}
}
}