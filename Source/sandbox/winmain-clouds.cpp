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
#if 0
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
#include <dcx/bitmask>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "dcx_d.lib")



enum {
  MAXIMUM_ENTITIES = 4096*2,
  ITERATIONS = 8,
  STEPS = 32,
  MAP_SIZE = 2048,
  VISIBLE_RANGE = 16,
};


struct Entity {
  Number x, z;
  Entity** nearby_entities_begin;
  Entity** nearby_entities_end;
};


//----[  randomize  ]----------------------------------------------------------
void initialize(Entity* entities,
                Entity** entity_pointers,
                size_t number_of_entities,
                Entity** proximity_table) {
  for (int i = 0; i < number_of_entities; ++i) {
    entity_pointers[i] = &entities[i];
    entities[i].x.int_value = rand_uint32() % (2048 << 16);
    entities[i].z.int_value = rand_uint32() % (2048 << 16);
    entities[i].nearby_entities_begin = &proximity_table[i*number_of_entities];
    entities[i].nearby_entities_end   = &proximity_table[i*number_of_entities];
  }
}

//
//#include < iostream.h >
//#include < conio.h >
//int heapSize = 10;
//void print(int a[]) {
//  for (int i = 0; i <= 9; i++) {
//    cout << a[i] << "-";
//  }
//  cout << endl;
//}
//
//int parent(int i) {
//if(i==1)
//return 0;
//
//if(i%2==0)
//    return ( (i / 2)-1);
//else
//    return ( (i / 2));
//}
//
//int left(int i) {
//  return (2 * i) + 1;
//}
//
//int right(int i) {
//  return (2 * i) + 2;
//}
//
//void heapify(int a[], int i) {
//  int l = left(i), great;
//  int r = right(i);
//  if ( (a[l] > a[i]) && (l < heapSize)) {
//    great = l;
//  }
//  else {
//    great = i;
//  }
//  if ( (a[r] > a[great]) && (r < heapSize)) {
//    great = r;
//  }
//  if (great != i) {
//    int temp = a[i];
//    a[i] = a[great];
//    a[great] = temp;
//    heapify(a, great);
//  }
//}
//
//void BuildMaxHeap(int a[]) {
//  for (int i = (heapSize - 1) / 2; i >= 0; i--) {
//    heapify(a, i);
//    print(a);
//  }
//}
//
//void HeapSort(int a[]) {
//  BuildMaxHeap(a);
//  for (int i = heapSize; i > 0; i--) {
//    int temp = a[0];
//    a[0] = a[heapSize - 1];
//    a[heapSize - 1] = temp;
//    heapSize = heapSize - 1;
//    heapify(a, 0);
//  }
//
//}
//
//void main() {
//
//  int arr[] = {
//      2, 9, 3, 6, 1, 4, 5, 7, 0, 8};
//  HeapSort(arr);
//  print(arr);
//}

bool HeapSortEntityPointers_Greater(Entity* a, Entity* b) {
  return a->x.int_value > b->x.int_value;
}

void HeapSortEntityPointers_Heapify(Entity** a, int i, int size) {
  int l = (2 * i) + 1, great;
  int r = (2 * i) + 2;
  if ((l < size) && HeapSortEntityPointers_Greater(a[l], a[i])) {
    great = l;
  } else {
    great = i;
  }
  if ((r < size) && HeapSortEntityPointers_Greater(a[r], a[great])) {
    great = r;
  }
  if (great != i) {
    Entity* temp = a[i];
    a[i] = a[great];
    a[great] = temp;
    HeapSortEntityPointers_Heapify(a, great, size);
  }
}

void HeapSortEntityPointers_BuildMaxHeap(Entity** entity_pointers, int number_of_entities) {
  for (int i = (number_of_entities - 1) >> 1; i >= 0; --i) {
    HeapSortEntityPointers_Heapify(entity_pointers, i, number_of_entities);
  }
}

void HeapSortEntityPointers(Entity** entity_pointers, int number_of_entities) {
  HeapSortEntityPointers_BuildMaxHeap(entity_pointers, number_of_entities);
  for (int i = number_of_entities; i > 0; --i) {
    Entity* temp = entity_pointers[0];
    --number_of_entities;
    entity_pointers[0] = entity_pointers[number_of_entities];
    entity_pointers[number_of_entities] = temp;
    HeapSortEntityPointers_Heapify(entity_pointers, 0, number_of_entities);
  }
}




//----[  CompareEntityX  ]-----------------------------------------------------
int CompareEntityX(const void* a, const void* b) {
  return (*(Entity**)a)->x.int_value - (*(Entity**)b)->x.int_value;
}


//----[  process  ]------------------------------------------------------------
// Order the entities in the X direction
// Create sets of entities that can see each other by comparing the Z direction
void process(Entity** entity_pointers,
             size_t number_of_entities) {
  //qsort(entity_pointers, number_of_entities, sizeof(Entity*), CompareEntityX);
  HeapSortEntityPointers(entity_pointers, number_of_entities);
  for (size_t i = 0; i < number_of_entities; ++i) {
    Entity* entity = entity_pointers[i];
    for (size_t j = i + 1; j < number_of_entities; ++j) {
      Entity* compare = entity_pointers[j];
      if (compare->x - entity->x > VISIBLE_RANGE) break;
      if (FixedPoint::abs(compare->z - entity->z) <= VISIBLE_RANGE) {
        *(entity->nearby_entities_end++) = compare;
        *(compare->nearby_entities_end++) = entity;
      }
    }
  }
}


//----[  WinMain  ]------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  dcx::dcxWin32Clock clock;
  clock.getAccurateSystemTime();
  Entity* entities = new Entity[MAXIMUM_ENTITIES];
  Entity** entity_pointers = new Entity*[MAXIMUM_ENTITIES];
  Entity** proximity_table = new Entity*[MAXIMUM_ENTITIES*MAXIMUM_ENTITIES];
  for (int setup = 0; setup >= 0; --setup) {
    for (int iteration = 0; iteration < ITERATIONS; ++iteration) {
      for (int step = 0; step < STEPS; ++step) {
        size_t number_of_entities = MAXIMUM_ENTITIES * (step+1) / STEPS;
        initialize(entities,
                   entity_pointers,
                   number_of_entities,
                   proximity_table);
        double duration = clock.getAccurateSystemTime();
        process(entity_pointers, number_of_entities);
        double end = clock.getAccurateSystemTime();
        duration = end - duration;
        
        if (setup == 0) {
          char data[512];
          sprintf_s(data, 512, "%i\t %f\n", number_of_entities, (1000.0 * duration));
          OutputDebugString(data);
        }
      }
    }
  }
  delete[] entities;
  delete[] entity_pointers;
  delete[] proximity_table;

  return 0;
}
#endif