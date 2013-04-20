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
#include "shared/magic/tools/magic_enchantment_shield.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_enchantment_shield.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Enchantment_Shield>::Implements<Evidyon::Magic::Tools::Magic> Magic_Enchantment_Shield::implementsMagic;


//----[  Magic_Enchantment_Shield  ]-------------------------------------------
Magic_Enchantment_Shield::Magic_Enchantment_Shield() {
  member("Special FX", &special_fx_);
  member("Absorbtion", &absorbtion_);
  member("Damage Points Absorbed", &damage_points_absorbed_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Enchantment_Shield::compile(MagicCompiler* compiler) {
  Event::Event_TargetedSpecialFX event_description;
  event_description.special_fx = special_fx_.getReferencedResourceIndex();
  event_description.duration = -1.0; // lasts until all points are used
  Event::EventIndex event_index = compiler->eventCompiler()->add(&event_description);

  Evidyon::Magic::Magic_Enchantment_Shield description;
  description.absorbtion = absorbtion_.getValue();
  description.damage_points_absorbed = damage_points_absorbed_.getValue();
  return compiler->add(event_index, &description);
}



}
}
}

