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
#ifndef __MAPTILE_FLUIDDATA_H__
#define __MAPTILE_FLUIDDATA_H__
#pragma once


#include "maptile_data.h"

namespace Evidyon {
namespace World {

//----[  MapTile_FluidData  ]--------------------------------------------------
struct MapTile_FluidData {
  static const MapTile_Data::SorterType Sorter;
  const MapTile_Data* data;
  inline Texture::TextureIndex texture() { return data->visual_data.fluid_texture; }
};

}
}


#endif