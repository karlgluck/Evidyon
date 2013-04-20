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
#ifndef __CLIENTITEMSINTERFACE_H__
#define __CLIENTITEMSINTERFACE_H__
#pragma once


#include "shared/networkdecl.h"
#include <dcx/bitmask>
#include "shared/avatar/evidyon_avatarinventory.h"
#include "shared/evidyon_storage.h"
#include "avatarinventoryitem.h"
#include "serverstorage.h"
#include "itempointer.h"
#include "shared/geosid/geonite.h"
#include "shared/magic/magicindex.h"

namespace Evidyon {
class GlobalDatabaseManager;
class WorldRegion;
class Map;



//----[  ClientItemsInterface  ]-----------------------------------------------
class ClientItemsInterface {
public:

  // Clears the class.  This method is NOT safe to call at any time--it assumes
  // that all internal information is invalid and overwrites it with default
  // values that are detectable as such.
  void zero();

  // Copies items from the database and puts them into this controller.  The
  // items are moved into the "active" table in the database so that if this
  // class disappears without being checked in, they don't disappear.
  // This interface's session ID is generated automatically.
  // Items cannot be checked out if they are currently active.
  bool checkOut(unsigned int account_id,
                unsigned int character_id);

  // Writes items to the database.  Only items that are not in the "active"
  // table or are in the table but are owned by this account, character and
  // session can be checked in.
  bool checkIn();

  // Performs a checkin/checkout so that all inventory items are now in
  // the database and marked in the active table.  This should be done
  // every few minutes to prevent items from being lost.
  bool commit();

  // Gives an item to the client.  If this isn't possible, 'false' is returned.
  // This method does NOT automatically perform stacking; for a method that
  // does, see giveItem.
  bool addToInventory(ItemPointer& item_pointer, unsigned int* assigned_index);

  // Puts an item in inventory, but checks to see if the item can be stacked
  // with another already there.  If the item can be completely stacked,
  // it will be erased from the database.  If it is incompletely stacked or
  // not stackable, addToInventory internally handles the remaining quantity.
  // If this method returns 'false', the item is still valid--but its
  // quantity may have changed.
  // assigned index is only returned with a valid value if calling the method
  // item_pointer->invalid() returns false (i.e. it was incompletely stacked)
  bool giveItem(ItemPointer* item_pointer, unsigned int* assigned_index);

  // Returns 'true' if there is enough space in inventory for the given number
  // of items.  This is important for bulk transfers, such as when a player to
  // player trade is being executed.
  bool canHoldItems(unsigned int number_of_items) const;

  // Removes an item from this avatar's inventory.  was_equipped is an optional
  // paramter; however, item_pointer is not.
  bool removeFromInventory(unsigned int index,
                           bool* was_equipped,
                           ItemPointer* item_pointer);

  // Moves an item from inventory to storage.
  bool depositInventoryItemInStorage(unsigned int source_index);

  // Moves an item from storage to inventory
  bool withdrawStorageItemToInventory(unsigned int source_index);

  // Changes the position of an item in the inventory
  bool moveInInventory(unsigned int source_index, unsigned int dest_index);

  // Changes the position of an item in storage
  bool moveInStorage(unsigned int source_index, unsigned int dest_index);

  // Poll these to see if an item needs to be updated. 
  bool updateInventory(NetworkPacket** packet);
  bool updateStorage(NetworkPacket** packet);

  // Updates the client's storage and inventory next time the update
  // methods are invoked.
  void refreshStorage();

  // Either equips or unequips an item
  bool changeEquipped(unsigned int item_index);

  // Force equipment update
  void setEquipmentChanged();

  // This is set to 'true' whenever the set of equipped items changes, and
  // it is not cleared until resetEquipmentChanged is called.
  bool hasEquipmentChanged() const;

  // Clears the flag that hasEquipmentChanged returns.
  void resetEquipmentChanged();

  // Used to look through the items this client has equipped so that their
  // bonuses can be applied.
  bool getNextEquippedItem(unsigned int start_search_index,
                           unsigned int* item_index,
                           ItemInstance** item);

  // Reduces the quantity of the item in a given slot.  If this could be done,
  // the method returns 'true' and sets *magic_index to the type of magic
  // consuming the item generates.
  bool consumeItem(unsigned int index, Magic::MagicIndex* magic_index);

  // Trys to find a equipped items to consume that generates on-death magic.
  // This will also sum the number of "wishing" items.
  bool onDeath(Magic::MagicIndex* on_death_magic, int* wishing_percent);

  // Puts items from this character on the ground.  Automatically performs
  // a commit.
  void dropItemsOnDeath(int equipped_items_to_drop,
                        Map* map,
                        int x,
                        int y);

