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
#include "shared/magic/tools/magic_leap.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_leap.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Leap>::Implements<Evidyon::Magic::Tools::Magic> Magic_Leap::implementsMagic;


//----[  Magic_Leap  ]---------------------------------------------------------
Magic_Leap::Magic_Leap() {
  member("Special FX", &special_fx_);
  member("Special FX Duration", &special_fx_duration_);
  member("Speed Factor", &speed_factor_);
  member("Maximum Distance", &max_distance_);
  member("First Hit Damage Bonus", &first_hit_damage_bonus_);

  speed_factor_.setValue(2.5);
  first_hit_damage_bonus_.setValue(1.0);
  max_distance_.setValue(8.0f);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Leap::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Leap description;
  description.event_index = compileTargetedSpecialFX(compiler);
  description.speed_factor = speed_factor_.getValue();
  description.max_distance = max_distance_.getValue();
  description.first_hit_damage_bonus = first_hit_damage_bonus_.getValue();
  return compiler->add(&description);
}



}
}
}

