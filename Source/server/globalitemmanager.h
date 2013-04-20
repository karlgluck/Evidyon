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
#ifndef __GLOBALITEMMANAGER_H__
#define __GLOBALITEMMANAGER_H__
#pragma once


#include <dcx/managedmemorypool>
#include <map>
#include "itemidtype.h"
#include "shared/evidyon_world.h"
#include <hash_map>
#include "worldregion_items.h"
#include "itempointer.h"
#include "item.h"

#include "shared/item/itemserverdescription.h"


#include "main.h" // for Unique64BitIDGenerator

struct _iobuf;
typedef struct _iobuf FILE;


namespace Evidyon {

class WorldRegion;
class Client;
struct AvatarInventoryItem;
struct CompiledTreasureClass;
struct CompiledTreasure;
class Map;
struct AvatarInventoryItem;
struct DatabaseInventoryItem;

// 16 bits per item
// quality: 
// quantity: 
// qualtiyquantity 1 byte each

struct TreasureClassDescription;


class GlobalItemManager {
public:
  //static const int MAX_ITEMS = 1<<16;
  static const int MAX_ITEMS = 1<<13; // 8192

private:
  typedef dcx::dcxManagedMemoryPool<ItemInstance, MAX_ITEMS> ItemMemoryPool;
  typedef stdext::hash_map<unsigned int, unsigned int>
    PermanentStorageHashToDescriptionIndexTable;

public:
  static GlobalItemManager* singleton();


public:
  GlobalItemManager();
  bool create();
  void destroy();
  bool setNumberOfItemDescriptions(size_t number);
  bool setItemDescription(
      size_t index,
      const Evidyon::Item::ItemServerDescription* description);
  bool setNumberOfTreasureClasses(size_t number);
  bool setTreasureClass(
      size_t index,
      const Evidyon::CompiledTreasureClass* description,
      const Evidyon::CompiledTreasure* treasure);

  // Creates items on the ground when a monster dies
  void dropAvatarItemsOnDeath(Map* map,
                              int x,
                              int y,
                              AvatarInventoryItem* inventory_item);

  // Called by a WorldRegion when it is being released to erase all
  // of the items in its local set.
  void releaseWorldRegionItems(WorldRegion_LocalItems* local_items);
  void releaseWorldRegionItems(WorldRegion_LocalItems* local_items,
                               WorldRegion_LocalItems::iterator first,
                               WorldRegion_LocalItems::iterator last);

  // Tries to stack the quantity from "source" into "destination".  If the
  // method has an effect on either item, the return value is 'true'.  There
  // is a possibility that the item stored in 'source' is invalidated as
  // a result of this operation.
  //bool stack(AvatarInventoryItem* destination,
  //           AvatarInventoryItem* source);

  // Erases all items from the client's inventory
  void releaseAvatarInventory(AvatarInventoryItem* inventory);

  // Erase a single item from the avatar's inventory
  void releaseAvatarInventoryItem(AvatarInventoryItem* inventory_item);

  // Reduces the quantity of this item.  If the quantity is 0 or this entry
  // is not stackable, the location is reset and the item resource freed.
  // Returns 'true' if an item was successfully consumed.
  //bool consumeAvatarInventoryItem(AvatarInventoryItem* inventory_item);

  // Takes the data from an item in the avatar's inventory and creates a
  // structure containing information that is saved to the database
  // If the return value is 'false', the translation failed.
  bool translateAvatarItemToDatabaseItem(const AvatarInventoryItem* inventory_item,
                                         DatabaseInventoryItem* database_item);

  // Creates a new item and configures the inventory slot for an item from
  // the database.  If the item couldn't be acquired, the return value is 'false'
  bool acquireAvatarItemForDatabaseItem(const DatabaseInventoryItem* database_item,
                                        AvatarInventoryItem* inventory_item);

  bool translatePermanentStorageHashToItemType(unsigned int hash, unsigned int* item_type);
  bool translateItemTypeToPermanentStorageHash(unsigned int item_type, unsigned int* hash);

  // Writes information about this class useful to a debugging the server.  This data
  // is returned to the admin console.
  void printDebugStats(char* buffer, size_t buffer_size);

  inline int getNumberOfItemTypes() { return number_of_item_descriptions_; }

public:
  bool acquireNewItem(int quantity, unsigned int item_type, ItemPointer* returned_item);
  bool acquireKnownItem(ItemIDType unique_id,
                        int quantity,
                        unsigned int item_type,
                        ItemPointer* returned_item);
  void releaseItem(ItemPointer* item_pointer);

  // Returns the next unique item ID number.  This is public so that the quest
  // manager can grab an item ID in order to log a starting point for the quest.
  // This makes it so that only items that qualify for completing the quest are
  // ones that are generated after the quest starts.
  void generateItemID(ItemIDType* unique_id);

private:
  TreasureClassDescription* getTreasureClassDescription(size_t index);



private:
  ItemMemoryPool item_memory_pool_;

  // This is built when the game-file is loaded to interpret saved avatar item
  // hashes into item types whose descriptions can be directly indexed.
  PermanentStorageHashToDescriptionIndexTable
    permanent_hash_to_description_index_table_;

  Evidyon::Item::ItemServerDescription* item_descriptions_;
  size_t number_of_item_descriptions_;
  TreasureClassDescription* treasure_classes_;
  size_t number_of_treasure_classes_;

  Unique64BitGenerator id_generator_;

private:
  static GlobalItemManager* singleton_;
};

}

#endif