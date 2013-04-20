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
#include "shared/magic/tools/magic_enchantment_armor.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_enchantment_armor.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Enchantment_Armor>::Implements<Evidyon::Magic::Tools::Magic> Magic_Enchantment_Armor::implementsMagic;


//----[  Magic_Enchantment_Armor  ]--------------------------------------------
Magic_Enchantment_Armor::Magic_Enchantment_Armor() {
  member("Special FX", &special_fx_);
  member("Defense", &defense_);
  member("Duration (ms)", &duration_ms);
  member("Armor Type (0, 1, or 2)", &armor_type_);
  duration_ms.setValue(5.0 * 60000);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Enchantment_Armor::compile(MagicCompiler* compiler) {
  special_fx_duration_.setValue(-1.0); // never expire
  Event::EventIndex event_index = compileTargetedSpecialFX(compiler);

  Evidyon::Magic::Magic_Enchantment_Armor description;
  description.defense = defense_.getValue();
  description.duration = duration_ms.getValue();
  return compiler->add(event_index, &description, armor_type_.getValue());
}



}
}
}

