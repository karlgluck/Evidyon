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
#include "avatarenchantments.h"

#include "common/fracture/todo.h"


namespace Evidyon {
namespace Server {




  
//----[  zero  ]---------------------------------------------------------------
void AvatarEnchantments::zero() {
  active_buffs.value = 0;
  for (int i = 0; i < Magic::NUMBER_OF_BUFFS; ++i) {
    buff_values[i] = 0;
    buff_timers[i].reset();
  }
  for (int i = 0; i < Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    enchantments[i] = Magic::INVALID_ENCHANTMENT_TYPE_INDEX;
  }
  for (int i = 0; i < 3; ++i) {
    armor_enchantments[i].defense = 0;
    armor_enchantments[i].expire_timer.reset();
    armor_enchantments[i].expire_timer.disable();
  }
  //shield.portion = 0;
  //shield.damage_points_remaining = 0;
  //reactive_healing.accumulator = 0;
  //reactive_healing.heal_timer.reset();
  //reactive_healing.expire_timer.reset();
  //thorns.damage_returned = 0;
  //thorns.expire_timer.reset();
}



//----[  clear  ]--------------------------------------------------------------
void AvatarEnchantments::clear() {
  for (int i = 0; i < Magic::NUMBER_OF_BUFFS; ++i) {
    buff_values[i] = 0;
    buff_timers[i].reset();
    buff_timers[i].disable();
  }
  for (int i = 0; i < Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    enchantments[i] = Magic::INVALID_ENCHANTMENT_TYPE_INDEX;
  }
  for (int i = 0; i < 3; ++i) {
    armor_enchantments[i].defense = 0;
    armor_enchantments[i].expire_timer.trigger();
  }
  //shield.portion = 0;
  //shield.damage_points_remaining = 0;
  //shield.enchantment = Magic::INVALID_ENCHANTMENT_TYPE_INDEX;
  //reactive_healing.accumulator = 0;
  //reactive_healing.heal_timer.trigger();
  //reactive_healing.expire_timer.trigger();
  //reactive_healing.enchantment = Magic::INVALID_ENCHANTMENT_TYPE_INDEX;
  //thorns.damage_returned = 0;
  //thorns.enchantment = Magic::INVALID_ENCHANTMENT_TYPE_INDEX;
  //thorns.expire_timer.trigger();
}




//----[  updateBuffs  ]--------------------------------------------------------
void AvatarEnchantments::update(
    Time::Timer<Time::RESOLUTION_500_MS>* recalculate_timer,
    bool* buffs_changed,
    bool* enchantments_changed) {
  assert(recalculate_timer);
  assert(buffs_changed);
  assert(enchantments_changed);

  recalculate_timer->reset();

  // check each buff to see if it is still active
  unsigned int old_active_buffs_value = active_buffs.value;
  for (int i = 0; i < Magic::NUMBER_OF_BUFFS; ++i) {
    if (buff_timers[i].poll()) {
      buff_values[i] = 0;
      buff_timers[i].disable();
    }
    bool valid = buff_values[Magic::Buff(i)] > 0;
    bool active = active_buffs.get(Magic::Buff(i));
    active_buffs.set(Magic::Buff(i), valid);
    if (valid) {
      if (!active) buff_timers[i].enable();
      recalculate_timer->earliestTime(&buff_timers[i]);
    }
  }

  // determine which buffs are active
  *buffs_changed = active_buffs.value != old_active_buffs_value;

  // armor enchantments
  for (int i = 0; i < 3; ++i) {
    if (armor_enchantments[i].expire_timer.poll()) {
      *enchantments_changed = true;
      armor_enchantments[i].defense = 0;
      enchantments[Magic::ENCHANTMENT_ARMOR_0 + i] =
        Magic::INVALID_ENCHANTMENT_TYPE_INDEX;
    }
    recalculate_timer->earliestTime(&armor_enchantments[i].expire_timer);
  }

  todo("karl","other enchantment types (just armors are enabled)");
}






}
}

