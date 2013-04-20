//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __BAZAAR_H__
#define __BAZAAR_H__
#pragma once



#include "../currencytype.h"

namespace Evidyon {

typedef unsigned __int64 BazaarListingID;


enum BazaarCategory {
  CATEGORY_WEAPONS,
  CATEGORY_ARMOR,
  CATEGORY_MISC,
  CATEGORY_COUNT,
  CATEGORY_OWNITEMS, // special "category"
};

#pragma pack(push,1)
struct ItemInBazaar {
  BazaarListingID listing_id;
  CurrencyType price;
  unsigned short type;

  // If this item was sold, the flag is set.  This field is
  // always 0 unless this structure is being sent to a client
  // viewing his or her own items.
  unsigned sold : 1;

  unsigned short quantity : 15;
};
#pragma pack(pop)


// Limit the number of items in a category to fit into a single packet
static const int MAX_ITEMS_PER_BAZAAR_CATEGORY = 20;


// Calculates the amount of commission a player has to pay in order to
// list an item in the bazaar.  This is used to prevent players from
// listing huge numbers of items--the commission gets extremely high for
// more than a few items.
CurrencyType BazaarCommission(CurrencyType listing_price,
                              unsigned int items_already_listed);

const char* BazaarCategoryString(BazaarCategory category);

}


#endif