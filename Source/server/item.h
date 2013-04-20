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
#ifndef __ITEM_H__
#define __ITEM_H__
#pragma once


#include "itemidtype.h"

namespace Evidyon {
namespace Item {
struct ItemServerDescription;
}
}

namespace Evidyon {

struct AvatarInventoryItem;


class ItemInstance {
public:
  void acquire(ItemIDType unique_id,
               int quantity,
               const Evidyon::Item::ItemServerDescription* description);
  void release();

  inline ItemIDType getUniqueID() const { return unique_id_; }
  inline const Evidyon::Item::ItemServerDescription* getDescription() const {
    return description_;
  }
  const unsigned int getGroundType() const;
  bool isStackable() const;

  // Returns the quantity that couldn't be added.  The quantity to add must
  // be greater than zero.
  int addQuantity(int quantity);

  // Returns the quantity that couldn't be removed.  The quantity must be
  // greater than zero (i.e. don't use a negative amount to remove quantity).
  // If the returned value is greater than or equal to zero, the item is now
  // invalid and should be deallocated.
  // If the returned value is less than zero (sentinel value: -1) this item was
  // able to remove the given quantity with at least 1 quantity left over, and
  // is still valid.
  int removeQuantity(int quantity);

  inline int getQuantity() const { return quantity_; }
  void setQuantity(int quantity) { quantity_ = quantity; }
  inline unsigned int getSpecialPropertyFlags() const { return special_property_flags_; }

private:
  ItemIDType unique_id_;
  int quantity_;
  unsigned int special_property_flags_; // for now, always 0
  const Evidyon::Item::ItemServerDescription* description_;
};

}

#endif