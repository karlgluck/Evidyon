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
#ifndef __SHARED_MAGIC_MAGIC_DOTAOEBUFF_H__
#define __SHARED_MAGIC_MAGIC_DOTAOEBUFF_H__
#pragma once

#include "aoemagicdata.h"
#include "magic_buff.h"
#include "shared/time/milliseconds.h"

namespace Evidyon {
namespace Magic {

//----[  Magic_DOTAOEBuff  ]---------------------------------------------------
// Not just for poison clouds!  Also used for fire-shield (damage those around
// caster for duration).
struct Magic_DOTAOEBuff {
  AOEMagicData data;  // basic targeting data; event only run once
  Time::Milliseconds action_delay;
  Time::Milliseconds period;       // time between applications
  Time::Milliseconds duration;     // total time the effect lasts
  Magic_Buff buff;    // buff to apply
  float dps_to_damage_multiplier; // damage delt per period
  bool terminate_on_death; // should this end if the player dies?
};


}
}

#endif