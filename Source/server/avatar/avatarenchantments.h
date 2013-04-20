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
#ifndef __SERVER_AVATAR_AVATARENCHANTMENTS_H__
#define __SERVER_AVATAR_AVATARENCHANTMENTS_H__
#pragma once

#include "shared/time/timer.h"
#include "shared/magic/enchantmenttypeindex.h"
#include "shared/magic/enchantmenttype.h"
#include "shared/magic/buffmask.h"
#include "shared/magic/buff.h"
#include "common/number.h"


namespace Evidyon {
namespace Server {

//----[  AvatarEnchantments  ]-------------------------------------------------
// Note: because the attribute equations chain-affect so many things, they
// cannot be enchanted.  To achieve the same effect, add damage, change HP/MP
// or adjust speed.
struct AvatarEnchantments {

  // Keeps track of which buffs have valid values
  Magic::BuffMask active_buffs;

  // Time at which each timer expires
  Time::Timer<Time::RESOLUTION_500_MS> buff_timers[Magic::NUMBER_OF_BUFFS];

  // Actual values of the buffs
  float buff_values[Magic::NUMBER_OF_BUFFS];

  // The type of enchantment in each active slot.  Used to build the actor's
  // enchantment description data.
  Magic::EnchantmentTypeIndex enchantments[Magic::NUMBER_OF_ENCHANTMENT_TYPES];

  struct {
    Number defense; // set to 0 if enchantment is invalid
    Time::Timer<Time::RESOLUTION_500_MS> expire_timer;
  } armor_enchantments[3];

  //// % of damage taken goes to shield
  //struct {
  //  Number portion;
  //  Number damage_points_remaining;
  //} shield;

  //struct {
  //  Number accumulator;
  //  Time::Timer<Time::RESOLUTION_500_MS> heal_timer;
  //  Time::Timer<Time::RESOLUTION_500_MS> expire_timer;
  //} reactive_healing;

  //// Deals portion of damage back to dealer
  //struct {
  //  Number damage_returned;
  //  Time::Timer<Time::RESOLUTION_500_MS> expire_timer;
  //} thorns;

  // Resets all internal values
  void zero();

  // Resets internal values in a way that is detectable to be different
  // from the previous enchantments.  This should be called when all
  // enchantments need to be removed and the actor's description updated.
  void clear();

  // Check to see if the buffs or enchantments have changed state
  void update(Time::Timer<Time::RESOLUTION_500_MS>* recalculate_timer,
              bool* buffs_changed,
              bool* enchantments_changed);

  // Checks all enchantments to see if they have expired.  If an
  // enchantment has expired, its type index is set accordingly
  // and values in that enchantment are reset.  If an enchantment
  // expires or a new enchantment was added, this method returns
  // 'true' so that the actor of the enchanted avatar can be
  // updated.
  bool updateEnchantments();
};


}
}

#endif