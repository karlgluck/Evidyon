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
#include "zonemaplayer.h"
#include "maplocationdata.h"

namespace Evidyon {
namespace World {
namespace Tools {



dc::dcClass<ZoneMapLayer>::Implements<MapLayer> ZoneMapLayer::implements_map_layer_;

  
//----[  ZoneMapLayer  ]------------------------------------------------------
ZoneMapLayer::ZoneMapLayer() {
  member("Mask Area", &mask_area_);
  member("Zone", &zone_);
}




//----[  ZoneMapLayerCompliationFunctionoid  ]--------------------------------
class ZoneMapLayerCompliationFunctionoid : public MapMaskLocationFunctionoid {
public:
  virtual bool process(MapLocationData* location) {
    location->this_layer.zone = zone;
    return true;
  }
  Zone::ZoneIndex zone;
};



//----[  compile  ]------------------------------------------------------------
void ZoneMapLayer::compile(const RECT* area,
                            MapCompilationContext* compilation_context) {
  ZoneMapLayerCompliationFunctionoid functionoid;
  functionoid.zone = zone_.getReferencedResourceIndex();
  mask_area_.processMaskLocations(getOwner()->getIndex(),
                                  area,
                                  compilation_context,
                                  &functionoid);
}




//----[  setArea  ]------------------------------------------------------------
void ZoneMapLayer::setArea(dc::dcList<MapMask>::Element* map_mask,
                            D3DCOLOR color) {
  mask_area_.set(map_mask, color);
}





}
}
}