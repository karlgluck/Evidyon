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
#ifndef __SHARED_GAMEFILE_CLIENTGAMEFILEMAPDATA_H__
#define __SHARED_GAMEFILE_CLIENTGAMEFILEMAPDATA_H__
#pragma once



namespace Evidyon {
namespace GameFile {
struct ClientGameFileMapLocationData;
}
}

namespace Evidyon {
namespace GameFile {



//----[  ClientGameFileMapData  ]----------------------------------------------
struct ClientGameFileMapData {

  // The size of the entries in the table is either 1, 2 or 4 bytes.  This
  // is found by: width*height * bytes_per_loc = location_table_length
  size_t width, height;
  void* location_table_data;
  size_t location_table_length;

  ClientGameFileMapLocationData* unique_map_locations;
  size_t number_of_unique_map_locations;
};


}
}

#endif
