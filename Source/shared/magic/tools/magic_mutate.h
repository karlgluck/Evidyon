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
#ifndef __SHARED_MAGIC_TOOLS_MAGIC_MUTATE_H__
#define __SHARED_MAGIC_TOOLS_MAGIC_MUTATE_H__
#pragma once

#include "shared/magic/tools/magic.h"
#include "shared/actor/tools/actorformreference.h"
#include <dc/float>
#include <dc/integer>

namespace Evidyon {
namespace Magic {
namespace Tools {


  
//----[  Magic_Mutate  ]-------------------------------------------------------
class Magic_Mutate : public dc::dcResource<Magic_Mutate, Evidyon::Magic::Tools::Magic> {
  static dc::dcClass<Magic_Mutate>::Implements<Evidyon::Magic::Tools::Magic> implementsMagic;
public:
  static std::string staticTypeName() { return "Magic_Mutate"; }
public:
  Magic_Mutate();
  virtual MagicIndex compile(MagicCompiler* compiler);
private:
  dc::dcBoolean can_speak_;
  dc::dcBoolean can_cast_spells_;
  dc::dcBoolean can_melee_attack_;
  dc::dcBoolean can_be_seen_by_monsters_;
  dc::dcBoolean flying, levitating_;
  dc::dcFloat movement_speed_added_;
  dc::dcFloat attack_delay_multiplier_;
  dc::dcInteger added_armor_;
  dc::dcFloat duration_;

  // ? take target's profile & form if avatar and < level
  dc::dcInteger copy_target_avatar_level_;
  Evidyon::Actor::Tools::ActorFormReference mutated_form_;
};



}
}
}

#endif