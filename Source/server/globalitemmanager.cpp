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
#include "globalitemmanager.h"
#include "worldregion.h"
#include "globaldatabasemanager.h"
#include <dc/debug>
#include "item.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "client.h"
#include "globalworldmanager.h"

#include "randint32.h"
#include "shared/server-editor/server_gamefile.h"

#include "../common/safemacros.h"
#include "avatarinventoryitem.h"
#include "databaseinventoryitem.h"

#include "shared/evidyon_version.h"

// for restart_counter
#include "main.h"

#include <assert.h>

#include "common/fracture/todo.h"


#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

namespace Evidyon {
//
//
//struct TreasureClassDescription {
//  CompiledTreasureClass info;
//  CompiledTreasure* treasure;
//};



GlobalItemManager* GlobalItemManager::singleton_ = NULL;
GlobalItemManager* GlobalItemManager::singleton() {
  return singleton_;
}




//----[  GlobalItemManager  ]--------------------------------------------------
GlobalItemManager::GlobalItemManager() {
  item_descriptions_ = NULL;
  number_of_item_descriptions_ = 0;
  treasure_classes_ = NULL;
  number_of_treasure_classes_ = 0;
}



//----[  create  ]-------------------------------------------------------------
bool GlobalItemManager::create() {
  CONFIRM(item_memory_pool_.create()) else return false;
  singleton_ = this;
  return true;
}




//----[  setNumberOfItemDescriptions  ]----------------------------------------
bool GlobalItemManager::setNumberOfItemDescriptions(size_t number) {
  permanent_hash_to_description_index_table_.clear();
  number_of_item_descriptions_ = number;
  if (number > 0) {
    item_descriptions_ = new Evidyon::Item::ItemServerDescription[number];
    return item_descriptions_ != NULL;
  }
  return true;
}


//----[  setItemDescription  ]-------------------------------------------------
bool GlobalItemManager::setItemDescription(
    size_t index,
    const Evidyon::Item::ItemServerDescription* description) {
  memcpy(&item_descriptions_[index],
         description,
         sizeof(Evidyon::Item::ItemServerDescription));
  PermanentStorageHashToDescriptionIndexTable::_Pairib result =
    permanent_hash_to_description_index_table_.insert(
        PermanentStorageHashToDescriptionIndexTable::value_type(
          description->permanent_storage_hash,
          index));
  assert(result.second);
  return result.second; // must be unique
}

//
////----[  setNumberOfTreasureClasses  ]-----------------------------------------
//bool GlobalItemManager::setNumberOfTreasureClasses(size_t number) {
//  number_of_treasure_classes_ = number;
//  if (number > 0) {
//    treasure_classes_
//      = new TreasureClassDescription[number];
//    return treasure_classes_ != NULL;
//  }
//  return true;
//}
//
//
////----[  setTreasureClass  ]---------------------------------------------------
//bool GlobalItemManager::setTreasureClass(
//    size_t index,
//    const Evidyon::CompiledTreasureClass* description,
//    const Evidyon::CompiledTreasure* treasure) {
//  memcpy(&treasure_classes_[index].info,
//         description,
//         sizeof(Evidyon::CompiledTreasureClass));
//  treasure_classes_[index].treasure
//    = new CompiledTreasure[description->treasure_item_count];
//  for (int i = 0; i < description->treasure_item_count; ++i) {
//    memcpy(&treasure_classes_[index].treasure[i],
//           &treasure[i],
//           sizeof(Evidyon::CompiledTreasure));
//  }
//  return true;
//}

//-----------------------------------------------------------------------------
// Name: destroy
// Desc: Erases the information contained in this class
//-----------------------------------------------------------------------------
void GlobalItemManager::destroy()
{
  singleton_ = NULL;

  //if (treasure_classes_) {
  //  for (size_t i = 0; i < number_of_treasure_classes_; ++i) {
  //    SAFE_DELETE_ARRAY(treasure_classes_[i].treasure);
  //  }

  //  SAFE_DELETE_ARRAY(treasure_classes_);
  //}

  SAFE_DELETE_ARRAY(item_descriptions_);

  //for (ActiveItems::iterator i = active_items_.begin();
  //     i != active_items_.end(); ++i) {
  //  i->first->removeReference();
  //}
  //active_items_.clear();

  item_memory_pool_.destroy();
}


//
//int CalculateRarityIndex(unsigned int random_value) {
//  random_value ^= random_value >> 16; // xor high/low words
//  random_value &= 0xFFFF; // take the low-word only
//
//  //// To initially generate the table algorithmically:
//  //BitsSetTable256[0] = 0;
//  //for (int i = 0; i < 256; i++)
//  //  BitsSetTable256[i] = (i & 1) + BitsSetTable256[i / 2];
//  static const unsigned char BitsSetTable256[] = 
//  {
//    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
//    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
//    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
//    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
//    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
//    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
//    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
//    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
//    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
//    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
//    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
//    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
//    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
//    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
//    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
//    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
//  };
//
//  // calculate the number of bits set, but
//  int rarity_index = BitsSetTable256[ random_value     & 0xFF] +
//                     BitsSetTable256[(random_value>>8) & 0xFF];
//
//  // return abs(rarity_index - 8)
//  rarity_index -= 8;
//  return rarity_index < 0 ? -rarity_index : rarity_index;
//}
//
//
//void GlobalItemManager::dropMonsterTreasure(Map* map,
//                                            int x,
//                                            int y,
//                                            int treasure_class_index,
//                                            unsigned int treasure_randomizer_mask) {
//  CONFIRM(map) else return;
//
//  WorldRegion* region = map->getRegionFromPoint(x, y);
//  CONFIRM(region) else return;
//
//  const TreasureClassDescription* treasure_class =
//      getTreasureClassDescription(treasure_class_index);
//  CONFIRM(treasure_class) else return;
//
//  int treasure_range = 0,
//      number_of_items = 0;
//  {
//    unsigned int treasure_randomizer = rand_uint32();
//    treasure_randomizer &= treasure_randomizer_mask;
//    int treasure_level = CalculateRarityIndex(treasure_randomizer);
//    CONFIRM(treasure_level >=0 && treasure_level < TREASURE_WEIGHTED_RANDOM_RANGE);
//    treasure_range = treasure_class->info.treasure_index_range[treasure_level];
//
//    int item_rarity_index = CalculateRarityIndex(rand_uint32());
//    CONFIRM(item_rarity_index >=0 && item_rarity_index < TREASURE_WEIGHTED_RANDOM_RANGE);
//    number_of_items = treasure_class->info.number_of_items_to_drop[item_rarity_index];
//  }
//
//  for (int i = 0; i < number_of_items; ++i) {
//    int treasure_item_index = rand()%treasure_range;
//    CONFIRM(treasure_item_index < treasure_class->info.treasure_item_count) else continue;
//    unsigned int item_type = treasure_class->treasure[treasure_item_index].item_type;
//    int quantity = 1; // todo
//
//    ItemPointer item_ptr;
//    CONFIRM(acquireNewItem(quantity, item_type, &item_ptr)) else continue;
//    CONFIRM(region->addItemOnGround(false, x, y, item_ptr)) else {
//      releaseItem(&item_ptr);
//    }
//  }
//
//  if (number_of_items > 0) {
//    region->broadcastUpdateItemsOnLocation(x, y);
//  }
//}
//
//




void GlobalItemManager::releaseWorldRegionItems(
    WorldRegion_LocalItems* local_items) {
  WorldRegion_LocalItems::iterator lower = local_items->begin();
  WorldRegion_LocalItems::iterator upper = local_items->end();
  for (WorldRegion_LocalItems::iterator i = lower; i != upper; ++i) {
    releaseItem(&i->second);
  }
  local_items->clear();
}

void GlobalItemManager::releaseWorldRegionItems(
    WorldRegion_LocalItems* local_items,
    WorldRegion_LocalItems::iterator first,
    WorldRegion_LocalItems::iterator last) {
  for (WorldRegion_LocalItems::iterator i = first; i != last; ++i) {
    releaseItem(&i->second);
  }
  local_items->erase(first, last);
}





void GlobalItemManager::releaseAvatarInventory(AvatarInventoryItem* inventory) {
  for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
    inventory[i].equipped = false;
    inventory[i].stackable = false;
    releaseItem(&inventory[i].item);
  }
}


