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
#include "shared/magic/tools/magic_directdamage.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_directdamage.h"
#include "shared/event/tools/eventcompiler.h"
#include "shared/event/event_targetedspecialfx.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_DirectDamage>::Implements<Evidyon::Magic::Tools::Magic> Magic_DirectDamage::implementsMagic;


//----[  Magic_DirectDamage  ]-------------------------------------------------
Magic_DirectDamage::Magic_DirectDamage() {
  member("Special FX", &special_fx_);
  member("Special FX Duration", &special_fx_duration_);
  member("Effect Delay", &effect_delay_ms);
  member("DPS Multiplier", &dps_multiplier);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_DirectDamage::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_DirectDamage description;
  description.event_index = compileTargetedSpecialFX(compiler);
  description.effect_delay = effect_delay_ms.getValue();
  description.dps_to_damage_multiplier
    = dps_multiplier.getValue() * (total_action_delay / 1000.0);
  return compiler->add(&description);
}



}
}
}

