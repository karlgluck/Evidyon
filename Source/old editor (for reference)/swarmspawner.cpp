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
#include "swarmspawner.h"
#include "..\shared\server-editor\swarmspawndescription.h"
#include <map>
#include <set>
#include <vector>
#include <list>
#include <d3dx9.h>

#include <dc/debug>
#include "../common/safemacros.h"
#include "../shared/evidyon_world.h"

#include "rarityscale.h" // for RARITY_SCALE_SIZE...this should be changed
// because its used to boost the treasure generation but this doesn't
// correlate exactly to the bits compiled...just do something else


const char* MapColorMask<SpawnLevel>::ELEMENT_LIST_NAME = "Spawn Levels";
const char* MapColorMask<SpawnLevel>::ELEMENT_DATA_MEMBER_NAME = "Spawn Level Range";
const char* MapColorMask<SpawnDensity>::ELEMENT_LIST_NAME = "Spawn Densities";
const char* MapColorMask<SpawnDensity>::ELEMENT_DATA_MEMBER_NAME = "Spawn Density";
const char* MapColorMask<SwarmSpawn>::ELEMENT_LIST_NAME = "Swarm Types";
const char* MapColorMask<SwarmSpawn>::ELEMENT_DATA_MEMBER_NAME = "Swarm Data";






SwarmSpawner::SwarmSpawner() {
  member("Spawn Type Masks", &spawn_);
  member("Levels", &levels_);
  member("Density", &density_);
}


//
//
//void ReleaseMapToTypeIndexMask(int* indexed_map) {
//  SAFE_DELETE_ARRAY(indexed_map);
//}
//
//
//typedef std::vector<D3DCOLOR> MaskColorTypes;
//bool AcquireMapToTypeIndexMask(LPDIRECT3DDEVICE9 d3d_device,
//                               const char* file_name,
//                               const MaskColorTypes& mask_color_types,
//                               unsigned int* width_out,
//                               unsigned int* height_out,
//                               int** map_out) {
//  unsigned int width, height;
//  D3DCOLOR* buffer = NULL;
//  CONFIRM(AcquireMapMask(d3d_device,
//                         file_name,
//                        &width,
//                        &height,
//                        &buffer)) else return false;
//  int* map = new int[width*height];
//  ASSERT(map) else {
//    ReleaseMapMask(buffer);
//    return false;
//  }
//
//  // Reset to all -1
//  memset(map, -1, sizeof(int) * width * height);
//
//  // Map each entry
//  int type_index = 0;
//  for (MaskColorTypes::const_iterator i = mask_color_types.begin();
//       i != mask_color_types.end(); ++i, ++type_index) {
//    D3DCOLOR color = 0x00FFFFFF & (*i); // pull out the RGB components (ignore alpha)
//    for (int y = 0; y < height; ++y) {
//      for (int x = 0; x < width; ++x) {
//        if (buffer[y*width+x] != color) continue;
//        CONFIRM(map[y*width+x] == -1); // safety check
//        map[y*width+x] = type_index;
//      }
//    }
//  }
//
//  ReleaseMapMask(buffer);
//
//  *map_out = map;
//  *width_out = width;
//  *height_out = height;
//
//  return true;
//}


