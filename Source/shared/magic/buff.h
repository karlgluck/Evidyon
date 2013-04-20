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
#ifndef __SHARED_MAGIC_BUFF_H__
#define __SHARED_MAGIC_BUFF_H__
#pragma once

namespace Evidyon {
namespace Magic {


//----[  Buff  ]---------------------------------------------------------------
enum Buff {
  BUFF_MAX_HP,            // max hp += this
  BUFF_MAX_MP,            // max mp += this
  BUFF_DEFENSE,           // defense += this
  BUFF_MELEE_DAMAGE,      // melee damage dealt += this
  BUFF_MAGIC_DAMAGE,      // magic damage dealt += this
  BUFF_HP_STEAL,          // hp += damage dealt * this
  BUFF_MP_STEAL,          // mp += damage dealt * this
  BUFF_ACTION_SPEED_MODIFIER, // delay *= (1 - this)
  BUFF_MOVEMENT_SPEED,    // movement speed += this
  BUFF_HP_REGEN,          // hp regen += this
  BUFF_POISON,            // hp regen -= this
  BUFF_MP_REGEN,          // mp regen += this
  BUFF_LEECH,             // mp regen -= this
  NUMBER_OF_BUFFS,
  INVALID_BUFF,
};



}
}

#endif