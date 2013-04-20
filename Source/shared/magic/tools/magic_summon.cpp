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
#include "magic_summon.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_summon.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Summon>::Implements<Evidyon::Magic::Tools::Magic> Magic_Summon::implementsMagic;


//----[  Magic_Summon  ]-------------------------------------------------------
Magic_Summon::Magic_Summon() {
  member("Special FX", &special_fx_);
  member("Special FX Duration", &special_fx_duration_);
  member("HP", &hp_);
  member("Percentage of Caster's HP", &percentage_of_casters_hp_);
  member("Melee Damage", &melee_damage_);
  member("Attack Delay", &attack_delay_);
  member("Flying?", &flying_);
  member("Can Swim?", &can_swim_);
  member("Summoned Form", &form_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Summon::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Summon description;
  description.event_index = compileTargetedSpecialFX(compiler);
  description.hp = hp_.getValue();
  description.percentage_of_casters_hp = percentage_of_casters_hp_.getValue();
  description.melee_damage = melee_damage_.getValue();
  description.attack_delay = attack_delay_.getValue();
  description.flying = flying_.getValue();
  description.can_swim = can_swim_.getValue();
  form_.compile(&description.actor_profile, &description.mesh_form);
  return compiler->add(&description);
}


}
}
}

