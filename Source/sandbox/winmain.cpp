#if 0
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
#include <windows.h>
#include <memory>
#include <math.h>
#include "common/fixedpoint/fixedpoint.h"
#include <assert.h>
#include "common/number.h"
#include "common/randint32.h"
#define DC_ENABLE_DEBUGGING
#include <dc/debug>
#include <dcx/win32>
#include <d3dx9math.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "dcx_d.lib")

unsigned long MultPack(unsigned long p, unsigned long v, unsigned long o) {
  return (p * o + v);
}

unsigned long MultUnpack(unsigned long p, unsigned long o, unsigned long* v) {
  unsigned long lv = p % o;
  *v = lv;
  return p / o;
}

class EntityVisibilityFunctionoid {};

struct Entity {
  Number x, z;
  struct {
    int cloud_x, cloud_id;
    Entity** first_entity;
    unsigned int number_of_entities;
    unsigned int visibility_mask;
  } proximity_cloud;
};

static size_t MAXIMUM_NUMBER_OF_ENTITIES = 4096*2;
static const size_t MAP_POWER_SIZE = 11;
static const size_t MAP_SIZE = 1 << MAP_POWER_SIZE;
static const Number VISIBLE_RANGE = Number(16);
//#define TEXTOUT

void randomize(Entity* entities) {
  for (int i = 0; i < MAXIMUM_NUMBER_OF_ENTITIES; ++i) {
    entities[i].x = int(rand_uint32()%MAP_SIZE);
    entities[i].z = int(rand_uint32()%MAP_SIZE);
    entities[i].proximity_cloud.cloud_id = 0;
    entities[i].proximity_cloud.cloud_x = 0;
    entities[i].proximity_cloud.visibility_mask = 0;
  }
  return;
  for (int a = 0; a < MAXIMUM_NUMBER_OF_ENTITIES; ++a) {
      Number x, z;
      x.int_value = (entities[a].x.int_value >> (16 + MAP_POWER_SIZE-3))<<(MAP_POWER_SIZE-3+16);
      z.int_value = (entities[a].z.int_value >> (16 + MAP_POWER_SIZE-3))<<(MAP_POWER_SIZE-3+16);
    //for (int b = a+1; b < MAXIMUM_NUMBER_OF_ENTITIES; ++b) {
      Number ox = /*entities[b].*/x - entities[a].x;
      Number oz = /*entities[b].*/z - entities[a].z;
      Number o = ox*ox + oz*oz;
      //if (o < Number(256)) {
        entities[a].x += ox / 2;
        entities[a].z += oz / 2;
      //}
      //entities[a].x = x;
      //entities[a].z = z;
    //}
  }
}

int CompareEntityX(const void* a, const void* b) {
  return (*(Entity**)a)->x.int_value - (*(Entity**)b)->x.int_value;
}
int CompareEntityXCloudZ(const void* a, const void* b) {
  Entity* entity_a = *(Entity**)a;
  Entity* entity_b = *(Entity**)b;
  int cloud = entity_a->proximity_cloud.cloud_x
            - entity_b->proximity_cloud.cloud_x;
  return cloud ? cloud : (entity_a->z.int_value - entity_b->z.int_value);
}


bool HeapSortEntityPointers_GreaterX(Entity* a, Entity* b) {
  return a->x.int_value > b->x.int_value;
}

void HeapSortEntityPointers_HeapifyX(Entity** a, int i, int size) {
  int l = (2 * i) + 1, great;
  int r = (2 * i) + 2;
  if ((l < size) && HeapSortEntityPointers_GreaterX(a[l], a[i])) {
    great = l;
  } else {
    great = i;
  }
  if ((r < size) && HeapSortEntityPointers_GreaterX(a[r], a[great])) {
    great = r;
  }
  if (great != i) {
    Entity* temp = a[i];
    a[i] = a[great];
    a[great] = temp;
    HeapSortEntityPointers_HeapifyX(a, great, size);
  }
}

void HeapSortEntityPointers_BuildMaxHeapX(Entity** entity_pointers, int number_of_entities) {
  for (int i = (number_of_entities - 1) >> 1; i >= 0; --i) {
    HeapSortEntityPointers_HeapifyX(entity_pointers, i, number_of_entities);
  }
}

