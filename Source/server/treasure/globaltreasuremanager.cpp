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
#include "globaltreasuremanager.h"
#include "server/worldregion.h"
#include "server/globalitemmanager.h"
#include "server/itempointer.h"
#include "common/null.h"
#include "common/safemacros.h"
#include "shared/treasure/treasuredropsequenceelement.h"
#include "shared/treasure/epictreasure.h"
#include "shared/treasure/treasureclass.h"
#include "common/randint32.h"
#include "shared/design/newdesign.h"

namespace Evidyon {
namespace Server {


static const size_t EPIC_COUNTER_RESET_VALUE = 300000;


//----[  TreasureClassData  ]--------------------------------------------------
struct TreasureClassData {
  Evidyon::Treasure::TreasureClass data;
  unsigned int next_index[Evidyon::Treasure::NUMBER_OF_TREASURE_GROUPS];
};


//----[  singleton_  ]---------------------------------------------------------
GlobalTreasureManager* GlobalTreasureManager::singleton_ = NULL;






  
//----[  GlobalTreasureManager  ]----------------------------------------------
GlobalTreasureManager::GlobalTreasureManager() {
  zero();
}




//----[  ~GlobalTreasureManager  ]---------------------------------------------
GlobalTreasureManager::~GlobalTreasureManager() {
  destroy(NULL);
}





//----[  create  ]-------------------------------------------------------------
void GlobalTreasureManager::create(size_t epic_counter,
                                   GlobalItemManager* item_manager) {
  epic_counter_ = epic_counter;
  item_manager_ = item_manager;
  assert(!singleton_);
  singleton_ = this;
}







//----[  destroy  ]------------------------------------------------------------
void GlobalTreasureManager::destroy(size_t* epic_counter) {
  singleton_ = NULL;
  SAFE_DELETE_ARRAY(treasure_classes_);
  SAFE_DELETE_ARRAY(epic_treasures_);
  SAFE_DELETE_ARRAY(drop_sequence_);
  if (epic_counter) *epic_counter = epic_counter_;
  zero();
}





//----[  initNumberOfTreasureClasses  ]----------------------------------------
bool GlobalTreasureManager::initNumberOfTreasureClasses(size_t number) {
  number_of_treasure_classes_ = number;
  treasure_classes_ = new TreasureClassData[number];
  return treasure_classes_ != NULL;
}




//----[  initTreasureClass  ]--------------------------------------------------
void GlobalTreasureManager::initTreasureClass(
    Evidyon::Treasure::TreasureClassIndex index,
    const Evidyon::Treasure::TreasureClass* treasure_class) {
  assert(index < number_of_treasure_classes_);
  TreasureClassData* treasure_class_data = treasure_classes_ + index;
  memcpy(&treasure_class_data->data, treasure_class, sizeof(*treasure_class));
  for (int i = 0; i < Evidyon::Treasure::NUMBER_OF_TREASURE_GROUPS; ++i) {
    treasure_class_data->next_index[i] = 0;
  }
}





//----[  initDropSequenceLength  ]---------------------------------------------
bool GlobalTreasureManager::initDropSequenceLength(size_t number) {
  drop_sequence_length_ = number;
  drop_sequence_ = new Evidyon::Treasure::TreasureDropSequenceElement[number];
  return drop_sequence_ != NULL;
}




//----[  initDropSequenceElement  ]--------------------------------------------
void GlobalTreasureManager::initDropSequenceElement(
    size_t index,
    const Evidyon::Treasure::TreasureDropSequenceElement* element) {
  assert(index < drop_sequence_length_);
  memcpy(drop_sequence_ + index, element, sizeof(*element));
}





//----[  initNumberOfEpicTreasures  ]------------------------------------------
bool GlobalTreasureManager::initNumberOfEpicTreasures(size_t number) {
  number_of_epic_treasures_ = number;
  epic_treasures_ = new Evidyon::Treasure::EpicTreasure[number];
  return epic_treasures_ != NULL;
}




//----[  initEpicTreasure  ]---------------------------------------------------
void GlobalTreasureManager::initEpicTreasure(
    size_t index,
    const Evidyon::Treasure::EpicTreasure* epic_treasure) {
  assert(index < number_of_epic_treasures_);
  memcpy(epic_treasures_ + index, epic_treasure, sizeof(*epic_treasure));
}






//----[  dropTreasureItems  ]--------------------------------------------------
void GlobalTreasureManager::dropTreasureItems(Treasure::TreasureClassIndex treasure,
                                              Avatar::AvatarLevel intended_level,
                                              WorldRegion* region,
                                              int x,
                                              int z) {

  using namespace Evidyon::Treasure;
  assert(treasure < number_of_treasure_classes_);
  TreasureClassData* data = treasure_classes_ + treasure;

  if (epic_counter_-- == 0) {
    dropEpicItem(region, intended_level, x, z);
    epic_counter_ = EPIC_COUNTER_RESET_VALUE;
  } else {

    Evidyon::Treasure::TreasureDropSequenceElement* next_drop_element
      = nextDropSequenceElement();

    if (next_drop_element->gold) {
      region->addCurrencyOnGround(x, z, Design::MonsterGold(intended_level));
    }

    if (next_drop_element->consumable) {
      dropTreasureGroupItem(data, TREASUREGROUP_CONSUMABLE, region, x, z);
    }

    static const TreasureGroup TREASURE_GROUP[] = {
      INVALID_TREASURE_GROUP,           // TreasureDropSequenceElement::NONE
      TREASUREGROUP_COMMON_EQUIPMENT,   // COMMON
      TREASUREGROUP_UNCOMMON_EQUIPMENT, // UNCOMMON
      TREASUREGROUP_RARE_EQUIPMENT,     // RARE
    };

    TreasureGroup group = TREASURE_GROUP[next_drop_element->equipment];
    if (group != INVALID_TREASURE_GROUP) {
      dropTreasureGroupItem(data, group, region, x, z);
    }
  }

  region->broadcastUpdateItemsOnLocation(x, z);
}




//----[  dropTreasureGroupItem  ]----------------------------------------------
void GlobalTreasureManager::dropTreasureGroupItem(
    TreasureClassData* data,
    Treasure::TreasureGroup group,
    WorldRegion* region,
    int x,
    int z) {

  using namespace Evidyon::Treasure;
  static const int DROPLIST_LENGTH[NUMBER_OF_TREASURE_GROUPS] = {
    TreasureClass::COMMON_LENGTH,
    TreasureClass::UNCOMMON_LENGTH,
    TreasureClass::RARE_LENGTH,
    TreasureClass::CONSUMABLE_LENGTH,
  };

  const Item::ItemIndex* drop_list[] = {
    data->data.common_list,
    data->data.uncommon_list,
    data->data.rare_list,
    data->data.consumable_list
  };

  ItemPointer item;
  unsigned int index = data->next_index[group];
  Item::ItemIndex item_type = drop_list[group][index];
  if ((item_type != Item::INVALID_ITEM_INDEX) &&
      item_manager_->acquireNewItem(1,
                                    item_type,
                                    &item)) {
    region->addItemOnGround(false, x, z, item);
  }

  // Advance the index
  index = (index + 1) % DROPLIST_LENGTH[group];
  data->next_index[group] = index;
}




//----[  dropEpicItem  ]-------------------------------------------------------
void GlobalTreasureManager::dropEpicItem(WorldRegion* region,
                                         int target_level,
                                         int x,
                                         int z) {
  for (size_t i = 0; i < number_of_epic_treasures_; ++i) {
    if (epic_treasures_[target_level].lowest_level <= target_level &&
        epic_treasures_[target_level].highest_level >= target_level) {
      ItemPointer acquired_item;
      if (item_manager_->acquireNewItem(
          1,
          epic_treasures_[target_level].item,
          &acquired_item)) {
        region->addItemOnGround(false, x, z, acquired_item);
      }
    }
  }
}



//----[  zero  ]---------------------------------------------------------------
void GlobalTreasureManager::zero() {
  item_manager_ = NULL;
  treasure_classes_ = NULL;
  number_of_treasure_classes_ = 0;
  epic_counter_ = 0xFFFFFFFF;
  epic_treasures_ = NULL;
  number_of_epic_treasures_ = 0;
  drop_sequence_ = NULL;
  drop_sequence_length_ = 0;
  next_drop_sequence_element_= 0;
}




//----[  nextDropSequenceElement  ]--------------------------------------------
Evidyon::Treasure::TreasureDropSequenceElement*
    GlobalTreasureManager::nextDropSequenceElement() {
  assert(drop_sequence_length_ > 0);
  next_drop_sequence_element_
    = (next_drop_sequence_element_ + 1) % drop_sequence_length_;
  return &drop_sequence_[next_drop_sequence_element_];
}



//----[  singleton  ]----------------------------------------------------------
GlobalTreasureManager* GlobalTreasureManager::singleton() {
  assert(singleton_);
  return singleton_;
}


}
}

