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
#include "globalbazaarmanager.h"
#include "globaldatabasemanager.h"
#include "globalitemmanager.h"
#include "shared/item/itemserverdescription.h"

#include "common/fracture/todo.h"

namespace Evidyon {




  
//----[ create ]---------------------------------------------------------------
bool GlobalBazaarManager::create(GlobalDatabaseManager* db) {
  zero();
  db_ = db;

  for (unsigned int category = 0; category < CATEGORY_COUNT; ++category) {
    listings_in_category_[category] =
      db->bazaar_getNumberOfItemsInCategory(category);
  }

  rotateListings(); // creates cached listings

  return true;
}







//----[ destroy ]--------------------------------------------------------------
void GlobalBazaarManager::destroy() {
  zero();
}








//----[ rotateListings ]-------------------------------------------------------
void GlobalBazaarManager::rotateListings() {
  for (int category = 0; category < CATEGORY_COUNT; ++category) {
    unsigned int lower = category_page_limits_[category][0];
    unsigned int upper = category_page_limits_[category][1];
    unsigned int max = listings_in_category_[category];
    if (max < MAX_ITEMS_PER_BAZAAR_CATEGORY) {
      // show on single page
      lower = 0;
      upper = max;
    } else {
      if (lower + MAX_ITEMS_PER_BAZAAR_CATEGORY >= max) {
        // reset to the start
        lower = 0;
        upper = MAX_ITEMS_PER_BAZAAR_CATEGORY;
      } else {
        // move to the next screen
        upper += MAX_ITEMS_PER_BAZAAR_CATEGORY;
        if (upper > max) upper = max;
        lower = upper - MAX_ITEMS_PER_BAZAAR_CATEGORY; // should never be < 0
      }
    }

    // variables are unsigned, so this means underflow! this should never
    // actually happen, but fixing it is so trivial it seems worthwhile
    if (lower > upper) {
      lower = 0;
      upper = MAX_ITEMS_PER_BAZAAR_CATEGORY;
    }

    // save limits
    category_page_limits_[category][0] = lower;
    category_page_limits_[category][1] = upper;

    // update the item cache
    updateCategoryCachedListings(category);
  }
}







//----[ getOwnListings ]-------------------------------------------------------
size_t GlobalBazaarManager::getOwnListings(
    unsigned int account_id,
    ItemInBazaar* buffer,
    size_t buffer_size) {
  return db_->bazaar_getOwnListings(account_id, buffer, buffer_size);
}







//----[ getListings ]----------------------------------------------------------
bool GlobalBazaarManager::getListings(
    BazaarCategory category,
    const ItemInBazaar** listings,
    size_t* number_of_listings) {
  unsigned int category_index = (unsigned int)category;
  if (category_index >= CATEGORY_COUNT) return false;
  *number_of_listings = number_of_cached_listings_[category_index];
  *listings = cached_listings_[category_index];
  return true;
}








//----[ purchaseListing ]------------------------------------------------------
bool GlobalBazaarManager::purchaseListing(
    BazaarListingID listing_id,
    CurrencyType currency_before,
    CurrencyType* currency_after,
    ItemPointer* returned_item,
    unsigned int* category_of_item) {

  unsigned int seller_account_id;
  unsigned int category;
  ItemInBazaar listing;
  ItemIDType item_id;

  // obtain the listing, check the price
  if (!db_->bazaar_getListing(listing_id,
                             &seller_account_id,
                             &category,
                             &item_id,
                             &listing)) return false;
  if (listing.price > currency_before) return false;

  db_->bazaar_removeListing(listing.listing_id);
  if (seller_account_id != 0) {
    // Add this as a sale if the account ID was valid.  The account's ID
    // can be zero if the sale is added by the bazaar itself or an NPC.
    db_->bazaar_insertSale(seller_account_id, &listing);
  }
  invalidateCachedListing((unsigned int)category, listing_id);

  // remove listing
  unsigned int listings_in_category = listings_in_category_[category];
  if (listings_in_category > 0) listings_in_category_[category] = listings_in_category - 1;

  // Create the item
  bool acquired =
    GlobalItemManager::singleton()->acquireKnownItem(
      item_id,
      listing.quantity,
      listing.type,
      returned_item);

  // only charge if the item was created successfully
  if (acquired) {
    *currency_after = currency_before - listing.price;
    if (category_of_item) *category_of_item = category;
    return true;
  }

  return false;
}









//----[ createListing ]--------------------------------------------------------
bool GlobalBazaarManager::createListing(
    ItemPointer* item,
    unsigned int owner_account_id,
    CurrencyType price,
    CurrencyType currency_before,
    CurrencyType* currency_after) {
  ItemInstance* item_object = item->dereference();
  const Evidyon::Item::ItemServerDescription* description
    = item_object->getDescription();
  if (!description) return false;
  unsigned int number_of_listed_items
    = db_->bazaar_getNumberOfListedItems(owner_account_id);
  CurrencyType cost_to_list = BazaarCommission(price, number_of_listed_items);
  if (cost_to_list > currency_before) return false;
  unsigned int category = getCategory(description);
  BazaarListingID listing_id;
  listing_generator_.next(&listing_id);
  if (db_->bazaar_addListing(listing_id,
                             category,
                             price,
                             item_object->getUniqueID(),
                             owner_account_id,
                             item_object->getDescription()->permanent_storage_hash,
                             item_object->getQuantity())) {
    *currency_after = currency_before - cost_to_list;
    GlobalItemManager::singleton()->releaseItem(item);
    listings_in_category_[category]++;
    return true;
  }

  return false;
}






//----[ retractListing ]-------------------------------------------------------
bool GlobalBazaarManager::retractListing(
    BazaarListingID listing_id,
    unsigned int owner_account_id, // confirmed as owner
    ItemPointer* returned_item) {

  unsigned int seller_account_id;
  unsigned int category;
  ItemInBazaar listing;
  ItemIDType item_id;

  // obtain the listing
  if (!db_->bazaar_getListing(listing_id,
                             &seller_account_id,
                             &category,
                             &item_id, &listing)) return false;

  // make sure it is the owner trying to retract the listing
  if (seller_account_id != owner_account_id) {
    todo("karl", "red flag!  seller_account_id != owner_account_id");
    return false;
  }

  db_->bazaar_removeListing(listing.listing_id);
  invalidateCachedListing((unsigned int)category, listing_id);

  // remove a listing from this category
  unsigned int listings_in_category = listings_in_category_[category];
  if (listings_in_category > 0) listings_in_category_[category] = listings_in_category - 1;

  // Recreate this item
  bool acquired =
    GlobalItemManager::singleton()->acquireKnownItem(
      item_id,
      listing.quantity,
      listing.type,
      returned_item);

  return acquired;
}






//----[ redeemListing ]--------------------------------------------------------
bool GlobalBazaarManager::redeemListing(
    BazaarListingID listing_id,
    unsigned int seller_account_id, // confirmed as owner
    CurrencyType currency_before,
    CurrencyType* currency_after) {
  CurrencyType profit;
  if (!db_->bazaar_removeSale(listing_id, seller_account_id, &profit)) return false;
  *currency_after = currency_before + profit;
  return true;
}






//----[ updateCategoryCachedListings ]-----------------------------------------
void GlobalBazaarManager::updateCategoryCachedListings(unsigned int category) {
  db_->bazaar_getCategoryListings(category,
                                  category_page_limits_[category][0],
                                  category_page_limits_[category][1],
                                  cached_listings_[category],
                                 &number_of_cached_listings_[category]);
}






//----[ invalidateCachedListing ]----------------------------------------------
void GlobalBazaarManager::invalidateCachedListing(unsigned int category,
                                                  BazaarListingID listing_id) {
  size_t number_of_cached_category_listings = number_of_cached_listings_[category];
  //size_t last_cached_listing = number_of_cached_listings_[category];
  //if (last_cached_listing == 0) return; // cache is empty
  //--last_cached_listing;
  //if (cached_listings_[category][0].listing_id <= listing_id &&
  //    cached_listings_[category][last_cached_listing].listing_id >= listing_id) {
    // ItemInstance is possibly in this cache; try to match it.
    todo("karl", "since we're assuming the list is sorted & elements are "\
                 "unique, we could do a binary search here");
    for (size_t i = 0; i < number_of_cached_category_listings; ++i) {
      ItemInBazaar* listing = &cached_listings_[category][i];
      if (listing->listing_id == listing_id) {
        listing->sold = 1;
        return;
      }
    }
  //}
}






//----[ getCategory ]----------------------------------------------------------
unsigned int GlobalBazaarManager::getCategory(
    const Evidyon::Item::ItemServerDescription* description) {
  switch (description->equipment_type) {
  using namespace Evidyon::Item;
  default: assert(false); return CATEGORY_MISC;
  case ITEMEQUIPMENTTYPE_1H:
  case ITEMEQUIPMENTTYPE_MAINHAND:
  case ITEMEQUIPMENTTYPE_OFFHAND:
  case ITEMEQUIPMENTTYPE_SHIELD:
  case ITEMEQUIPMENTTYPE_JAVELIN:
  case ITEMEQUIPMENTTYPE_THROWN:
  case ITEMEQUIPMENTTYPE_2H:
  case ITEMEQUIPMENTTYPE_POLEARM:
  case ITEMEQUIPMENTTYPE_BOW:
  case ITEMEQUIPMENTTYPE_CROSSBOW:
  case ITEMEQUIPMENTTYPE_DUALWIELD:
  case ITEMEQUIPMENTTYPE_UNARMED:
  case ITEMEQUIPMENTTYPE_SPEAR:
    return CATEGORY_WEAPONS;
  case ITEMEQUIPMENTTYPE_ARMOR:
  case ITEMEQUIPMENTTYPE_HELMET:
  case ITEMEQUIPMENTTYPE_BOOTS:
  case ITEMEQUIPMENTTYPE_GLOVES:
  case ITEMEQUIPMENTTYPE_BELT:
    return CATEGORY_ARMOR;
  case ITEMEQUIPMENTTYPE_RING:
  case ITEMEQUIPMENTTYPE_ACCESSORY:
  case ITEMEQUIPMENTTYPE_NONE:
    return CATEGORY_MISC;
  }
}







//----[ zero ]-----------------------------------------------------------------
void GlobalBazaarManager::zero() {
  db_ = NULL;
  memset(listings_in_category_,      0, sizeof(listings_in_category_));
  memset(category_page_limits_,      0, sizeof(category_page_limits_));
  memset(cached_listings_,           0, sizeof(cached_listings_));
  memset(number_of_cached_listings_, 0, sizeof(number_of_cached_listings_));
}



}