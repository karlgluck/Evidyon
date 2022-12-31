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
#include "treasuregeneration.h"

TreasureGeneration::TreasureGeneration() {
  for (int i = 0; i < (int)RARITY_SCALE_SIZE; ++i) {
    char name[128];
    sprintf_s(name, "Monster Level to Boost Rarity %i Level%s", i + 1, i == 0 ? "" : "s");
    member(name, &monster_level_treasure_rarity_boost_[i]);
  }

  member("Treasure Classes", &treasure_classes_);
}


void TreasureGeneration::compileRarityBoost(int level[RARITY_SCALE_SIZE]) {
  for (int i = 0; i < RARITY_SCALE_SIZE; ++i) {
    level[i] = monster_level_treasure_rarity_boost_[i].getValue();
  }
}


bool TreasureGeneration::compileForServer(dc::dcStreamOut* stream) {
  size_t classes = treasure_classes_.getMembers().size();
  CONFIRM(stream->write(&classes, sizeof(classes))) else return false;

  const dc::dcGenericResource::Array& elements = treasure_classes_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = elements.begin();
       i != elements.end(); ++i) {
    dc::dcList<TreasureClass>::Element* element = 
      (dc::dcList<TreasureClass>::Element*)(*i);
    CONFIRM(element->compileForServer(stream)) else return false;
  }

  return true;
}


std::string TreasureGeneration::staticTypeName() {
  return "TreasureGeneration";
}