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
#include "maptile_terraindata.h"





//----[  MapTile_TerrainData_Sorter  ]-----------------------------------------
int MapTile_TerrainData_Sorter(const void* p1, const void* p2) {
  using namespace Evidyon::World;
  const MapTile_TerrainData* a = (const MapTile_TerrainData*)p1;
  const MapTile_Data* a_data = a->data;
  const MapTile_TerrainData* b = (const MapTile_TerrainData*)p2;
  const MapTile_Data* b_data = b->data;
  if (a_data->visibility == VISIBILITY_INVISIBLE) return +1; // invalid (invisible) quads to end
  if (b_data->visibility == VISIBILITY_INVISIBLE) return -1; // invalid (invisible) quads to end
  Evidyon::Texture::TextureIndex a_texture = a_data->visual_data.terrain_texture;
  Evidyon::Texture::TextureIndex b_texture = b_data->visual_data.terrain_texture;
  if (a_texture > b_texture) return +1; // sort by ascending texture
  if (a_texture < b_texture) return -1;
  if (a->data->top > b->data->top) return +1; // sort front-to-back
  if (a->data->top < b->data->top) return +1;
  return 0;
}

namespace Evidyon {
namespace World {

const MapTile_Data::SorterType MapTile_TerrainData::Sorter
  = MapTile_TerrainData_Sorter;

}
}
