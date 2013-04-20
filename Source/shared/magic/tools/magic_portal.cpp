//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#include "shared/magic/tools/magic_portal.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_portal.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Portal>::Implements<Evidyon::Magic::Tools::Magic> Magic_Portal::implementsMagic;


//----[  Magic_Portal  ]-------------------------------------------------------
Magic_Portal::Magic_Portal() {
  member("Type", &type_);
  member("Source Effect", &source_fx_);
  member("Target Effect", &target_fx_);
  member("Chaos Effect", &chaos_fx_);
  member("Open Delay (ms)", &period_ms);
  member("Duration (ms)", &duration_ms);
  member("Radius", &range);
  member("Destination Map", &target_map_);
  member("Destination X", &target_x_);
  member("Destination Z", &target_z_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Portal::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Portal description;
  description.type = type_.getValue();
  description.open_delay
    = Number(period_ms.getValue()).wholePart();
  description.close_delay
    = description.open_delay + duration_ms.getValue();
  { // source effect
    Event::Event_TargetedSpecialFX event_description;
    event_description.special_fx = source_fx_.getReferencedResourceIndex();
    if (event_description.special_fx != SpecialFX::INVALID_SPECIALFX_INDEX) {
      event_description.duration = description.close_delay / 1000.0;
      description.source_event_index
        = compiler->eventCompiler()->add(&event_description);
    } else {
      description.source_event_index = Event::INVALID_EVENT_INDEX;
    }
  }
  { // target effect
    Event::Event_TargetedSpecialFX event_description;
    event_description.special_fx = target_fx_.getReferencedResourceIndex();
    if (event_description.special_fx != SpecialFX::INVALID_SPECIALFX_INDEX) {
      event_description.duration = description.close_delay / 1000.0;
      description.target_event_index
        = compiler->eventCompiler()->add(&event_description);
    } else {
      description.target_event_index = Event::INVALID_EVENT_INDEX;
    }
  }
  { // chaos effect
    Event::Event_TargetedSpecialFX event_description;
    event_description.special_fx = chaos_fx_.getReferencedResourceIndex();
    if (event_description.special_fx != SpecialFX::INVALID_SPECIALFX_INDEX) {
      event_description.duration = description.close_delay / 1000.0;
      description.chaos_event_index
        = compiler->eventCompiler()->add(&event_description);
    } else {
      description.chaos_event_index = Event::INVALID_EVENT_INDEX;
    }
  }
  description.radius_sq = range.getValue() * range.getValue();
  description.target_map = target_map_.getReferencedResourceIndex();
  description.target_x = target_x_.getValue();
  description.target_z = target_z_.getValue();
  return compiler->add(&description);
}



}
}
}

