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
#include "treasureclass.h"
#include "../shared/server-editor/compiledtreasure.h"


const char* GetRarityValueName(Rarity scale) {
  switch (scale) {
    case ONE_IN_3:    return "1-in-3";
    case ONE_IN_4:    return "1-in-4";
    case ONE_IN_5:    return "1-in-5";
    case ONE_IN_8:    return "1-in-8";
    case ONE_IN_20:    return "1-in-20";
    case ONE_IN_64:    return "1-in-64";
    case ONE_IN_300:    return "1-in-300";
    case ONE_IN_2000:    return "1-in-2000";
    case ONE_IN_32000:    return "1-in-32000";
    default: return "";
  }
}


TreasureClass::TreasureClass() {
  member("Treasure", &treasure_);

  for (int i = 0; i < (int)RARITY_SCALE_SIZE; ++i) {
    std::string name = "Treasure Limit ";
    name.append(GetRarityValueName((Rarity)i));
    name.append(" Times");
    member(name, &rarity_scale_[i]);
  }

  for (int i = 0; i < (int)RARITY_SCALE_SIZE; ++i) {
    std::string name = "Items to Drop ";
    name.append(GetRarityValueName((Rarity)i));
    name.append(" Times");
    member(name, &number_of_items_to_drop_[i]);
    number_of_items_to_drop_[i].setValue(1);
  }
}


bool TreasureClass::compileForServer(dc::dcStreamOut* stream) {
  treasure_.sort();

  CompiledTreasureClass description;

  // build the treasure index range
  for (int i = 0; i < RARITY_SCALE_SIZE; ++i) {
    int treasure_index;
    { // places 0 and 1 are switched, because 0 is not
      // duplicated (it is at the center of the distribution)
      // but 1 IS, so its probability actually ends up being
      // higher than 0.
      int rarity_index = 0;
      switch(i) {
        case 0:  rarity_index = 1; break;
        case 1:  rarity_index = 0; break;
        default: rarity_index = i;
      }

      dc::dcList<Treasure>::Element* element =
          rarity_scale_[rarity_index].getReferencedResource();
      treasure_index = 1 + (element == NULL ? 0 : element->getIndex());
    }

    CONFIRM(treasure_index <= treasure_.size()) else {
      treasure_index = treasure_.size();
    }

    description.treasure_index_range[i] = treasure_index;
  }

  { // build the number of items being dropped
    for (int i = 0; i < RARITY_SCALE_SIZE; ++i) {

      // places 0 and 1 are switched, because 0 is not
      // duplicated (it is at the center of the distribution)
      // but 1 IS, so its probability actually ends up being
      // higher than 0.
      int rarity_index = 0;
      switch(i) {
        case 0:  rarity_index = 1; break;
        case 1:  rarity_index = 0; break;
        default: rarity_index = i;
      }

      description.number_of_items_to_drop[i] =
        number_of_items_to_drop_[rarity_index].getValue();
    }
  }

  description.treasure_item_count = treasure_.size();

  CONFIRM(stream->write(&description,
                        sizeof(description))) else return false;

  // write out each of the treasures
  const dc::dcGenericResource::Array& elements = treasure_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = elements.begin();
       i != elements.end(); ++i) {
    dc::dcList<Treasure>::Element* element = 
      (dc::dcList<Treasure>::Element*)(*i);
    Evidyon::CompiledTreasure compiled_treasure;
    element->compileForServer(&compiled_treasure);
    CONFIRM(stream->write(&compiled_treasure,
                          sizeof(compiled_treasure))) else return false;
  }

  return true;
}

std::string TreasureClass::staticTypeName() {
  return "TreasureClass";
}