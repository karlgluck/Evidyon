//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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