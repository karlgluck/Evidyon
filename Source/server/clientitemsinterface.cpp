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
#include "clientitemsinterface.h"
#include "globaldatabasemanager.h"
#include "worldregion.h"
#include "randint32.h"
#include "../common/null.h"
#include "shared/server-editor/server_gamefile.h"
#include "globalitemmanager.h"
#include "globalworldmanager.h"
#include "item.h"
#include "encodepackets.h"

#include "common/fracture/todo.h"
#include "common/kg/kg.h"

namespace Evidyon {



  
//----[  zero  ]---------------------------------------------------------------
void ClientItemsInterface::zero() {
  account_id_ = 0;
  character_id_ = 0;
  session_id_ = 0;
  equipment_changed_ = false;
  update_inventory_ = false;
  update_storage_ = false;
  inventory_dirty_bits_.clearAll();
  storage_dirty_bits_.clearAll();
  items_in_inventory_ = 0;
  memset(inventory_, 0, sizeof(inventory_));
  memset(storage_, 0, sizeof(storage_));
}




//----[  checkOut  ]-----------------------------------------------------------
bool ClientItemsInterface::checkOut(unsigned int account_id,
                                    unsigned int character_id) {
  GlobalDatabaseManager* db = GlobalDatabaseManager::singleton();
  unsigned int session_id = rand_uint32();
  account_id_ = account_id;
  character_id_ = character_id;
  session_id_ = session_id;
  equipment_changed_ = true;

  // Withdraw the inventory items
  unsigned int number_of_items;
  if (db->inventoryCheckOut(account_id,
                            character_id,
                            session_id,
                            inventory_,
                           &number_of_items)) {
    items_in_inventory_ = number_of_items;
    inventory_dirty_bits_.setRange(0, number_of_items);
    update_inventory_ = true;
  } else {
    zero();
    return false;
  }

  // Withdraw the storage items
  if (db->storageCheckOut(account_id,
                          character_id,
                          session_id,
                          storage_,
                          &number_of_items)) {
    items_in_storage_ = number_of_items;
    storage_dirty_bits_.setRange(0, number_of_items);
    update_storage_ = true;
  } else {
    db->inventoryCheckIn(account_id,
                         character_id,
                         session_id,
                         inventory_,
                         items_in_inventory_);
    zero();
    return false;
  }

  return true;
}





//----[  checkIn  ]------------------------------------------------------------
bool ClientItemsInterface::checkIn() {
  GlobalDatabaseManager* db = GlobalDatabaseManager::singleton();

  if (!account_id_ ||
      !character_id_ ||
      !session_id_) return false;

  db->inventoryCheckIn(account_id_,
                       character_id_,
                       session_id_,
                       inventory_,
                       items_in_inventory_);

  db->storageCheckIn(account_id_,
                     character_id_,
                     session_id_,
                     storage_,
                     items_in_storage_);

  zero();

  return true;
}





//----[  commit  ]-------------------------------------------------------------
bool ClientItemsInterface::commit() {
  unsigned int account_id = account_id_;
  unsigned int character_id = character_id_;

  if (!checkIn() ||
      !checkOut(account_id, character_id)) return false;

  return true;
}





//----[  giveItem  ]-----------------------------------------------------------
bool ClientItemsInterface::giveItem(ItemPointer* item_pointer,
                                    unsigned int* assigned_index) {
  if (!item_pointer || item_pointer->invalid()) return false;
  ItemInstance* item = item_pointer->dereferenceAssumingValid(); // justified by prev. line

  int source_quantity = item->getQuantity();
  if (source_quantity < 0) return false;

  // stack with existing items
  if (item->isStackable()) {
    unsigned int start_search_index = 0;
    while (item &&
           start_search_index < items_in_inventory_) { // keep the index safe!
      ItemInstance* stackable_item = NULL;
      if (!findStackableItemInInventory(item,
                                       start_search_index,
                                      &start_search_index,
                                      &stackable_item)) break;
      inventory_dirty_bits_.set(start_search_index);
      update_inventory_ = true;
      ++start_search_index;
      item->setQuantity(stackable_item->addQuantity(item->getQuantity()));
      if (item->getQuantity() <= 0) {
        GlobalItemManager::singleton()->releaseItem(item_pointer);
        item = NULL;
      }
    }
  }

  // add to the inventory
  return item ? addToInventory(*item_pointer, assigned_index) : true;
}




//----[  canHoldItems  ]-------------------------------------------------------
bool ClientItemsInterface::canHoldItems(unsigned int number_of_items) const {
  return (Evidyon::AVATAR_INVENTORY_SIZE - items_in_inventory_) >= number_of_items;
}




//----[  removeFromInventory  ]------------------------------------------------
bool ClientItemsInterface::removeFromInventory(unsigned int index,
                                               bool* was_equipped,
                                               ItemPointer* item_pointer) {
  if (inventoryItemIsValid(index)) {
    if (was_equipped) {
      bool equipped = inventory_[index].equipped;
      *was_equipped = equipped;
      equipment_changed_ = equipment_changed_ || equipped;
    }
    item_pointer->copy(inventory_[index].item);
    shiftDownToEraseInInventory(index);
    return true;
  } else {
    return false;
  }
}




//----[  depositInventoryItemInStorage  ]--------------------------------------
bool ClientItemsInterface::depositInventoryItemInStorage(unsigned int source_index) {
  if (!inventoryItemIsValid(source_index)) return false;
  unsigned int destination_index;
  if (!findEmptyStorageIndex(&destination_index)) return false;
  setStorageItem(destination_index, &inventory_[source_index].item);
  shiftDownToEraseInInventory(source_index);
  ++items_in_storage_;
  return true;
}




//----[  withdrawStorageItemToInventory  ]-------------------------------------
bool ClientItemsInterface::withdrawStorageItemToInventory(unsigned int source_index) {
  if (!storageItemIsValid(source_index)) return false;
  unsigned int destination_index;
  if (!findEmptyInventoryIndex(&destination_index)) return false;
  GlobalItemManager* item_manager = GlobalItemManager::singleton();
  ItemPointer item_ptr;
  if (!item_manager->acquireKnownItem(storage_[source_index].id,
                                      storage_[source_index].quantity,
                                      storage_[source_index].type,
                                     &item_ptr)) {
    return false;
  }
  setInventoryItem(destination_index, item_ptr);
  ++items_in_inventory_;
  shiftDownToEraseInStorage(source_index);
  return true;
}


//
//bool ClientItemsInterface::moveInInventory(unsigned int source_index, unsigned int dest_index) {
//  return false; // todo
//}
//
//
//bool ClientItemsInterface::moveInStorage(unsigned int source_index, unsigned int dest_index) {
//  return false; // todo
//}


//----[  updateInventory  ]----------------------------------------------------
bool ClientItemsInterface::updateInventory(NetworkPacket** packet) {
  if (!update_inventory_) return false;
  generateInventoryUpdatePacket(packet);
  return true;
}




//----[  updateStorage  ]------------------------------------------------------
bool ClientItemsInterface::updateStorage(NetworkPacket** packet) {
  if (!update_storage_) return false;
  generateStorageUpdatePacket(packet);
  return true;
}



//----[  refreshStorage  ]-----------------------------------------------------
void ClientItemsInterface::refreshStorage() {
  update_storage_ = true;
  storage_dirty_bits_.setAll();
}



//----[  changeEquipped  ]-----------------------------------------------------
bool ClientItemsInterface::changeEquipped(unsigned int item_index) {
  if (!inventoryItemIsValid(item_index)) return false;

  ItemInstance* item = inventory_[item_index].item.dereference();
  int quantity = item->getQuantity();
  if (!canHoldItems(1) && quantity > 1) return false;

  equipment_changed_ = true;
  inventory_[item_index].equipped = !inventory_[item_index].equipped;
  update_inventory_ = true;
  inventory_dirty_bits_.set(item_index);

  if (quantity > 1) {
    int quantity_to_remove = quantity - 1;
    item->removeQuantity(quantity_to_remove);
    ItemPointer new_item;
    bool succeeded = false;
    if (GlobalItemManager::singleton()->acquireNewItem(
          quantity_to_remove,
          item->getDescription()->type_index,
          &new_item)) {
      unsigned int new_index;
      if (giveItem(&new_item, &new_index)) {
        succeeded = true;
      }
    }
    if (!succeeded) {
      // put the quantity back since this new item doesn't exist
      item->addQuantity(quantity_to_remove);
      // make the item unequipped
      inventory_[item_index].equipped = false;
    }
  }

  return true;
}

//----[  setEquipmentChanged  ]------------------------------------------------
void ClientItemsInterface::setEquipmentChanged() {
  equipment_changed_ = true;
}

//----[  hasEquipmentChanged  ]------------------------------------------------
bool ClientItemsInterface::hasEquipmentChanged() const {
  return equipment_changed_;
}


//----[  resetEquipmentChanged  ]----------------------------------------------
void ClientItemsInterface::resetEquipmentChanged() {
  equipment_changed_ = false;
}



//----[  getNextEquippedItem  ]------------------------------------------------
bool ClientItemsInterface::getNextEquippedItem(
    unsigned int start_search_index,
    unsigned int* item_index,
    ItemInstance** item) {
  unsigned int i = start_search_index;

  int repack_tries = 0;
  while (i < items_in_inventory_) {
    if (inventoryItemIsValid(i) == false) {
      if (repack_tries >= i) break;
      ++repack_tries;
      repackInventory();
    } else {

      if (inventory_[i].equipped) {
        // guaranteed to be safe by inventoryItemIsValid
        *item = inventory_[i].item.dereferenceAssumingValid();
        *item_index = i;
        return true;
      }

      ++i;
    }
  }

  // No more equipped items found
  return false;
}




//----[  consumeItem  ]--------------------------------------------------------
bool ClientItemsInterface::consumeItem(unsigned int index,
                                       Magic::MagicIndex* magic_index) {
  assert(magic_index);
  if (!inventoryItemIsValid(index)) return false;
  ItemInstance* item = inventory_[index].item.dereferenceAssumingValid(); // guaranteed OK
  const Item::ItemServerDescription* description
    = item->getDescription();
  if (description->consumable_magic ==
        Magic::INVALID_MAGIC_INDEX) return false;
  *magic_index = description->consumable_magic;
  if (item->removeQuantity(1) >= 0) {
    // this item has been fully consumed
    GlobalDatabaseManager::singleton()->itemOnAvatarErased(item->getUniqueID());
    GlobalItemManager::singleton()->releaseItem(&inventory_[index].item);
    shiftDownToEraseInInventory(index);
  } else {
    // item is still available
    update_inventory_ = true;
    inventory_dirty_bits_.set(index);
  }
  todo("karl","if equipped consumable and consumed, change equipment");
  return true;
}



//----[  onDeath  ]------------------------------------------------------------
bool ClientItemsInterface::onDeath(Magic::MagicIndex* on_death_magic,
                                   int* wishing_percent) {
  assert(on_death_magic);
  assert(wishing_percent);
  bool found_on_death_magic = false;
  int wishing_percent_sum = 0;

  // iterate backward because we might remove an element
  for (int i = items_in_inventory_; i > 0;) {
    --i;
    if (!inventory_[i].equipped) continue;
    ItemInstance* item = inventory_[i].item.dereference();
    assert(item);
    Magic::MagicIndex death_magic = item->getDescription()->on_death_magic;
    wishing_percent_sum += item->getDescription()->wishing_percent;
    if (!found_on_death_magic &&
        (death_magic != Magic::INVALID_MAGIC_INDEX)) {
      found_on_death_magic = true;
      *on_death_magic = death_magic;
      update_inventory_ = true;
      GlobalDatabaseManager::singleton()
        ->itemOnAvatarErased(item->getUniqueID());
      GlobalItemManager::singleton()
        ->releaseItem(&inventory_[i].item);
      shiftDownToEraseInInventory(i);
    }
  }
  *wishing_percent = wishing_percent_sum;
  return found_on_death_magic;
}



//----[  dropItemsOnDeath  ]---------------------------------------------------
void ClientItemsInterface::dropItemsOnDeath(int equipped_items_to_drop,
                                            Map* map,
                                            int x,
                                            int y) {
  WorldRegion* region = map->acquireRegionFromPoint(x, y);
  if (!region || (items_in_inventory_ == 0)) return;

  // equipped items are temporarily listed here
  kg::Array<AvatarInventoryItem,AVATAR_INVENTORY_SIZE> items_not_dropped;

  // drop all inventory items and count the equipped items
  for (unsigned int i = 0; i < items_in_inventory_; ++i) {
    ItemInstance* item = inventory_[i].item.dereference();
    confirm(item) else continue;
    if (!item->getDescription()->drop_on_death ||
        inventory_[i].equipped) {
      items_not_dropped.add(inventory_[i]);
    } else {
      region->addItemOnGround(true, x, y, inventory_[i].item);
    }
    inventory_[i].item.reset();
    inventory_[i].stackable = false;
    inventory_[i].equipped = false;
  }

  // NOTE: items_in_inventory_ (the private member variable) will hold
  //       the OLD number of items until the end of this method--so
  //       don't touch it!  this is so that we can tell if the # of items
  //       changed.

  // move non-drop items back into the inventory
  unsigned int items_in_inventory = 0;
  for (unsigned int i = 0; i < items_not_dropped.count;) {
    ItemInstance* item = items_not_dropped[i].item.dereference();
    if (!item->getDescription()->drop_on_death) {
      memcpy(&inventory_[items_in_inventory],
             &items_not_dropped[i],
             sizeof(AvatarInventoryItem));
      ++items_in_inventory;
      items_not_dropped[i].item.reset();
      items_not_dropped.swapRemoveMemcpy(i);
    } else {
      ++i;
    }
  }

  // drop equipped items
  if (equipped_items_to_drop >= items_not_dropped.count) { 

    // drop everything
    for (unsigned int i = 0; i < items_not_dropped.count;) {
      ItemInstance* item = items_not_dropped[i].item.dereference();
      region->addItemOnGround(true, x, y, items_not_dropped[i].item);
      items_not_dropped[i].item.reset();
      items_not_dropped.swapRemoveMemcpy(i);
    }

  } else {

    // randomly pick items to drop
    int number_of_items_dropped = 0;
    while (number_of_items_dropped < equipped_items_to_drop) {
      confirm(items_not_dropped.count > 0) else break;
      int slot = rand()%items_not_dropped.count;

      ItemInstance* item = items_not_dropped[slot].item.dereference();
      region->addItemOnGround(true, x, y, items_not_dropped[slot].item);
      items_not_dropped[slot].item.reset();
      items_not_dropped.swapRemoveMemcpy(slot);

      ++number_of_items_dropped;
    }

  }

  // move items that weren't dropped back into the inventory
  for (unsigned int i = 0; i < items_not_dropped.count; ++i) {
    ItemInstance* item = items_not_dropped[i].item.dereference();
    memcpy(&inventory_[items_in_inventory],
           &items_not_dropped[i],
           sizeof(AvatarInventoryItem));
    // If the item provides on-death magic, ensure it is no longer
    // equipped.  This causes Valtus Mederi Charms and the like to
    // only prevent death once (until the user can get back up and
    // re-equip one) even if multiple are equipped.
    if (item->getDescription()->on_death_magic
          != Magic::INVALID_MAGIC_INDEX) {
      inventory_[items_in_inventory].equipped = false;
    }

    ++items_in_inventory;
  }

  /*

  // drop all inventory items and count the equipped items
  unsigned int first_remaining_item = AVATAR_INVENTORY_SIZE,
               last_remaining_item = 0;
  for (unsigned int i = 0; i < items_in_inventory_; ++i) {
    ItemInstance* item = inventory_[i].item.dereference();
    assert(item);
    if (!item->getDescription()->drop_on_death ||
        inventory_[i].equipped) {
      if (i < first_remaining_item) first_remaining_item = i;
      last_remaining_item = i;
    } else {
      region->addItemOnGround(true, x, y, inventory_[i].item);
      inventory_[i].item.reset();
      inventory_[i].stackable = false;
    }
  }

  // will hold the number of items left in the inventory
  int items_in_inventory = 0;

  // copy down remaining equipped items
  for (unsigned int i = first_remaining_item;
                    i <= last_remaining_item;
                  ++i) {
    if (inventory_[i].item.invalid()) continue;
    assert(inventory_[i].equipped ||
          !inventory_[i].item.dereference()->getDescription()->drop_on_death);
    if (i != items_in_inventory) {
      memcpy(&inventory_[items_in_inventory],
             &inventory_[i],
             sizeof(AvatarInventoryItem));
    }
    ++items_in_inventory;
  }

  if (equipped_items_to_drop >= items_in_inventory) {

    int items_remaining = 0;

    // drop everything; leave nothing (except non-drop items)
    for (unsigned int i = 0;
                      i < items_in_inventory;
                    ++i) {
      ItemInstance* item = inventory_[i].item.dereference();
      assert(item);
      if (item->getDescription()->drop_on_death) {
        equipment_changed_ = true;
        region->addItemOnGround(true, x, y, inventory_[i].item);
        inventory_[i].item.reset();
        inventory_[i].stackable = false;
        inventory_[i].equipped = false;
      } else {
        if (i != items_remaining) {
          memcpy(&inventory_[items_remaining],
                 &inventory_[i],
                 sizeof(AvatarInventoryItem));
        }
        ++items_remaining;
      }
    }

    items_in_inventory = items_remaining;

  } else {
    // go in a loop
    assert(items_in_inventory > 0);
    int first_item = rand()%items_in_inventory;
    int i = first_item;
    int number_of_items_dropped = 0;
    do {
      ItemInstance* item = inventory_[i].item.dereference();
      assert(item);
      if (item->getDescription()->drop_on_death) {
        assert(inventory_[i].equipped);
        region->addItemOnGround(true, x, y, inventory_[i].item);
        inventory_[i].item.reset();
        inventory_[i].stackable = false;
        inventory_[i].equipped = false;
        ++number_of_items_dropped;
      }
      i = (i + 1) % items_in_inventory;
    } while (i != first_item &&
             (number_of_items_dropped < equipped_items_to_drop));

    // copy down all remaining valid items, but unequip them
    // if they gave on-death magic
    int remaining_items = 0;
    for (int j = 0; j < items_in_inventory; ++j) {
      if (inventory_[j].item.invalid() == false) {
        if (inventory_[j].item.dereference()->getDescription()->on_death_magic
              != Magic::INVALID_MAGIC_INDEX) {
          // this should cause Valtus Mederi Charms and other special
          // items to unequip themselves when a character dies, but
          // it will leave all non-dropped equipment on the character
          inventory_[j].equipped = false;
        }
        if (j != remaining_items) {
          memcpy(&inventory_[remaining_items],
                 &inventory_[j],
                 sizeof(AvatarInventoryItem));
          ++remaining_items;
        }
      }
    }

    items_in_inventory_ = remaining_items;
  }
*/
  // did anything change?
  if (items_in_inventory != items_in_inventory_) {
    equipment_changed_ = true;
    region->broadcastUpdateItemsOnLocation(x, y);
    inventory_dirty_bits_.setRange(0, items_in_inventory_);
    items_in_inventory_ = items_in_inventory;
    commit();
  }
}




//----[  dropItemOnGround  ]---------------------------------------------------
void ClientItemsInterface::dropItemOnGround(WorldRegion* strict_region,
                                            int x,
                                            int y,
                                            unsigned int item_index) {
  if (!inventoryItemIsValid(item_index)) return;
  if (strict_region->addItemOnGround(true, x, y, inventory_[item_index].item)) {
      equipment_changed_ = equipment_changed_ || inventory_[item_index].equipped;
    shiftDownToEraseInInventory(item_index);
    strict_region->broadcastUpdateItemsOnLocation(x, y);
  }
}




//----[  getInventoryItem  ]---------------------------------------------------
bool ClientItemsInterface::getInventoryItem(unsigned int index, bool* equipped, ItemPointer* item_ptr) {
  if (!inventoryItemIsValid(index)) return false;
  if (equipped) *equipped = inventory_[index].equipped;
  if (item_ptr) item_ptr->copy(inventory_[index].item);
  return true;
}




//----[  inventoryItemIsValid  ]-----------------------------------------------
bool ClientItemsInterface::inventoryItemIsValid(unsigned int index) const {
  return (index < items_in_inventory_) &&
         (inventory_[index].item.invalid() == false);
}




//----[  storageItemIsValid  ]-------------------------------------------------
bool ClientItemsInterface::storageItemIsValid(unsigned int index) const {
  return (index < items_in_storage_) &&
         (storage_[index].quantity > 0);
}




//----[  geosidSacrificeItem  ]------------------------------------------------
bool ClientItemsInterface::geosidSacrificeItem(unsigned int index,
                                               Geosid::Geonite* geonite_value) {
  if (!inventoryItemIsValid(index)) return false;
  AvatarInventoryItem* inventory_item = &inventory_[index];
  if (inventory_item->equipped) return false; // can't sacrifice equipped items
  ItemInstance* item = inventory_item->item.dereferenceAssumingValid();
  *geonite_value = item->getDescription()->geonite_value;
  GlobalItemManager::singleton()->releaseItem(&inventory_item->item);
  shiftDownToEraseInInventory(index);
  return true;
}




//----[  canHoldItems  ]-------------------------------------------------------
bool ClientItemsInterface::canHoldItems(unsigned int number_of_items) {
  return number_of_items < (Evidyon::AVATAR_INVENTORY_SIZE - items_in_inventory_);
}



//----[  splitItem  ]----------------------------------------------------------
bool ClientItemsInterface::splitItem(unsigned int index) {
  if (!inventoryItemIsValid(index)) return false;
  AvatarInventoryItem* inventory_item = &inventory_[index];
  if (!inventory_item->stackable || // item must be stackable...
      inventory_item->equipped) return false; // ...and in the inventory
  ItemInstance* item = inventory_item->item.dereferenceAssumingValid(); // justified by validity check
  if (canHoldItems(1) == false) return false; // 
  const int original_quantity = item->getQuantity();
  if (original_quantity <= 1) return false; // can't split a stack of 1
  int new_quantity = original_quantity >> 1; // divide in half
  int quantity_remaining = original_quantity - new_quantity;

  // Create the new item
  ItemPointer new_item;
  if (!GlobalItemManager::singleton()->acquireNewItem(new_quantity,
                                                      item->getDescription()->type_index,
                                                     &new_item)) {
    return false;
  }

  // Put the new item in the inventory
  if (!addToInventory(new_item, NULL)) {
    GlobalItemManager::singleton()->releaseItem(&new_item);
  }

  // Adjust the quantity of the original item
  item->setQuantity(quantity_remaining);
  return true;
}



//----[  isCarrying  ]---------------------------------------------------------
bool ClientItemsInterface::isCarrying(unsigned int item_type, int quantity) {
  for (unsigned int i = 0; i < items_in_inventory_; ++i) {
    ItemInstance* item = inventory_[i].item.dereference();
    if (!item) {
      repackInventory();
      return false;
    } else {
      if (item->getDescription()->type_index == item_type) {
        quantity -= item->getQuantity();
        if (quantity <= 0) return true;
      }
    }
  }
  return false;
}



//----[  isCarryingNew  ]------------------------------------------------------
bool ClientItemsInterface::isCarryingNew(unsigned int item_type,
                                         int quantity,
                                         ItemIDType oldest_id) {
  for (unsigned int i = 0; i < items_in_inventory_; ++i) {
    ItemInstance* item = inventory_[i].item.dereference();
    if (!item) {
      repackInventory();
      return false;
    } else {
      if (item->getDescription()->type_index == item_type &&
          item->getUniqueID() >= oldest_id) { // consider only new items
        quantity -= item->getQuantity();
        if (quantity <= 0) return true;
      }
    }
  }
  return false;
}



//----[  hasEquipped  ]--------------------------------------------------------
bool ClientItemsInterface::hasEquipped(unsigned int item_type, int quantity) {
  for (unsigned int i = 0; i < items_in_inventory_; ++i) {
    if (inventory_[i].equipped == false) continue;
    ItemInstance* item = inventory_[i].item.dereference();
    if (!item) {
      repackInventory();
      return false;
    } else {
      if (item->getDescription()->type_index == item_type) {
        quantity -= item->getQuantity();
        if (quantity <= 0) return true;
      }
    }
  }
  return false;
}




//----[  eraseItems  ]---------------------------------------------------------
int ClientItemsInterface::eraseItems(unsigned int item_type, int quantity) {
  for (unsigned int i = 0; (quantity > 0) && (i < items_in_inventory_);) {
    ItemInstance* item = inventory_[i].item.dereference();
    if (!item) {
      repackInventory();
      break;
    } else {
      if (item->getDescription()->type_index == item_type) {
        quantity = item->removeQuantity(quantity);
        update_inventory_ = true;
        equipment_changed_ = equipment_changed_ || inventory_[i].equipped;
        inventory_dirty_bits_.set(i);

        // check to see if this item was destroyed by removing
        // the given quantity.
        if (quantity >= 0) {
          GlobalDatabaseManager::singleton()
              ->itemOnAvatarErased(item->getUniqueID());
          GlobalItemManager::singleton()->releaseItem(&inventory_[i].item);
          shiftDownToEraseInInventory(i);
        }

        // If the quantity has been consumed, we're done!
        if (quantity <= 0) return 0;
      } else {
        ++i;
      }
    }
  }

  return quantity;
}



//----[  addToInventory  ]-----------------------------------------------------
bool ClientItemsInterface::addToInventory(ItemPointer& item_pointer,
                                          unsigned int* assigned_index) {
  unsigned int available_index;
  if (item_pointer.invalid() ||
      !findEmptyInventoryIndex(&available_index)) return false;
  setInventoryItem(available_index, item_pointer);
  if (assigned_index) *assigned_index = available_index;
  ++items_in_inventory_;
  return true;
}




//----[  setInventoryItem  ]---------------------------------------------------
void ClientItemsInterface::setInventoryItem(unsigned int index,
                                            ItemPointer& item_pointer) {
  ItemInstance* item = item_pointer.dereferenceAssumingValid();
  inventory_[index].item.copy(item_pointer);
  inventory_[index].stackable = item->isStackable();
  inventory_[index].equipped = false;
  inventory_dirty_bits_.set(index);
  update_inventory_ = true;

  { // become the owner of this item
    //inventory_[index].item.becomeUniquePointer();
    GlobalDatabaseManager::singleton()->itemChangeOwner(item->getUniqueID(),
                                                        account_id_,
                                                        character_id_,
                                                        session_id_);
  }
}




//----[  setStorageItem  ]-----------------------------------------------------
void ClientItemsInterface::setStorageItem(unsigned int index,
                                          ItemPointer* item_pointer) {
  ItemInstance* item = item_pointer->dereferenceAssumingValid();
  storage_[index].id = item->getUniqueID();
  storage_[index].quantity = item->getQuantity();
  storage_[index].type = item->getDescription()->type_index;
  GlobalItemManager::singleton()->releaseItem(item_pointer);
  storage_dirty_bits_.set(index);
  update_storage_ = true;
}




//----[  findEmptyInventoryIndex  ]--------------------------------------------
bool ClientItemsInterface::findEmptyInventoryIndex(unsigned int* index) {
  bool repacked = false;
FIND_NEXT_AVAILABLE_INDEX:
  unsigned int next_available_index = items_in_inventory_;
  if (next_available_index >= AVATAR_INVENTORY_SIZE) return false;
  if (inventoryItemIsValid(next_available_index)) {
    if (repacked) return false;
    repackInventory();
    repacked = true;
    goto FIND_NEXT_AVAILABLE_INDEX; // try again
  }
  *index = next_available_index;
  return true;
}




//----[  findEmptyStorageIndex  ]----------------------------------------------
bool ClientItemsInterface::findEmptyStorageIndex(unsigned int* index) {
  bool repacked = false;
FIND_NEXT_AVAILABLE_INDEX:
  unsigned int next_available_index = items_in_storage_;
  if (next_available_index >= MAX_ITEMS_IN_STORAGE) return false;
  if (storageItemIsValid(next_available_index)) {
    if (repacked) return false;
    repackStorage();
    repacked = true;
    goto FIND_NEXT_AVAILABLE_INDEX; // try again
  }
  *index = next_available_index;
  return true;
}




//----[  findStackableItemInInventory  ]---------------------------------------
bool ClientItemsInterface::findStackableItemInInventory(
    ItemInstance* find_match_for,
    unsigned int start_search_index,
    unsigned int* found_index,
    ItemInstance** current_item) {
  if (!find_match_for) return false; // no item?
  const Evidyon::Item::ItemServerDescription* description
    = find_match_for->getDescription();
  if (description->max_stack_size <= 1) return false; // not stackable?

  unsigned int i = start_search_index;
  while (i < items_in_inventory_) {
    ItemInstance* item = inventory_[i].item.dereference();
    if (!item) {
      // This item is invalid, but is listed as valid--fix the problem,
      // then try again.  This will loop until no problems are found
      // or the list is emptied because all are bugged.
      repackInventory();
    } else {
      if (item->getDescription() == description &&
          !inventory_[i].equipped) { // don't stack equipped items
        *found_index = start_search_index;
        *current_item = item;
        return true;
      } else {
        ++i;
      }
    }
  }
  return false;
}


//bool ClientItemsInterface::findStackableItemInStorage(
//    ItemInstance* find_match_for,
//    unsigned int start_search_index,
//    unsigned int* found_index,
//    ItemInstance** current_item) {
//  return false; // todo
//}




//----[  shiftDownToEraseInInventory  ]----------------------------------------
void ClientItemsInterface::shiftDownToEraseInInventory(int index_to_erase) {
  inventory_dirty_bits_.setRange(index_to_erase, items_in_inventory_);
  update_inventory_ = true;
  int items_to_move = items_in_inventory_ - index_to_erase - 1;
  if (items_to_move > 0) {
    memcpy(&inventory_[index_to_erase],
           &inventory_[index_to_erase + 1],
           sizeof(ServerStorageItem) * items_to_move);
    inventory_[items_in_inventory_].item.reset();
  }
  --items_in_inventory_;
  inventory_[items_in_inventory_].item.reset();
}




//----[  shiftDownToEraseInStorage  ]------------------------------------------
void ClientItemsInterface::shiftDownToEraseInStorage(int index_to_erase) {
  storage_dirty_bits_.setRange(index_to_erase, items_in_storage_);
  update_storage_ = true;
  int items_to_move = items_in_storage_ - index_to_erase - 1;
  if (items_to_move > 0) {
    memcpy(&storage_[index_to_erase],
           &storage_[index_to_erase + 1],
           sizeof(ServerStorageItem) * items_to_move);
  }
  storage_[items_in_storage_].quantity = -1;
  storage_[items_in_storage_].type = 0;
  --items_in_storage_;
  storage_[items_in_storage_].quantity = -1;
  storage_[items_in_storage_].type = 0;
}




//----[  generateInventoryUpdatePacket  ]--------------------------------------
void ClientItemsInterface::generateInventoryUpdatePacket(NetworkPacket** packet) {
  todo("karl","this implementation should only send the dirty entries");

  using namespace Evidyon::Network::Packet::Server;
  update_inventory_ = false;
  inventory_dirty_bits_.clearAll();
  int indices[AVATAR_INVENTORY_SIZE];
  int number_of_indices = 0;
  for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
    // not necessary b/c Encode::updateInventory auto-skips invalid entries
    //if (inventory_[i].item.invalid()) {
    //  repackInventory();
    //  i = 0;
    //  continue;
    //}
    indices[number_of_indices++] = i;
  }