void HeapSortEntityPointersX(Entity** entity_pointers, int number_of_entities) {
  HeapSortEntityPointers_BuildMaxHeapX(entity_pointers, number_of_entities);
  for (int i = number_of_entities; i > 0; --i) {
    Entity* temp = entity_pointers[0];
    --number_of_entities;
    entity_pointers[0] = entity_pointers[number_of_entities];
    entity_pointers[number_of_entities] = temp;
    HeapSortEntityPointers_HeapifyX(entity_pointers, 0, number_of_entities);
  }
}


bool HeapSortEntityPointers_GreaterXCloudZ(Entity* a, Entity* b) {
  int cloud = a->proximity_cloud.cloud_x
            - b->proximity_cloud.cloud_x;
  return cloud == 0 ? (a->z.int_value > b->z.int_value) : cloud > 0;
}

void HeapSortEntityPointers_HeapifyXCloudZ(Entity** a, int i, int size) {
  int l = (2 * i) + 1, great;
  int r = (2 * i) + 2;
  if ((l < size) && HeapSortEntityPointers_GreaterXCloudZ(a[l], a[i])) {
    great = l;
  } else {
    great = i;
  }
  if ((r < size) && HeapSortEntityPointers_GreaterXCloudZ(a[r], a[great])) {
    great = r;
  }
  if (great != i) {
    Entity* temp = a[i];
    a[i] = a[great];
    a[great] = temp;
    HeapSortEntityPointers_HeapifyXCloudZ(a, great, size);
  }
}

void HeapSortEntityPointers_BuildMaxHeapXCloudZ(Entity** entity_pointers, int number_of_entities) {
  for (int i = (number_of_entities - 1) >> 1; i >= 0; --i) {
    HeapSortEntityPointers_HeapifyXCloudZ(entity_pointers, i, number_of_entities);
  }
}

void HeapSortEntityPointersXCloudZ(Entity** entity_pointers, int number_of_entities) {
  HeapSortEntityPointers_BuildMaxHeapXCloudZ(entity_pointers, number_of_entities);
  for (int i = number_of_entities; i > 0; --i) {
    Entity* temp = entity_pointers[0];
    --number_of_entities;
    entity_pointers[0] = entity_pointers[number_of_entities];
    entity_pointers[number_of_entities] = temp;
    HeapSortEntityPointers_HeapifyXCloudZ(entity_pointers, 0, number_of_entities);
  }
}

int CompareEntityZ(const void* a, const void* b) {
  //int a_z = ((*(Entity**)a)->z.int_value);
  //int b_z = ((*(Entity**)b)->z.int_value);
  //int a_x = ((*(Entity**)a)->x.int_value);
  //int b_x = ((*(Entity**)b)->x.int_value);
  //if (a_z > b_z) {
  //  return a_z - VISIBLE_RANGE > b_z ? +1 : (a_x - b_x);
  //} else { // b_z >= a_z
  //  return b_z - VISIBLE_RANGE > b_x ? -1 : (a_x - b_x);
  //}
  return (*(Entity**)a)->z.int_value - (*(Entity**)b)->z.int_value;
}

int CompareEntityCloud(const void* a, const void* b) {
  return (*(Entity**)a)->proximity_cloud.cloud_id - (*(Entity**)b)->proximity_cloud.cloud_id;
}

