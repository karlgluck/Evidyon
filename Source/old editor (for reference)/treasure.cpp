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
#include "treasure.h"
#include "../shared/server-editor/compiledtreasure.h"


Treasure::Treasure() {
  member("Item", &item_);
  member("Treasure Level Modifier", &value_modifier_);
}

void Treasure::compileForServer(Evidyon::CompiledTreasure* description) {
  description->item_type = item_.getReferencedResourceIndex();
}

int Treasure::getTreasureValue() const {
  const dc::dcList<Item>::Element* my_item = item_.getReferencedResource();
  if (my_item == NULL) return 0;
  return my_item->getTreasureLevelValue() + value_modifier_.getValue();
}

int Treasure::compareTo(const dc::dcGenericResource* other) const {
  if (other == 0 || getTypeID() != other->getTypeID())
      return dc::dcGenericResource::compareTo(other);
  else {
    int my_value = getTreasureValue();
    int other_value = ((const Treasure*)other)->getTreasureValue();
    return my_value - other_value;
  }

}

std::string Treasure::staticTypeName() {
  return "Treasure";
}