  // Puts items on the ground from the inventory.  Will not drop items that
  // are equipped.
  void dropItemOnGround(WorldRegion* strict_region,
                        int x,
                        int y,
                        unsigned int item_index);

  // Both 'equipped' and 'item_ptr' are optional parameters
  bool getInventoryItem(unsigned int index, bool* equipped, ItemPointer* item_ptr);

  // Is the item in the inventory location 'index' valid?
  bool inventoryItemIsValid(unsigned int index) const;

  // Is the item in the storage location 'index' valid?
  bool storageItemIsValid(unsigned int index) const;

  // Erases an item from inventory (and the database).  The geonite value if the item
  // is returned.
  bool geosidSacrificeItem(unsigned int index,
                           Geosid::Geonite* geonite_value);

  // Finds out whether or not there is enough space in the client's
  // inventory to hold the given number of items.
  bool canHoldItems(unsigned int number_of_items);

  // Splits the quantity of the item at the given index into two
  // stacks.  If it can't be done (due to either inventory full
  // or invalid item) then nothing happens and the method returns
  // 'false'.  If it returns 'true', the client should have its
  // list of items updated.
  bool splitItem(unsigned int index);

  // Returns true if the user has 'quantity' items of 'item_type' either
  // equipped or somewhere in the inventory.
  bool isCarrying(unsigned int item_type, int quantity);

  // Returns true if the user is carrying the given quantity of items of
  // item_type.  However, this method only considers items that are
  // newer than oldest_id, as defined by having ID numbers that are greater
  // than the provided value.  This method was created for use by quests.
  bool isCarryingNew(unsigned int item_type,
                     int quantity,
                     ItemIDType oldest_id);

  // Returns true if the user has 'quantity' items of 'item_type' equipped
  bool hasEquipped(unsigned int item_type, int quantity);

  // Erases items of 'item_type' from this client's inventory until
  // 'quantity' items are erased.  This method returns the number of
  // items that were NOT erased (so if it succeeds it returns 0).
  // This method is used by the quest initiation logic after isCarrying to
  // enact the cost of starting a quest.
  int eraseItems(unsigned int item_type, int quantity);


private:

  // Since this method is private, it does NOT check the validity of
  // item_pointer or index and the caller must be sure it is passing
  // valid arguments.
  void setInventoryItem(unsigned int index, ItemPointer& item_pointer);

  // Searches for a place to put a new item
  bool findEmptyInventoryIndex(unsigned int* index);
  bool findEmptyStorageIndex(unsigned int* index);

  // Puts an item into storage at the given index.  Will automatically
  // deallocate the item provided as a parameter.  Like setInventoryItem,
  // this method must be called with valid arguments or it will cause
  // memory access errors.
  void setStorageItem(unsigned int index, ItemPointer* item_pointer);

  // Searches for a compatible item with which to stack another item
  bool findStackableItemInInventory(ItemInstance* find_match_for,
                                    unsigned int start_search_index,
                                    unsigned int* found_index,
                                    ItemInstance** current_item);
  bool findStackableItemInStorage(ItemInstance* find_match_for,
                                  unsigned int start_search_index,
                                  unsigned int* found_index,
                                  ItemInstance** current_item);

  // Copies items down in the given list so that the index is erased.
  // Be absolutely sure that the item has been deallocated before doing this!
  void shiftDownToEraseInInventory(int index_to_erase);
  void shiftDownToEraseInStorage(int index_to_erase);

  // Creates an update packet to be sent to the client in order to synchronize
  // it.  These methods clear the dirty bits in the lists and reset the
  // update flags.
  void generateInventoryUpdatePacket(NetworkPacket** packet);
  void generateStorageUpdatePacket(NetworkPacket** packet);

  // Re-packs the items in the item list because the number-of-items variable
  // has become unsynchronized with the actual contents of the list.
  void repackInventory();
  void repackStorage();



private:
  unsigned int account_id_, character_id_, session_id_;
  bool equipment_changed_;
  bool update_inventory_, update_storage_;
  dcx::dcxBitMask<Evidyon::AVATAR_INVENTORY_SIZE> inventory_dirty_bits_;
  dcx::dcxBitMask<Evidyon::MAX_ITEMS_IN_STORAGE> storage_dirty_bits_;

  unsigned int items_in_inventory_, items_in_storage_;
  AvatarInventoryItem inventory_[Evidyon::AVATAR_INVENTORY_SIZE];
  ServerStorageItem storage_[Evidyon::MAX_ITEMS_IN_STORAGE];
};



}


#endif