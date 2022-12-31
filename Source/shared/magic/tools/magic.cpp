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

