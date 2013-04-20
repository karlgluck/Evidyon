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
#include "shared/magic/tools/magic_enchantment_thorns.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_enchantment_thorns.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Enchantment_Thorns>::Implements<Evidyon::Magic::Tools::Magic> Magic_Enchantment_Thorns::implementsMagic;


//----[  Magic_Enchantment_Thorns  ]-------------------------------------------
Magic_Enchantment_Thorns::Magic_Enchantment_Thorns() {
  member("Special FX", &special_fx_);
  member("Damage Returned", &damage_returned_);
  member("Duration (ms)", &duration_ms);
  duration_ms.setValue(5.0 * 60000);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Enchantment_Thorns::compile(MagicCompiler* compiler) {
  special_fx_duration_.setValue(-1.0); // never expire
  Event::EventIndex event_index = compileTargetedSpecialFX(compiler);

  Evidyon::Magic::Magic_Enchantment_Thorns description;
  description.damage_returned = damage_returned_.getValue();
  description.duration = duration_ms.getValue();
  return compiler->add(event_index, &description);
}



}
}
}

