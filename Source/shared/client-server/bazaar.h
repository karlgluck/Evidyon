//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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