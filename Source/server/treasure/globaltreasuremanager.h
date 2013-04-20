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
#ifndef __SERVER_TREASURE_GLOBALTREASUREMANAGER_H__
#define __SERVER_TREASURE_GLOBALTREASUREMANAGER_H__
#pragma once

#include "shared/treasure/treasureclassindex.h"
#include "shared/treasure/treasuregroup.h"
#include "shared/avatar/avatarlevel.h"

namespace Evidyon {
class GlobalItemManager;
class WorldRegion;
namespace Treasure {
struct TreasureClass;
struct EpicTreasure;
struct TreasureDropSequenceElement;
}
}


namespace Evidyon {
namespace Server {

// One of these is created for each treasure type so that the drop indices
// can be tracked.
struct TreasureClassData;


//----[  GlobalTreasureManager  ]----------------------------------------------
class GlobalTreasureManager {
public:
  GlobalTreasureManager();
  ~GlobalTreasureManager();

  // Initializes the treasure manager
  void create(size_t epic_counter,
              GlobalItemManager* item_manager);

  // Shuts down the treasure manager.  The epic counter pointer should be
  // non-null because the counter needs to be saved between executions of
  // the server (in the ini file)
  void destroy(size_t* epic_counter);

  // Sets up the treasures using data from the game file
  bool initNumberOfTreasureClasses(size_t number);
  void initTreasureClass(
    Evidyon::Treasure::TreasureClassIndex index,
    const Evidyon::Treasure::TreasureClass* treasure_class);

  // Sets up the treasures using data from the game file
  bool initDropSequenceLength(size_t number);
  void initDropSequenceElement(
    size_t index,
    const Evidyon::Treasure::TreasureDropSequenceElement* element);

  // Sets up the treasures using data from the game file
  bool initNumberOfEpicTreasures(size_t number);
  void initEpicTreasure(
    size_t index,
    const Evidyon::Treasure::EpicTreasure* epic_treasure);

  // Creates items/gold from the given treasure group on the ground, and sends
  // an update packet to nearby clients.
  void dropTreasureItems(Treasure::TreasureClassIndex treasure,
                         Avatar::AvatarLevel intended_level,
                         WorldRegion* region,
                         int x,
                         int z);

  // Puts an item from the epic list on the ground at (x,z).  It is randomly
  // selected from all epics that could be dropped for the target level.
  void dropEpicItem(WorldRegion* region,
                    int target_level,
                    int x,
                    int z);

private:
  void zero();

  // Returns the next drop descriptor and advances the internal sequence
  // counter.
  Evidyon::Treasure::TreasureDropSequenceElement* nextDropSequenceElement();

  // Drops an item from the given group on the ground
  void dropTreasureGroupItem(TreasureClassData* data,
                             Treasure::TreasureGroup group,
                             WorldRegion* region,
                             int x,
                             int z);

private:

  // Used to create items
  GlobalItemManager* item_manager_;

  // All the different kinds of treasure
  TreasureClassData* treasure_classes_;
  size_t number_of_treasure_classes_;

  // When this value reaches zero, an epic is dropped
  size_t epic_counter_;

  // List of epics
  Evidyon::Treasure::EpicTreasure* epic_treasures_;
  size_t number_of_epic_treasures_;

  // Drop sequence
  Evidyon::Treasure::TreasureDropSequenceElement* drop_sequence_;
  size_t drop_sequence_length_;
  size_t next_drop_sequence_element_;


public:
  static GlobalTreasureManager* singleton();
private:
  static GlobalTreasureManager* singleton_;
};


}
}

#endif