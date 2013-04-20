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
#include "bazaar.h"

namespace Evidyon {


// Calculates the amount of commission a player has to pay in order to
// list an item in the bazaar.  This is used to prevent players from
// listing huge numbers of items--the commission gets extremely high for
// more than a few items.
CurrencyType BazaarCommission(CurrencyType listing_price,
                              unsigned int items_already_listed) {

  // only take commission up to a certain price
  static const CurrencyType MAXIMUM_LISTING_PRICE = 1000000;
  listing_price = listing_price < MAXIMUM_LISTING_PRICE ?
    listing_price : MAXIMUM_LISTING_PRICE;
  switch (items_already_listed) {
    case 0: return 0;
    case 1: return listing_price / 100;        // 1%
    case 2: return 5 + listing_price / 20;     // 5%
    case 3: return 50 + listing_price / 10;    // 10%
    case 4: return 500 + listing_price / 2;    // 50%
    case 5: return 5000 + listing_price / 2;   // 50%
    case 6: return 50000 + listing_price / 2;  // 50%
    case 7: return 500000 + listing_price / 2; // 50%

     // stupidly large amount of money
    default: return 5000000 + listing_price / 2;

    // too many items--basically, make the cost infinite
    case MAX_ITEMS_PER_BAZAAR_CATEGORY - 2:
    case MAX_ITEMS_PER_BAZAAR_CATEGORY - 1:
      return (CurrencyType)(-1);
  }
}

const char* BazaarCategoryString(BazaarCategory category) {
  switch (category) {
  case CATEGORY_WEAPONS:  return "Weapons";
  case CATEGORY_ARMOR:    return "Armor";
  case CATEGORY_MISC:     return "Misc.";
  case CATEGORY_OWNITEMS: return "Your Items";
  default: return 0;
  }
}

}