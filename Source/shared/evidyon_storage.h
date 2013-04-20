#ifndef __EVIDYON_STORAGE_H__
#define __EVIDYON_STORAGE_H__
#pragma once




namespace Evidyon {

static const int MAX_ITEMS_IN_STORAGE = 64;


#pragma pack(push, 1)

struct ItemInStorage {
  unsigned index    : 8;
  unsigned quantity : 8;
  unsigned type     : 16; // set to all 1's if this entry should be cleared
};

#pragma pack(pop)


}



#endif