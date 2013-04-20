//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#include "maplayer.h"


namespace Evidyon {
namespace World {
namespace Tools {


  
//----[  MapLayer  ]-----------------------------------------------------------
MapLayer::MapLayer() {
  member("Priority", &priority_);
}




//----[  brushPreview  ]-------------------------------------------------------
void MapLayer::brushPreview(MapMask* mask,
                            D3DCOLOR color,
                            LocationVisualData* visual_data) {
}


//----[  compareTo  ]----------------------------------------------------------
int MapLayer::compareTo(const dc::dcGenericResource* other) const {
  if (!other || getOwner()->getTypeID() != other->getOwner()->getTypeID()) {
     return dc::dcGenericResource::compareTo(other);
  } else {
    return priority_.compareTo(&((const MapLayer*)other)->priority_);
  }
}



//----[  setPriority  ]--------------------------------------------------------
void MapLayer::setPriority(int priority) {
  priority_.setValue(priority);
}

}
}
}