void process(Entity** entities[2]) {
  //qsort(entities[0], MAXIMUM_NUMBER_OF_ENTITIES, sizeof(Entity**), CompareEntityX);
  ////HeapSortEntityPointers(entities[0], MAXIMUM_NUMBER_OF_ENTITIES);

  //for (int i = 0; i < MAXIMUM_NUMBER_OF_ENTITIES; ++i) {
  //  Entity* entity = entities[0][i];
  //  for (int j = i+1; j < MAXIMUM_NUMBER_OF_ENTITIES; ++j) {
  //    Entity* compare = entities[0][j];
  //    Number offset = compare->x - entity->x;
  //    if (offset > VISIBLE_RANGE) break;
  //    if (FixedPoint::abs(compare->z - entity->z) <= VISIBLE_RANGE) {
  //      compare->proximity_cloud.visibility_mask = 1;
  //    }
  //    //compare->proximity_cloud.visibility_mask
  //    //  = (compare->x - entity->x < VISIBLE_RANGE &&
  //    //     compare->z - entity->z < VISIBLE_RANGE) ? 1 : 0;
  //  }
  //}

  //return;

  // sort by x
  //HeapSortEntityPointersX(entities[0], MAXIMUM_NUMBER_OF_ENTITIES);
  qsort(entities[0], MAXIMUM_NUMBER_OF_ENTITIES, sizeof(Entity**), CompareEntityX);

  // assign z cloud index
  int cloud_x = 0;
  Number last_x = entities[0][0]->x;
  for (int x = 0; x < MAXIMUM_NUMBER_OF_ENTITIES; ++x) {
    Number entity_x = entities[0][x]->x;
    if (entity_x - last_x > VISIBLE_RANGE) ++cloud_x;
    entities[0][x]->proximity_cloud.cloud_x = cloud_x;
    last_x = entity_x;
  }
  ++cloud_x;

  // sort within cloud by x
  qsort(entities[0], MAXIMUM_NUMBER_OF_ENTITIES, sizeof(Entity**), CompareEntityXCloudZ);
  //HeapSortEntityPointersXCloudZ(entities[0], MAXIMUM_NUMBER_OF_ENTITIES);

  // assign cloud index
  int next_cloud_index = 0;
  Number last_z = entities[0][0]->z;
  for (int x = 0; x < MAXIMUM_NUMBER_OF_ENTITIES;) {
    int cloud = entities[0][x]->proximity_cloud.cloud_x;
    do {
      if (x >= MAXIMUM_NUMBER_OF_ENTITIES) break;
      Number entity_z = entities[0][x]->z;
      Number delta = entity_z - last_z;
      if (delta < 0 || delta > VISIBLE_RANGE) ++next_cloud_index;
      entities[0][x]->proximity_cloud.cloud_id = next_cloud_index;
      last_z = entity_z;
    } while (entities[0][x++]->proximity_cloud.cloud_x == cloud);
    ++next_cloud_index;
  }

  int number_of_clouds = 0;
  for (int i = 0; i < MAXIMUM_NUMBER_OF_ENTITIES;) {
    int cloud_id = entities[0][i]->proximity_cloud.cloud_id;
    int j = i + 1;
    Number min_x = entities[0][i]->x, max_x = min_x;
    Number min_z = entities[0][i]->z, max_z = min_z;
    while (j < MAXIMUM_NUMBER_OF_ENTITIES &&
           entities[0][j]->proximity_cloud.cloud_id == cloud_id) {
      min_x = FixedPoint::minfast(entities[0][j]->x, min_x);
      max_x = FixedPoint::maxfast(entities[0][j]->x, max_x);
      min_z = FixedPoint::minfast(entities[0][j]->x, min_z);
      max_z = FixedPoint::maxfast(entities[0][j]->x, max_z);
      ++j;
    }
    ++number_of_clouds;

    Entity** first_entity = &entities[0][i];
    int number_of_entries = j - i;

    // [i, j) is proximity cloud set
    //DEBUG_INFO("# = %i", number_of_entries);

    // divide up this region into visibility areas
    const int BASE_SIZE = 256;
    const int OVERLAP_SIZE = BASE_SIZE-1;
    int base_mask[BASE_SIZE][BASE_SIZE];
    memset(base_mask, -1, sizeof(base_mask));
    int overlap_mask[OVERLAP_SIZE][OVERLAP_SIZE];
    memset(overlap_mask, -1, sizeof(overlap_mask));
    int next_available_index = 0;
    for (int low = i; low < j; ++low) {
      Entity* entity = entities[0][low];
      entity->proximity_cloud.number_of_entities = number_of_entries;
      entity->proximity_cloud.first_entity = first_entity;
      { // base
        int col = ((entity->x - min_x) / (VISIBLE_RANGE)).wholePart();
        int row = ((entity->z - min_z) / (VISIBLE_RANGE)).wholePart();
        assert(col < BASE_SIZE);
        assert(row < BASE_SIZE);
        int type = base_mask[row][col];
        if (type < 0) {
          type = next_available_index++;
          base_mask[row][col] = type;
        }
        entity->proximity_cloud.visibility_mask |= 1<<type;
      }
      { // overlap
        int col = ((entity->x - min_x - VISIBLE_RANGE / 2)/VISIBLE_RANGE).wholePart();
        int row = ((entity->z - min_z - VISIBLE_RANGE / 2)/VISIBLE_RANGE).wholePart();
        assert(col < OVERLAP_SIZE);
        assert(row < OVERLAP_SIZE);
        if (col >= 0 && row >= 0) {
          int type = overlap_mask[row][col];
          if (type < 0) {
            type = next_available_index++;
            overlap_mask[row][col] = type;
          }
          entity->proximity_cloud.visibility_mask |= 1<<type;
        }
      }
    }
    // make sure everything worked
    //assert(next_available_index <= 32);
    for (int low = i; low < j; ++low) {
      Entity* entity = entities[0][low];
      Number x = entity->x;
      Number z = entity->z;
      for (int k = low + 1; k < j; ++k) {
        Entity* compare_entity = entities[0][k];
        Number compare_x = compare_entity->x - x;
        Number compare_z = compare_entity->z - z;
        if (FixedPoint::abs(compare_x) <= VISIBLE_RANGE &&
            FixedPoint::abs(compare_z) <= VISIBLE_RANGE) {
          assert(entity->proximity_cloud.visibility_mask |
                 compare_entity->proximity_cloud.visibility_mask);
        }
      }
    }

    //Number width = (max_x - min_x) / 8;
    //Number height = (

    /*unsigned int mask = 1;
    for (int low = i; low < j; ++low) {
      entities[0][low]->proximity_cloud.first_entity = first_entity;
      entities[0][low]->proximity_cloud.number_of_entities = number_of_entries;
      unsigned int this_mask
        = mask | entities[0][low]->proximity_cloud.visibility_mask;
      entities[0][low]->proximity_cloud.visibility_mask = this_mask;
      Number x = entities[0][low]->x;
      Number z = entities[0][low]->z;
      for (int k = low; k < j; ++k) {
        // are they already in a visibility set?
        if (entities[0][k]->proximity_cloud.visibility_mask & this_mask) {
          float x_diff = FixedPoint::abs(entities[0][k]->x - x).toFloat();
          float z_diff = FixedPoint::abs(entities[0][k]->z - z).toFloat();
          //assert(FixedPoint::abs(entities[0][k]->x - x) <= VISIBLE_RANGE &&
          //        FixedPoint::abs(entities[0][k]->z - z) <= VISIBLE_RANGE);
          continue;
        }
        // can they see each other?
        //assert(entities[0][k]->x >= x);
        //assert(entities[0][k]->z >= z);
        if (FixedPoint::abs(entities[0][k]->x - x) <= VISIBLE_RANGE &&
            FixedPoint::abs(entities[0][k]->z - z) <= VISIBLE_RANGE) {
          entities[0][k]->proximity_cloud.visibility_mask |= mask;
        }
        //entities[0][k]->proximity_cloud.visibility_mask |= (low - i);
      }
      mask <<= 1;
      DEBUG_INFO("%8i = %8x - %8x", cloud_id, mask, this_mask);
    }*/

    // move to the next cloud
    i = j;
  }

  return;
#if 0
  qsort(entities[0], MAXIMUM_NUMBER_OF_ENTITIES, sizeof(Entity**), CompareEntityX);
  qsort(entities[1], MAXIMUM_NUMBER_OF_ENTITIES, sizeof(Entity**), CompareEntityZ);


  // compare on both axes
  int cloud_x = 0;
  Number last_x = entities[0][0]->x;
  for (int x = 0; x < MAXIMUM_NUMBER_OF_ENTITIES; ++x) {
    Number entity_x = entities[0][x]->x;
    if (entity_x - last_x > VISIBLE_RANGE) ++cloud_x;
    entities[0][x]->proximity_cloud.cloud_x = cloud_x;
    last_x = entity_x;
  }
  ++cloud_x;
  int cloud_z = 0;
  Number last_z = entities[1][0]->z;
  for (int z = 0; z < MAXIMUM_NUMBER_OF_ENTITIES; ++z) {
    Number entity_z = entities[1][z]->z;
    if (entity_z - last_z > VISIBLE_RANGE) ++cloud_z;
    entities[1][z]->proximity_cloud.cloud_id
      = cloud_z * cloud_x + entities[1][z]->proximity_cloud.cloud_x;
    entities[0][z] = entities[1][z];
    last_z = entity_z;
  }

  //std::string clouds;
  //for (int i = 0; i < MAXIMUM_NUMBER_OF_ENTITIES;++i) {
  //  char str[512];
  //  sprintf_s(str, 512, "<%i>", entities[0][i]->proximity_cloud.cloud_id);
  //  assert(i == 0 || (entities[0][i-1]->proximity_cloud.cloud_id <= entities[0][i]->proximity_cloud.cloud_id));
  //  clouds += str;
  //}
  //OutputDebugString(clouds.c_str());
  qsort(entities[0], MAXIMUM_NUMBER_OF_ENTITIES, sizeof(Entity**), CompareEntityCloud);

  // fill in clouds
  int number_of_clouds = 0;
  for (int i = 0; i < MAXIMUM_NUMBER_OF_ENTITIES;) {
    int cloud_id = entities[0][i]->proximity_cloud.cloud_id;
    int j = i + 1;
    while (j < MAXIMUM_NUMBER_OF_ENTITIES &&
           entities[0][j]->proximity_cloud.cloud_id == cloud_id) {
      ++j;
    }
    ++number_of_clouds;

    Entity** first_entity = &entities[0][i];
    int number_of_entries = j - i;

    // [i, j) is proximity cloud set
    unsigned int mask = 1;
    for (int low = i; low < j; ++low) {
      entities[0][low]->proximity_cloud.first_entity = first_entity;
      entities[0][low]->proximity_cloud.number_of_entities = number_of_entries;
      unsigned int this_mask = entities[0][low]->proximity_cloud.visibility_mask;
      entities[0][low]->proximity_cloud.visibility_mask = this_mask | mask;
      Number x = entities[0][low]->x;
      Number z = entities[0][low]->z;
      for (int k = low; k < j; ++k) {
        // are they already in a visibility set?
        if (entities[0][k]->proximity_cloud.visibility_mask & this_mask) continue;
        // can they see each other?
        //assert(entities[0][k]->x >= x);
        //assert(entities[0][k]->z >= z);
        if (FixedPoint::abs(entities[0][k]->x - x) < VISIBLE_RANGE ||
          FixedPoint::abs(entities[0][k]->z - z) < VISIBLE_RANGE) {
          entities[0][k]->proximity_cloud.visibility_mask |= mask;
        }
        //entities[0][k]->proximity_cloud.visibility_mask |= (low - i);
      }
      mask <<= 1;
    }

    // move to the next cloud
    i = j;
  }
#endif
  //DEBUG_INFO("%i clouds",number_of_clouds);
}

