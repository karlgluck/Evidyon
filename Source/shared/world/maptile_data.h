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
#ifndef __MAPTILE_DATA_H__
#define __MAPTILE_DATA_H__
#pragma once


#include "locationvisualdata.h"
#include "visibility.h"

#ifndef D3DCOLOR_DEFINED
typedef unsigned long D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

namespace Evidyon {
namespace World {
struct MapTileCorner_Data;

//----[  MapTile_Data  ]------------------------------------------------
struct MapTile_Data {

  // This data is obtained from the map renderer's data source and
  // defines the properties of this tile.
  LocationVisualData visual_data;

  // ---------------------------------------------------------------
  // This data is computed automatically by the renderer

  // Wherever the tile is in the world
  float left, top;

  // Determined by the player's location
  Visibility visibility;

  //  0
  // 3 1
  //  2
  MapTile_Data* neighbors[4];

  // 0 1
  // 2 3
  MapTileCorner_Data* corners[4];

  // average of 4 corners
  float terrain_center_vertex_height;
  D3DCOLOR terrain_center_vertex_color;


  typedef int (*SorterType)(const void*, const void*);
};

}
}


#endif