// Algorithm for compiling swarm spawn:
//*1. load level image, and create a lookup table of coordinate->level
//*2. load spawn image and createa a multimap of coordinate->swarm types
//*3. load spawn density image
//*4. create an empty swarm coordinate set
//*5. for each color:
//*   a. make a list of all of the locations of this color
//    b. calculate total_beings = 1/density * number of locations
//    c. pick a grid of spawn coordinates separated by sqrt(density) on each axis
//    d. offset each spawn coordinate by [-2,2] on each axis
//    e. remove coordinates not in mask
//    f. add random spawn coordinates in mask area until total number
//       of coordinates equals total_beings, as calculated in (b)
//    g. add all coordinates to swarm coordinate set
//*6. segment the swarm coordinate set into world regions
// 7. for each world region:
//    a. build a list of the coordinates in the region
//    b. remove coordinates that do not exist in any spawn mask
//        or are not in the level mask
//    c. remove coordinates randomly until only MAX_SWARM_SPAWNS_PER_WORLD_REGION remain
//    d. randomly order the coordinates [[so the server doesn't have to!]]
//    e. create an empty set of spawn types
//    f. for each coordinate:
//        i. look up the coordinate's associated level
//    g. for each coordinate and each coordinate's spawn type (from mask):
//        i. create or access the spawn type in the set
//        ii. increment spawn type granularity counter
//        iii. set the coordinate's bit-index in the mask
//    h. write out up to the first 16 spawn types into the file
//        i. granularity = # of references in region
//        ii. activate_period = time_between_individuals / SWARM_SPAWN_PERIOD
//        iii. reset_value = time_between_waves / time_between_individuals * granularity
//    i. write out the coordinate location/info array
//       (always MAX_SWARM_SPAWNS_PER_WORLD_REGION entries, pad by repeating if necessary)
//
// The monster level rarity boost was added to deal with treasure.  For the highest N for
// which the average monster level in a region is higher than entry N in the scale's values,
// then set the rarity mask in the swarm region's description to have N matches.
bool SwarmSpawner::compileForServer(LPDIRECT3DDEVICE9 d3d_device,
                                    int map_regions_wide,
                                    int map_regions_high,
                                    int* monster_level_rarity_boost,
                                    dc::dcStreamOut* stream) {
  typedef std::pair<int,int> MapCoordinate;
  typedef std::map<MapCoordinate, int> Levels;
  typedef std::multimap<MapCoordinate, const SwarmSpawn*> SwarmSpawns;
  typedef std::set<MapCoordinate> CoordinateSet;

  Levels levels;
  { // Load the coordinate->level map
    MaskColorTypes level_colors;
    {
      dc::dcGenericResource::Array::const_iterator end = levels_.getElementsList().end();
      dc::dcGenericResource::Array::const_iterator i = levels_.getElementsList().begin();
      for (; i != end; ++i) {
        MapColorMask<SpawnLevel>::Element* element = 
          (MapColorMask<SpawnLevel>::Element*)(*i);
        level_colors.push_back(element->getMaskD3DColor());
      }
    }

    unsigned int width, height;
    int* types_map = NULL;
    bool has_types = AcquireMapToTypeIndexMask(d3d_device,
                                               levels_.getMaskImageFileName().c_str(),
                                               level_colors,
                                              &width,
                                              &height,
                                              &types_map);
    if (!APP_WARNING(!has_types)("Map level mask invalid/undefined")) {
      // Iterate through all locations and spit out levels for the valid ones
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          if (types_map[y*width+x] < 0) continue;
          const MapColorMask<SpawnLevel>::Element* element =
            (MapColorMask<SpawnLevel>::Element*)(levels_.getElementsList().at(types_map[y*width+x]));
          const SpawnLevel& level_data = element->getData();
          levels.insert(Levels::value_type(MapCoordinate(x, y),
                                           level_data.getSpawnLevel()));
        }
      }

      ReleaseMapToTypeIndexMask(types_map);
    }
  }




  SwarmSpawns swarm_spawns;
  for (dc::dcGenericResource::Array::const_iterator i = spawn_.getMembers().begin();
       i != spawn_.getMembers().end(); ++i) {
    MapColorMask<SwarmSpawn>* mask =
      (dc::dcList<MapColorMask<SwarmSpawn>>::Element*)(*i);

    // Get all of the colors in this type mask
    MaskColorTypes colors;
    {
      dc::dcGenericResource::Array::const_iterator end = mask->getElementsList().end();
      dc::dcGenericResource::Array::const_iterator i = mask->getElementsList().begin();
      for (; i != end; ++i) {
        const MapColorMask<SwarmSpawn>::Element* element = 
          (const MapColorMask<SwarmSpawn>::Element*)(*i);
        colors.push_back(element->getMaskD3DColor());
      }
    }

    unsigned int width, height;
    int* types_map = NULL;
    bool has_types =  AcquireMapToTypeIndexMask(d3d_device,
                                                mask->getMaskImageFileName().c_str(),
                                                colors,
                                               &width,
                                               &height,
                                               &types_map);
    if (!APP_WARNING(!has_types)("Map spawn mask invalid/undefined")) {

      // Iterate through all locations and spit out levels for the valid ones
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          if (types_map[y*width+x] < 0) continue;
          const MapColorMask<SwarmSpawn>::Element* element =
            (const MapColorMask<SwarmSpawn>::Element*)(mask->getElementsList().at(types_map[y*width+x]));
          const SwarmSpawn& swarm_spawn = element->getData();

          swarm_spawns.insert(SwarmSpawns::value_type(MapCoordinate(x, y),
                                                     &swarm_spawn));
        }
      }

      ReleaseMapToTypeIndexMask(types_map);
    }
  }


  // Create the coordinate density
  CoordinateSet coordinates_with_spawn;
  {
    unsigned int width, height;
    D3DCOLOR* density_data = NULL;
    bool has_types = AcquireMapMask(d3d_device,
                                    density_.getMaskImageFileName().c_str(),
                                   &width,
                                   &height,
                                   &density_data);
    if (!APP_WARNING(!has_types)("Map density mask invalid/undefined")) {

      dc::dcGenericResource::Array::const_iterator end = density_.getElementsList().end();
      dc::dcGenericResource::Array::const_iterator i = density_.getElementsList().begin();
      for (; i != end; ++i) {
        CoordinateSet this_density_coordinates;

        MapColorMask<SpawnDensity>::Element* element = 
          (MapColorMask<SpawnDensity>::Element*)(*i);

        D3DCOLOR density_color = element->getMaskD3DColor();
        int density = element->getData().getValue();
        if (density <= 0) continue;

        float sqrt_density = sqrtf((float)density);
        float flt_width = (float)width, flt_height = (float)height;
        for (float flt_y = 0.0f; flt_y < flt_height; flt_y += sqrt_density) {
          int y = (int)floorf(flt_y);
          for (float flt_x = 0.0f; flt_x < flt_width; flt_x += sqrt_density) {
            int x = (int)floorf(flt_x);

            // Is this coordinate the correct color?
            if (density_data[y*width+x] == density_color) continue;
            this_density_coordinates.insert(MapCoordinate(x,y));
          }
        }

        // Move local->global
        coordinates_with_spawn.insert(this_density_coordinates.begin(),
                                      this_density_coordinates.end());
      }

      ReleaseMapMask(density_data);


      // Remove all spawn coordinates that don't have a level associated with
      // them or have no swarm type definition
      for (CoordinateSet::iterator i = coordinates_with_spawn.begin();
                                   i != coordinates_with_spawn.end(); ) {
        CoordinateSet::iterator next = i; ++next;
        const MapCoordinate& coord = *i;
        if (levels.find(coord) == levels.end() ||
            swarm_spawns.lower_bound(coord) == swarm_spawns.end()) {
          coordinates_with_spawn.erase(i);
        }
        i = next; // advance
      }
    }
  }

  // write out the map regions
  CONFIRM(stream->write(&map_regions_wide, sizeof(map_regions_wide)) &&
          stream->write(&map_regions_high, sizeof(map_regions_high)));

  // We now have the following information:
  //  levels - map of coordinate -> level of monster spawned at that coordinate
  //  swarm_spawns - map of coordinate -> one or more monster swarm types that can be spawned there
  //  coordinates_with_spawn - all of the coordinates in the world at
  //                           which monsters can be spawned
  // The next step is to create, for each of the world regions, data about the
  // swarm-type monsters that can be spawned in that region.
  for (int map_region_y = 0; map_region_y < map_regions_high; ++map_region_y) {
    int region_min_y = map_region_y * Evidyon::World::REGION_SIZE;
    int region_max_y = region_min_y + Evidyon::World::REGION_SIZE;
    for (int map_region_x = 0; map_region_x < map_regions_wide; ++map_region_x) {
      int region_min_x = map_region_x * Evidyon::World::REGION_SIZE;
      int region_max_x = region_min_x + Evidyon::World::REGION_SIZE;

      typedef std::vector<size_t> CoordinateIndices;
      CoordinateIndices coordinate_indices;

      // find all of the density coordinates in this region
      CoordinateSet regional_coordinates;
      for (int ry = region_min_y; ry < region_max_y; ++ry) {
        for (int rx = region_min_x; rx < region_max_x; ++rx) {
          MapCoordinate coord(rx,ry);
          if (coordinates_with_spawn.end() !=
              coordinates_with_spawn.find(coord)) {
            regional_coordinates.insert(coord);
          }
        }
      }

      // Reduce the number of locations in this region through random removal
      while (regional_coordinates.size() > Evidyon::MAX_SWARM_SPAWNS_PER_WORLD_REGION) {
        size_t index_to_remove = rand() % regional_coordinates.size();
        CoordinateSet::iterator i = regional_coordinates.begin();
        for (size_t index = 0; index < index_to_remove; ++index, ++i);
        // since we picked the index based on size, this should never happen
        ASSERT(i != regional_coordinates.end()) else return false;
        regional_coordinates.erase(i);
      }

      {
        unsigned int current_index = 0;
        for (CoordinateSet::iterator i = regional_coordinates.begin();
             i != regional_coordinates.end(); ++i, ++current_index) {
          coordinate_indices.push_back(current_index);
        }
      }

      int average_monster_level = 0;

      // Reorder the coordinates randomly and fill the compiled location data
      Evidyon::CompiledSwarmSpawnLocationInfo compiled_locations[Evidyon::MAX_SWARM_SPAWNS_PER_WORLD_REGION];
      int number_of_compiled_locations = regional_coordinates.size();
      for (CoordinateSet::iterator i = regional_coordinates.begin();
           i != regional_coordinates.end(); ++i) {

        CoordinateIndices::iterator src_index = coordinate_indices.begin();
        { // get an iterator at the random source index
          size_t random_src_index = ((size_t)rand())%coordinate_indices.size();
          for (size_t src_index_counter = 0; src_index_counter < random_src_index;
             ++src_index_counter, ++src_index);
        }

        Evidyon::CompiledSwarmSpawnLocationInfo* target =
          &compiled_locations[*src_index];
        const MapCoordinate& coord = *i;
        target->x = coord.first;
        target->y = coord.second;
        target->level = levels.find(coord)->second; // this always has to succeed
        coordinate_indices.erase(src_index);
      }

      // we should have used up all of the random indices now
      ASSERT(coordinate_indices.empty());

      typedef std::map<const SwarmSpawn*, unsigned int> SwarmSpawnToLocationMask;
      SwarmSpawnToLocationMask local_swarms;
      for (int i = 0; i < number_of_compiled_locations; ++i) {
        const Evidyon::CompiledSwarmSpawnLocationInfo* location =
          &compiled_locations[i];

        unsigned int coordinate_mask = 1 << i;

        // find all of the swarm types at this location and ensure the source
        // types both exist locally and have the bit set for this coordinate
        MapCoordinate coord(location->x, location->y);
        for (SwarmSpawns::iterator swarm = swarm_spawns.lower_bound(coord);
             swarm != swarm_spawns.upper_bound(coord); ++swarm) {
          const SwarmSpawn* swarm_spawn = swarm->second;
          SwarmSpawnToLocationMask::iterator local = local_swarms.find(swarm_spawn);
          if (local == local_swarms.end()) {
            local_swarms.insert(SwarmSpawnToLocationMask::value_type(swarm_spawn, coordinate_mask));
          } else {
            local->second |= coordinate_mask;
          }
        }

        average_monster_level += location->level;
      }

      // find the average monster level
      if (number_of_compiled_locations > 0) {
        average_monster_level /= number_of_compiled_locations;
      }

      { // calculate the number of bits to set based on the average monster level
        int treasure_mask_rarity = 0;
        while (monster_level_rarity_boost[treasure_mask_rarity] < average_monster_level &&
               treasure_mask_rarity < RARITY_SCALE_SIZE) {
          treasure_mask_rarity++;
        }
        {
          // create bits in the low word
          unsigned int treasure_mask = 0xFFFF & ((~0L) << treasure_mask_rarity);

          // copy into high/low
          unsigned int treasure_randomizer_mask = (treasure_mask << 16 | treasure_mask);
          CONFIRM(stream->write(&treasure_randomizer_mask,
                                sizeof(treasure_randomizer_mask))) else return false;
        }
      }


      // write out the spawn types into the file
      {
        size_t swarms = local_swarms.size();
        CONFIRM(stream->write(&swarms, sizeof(swarms))) else return false;
      }
      for (SwarmSpawnToLocationMask::iterator i = local_swarms.begin();
           i != local_swarms.end(); ++i) {
        const SwarmSpawn* swarm_spawn = i->first;
        unsigned int spawn_location_mask = i->second;

        // get the number of bits set in the mask
        unsigned int granularity = 0;
        { // from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
          unsigned int v; // count the number of bits set in v
          unsigned int c; // c accumulates the total bits set in v
          v = spawn_location_mask;
          for (c = 0; v; c++) {
            v &= v - 1; // clear the least significant bit set
          }
          granularity = c;
        }

        Evidyon::CompiledRegionSwarmSpawn description;
        CONFIRM(swarm_spawn->compile(granularity,
                                     &description)) else return false;
        description.spawn_location_mask = spawn_location_mask;
        CONFIRM(stream->write(&description, sizeof(description))) else return false;
      }

      // write the compiled locations
      CONFIRM(stream->write(compiled_locations,
                            sizeof(compiled_locations))) else return false;
    }
  }

  // Success
  return true;
}




std::string SwarmSpawner::staticTypeName() {
  return "SwarmSpawner";
}