void GlobalItemManager::releaseAvatarInventoryItem(AvatarInventoryItem* inventory_item) {
  inventory_item->equipped = false;
  inventory_item->stackable = false;
  releaseItem(&inventory_item->item);
}



bool GlobalItemManager::translateAvatarItemToDatabaseItem(
    const AvatarInventoryItem* inventory_item,
    DatabaseInventoryItem* database_item) {
  const ItemInstance* item = inventory_item->item.dereference();
  if (NULL == item) return false;
  database_item->id = item->getUniqueID();
  const Evidyon::Item::ItemServerDescription* description = item->getDescription();
  if (NULL == description) return false;
  database_item->permanent_storage_hash = description->permanent_storage_hash;
  database_item->equipped = inventory_item->equipped;
  {
    int quantity = item->getQuantity();
    CONFIRM(quantity > 0) else return false;
    database_item->quantity = quantity;
  }
  return true;
}

bool GlobalItemManager::acquireAvatarItemForDatabaseItem(
    const DatabaseInventoryItem* database_item,
    AvatarInventoryItem* inventory_item) {
  unsigned int item_type;
  if (!translatePermanentStorageHashToItemType(database_item->permanent_storage_hash, &item_type)) return false;
  ItemPointer item_pointer;
  CONFIRM(acquireKnownItem(database_item->id,
                           database_item->quantity,
                           item_type,
                          &item_pointer)) else return false;
  inventory_item->item.copy(item_pointer);
  inventory_item->stackable = item_pointer.dereferenceAssumingValid()->isStackable();
  inventory_item->equipped = database_item->equipped;
  return true;
}


