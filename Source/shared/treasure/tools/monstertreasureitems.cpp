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
#include "monstertreasureitems.h"
#include "treasureweight.h"

#include <assert.h>

namespace Evidyon {
namespace Treasure {
namespace Tools {


  
//----[  Element  ]------------------------------------------------------------
MonsterTreasureItems::Element::Element() {
  member("Weight", &weight_);
  member("Item", &item_);
  weight_.setValue(1.0);
}


//----[  compareTo  ]----------------------------------------------------------
int MonsterTreasureItems::Element::compareTo(const dc::dcGenericResource* other) const {
  if (other && getTypeID() == other->getTypeID()) {
      return weight_.compareTo(&((Element*)other)->weight_);
  } else {
    return dc::dcGenericResource::compareTo(other);
  }
}



//----[  getWeightValue  ]-----------------------------------------------------
TreasureWeightValue MonsterTreasureItems::Element::getWeightValue(const TreasureWeightValue* weights) {
  Evidyon::Tools::ItemElement* item = item_.getReferencedResource();
  if (!item) return TreasureWeightValue(0);

  // classify
  TreasureWeight weight_type = TREASUREWEIGHT_ARMOR;
  Evidyon::Item::Tools::ItemIntendedUser intended_user;
  Evidyon::Item::Tools::ItemIntendedUse intended_use;
  Evidyon::Item::ItemEquipmentType equipment_type;
  item->getTreasureSettings(&intended_user, &intended_use, &equipment_type);
  TreasureWeightValue weight = 0;
  switch (equipment_type) {
    using namespace Evidyon::Item;

  // 1-handed melee items
  case ITEMEQUIPMENTTYPE_JAVELIN:
  case ITEMEQUIPMENTTYPE_THROWN:
    weight += weights[TREASUREWEIGHT_RANGED];
  case ITEMEQUIPMENTTYPE_1H:
  case ITEMEQUIPMENTTYPE_MAINHAND:
  case ITEMEQUIPMENTTYPE_OFFHAND:
    weight += weights[TREASUREWEIGHT_WEAPON];
    weight += weights[TREASUREWEIGHT_ONE_HANDED];
    break;

  case ITEMEQUIPMENTTYPE_SHIELD:
    weight += weights[TREASUREWEIGHT_ARMOR];
    weight += weights[TREASUREWEIGHT_ONE_HANDED];
    break;

  // 2-handed melee items
  case ITEMEQUIPMENTTYPE_BOW:
  case ITEMEQUIPMENTTYPE_CROSSBOW:
    weight += weights[TREASUREWEIGHT_RANGED];
  case ITEMEQUIPMENTTYPE_SPEAR:
  case ITEMEQUIPMENTTYPE_2H:
  case ITEMEQUIPMENTTYPE_POLEARM:
  case ITEMEQUIPMENTTYPE_DUALWIELD:
  case ITEMEQUIPMENTTYPE_UNARMED:
    weight += weights[TREASUREWEIGHT_WEAPON];
    weight += weights[TREASUREWEIGHT_TWO_HANDED];
    break;

  // other types
  case ITEMEQUIPMENTTYPE_ARMOR:
    weight += weights[TREASUREWEIGHT_BODY];
    weight += weights[TREASUREWEIGHT_ARMOR];
    break;
  case ITEMEQUIPMENTTYPE_HELMET:
    weight += weights[TREASUREWEIGHT_HELM];
    weight += weights[TREASUREWEIGHT_ARMOR];
    break;
  case ITEMEQUIPMENTTYPE_BELT:
    weight += weights[TREASUREWEIGHT_ARMOR];
    break;
  case ITEMEQUIPMENTTYPE_RING:
    weight += weights[TREASUREWEIGHT_RING];
    break;
  case ITEMEQUIPMENTTYPE_BOOTS:
    weight += weights[TREASUREWEIGHT_ARMOR];
    weight += weights[TREASUREWEIGHT_BOOTS];
    break;
  case ITEMEQUIPMENTTYPE_GLOVES:
    weight += weights[TREASUREWEIGHT_ARMOR];
    weight += weights[TREASUREWEIGHT_GLOVES];
    break;

  default:
    assert(false);
  case ITEMEQUIPMENTTYPE_ACCESSORY:
  case ITEMEQUIPMENTTYPE_NONE:
    break;
  }

  switch (intended_use) {
  using namespace Evidyon::Item::Tools;
  default:
    break;
  case INTENDEDUSE_CONSUMABLE_SELF:
    weight += weights[TREASUREWEIGHT_CONSUMABLE_SELF];
    break;
  case INTENDEDUSE_CONSUMABLE_OTHER:
    weight += weights[TREASUREWEIGHT_CONSUMABLE_OTHER];
    break;
  case INTENDEDUSE_CONSUMABLE_BUFF:
    weight += weights[TREASUREWEIGHT_CONSUMABLE_BUFF];
    break;
  case INTENDEDUSE_CONSUMABLE_ATTACK:
    weight += weights[TREASUREWEIGHT_CONSUMABLE_ATTACK];
    break;
  case INTENDEDUSE_CONSUMABLE_DEFEND:
    weight += weights[TREASUREWEIGHT_CONSUMABLE_DEFEND];
    break;
  case INTENDEDUSE_CONSUMABLE_TRANSPORT:
    weight += weights[TREASUREWEIGHT_CONSUMABLE_TRANSPORT];
    break;
  case INTENDEDUSE_CONSUMABLE_RESTORE:
    weight += weights[TREASUREWEIGHT_CONSUMABLE_RESTORE];
    break;
  }

  switch (intended_user) {
  using namespace Evidyon::Item::Tools;
  default: assert(false);
  case INTENDEDUSER_NONSPECIFIC:
    weight += weights[TREASUREWEIGHT_NONSPECIFIC_USER];
    break;
  case INTENDEDUSER_CASTER:
    weight += weights[TREASUREWEIGHT_CASTER];
    break;
  case INTENDEDUSER_WARRIOR:
    weight += weights[TREASUREWEIGHT_WARRIOR];
    break;
  }

  return weight_.getValue() + weight;
}




//----[  MonsterTreasureItems  ]-----------------------------------------------
MonsterTreasureItems::MonsterTreasureItems() {

}


//----[  fillDropList  ]-------------------------------------------------------
void MonsterTreasureItems::fillDropList(
    const TreasureWeightValue* weights,
    Evidyon::Item::ItemIndex* drop_list,
    size_t drop_list_length) {
  ItemDropPercentages item_percentages;
  getItemToDropPercentageList(weights, &item_percentages);
  TreasureWeightValue leftover_weight = 0;
  size_t next_element = 0;
  Evidyon::Item::ItemIndex most_common_item = Evidyon::Item::INVALID_ITEM_INDEX;
  TreasureWeightValue most_common_item_weight = 0;
  for (ItemDropPercentages::iterator i = item_percentages.begin();
        i != item_percentages.end(); ++i) {
    Item::ItemIndex item_index
      = i->first;
    if (i->second > most_common_item_weight) {
      most_common_item_weight = i->second;
      most_common_item = item_index;
    }
    TreasureWeightValue values_to_add
      = i->second * drop_list_length;
    TreasureWeightValue value
      = values_to_add + leftover_weight;
    leftover_weight = value - floor(value);
    int number_to_add = int(values_to_add);
    while (number_to_add > 0) {
      drop_list[next_element++] = item_index;
      if (next_element >= drop_list_length) {
        return;
      }
      --number_to_add;
    }
  }
  while (next_element < drop_list_length) {
    drop_list[next_element++] = most_common_item;
  }
}



//----[  getItemToDropPercentageList  ]----------------------------------------
void MonsterTreasureItems::getItemToDropPercentageList(
    const TreasureWeightValue* weights,
    ItemDropPercentages* items) {
  dc::dcGenericResource::Array::const_iterator i, end;
  i = getMembers().begin();
  end = getMembers().end();
  TreasureWeightValue sum = 0;
  while (i != end) {
    Element* resource = (Element*)(*i);
    TreasureWeightValue weight = resource->getWeightValue(weights);
    items->push_back(
      ItemDropPercentages::value_type(
        resource->getItem()->getReferencedResourceIndex(),
        weight));
    sum += weight;
    ++i;
  }
  if (items->size() == 1) {
    items->front().second = 1.0;
    return;
  }
  for (ItemDropPercentages::iterator i = items->begin();
        i != items->end(); ++i) {
    i->second /= sum;
  }
}




//----[  allowDuplicates  ]----------------------------------------------------
bool MonsterTreasureItems::allowDuplicates() const { return true; }


//----[  keepSorted  ]---------------------------------------------------------
bool MonsterTreasureItems::keepSorted() const { return false; }

}
}
}

