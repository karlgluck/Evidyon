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
#ifndef __SHARED_TREASURE_TOOLS_TREASURECOMPILER_H__
#define __SHARED_TREASURE_TOOLS_TREASURECOMPILER_H__
#pragma once

#include "shared/treasure/treasureclassindex.h"
#include "shared/treasure/treasuregroup.h"
#include "shared/treasure/tools/treasureweight.h"
#include "shared/treasure/tools/treasureweightvalue.h"
#include <vector>

namespace Evidyon {
namespace Treasure {
struct TreasureClass;
struct EpicTreasure;
struct TreasureDropSequenceElement;
namespace Tools {
class GlobalTreasure;
}
}
}

namespace Evidyon {
namespace Treasure {
namespace Tools {


  
//----[  TreasureCompiler  ]---------------------------------------------------
class TreasureCompiler {
  typedef std::vector<Evidyon::Treasure::TreasureClass*> TreasureClassList;
  typedef std::vector<Evidyon::Treasure::EpicTreasure*> EpicTreasureList;
public:
  TreasureCompiler();
  ~TreasureCompiler();
  void clear();

  // Writes the global weights for the treasures so that the
  // getTreasureGroupWeights method will return configured values.  This
  // also compiles the drop sequence.
  void initialize(GlobalTreasure* global_treasure);

  // Obtains the set of global treasure weights for the given group
  const TreasureWeightValue* getTreasureGroupWeights(TreasureGroup group) const;

  // Adds a new treasure-related object to the compiler
  Evidyon::Treasure::TreasureClassIndex add(
    const Evidyon::Treasure::TreasureClass* treasure);
  void add(const Evidyon::Treasure::EpicTreasure* epic_treasure);

  size_t numberOfTreasureClasses() const;
  void compileTreasureClass(size_t index,
                            Evidyon::Treasure::TreasureClass* treasure_class);

  size_t numberOfEpicTreasures() const;
  void compileEpicTreasure(size_t index,
                           Evidyon::Treasure::EpicTreasure* epic_treasure);

  size_t numberOfDropSequenceElements() const;
  void compileDropSequenceElement(size_t index,
    Evidyon::Treasure::TreasureDropSequenceElement* element);


private:
  void zero();

private:
  TreasureWeightValue global_weights_[NUMBER_OF_TREASURE_GROUPS]
                                     [NUMBER_OF_TREASURE_WEIGHTS];
  TreasureClassList treasure_classes_;
  EpicTreasureList epic_treasures_;

  Evidyon::Treasure::TreasureDropSequenceElement* drop_sequence_;
  size_t drop_sequence_length_;
};

}
}
}

#endif