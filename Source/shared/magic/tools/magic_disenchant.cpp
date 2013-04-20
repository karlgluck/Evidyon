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
#include "shared/magic/tools/magic_disenchant.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_disenchant.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Disenchant>::Implements<Evidyon::Magic::Tools::Magic> Magic_Disenchant::implementsMagic;


//----[  Magic_Disenchant  ]---------------------------------------------------
Magic_Disenchant::Magic_Disenchant() {
  member("Special FX", &special_fx_);
  member("Special FX Duration", &special_fx_duration_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Disenchant::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Disenchant description;
  Event::Event_TargetedSpecialFX event_description;
  event_description.special_fx = special_fx_.getReferencedResourceIndex();
  event_description.duration = special_fx_duration_.getValue();
  description.event_index = compiler->eventCompiler()->add(&event_description);
  return compiler->add(&description);
}



}
}
}