int search(Entity* entities) {
  int total = 0;
  for (int i = 0; i < MAXIMUM_NUMBER_OF_ENTITIES; ++i) {
    Entity* entity = &entities[i];
    Entity** prox_entity = entity->proximity_cloud.first_entity;
    int entities = entity->proximity_cloud.number_of_entities;
    total += entity->proximity_cloud.number_of_entities;
    //while (entities--) {
    //  if ((*prox_entity)->proximity_cloud.visibility_mask &
    //              entity->proximity_cloud.visibility_mask) {
    //    ++total;
    //  }
    //  ++prox_entity;
    //}
  }
  return total;
}

//----[  WinMain  ]------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  dcx::dcxWin32Clock clock;
  static const int ENTITY_SETS = 32;
  Entity* random_entities[ENTITY_SETS];
  Entity** entity_lists[ENTITY_SETS][2];
  double processing_times[ENTITY_SETS];
  int number_of_entities[ENTITY_SETS];
  for (int i = 0; i < ENTITY_SETS; ++i) {
    number_of_entities[i] = (1+i) * MAXIMUM_NUMBER_OF_ENTITIES / ENTITY_SETS;
    random_entities[i] = new Entity[MAXIMUM_NUMBER_OF_ENTITIES];
    entity_lists[i][0] = new Entity*[MAXIMUM_NUMBER_OF_ENTITIES];
    entity_lists[i][1] = new Entity*[MAXIMUM_NUMBER_OF_ENTITIES];
  }

  static const int NUMBER_OF_ITERATIONS = 8;

  for (int iteration = 0; iteration < NUMBER_OF_ITERATIONS; ++iteration) {
    for (int i = 0; i < ENTITY_SETS; ++i) {
      randomize(random_entities[i]);
      for (int x = 0; x < MAXIMUM_NUMBER_OF_ENTITIES; ++x) {
        entity_lists[i][0][x] = &random_entities[i][x];
        entity_lists[i][1][x] = &random_entities[i][x];
      }
    }

  #ifdef TEXTOUT
    for (int i = 0; i < ENTITY_SETS; ++i) {
      char presence[MAP_SIZE][MAP_SIZE+2];
      for (int z = 0; z < MAP_SIZE; ++z) {
        for (int x = 0; x < MAP_SIZE; ++x) {
          presence[z][x] = ' ';
        }
        presence[z][MAP_SIZE] = '\n';
        presence[z][MAP_SIZE+1] = '\0';
      }
      
      for (int x = 0; x < MAXIMUM_NUMBER_OF_ENTITIES; ++x) {
        int c = random_entities[i][x].x.wholePart();
        int r = random_entities[i][x].z.wholePart();
        if (presence[r][c] == ' ') {
          presence[r][c] = '1';
        } else if (presence[r][c] >= '1' && presence[r][c] < '9') {
          presence[r][c]++;
        } else {
          presence[r][c] = '*';
        }
      }
      char lol[512];
      sprintf_s(lol, "\n\n====  Entity Set %i ====\n", i);
      OutputDebugString(lol);
      for (int z = 0; z < MAP_SIZE; ++z) {
        OutputDebugString(presence[z]);
      }
    }
  #endif
    for (int i = 0; i < ENTITY_SETS; ++i) {
      processing_times[i] = clock.getAccurateSystemTime();
      MAXIMUM_NUMBER_OF_ENTITIES = number_of_entities[i];
      process(entity_lists[0]);
      processing_times[i]
        = clock.getAccurateSystemTime() - processing_times[i];
    }
  #ifdef TEXTOUT
    for (int i = 0; i < ENTITY_SETS; ++i) {
      char presence[MAP_SIZE][MAP_SIZE+2];
      for (int z = 0; z < MAP_SIZE; ++z) {
        for (int x = 0; x < MAP_SIZE; ++x) {
          presence[z][x] = ' ';
        }
        presence[z][MAP_SIZE] = '\n';
        presence[z][MAP_SIZE+1] = '\0';
      }
      
      for (int x = 0; x < MAXIMUM_NUMBER_OF_ENTITIES; ++x) {
        int c = random_entities[i][x].x.wholePart();
        int r = random_entities[i][x].z.wholePart();
        int cloud = random_entities[i][x].proximity_cloud.cloud_id;
        if (cloud < 10) {
          presence[r][c] = '1' + cloud;
        } else if (cloud < 10 + 26) {
          presence[r][c] = 'A' + cloud - 10;
        } else if (cloud < 10 + 26 + 26) {
          presence[r][c] = 'a' + cloud - (10 + 26);
        } else {
          presence[r][c] = '*';
        }
      }
      char lol[512];
      sprintf_s(lol, "\n\n====  Clouds %i ====\n", i);
      OutputDebugString(lol);
      for (int z = 0; z < MAP_SIZE; ++z) {
        OutputDebugString(presence[z]);
      }
    }
  #endif 
    int total_searches = 0;
    for (int i = 0; i < ENTITY_SETS; ++i) {
      char text[512];
      sprintf_s(text,512,
                "%i\t%f\n",
                 number_of_entities[i],
                 1000.0 * processing_times[i]);
      OutputDebugString(text);
      //total_searches += search(random_entities[i]);
    }
  }
  //DEBUG_INFO("Processing Time:  %f milliseconds (%f /set; %f sets/second); %f searched/entity",
  //           float(1000.0 * (end - start)),
  //           float(1000.0 * (end - start) / double(ENTITY_SETS)),
  //           float(double(ENTITY_SETS)/((end - start))),
  //           float(double(total_searches)/double(MAXIMUM_NUMBER_OF_ENTITIES * ENTITY_SETS)));
  for (int i = 0; i < ENTITY_SETS; ++i) {
    delete[] random_entities[i];
    delete[] entity_lists[i][0];
    delete[] entity_lists[i][1];
  }
  return 1;
}
#endif