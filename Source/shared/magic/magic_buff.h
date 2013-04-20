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
#ifndef __SHARED_MAGIC_MAGIC_BUFF_H__
#define __SHARED_MAGIC_MAGIC_BUFF_H__
#pragma once

#include "shared/avatar/avatarattribute.h"
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"
#include "shared/magic/buff.h"
#include "shared/magic/buffmask.h"
#include "common/number.h"

namespace Evidyon {
namespace Magic {

//----[  Magic_Buff  ]---------------------------------------------------------
// Changes the buff enchantments of a target
struct Magic_Buff {
  Event::EventIndex event_index;
  BuffMask enabled_mask; // which of the buff values are enabled?
  float buff_values[NUMBER_OF_BUFFS];
  Time::Milliseconds effect_delay;
  Time::Milliseconds duration;  // length of time to last
  bool target_self;
};

}
}

#endif