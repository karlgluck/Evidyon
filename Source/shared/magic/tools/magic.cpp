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
#include "magic.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {



  
//----[  Magic  ]--------------------------------------------------------------
Magic::Magic() {
  total_action_delay = 1000;
  special_fx_duration_.setValue(1.0f);
  dps_multiplier.setValue(1.0f);
  targets.setValue(1);
  range.setValue(4.5f);
}

//----[  compileTargetedSpecialFX  ]-------------------------------------------
Event::EventIndex Magic::compileTargetedSpecialFX(MagicCompiler* compiler) {
  Event::Event_TargetedSpecialFX event_description;
  event_description.special_fx = special_fx_.getReferencedResourceIndex();
  event_description.duration = special_fx_duration_.getValue();
  return compiler->eventCompiler()->add(&event_description);
}


}
}
}

