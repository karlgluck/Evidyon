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
#ifndef __MAPTILECORNER_DATA_H__
#define __MAPTILECORNER_DATA_H__
#pragma once


namespace Evidyon {
namespace World {

//----[  MapTileCorner_Data  ]------------------------------------------
struct MapTileCorner_Data {

  // This is computed from the surrounding tiles' terrain heights
  float terrain_height;

  // This color is based on the surrounding tiles' visibility
  D3DCOLOR vertex_color;

  // This color is based on both the surrounding tiles' visibility and the
  // height of the vertex.  This allows pits to be black.
  D3DCOLOR terrain_color;
};

}
}


#endif