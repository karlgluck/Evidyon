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
#ifndef __SHARED_MAGIC_BUFFMASK_H__
#define __SHARED_MAGIC_BUFFMASK_H__
#pragma once

namespace Evidyon {
namespace Magic {
enum Buff;
}
}

namespace Evidyon {
namespace Magic {

  
//----[  BuffMask  ]-----------------------------------------------------------
// These entries are in exactly the same order as the Buff enumerated type.
union BuffMask {
  struct {
    unsigned max_hp  : 1;
    unsigned max_mp  : 1;
    unsigned defense  : 1;
    unsigned melee_damage   : 1;
    unsigned magic_damage   : 1;
    unsigned hp_steal : 1;
    unsigned mp_steal : 1;
    unsigned action_speed_modifier : 1;
    unsigned movement_speed : 1;
    unsigned hp_regen : 1;
    unsigned poison   : 1;
    unsigned mp_regen : 1;
    unsigned leech    : 1;
  } fields;
  unsigned int value;

  // Returns whether or not the corresponding field is set to 1
  bool get(Buff buff) const;

  // Changes the value of the specified field
  void set(Buff buff, bool enabled);
};


}
}

#endif