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
#include "item.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include <dc/debug>
#include "shared/server-editor/server_gamefile.h"
#include "shared/item/itemserverdescription.h"

namespace Evidyon {

  
//----[  acquire  ]------------------------------------------------------------
void ItemInstance::acquire(ItemIDType unique_id,
                   int quantity,
                   const Evidyon::Item::ItemServerDescription* description) {
  unique_id_ = unique_id;
  quantity_ = quantity;
  special_property_flags_ = 0;
  description_ = description;
}



//----[  release  ]------------------------------------------------------------
void ItemInstance::release() {
  unique_id_ = 0;
  description_ = NULL;
}



//----[  getGroundType  ]------------------------------------------------------
const unsigned int ItemInstance::getGroundType() const {
#if defined(EXTENDED_DEBUGGING)
  CONFIRM(description_);
#endif
  return description_->ground_scenery;
}



//----[  isStackable  ]--------------------------------------------------------
bool ItemInstance::isStackable() const {
#if defined(EXTENDED_DEBUGGING)
  CONFIRM(description_);
#endif
  return description_->max_stack_size > 1;
}



//----[  addQuantity  ]--------------------------------------------------------
int ItemInstance::addQuantity(int quantity) {
#if defined(EXTENDED_DEBUGGING)
  CONFIRM(description_);
  ASSERT(quantity >= 0);
#endif
  int available_space = description_->max_stack_size - quantity_;
  if (available_space < 0) return quantity;
  if (quantity > available_space) {
    quantity_ = description_->max_stack_size;
    return quantity - available_space;
  } else {
    quantity_ += quantity;
    return 0;
  }
}



//----[  removeQuantity  ]-----------------------------------------------------
int ItemInstance::removeQuantity(int quantity) {
  int quantity_left = quantity_ - quantity;
  if (quantity_left <= 0) {
    quantity_ = 0;
    return -quantity_left; // return quantity - quantity_;
  } else {
    quantity_ = quantity_left;
    return -1; // sentinel value: don't deallocate this item, it's still valid!
  }
}


}