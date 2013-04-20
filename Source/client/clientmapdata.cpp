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
#include "clientmapdata.h"
#include "common/null.h"
#include "common/safemacros.h"
#include <memory.h> // for memcpy
#include <map> // for finding zones; TODO: do this more efficiently
#include "shared/zone/zone.h"



namespace Evidyon {
namespace Client {

//
//void SaveMapImage(size_t width, size_t height, const void* location_table) {
//  // create the image to hold each map location's table index
//  LPDIRECT3DTEXTURE9 map_image = NULL;
//  HRESULT hr = D3DXCreateTexture(source_->getD3DDevice(),
//                                 width,
//                                 height,
//                                 1,
//                                 0,
//                                 D3DFMT_R5G6B5,
//                                 D3DPOOL_SCRATCH,
//                                 &map_image);
//  D3DLOCKED_RECT lr;
//  if (FAILED(hr) || FAILED(map_image->LockRect(0, &lr, NULL, 0))) {
//    SAFE_DELETE_ARRAY(map_location_table_index);
//    SAFE_RELEASE(map_image);
//    error("Unable to create map texture");
//    element->destroyCompilationContext(&compilation_context);
//    return false;
//  }
//}

  
//----[  ClientMapData  ]------------------------------------------------------
ClientMapData::ClientMapData() {
  zero();
}




//----[  ~ClientMapData  ]-----------------------------------------------------
ClientMapData::~ClientMapData() {
  destroy();
}







//----[  create  ]-------------------------------------------------------------
bool ClientMapData::create(
    size_t width,
    size_t height,
    const void* location_table,
    size_t location_byte_size,
    const GameFile::ClientGameFileMapLocationData* unique_locations,
    size_t number_of_unique_locations) {
  width_ = width;
  height_ = height;
  size_t number_of_locations = width * height;
  locations_ = new Location[number_of_locations];
  if (!locations_) return false;
  unique_locations_ =
    new GameFile::ClientGameFileMapLocationData[number_of_unique_locations];
  if (!unique_locations_) {
    SAFE_DELETE_ARRAY(locations_);
    return false;
  }
  memcpy(unique_locations_,
         unique_locations,
         sizeof(GameFile::ClientGameFileMapLocationData)
          * number_of_unique_locations);
  number_of_unique_locations_ = number_of_unique_locations;
  if (location_byte_size == sizeof(unsigned __int32)) {
    unsigned __int32* table = (unsigned __int32*)location_table;
    for (size_t i = 0; i < number_of_locations; ++i) {
      size_t source_index = *(table++);
      if (source_index >= number_of_unique_locations_) goto FAILED;
      locations_[i].data = &unique_locations_[source_index];
    }
  } else if (location_byte_size == sizeof(unsigned __int16)) {
    unsigned __int16* table = (unsigned __int16*)location_table;
    for (size_t i = 0; i < number_of_locations; ++i) {
      size_t source_index = *(table++);
      if (source_index >= number_of_unique_locations_) goto FAILED;
      locations_[i].data = &unique_locations_[source_index];
    }
  } else if (location_byte_size == sizeof(unsigned __int8)) {
    unsigned __int8* table = (unsigned __int8*)location_table;
    for (size_t i = 0; i < number_of_locations; ++i) {
      size_t source_index = *(table++);
      if (source_index >= number_of_unique_locations_) goto FAILED;
      locations_[i].data = &unique_locations_[source_index];
    }
  }

  return true;

FAILED:
  destroy();
  return false;
}





//----[  destroy  ]------------------------------------------------------------
void ClientMapData::destroy() {
  SAFE_DELETE_ARRAY(unique_locations_);
  SAFE_DELETE_ARRAY(locations_);
  zero();
}





//----[  getZone  ]------------------------------------------------------------
Evidyon::Zone::ZoneIndex ClientMapData::getZone(int x, int z) const {
  return atOrDefault(x,z)->zone;
}



//----[  getNavigability  ]----------------------------------------------------
Evidyon::World::Navigability ClientMapData::getNavigability(int x, int z) const {
  return atOrDefault(x,z)->navigability;
}


//----[  getChaosLevel  ]------------------------------------------------------
float ClientMapData::getChaosLevel(int x,
                                   int z,
                                   const Zone::Zone* zones) const {
  return atOrDefault(x,z)->chaos_level;
  //static const int EXTENT = 10;
  //int chaos_zones = 0;
  //for (int dz = z - EXTENT;
  //         dz <= z + EXTENT; ++dz) {
  //  for (int dx = x - EXTENT;
  //           dx <= x + EXTENT; ++dx) {
  //    Evidyon::Zone::ZoneIndex zone = atOrDefault(dx, dz)->zone;
  //    chaos_zones +=
  //      (zone == Zone::INVALID_ZONE_INDEX) ? 1 : (zones[zone].chaos ? 1 : 0);
  //  }
  //}
  //return chaos_zones / float((2*EXTENT+1) * (2*EXTENT+1));
}




//----[  changeZone  ]---------------------------------------------------------
bool ClientMapData::changeZone(int x,
                               int z,
                               Zone::ZoneIndex current_zone,
                               Zone::ZoneIndex* new_zone) {
  static const int EXTENT = 3;

  // when zone that is not the current zone has a large % (say, 80%) or more
  // of the map's tiles, switch to that zone.
  typedef std::map<Zone::ZoneIndex,int> Zones;
  Zones zones;
  for (int dz = z - EXTENT;
           dz <= z + EXTENT; ++dz) {
    for (int dx = x - EXTENT;
             dx <= x + EXTENT; ++dx) {
      Evidyon::Zone::ZoneIndex zone = atOrDefault(dx, dz)->zone;
      if (zone == current_zone) continue;
      Zones::iterator i = zones.find(zone);
      if (i == zones.end())  {
        zones.insert(Zones::value_type(zone,1));
      } else {
        i->second++;
      }
    }
  }

  for (Zones::iterator i = zones.begin(); i != zones.end(); ++i) {
    if (i->second > ((2*EXTENT+1) * (2*EXTENT+1) * 0.8)) {
      *new_zone = i->first;
      return true;
    }
  }

  return false;
}



//----[  updateMapTileData  ]--------------------------------------------------
void ClientMapData::updateMapTileData(
    int x,
    int y,
    Evidyon::World::LocationVisualData* data) {
  memcpy(data,
         &atOrDefault(x,y)->visual_data,
         sizeof(Evidyon::World::LocationVisualData));
}


//----[  atOrDefault  ]--------------------------------------------------------
const GameFile::ClientGameFileMapLocationData*
    ClientMapData::atOrDefault(int x, int z) const {
  return (x >= 0 && z >= 0 && x < width_ && z < height_)
    ? locations_[z*width_+x].data
    : &default_location_;
}


//----[  zero  ]---------------------------------------------------------------
void ClientMapData::zero() {
  default_location_.visual_data.terrain_height = 0.0f;
  default_location_.visual_data.terrain_texture = Texture::INVALID_TEXTURE_INDEX;
  default_location_.visual_data.terrain_texture_rotation = 0;
  default_location_.visual_data.fluid_texture = Texture::INVALID_TEXTURE_INDEX;
  default_location_.visual_data.wall_texture = Texture::INVALID_TEXTURE_INDEX;
  default_location_.visual_data.wall_type = World::WALLTYPE_FLOOR;
  default_location_.visual_data.occluding = true;
  default_location_.visual_data.scenery = Scenery::INVALID_SCENERY_INDEX;
  default_location_.visual_data.skinned_mesh
    = SkinnedMesh::INVALID_SKINNED_MESH_INDEX;
  default_location_.zone = Zone::INVALID_ZONE_INDEX;
  default_location_.navigability = World::NAVIGABILITY_IMPASSABLE;
  default_location_.chaos_level = 1.0f;
}


  
}
}

