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

