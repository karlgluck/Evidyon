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
#ifndef __MAPLAYERBLENDINGTYPE_H__
#define __MAPLAYERBLENDINGTYPE_H__
#pragma once



namespace Evidyon {
namespace World {
namespace Tools {

//----[  MapLayerBlendingType  ]-----------------------------------------------
enum MapLayerBlendingType {

  //  o   i             i
  // iii  io    iii    oi
  //      i      o      i
  NORTH, EAST, SOUTH, WEST,

  //  o            o
  //  io  io  oi  oi
  //      o    o  
  NORTHEAST_OUTER_CORNER,
  SOUTHEAST_OUTER_CORNER,
  SOUTHWEST_OUTER_CORNER,
  NORTHWEST_OUTER_CORNER,

  //  io          oi
  //  ii  ii  ii  ii
  //      io  oi  
  NORTHEAST_INNER_CORNER,
  SOUTHEAST_INNER_CORNER,
  SOUTHWEST_INNER_CORNER,
  NORTHWEST_INNER_CORNER,

  NUMBER_OF_MAP_LAYER_BLENDING_TYPES,
  INVALID_MAP_LAYER_BLENDING_TYPE,
};



//----[  MapLayerBlendingTypeString  ]-----------------------------------------
const char* MapLayerBlendingTypeString(MapLayerBlendingType blending_type);


}
}
}

#endif