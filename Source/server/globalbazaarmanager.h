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
#ifndef __GLOBALBAZAARMANAGER_H__
#define __GLOBALBAZAARMANAGER_H__
#pragma once


#include "shared/client-server/bazaar.h"
#include "main.h"
#include "itempointer.h"

namespace Evidyon {
namespace Item {
struct ItemServerDescription;
}
}


namespace Evidyon {

class GlobalDatabaseManager;


class GlobalBazaarManager {
public:

  // Initializes the bazaar
  bool create(GlobalDatabaseManager* db);
  void destroy();

  // Every few minutes, this method is called so that the listings sent
  // to clients browsing categories are changed.  This is how categories
  // show listings with more than a page of items.
  void rotateListings();

  // Obtains the listings the client has made in the bazaar.  The method
  // returns the number of spaces filled in the buffer.
  size_t getOwnListings(unsigned int account_id,
                        ItemInBazaar* buffer,
                        size_t buffer_size); // MAX_ITEMS_PER_BAZAAR_CATEGORY

  // Gets the listings present in the given bazaar category.  The
  // number of items in the buffer are returned.  It is up to the client
  // to compare listings to see if it is the one listing the items in the
  // given category.
  bool getListings(BazaarCategory category,
                   const ItemInBazaar** listings,
                   size_t* number_of_listings);

  // Attempts to purchase a listing.  This method will only succeed if the
  // currency_before parameter indicates that the buyer has enough money
  // to pay the seller.  The amount of money the buyer has left is saved
  // in *currency_after and 'item' references a valid structure if the
  // method returns 'true'.  If the item is cached, it is marked as 'sold'.
  // If the item cannot be bought, the listing has been removed by the
  // owner or was already purchased and the client should be informed that
  // it is no longer available.
  bool purchaseListing(BazaarListingID listing_id,
                       CurrencyType currency_before,
                       CurrencyType* currency_after,
                       ItemPointer* returned_item,
                       unsigned int* category_of_item);


  // Adds an item listing to the bazaar and destroys the source item pointer.
  // This method also removes the item from the active table (in the main
  // database) so that duping problems with the bazaar are prevented.
  bool createListing(ItemPointer* item,
                     unsigned int owner_account_id,
                     CurrencyType price,
                     CurrencyType currency_before,
                     CurrencyType* currency_after);


  // An owner withdraws a listing from the database.  If the item is being
  // displayed in the cached listings, it is marked as "sold" so other
  // clients can see it is no longer available.
  bool retractListing(BazaarListingID listing_id,
                      unsigned int owner_account_id, // confirmed as owner
                      ItemPointer* returned_item);

  // Owner redeems a sold listing for currency.  Listing is removed from the
  // sales database and money is given to the seller.
  bool redeemListing(BazaarListingID listing_id,
                     unsigned int seller_account_id, // confirmed as owner
                     CurrencyType currency_before,
                     CurrencyType* currency_after);

private:
  void zero();
  void updateCategoryCachedListings(unsigned int category);

  // 
  void invalidateCachedListing(unsigned int category,
                               BazaarListingID listing_id);

  // Returns the category in which an item should be listed
  unsigned int getCategory(
    const Evidyon::Item::ItemServerDescription* description);


private:

  // Link to the main database
  GlobalDatabaseManager* db_;

  // Creates unique listing numbers
  Unique64BitGenerator listing_generator_;

  // tracks listing counts for the category page rotation
  unsigned int listings_in_category_[CATEGORY_COUNT];

  // Limits are the [start, end] of listings queried from the database
  // when the client requests a listing.
  unsigned int category_page_limits_[CATEGORY_COUNT][2];

  // When the listings are rotated, this field is updated.  If an item
  // is purchased or withdrawn that is in the list, it is marked as
  // 'sold' so other clients don't try to buy it.  Since items are
  // sorted by listing ID, finding out if an item is cached is as easy
  // as checking to see if it's between the first and last items of the
  // category in which it was listed.
  // Note: Not only does this make the cache much easier to manage, since
  // it's not being changed constantly, it also should give an interesting
  // sense of activity in the bazaar--you can actually watch items being
  // purchased by others!
  ItemInBazaar cached_listings_[CATEGORY_COUNT][MAX_ITEMS_PER_BAZAAR_CATEGORY];
  size_t number_of_cached_listings_[CATEGORY_COUNT];
};


}




#endif