bool GlobalItemManager::translatePermanentStorageHashToItemType(unsigned int hash, unsigned int* item_type) {
  PermanentStorageHashToDescriptionIndexTable::iterator i =
    permanent_hash_to_description_index_table_.find(hash);
  if (i == permanent_hash_to_description_index_table_.end()) return false;
  *item_type = i->second;
  return true;
}


bool GlobalItemManager::translateItemTypeToPermanentStorageHash(unsigned int item_type, unsigned int* hash) {
  if (item_type >= number_of_item_descriptions_) return false;
  *hash = item_descriptions_[item_type].permanent_storage_hash;
  return true;
}


void GlobalItemManager::printDebugStats(char* buffer, size_t buffer_size) {
  sprintf_s(buffer,
            buffer_size,
            "%lu items allocated (%lu available)",
            item_memory_pool_.numberOfAllocatedEntries(),
            item_memory_pool_.numberOfFreeEntries());
}


//
//TreasureClassDescription* GlobalItemManager::getTreasureClassDescription(size_t index) {
//  return index < number_of_treasure_classes_ ? &treasure_classes_[index] : NULL;
//}




//----[  acquireNewItem  ]-----------------------------------------------------
bool GlobalItemManager::acquireNewItem(int quantity,
                                       unsigned int item_type,
                                       ItemPointer* returned_item) {
  ItemIDType unique_id;
  id_generator_.next(&unique_id);
//  DEBUG_INFO("Unique ID = %lu.%lu", unsigned int(unique_id>>32), unsigned int(unique_id));
  return acquireKnownItem(unique_id, quantity, item_type, returned_item);
}



//----[  acquireKnownItem  ]---------------------------------------------------
bool GlobalItemManager::acquireKnownItem(ItemIDType unique_id,
                                         int quantity,
                                         unsigned int item_type,
                                         ItemPointer* returned_item) {
  CONFIRM(item_type < number_of_item_descriptions_) else return false;
  CONFIRM(quantity > 0) else { // assign a default quantity
    quantity = 1;
  }

  const Evidyon::Item::ItemServerDescription* description
    = &item_descriptions_[item_type];

  CONFIRM(item_memory_pool_.acquire(returned_item)) else return false;
  ItemInstance* item = returned_item->dereferenceAssumingValid();
  item->acquire(unique_id, quantity, description);
  return true;
}





//----[  releaseItem  ]--------------------------------------------------------
void GlobalItemManager::releaseItem(ItemPointer* item_pointer) {
  CONFIRM(item_pointer) else return;
  ItemInstance* item = item_pointer->dereference();
  if (item) {
    item->release();
  }
  item_memory_pool_.release(item_pointer);
}



//----[  generateItemID  ]-----------------------------------------------------
void GlobalItemManager::generateItemID(ItemIDType* unique_id) {
  id_generator_.next((void*)unique_id);
}


}
