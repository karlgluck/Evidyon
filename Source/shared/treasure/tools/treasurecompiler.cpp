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
#include "treasurecompiler.h"
#include "shared/treasure/tools/globaltreasure.h"
#include "shared/treasure/treasuredropsequenceelement.h"
#include "shared/treasure/treasureclass.h"
#include "shared/treasure/epictreasure.h"
#include "common/null.h"
#include "common/safemacros.h"
#include <assert.h>


namespace Evidyon {
namespace Treasure {
namespace Tools {


static const size_t DROP_SEQUENCE_LENGTH = 1024;


  
//----[  TreasureCompiler  ]---------------------------------------------------
TreasureCompiler::TreasureCompiler() {
  zero();
}



//----[  ~TreasureCompiler  ]--------------------------------------------------
TreasureCompiler::~TreasureCompiler() {
  clear();
}



//----[  clear  ]--------------------------------------------------------------
void TreasureCompiler::clear() {
  for (TreasureClassList::iterator i = treasure_classes_.begin();
        i != treasure_classes_.end(); ++i) {
    Evidyon::Treasure::TreasureClass* treasure_class = *i;
    SAFE_DELETE(treasure_class);
  }
  for (EpicTreasureList::iterator i = epic_treasures_.begin();
        i != epic_treasures_.end(); ++i) {
    Evidyon::Treasure::EpicTreasure* epic_treasure = *i;
    SAFE_DELETE(epic_treasure);
  }
  SAFE_DELETE_ARRAY(drop_sequence_);
  zero();
}




//----[  beginCompiling  ]-----------------------------------------------------
void TreasureCompiler::initialize(GlobalTreasure* global_treasure) {
  for (int group = 0; group < NUMBER_OF_TREASURE_GROUPS; ++group) {
    global_treasure->writeBaseWeights(TreasureGroup(group),
                                      global_weights_[group]);
  }
  drop_sequence_length_ = DROP_SEQUENCE_LENGTH;
  drop_sequence_ = new TreasureDropSequenceElement[drop_sequence_length_];
  global_treasure->compileDropSequence(drop_sequence_, drop_sequence_length_);
}



//----[  getTreasureGroupWeights  ]--------------------------------------------
const TreasureWeightValue* TreasureCompiler::getTreasureGroupWeights(TreasureGroup group) const {
  return global_weights_[group];
}




//----[  add  ]----------------------------------------------------------------
Evidyon::Treasure::TreasureClassIndex TreasureCompiler::add(
    const Evidyon::Treasure::TreasureClass* treasure_class) {
  Evidyon::Treasure::TreasureClass* internal_treasure_class
    = new Evidyon::Treasure::TreasureClass;
  memcpy(internal_treasure_class, treasure_class, sizeof(*treasure_class));
  treasure_classes_.push_back(internal_treasure_class);
  return treasure_classes_.size() - 1;
}



//----[  add  ]----------------------------------------------------------------
void TreasureCompiler::add(
    const Evidyon::Treasure::EpicTreasure* epic_treasure) {
  Evidyon::Treasure::EpicTreasure* internal_epic_treasure
    = new Evidyon::Treasure::EpicTreasure;
  memcpy(internal_epic_treasure, epic_treasure, sizeof(*epic_treasure));
  epic_treasures_.push_back(internal_epic_treasure);
}





//----[  numberOfTreasureClasses  ]--------------------------------------------
size_t TreasureCompiler::numberOfTreasureClasses() const {
  return treasure_classes_.size();
}




//----[  compileTreasureClass  ]-----------------------------------------------
void TreasureCompiler::compileTreasureClass(size_t index,
    Evidyon::Treasure::TreasureClass* treasure_class) {
  memcpy(treasure_class, treasure_classes_.at(index), sizeof(*treasure_class));
}



//----[  numberOfEpicTreasures  ]----------------------------------------------
size_t TreasureCompiler::numberOfEpicTreasures() const {
  return epic_treasures_.size();
}




//----[  compileEpicTreasure  ]------------------------------------------------
void TreasureCompiler::compileEpicTreasure(size_t index,
    Evidyon::Treasure::EpicTreasure* epic_treasure) {
  memcpy(epic_treasure, epic_treasures_.at(index), sizeof(*epic_treasure));
}





//----[  numberOfDropSequenceElements  ]----------------------------------------------
size_t TreasureCompiler::numberOfDropSequenceElements() const {
  return drop_sequence_length_;
}




//----[  compileDropSequenceElement  ]------------------------------------------------
void TreasureCompiler::compileDropSequenceElement(size_t index,
    Evidyon::Treasure::TreasureDropSequenceElement* element) {
  memcpy(element, &drop_sequence_[index], sizeof(*element));
}






//----[  zero  ]---------------------------------------------------------------
void TreasureCompiler::zero() {
  memset(global_weights_, 0, sizeof(global_weights_));
  treasure_classes_.clear();
  epic_treasures_.clear();
  drop_sequence_ = NULL;
  drop_sequence_length_ = 0;
}



}
}
}

