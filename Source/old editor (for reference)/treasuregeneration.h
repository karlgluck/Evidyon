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
