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
#ifndef __TREASURECLASS_H__
#define __TREASURECLASS_H__
#pragma once


#include <dc/list>
#include <dc/integer>
#include "treasure.h"
#include "rarityscale.h"


const char* GetRarityValueName(Rarity scale);


class TreasureClass : public dc::dcResource<TreasureClass> {
public:
  TreasureClass();

  // Writes this class into the output stream.  Called by TreasureGeneration
  bool compileForServer(dc::dcStreamOut* stream);
  dc::dcList<Treasure>* getTreasureList() { return &treasure_; }

public:
  static std::string staticTypeName();


private:
  dc::dcList<Treasure> treasure_;

  // Reference to items that match the appropriate rarity on the
  // scale.  These should be selected from the this class's
  // list of treasure.
  dc::dcList<Treasure>::Reference rarity_scale_[RARITY_SCALE_SIZE];

  // How many items to drop, given a randomly selected value.  This is
  // independent of the random value used to select the actual items.
  dc::dcInteger number_of_items_to_drop_[RARITY_SCALE_SIZE];
};




#endif