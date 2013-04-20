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