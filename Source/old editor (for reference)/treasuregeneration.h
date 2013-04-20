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
#ifndef __TREASUREGENERATION_H__
#define __TREASUREGENERATION_H__
#pragma once


#include <dc/list>
#include <dc/integer>
#include "treasureclass.h"



class TreasureGeneration : public dc::dcResource<TreasureGeneration> {
public:
  TreasureGeneration();

  // Used by the AI spawners during compilation
  void compileRarityBoost(int level[RARITY_SCALE_SIZE]);

  // Save the treasure classes into the output stream
  bool compileForServer(dc::dcStreamOut* stream);


public:
  static std::string staticTypeName();


private:
  // If the average level of a region is higher than the value in this chart,
  // monsters in that region generate treasure on a scale at minimum of
  // the rarity indicated.
  dc::dcInteger monster_level_treasure_rarity_boost_[RARITY_SCALE_SIZE];

  // The different kinds of treasure that can be generated.  Monsters refer
  // to these structures.
  dc::dcList<TreasureClass> treasure_classes_;
};




#endif