  *packet = Encode::updateInventory(true,
                                    inventory_,
                                    indices,
                                    number_of_indices);
}




//----[  generateStorageUpdatePacket  ]----------------------------------------
void ClientItemsInterface::generateStorageUpdatePacket(NetworkPacket** packet) {

  todo("karl","this implementation should only send the dirty entries");

  update_storage_ = false;
  storage_dirty_bits_.clearAll();

  int number_of_items = items_in_storage_;

  // build the storage
  ItemInStorage storage[Evidyon::MAX_ITEMS_IN_STORAGE];
  for (int i = 0; i < number_of_items; ++i) {
    storage[i].index = i;
    storage[i].quantity = storage_[i].quantity;
    storage[i].type = storage_[i].type;
  }

  using namespace Evidyon::Network::Packet::Server;
  *packet = Encode::storageTell(storage, number_of_items);
}




//----[  repackInventory  ]----------------------------------------------------
void ClientItemsInterface::repackInventory() {
  GlobalItemManager* item_manager = GlobalItemManager::singleton();

  // Erase any empty indices in the inventory
  unsigned int i = 0;
  while (i < items_in_inventory_) {
    ItemInstance* item = inventory_[i].item.dereference();
    if (!item) {
      // be sure we don't leak memory, then erase the item
      item_manager->releaseAvatarInventoryItem(&inventory_[i]);
      shiftDownToEraseInInventory(i);
    } else {
      ++i;
    }
  }

  // Make sure any other indices are invalidated
  while (i < Evidyon::AVATAR_INVENTORY_SIZE) {
    item_manager->releaseAvatarInventoryItem(&inventory_[i]);
    ++i;
  }

  // Update everything
  inventory_dirty_bits_.setAll();
}




//----[  repackStorage  ]------------------------------------------------------
void ClientItemsInterface::repackStorage() {
#if defined(DEBUG) || defined(_DEBUG)
  __asm int 3;
#endif
}



}