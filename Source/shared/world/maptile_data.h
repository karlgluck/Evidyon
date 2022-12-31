//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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