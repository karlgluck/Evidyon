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
#include "shared/magic/tools/magic_spinattack.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_spinattack.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"
#include "shared/magic/magiclimits.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_SpinAttack>::Implements<Evidyon::Magic::Tools::Magic> Magic_SpinAttack::implementsMagic;


//----[  Magic_SpinAttack  ]---------------------------------------------------
Magic_SpinAttack::Magic_SpinAttack() {
  member("Special FX", &special_fx_);
  member("Special FX Duration", &special_fx_duration_);
  member("Effect Delay", &effect_delay_ms);
  member("Damage Multiplier", &dps_multiplier);
  member("Targets", &targets);
  member("Allow Projectile Weapons?", &allow_projectiles_);
  member("Allow Melee Weapons?", &allow_melee_);
  targets.setValue(6);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_SpinAttack::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_SpinAttack description;
  description.event_index = compileTargetedSpecialFX(compiler);
  description.effect_delay = effect_delay_ms.getValue();
  description.damage_factor = dps_multiplier.getValue();
  description.max_targets = targets.getValue();
  if (description.max_targets > Evidyon::Magic::MAX_TARGETS) {
    description.max_targets = Evidyon::Magic::MAX_TARGETS;
  }
  description.allow_projectiles = allow_projectiles_.getValue();
  description.allow_melee = allow_melee_.getValue();
  return compiler->add(&description);
}



}
}
}

