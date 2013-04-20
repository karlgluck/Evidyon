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
#include "shared/magic/tools/aoemagicdata.h"
#include "shared/magic/aoemagicdata.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"
#include "shared/magic/magiclimits.h"


namespace Evidyon {
namespace Magic {
namespace Tools {


//----[  AOEMagicData  ]-------------------------------------------------------
AOEMagicData::AOEMagicData() {
  member("Source (source fx binding)", &source_);
  member("Effect Location (target fx binding, center of effect)", &effect_);
  member("Special FX", &special_fx_);
  member("Special FX Duration", &special_fx_duration_);
  member("Radius", &radius_);
  member("Target Caster", &target_caster_);
  member("Target Friends", &target_friends_);
  member("Target Enemies", &target_enemies_);
  member("Max Targets", &max_targets_);
  max_targets_.setValue(6);
  target_enemies_.setValue(true);
  source_.setValue(Magic::AOEMagicData::SELF);
  effect_.setValue(Magic::AOEMagicData::SELF);
  special_fx_duration_.setValue(1.0f);
  radius_.setValue(4.5f);
}

//----[  compile  ]------------------------------------------------------------
void AOEMagicData::compile(Evidyon::Event::Tools::EventCompiler* event_compiler,
                           Evidyon::Magic::AOEMagicData* description) {
  description->source = source_.getValue();
  description->effect = effect_.getValue();
  Event::Event_TargetedSpecialFX event_description;
  event_description.special_fx = special_fx_.getReferencedResourceIndex();
  event_description.duration = special_fx_duration_.getValue();
  description->event_index = event_compiler->add(&event_description);
  description->radius_sq = radius_.getValue() * radius_.getValue();
  description->target_caster = target_caster_.getValue();
  description->target_friends = target_friends_.getValue();
  description->target_enemies = target_enemies_.getValue();
  description->max_targets = max_targets_.getValue();
  if (description->max_targets > Evidyon::Magic::MAX_TARGETS) {
    description->max_targets = Evidyon::Magic::MAX_TARGETS;
  }
}



//----[  getDurationMillis  ]--------------------------------------------------
Time::Milliseconds AOEMagicData::getDurationMillis() {
  return Time::Milliseconds(special_fx_duration_.getValue() * 1000.0);
}

//----[  setDurationMillis  ]--------------------------------------------------
void AOEMagicData::setDurationMillis(Time::Milliseconds time) {
  special_fx_duration_.setValue(time / 1000.0);
}

}
}
}

