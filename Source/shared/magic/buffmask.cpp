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
#include "buffmask.h"
#include "buff.h"
#include <assert.h>

namespace Evidyon {
namespace Magic {


// From MSDN:
//  Microsoft Specific
//    The ordering of data declared as bit fields is from low to high bit,
//    as shown in the figure above.
// END Microsoft Specific
// This method is just to make sure everything works ok.
bool SwitchedBuffMaskGet(Buff buff, const BuffMask* mask) {
  switch (buff) {
  default: assert(false); return false;
  case BUFF_MAX_HP: return mask->fields.max_hp == 1;
  case BUFF_MAX_MP: return mask->fields.max_mp == 1;
  case BUFF_DEFENSE:  return mask->fields.defense == 1;
  case BUFF_MELEE_DAMAGE: return mask->fields.melee_damage == 1;
  case BUFF_MAGIC_DAMAGE: return mask->fields.magic_damage == 1;
  case BUFF_HP_STEAL: return mask->fields.hp_steal == 1;
  case BUFF_MP_STEAL: return mask->fields.mp_steal == 1;
  case BUFF_ACTION_SPEED_MODIFIER:  return mask->fields.action_speed_modifier == 1;
  case BUFF_MOVEMENT_SPEED: return mask->fields.movement_speed == 1;
  case BUFF_HP_REGEN: return mask->fields.hp_regen == 1;
  case BUFF_POISON: return mask->fields.poison == 1;
  case BUFF_MP_REGEN: return mask->fields.mp_regen == 1;
  case BUFF_LEECH:  return mask->fields.leech == 1;
  }
}

//----[  get  ]----------------------------------------------------------------
bool BuffMask::get(Buff buff) const {
  assert(SwitchedBuffMaskGet(buff, this) == (((value >> buff)&1) == 1));
  return (((value >> buff)&1) == 1);
}



//----[  set  ]----------------------------------------------------------------
void BuffMask::set(Buff buff, bool enabled) {
  unsigned int mask = 1 << buff;
  value &= ~mask; // clear the bit
  assert(SwitchedBuffMaskGet(buff, this) == false);
  if (enabled) {
    value |= mask; // set the bit
    assert(SwitchedBuffMaskGet(buff, this) == true);
  }
}


}
}