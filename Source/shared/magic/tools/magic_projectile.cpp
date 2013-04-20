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
#include "shared/magic/tools/magic_projectile.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_projectile.h"

#include <d3dx9math.h> // for D3DXToRadian

namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Projectile>::Implements<Evidyon::Magic::Tools::Magic> Magic_Projectile::implementsMagic;


//----[  Magic_Projectile  ]---------------------------------------------------
Magic_Projectile::Magic_Projectile() {
  member("DPS Multiplier", &dps_multiplier);
  member("Projectiles Per Stage", &targets);
  member("Forward Spread Angle", &spread_);
  member("Period (ms)", &period_ms);
  member("Effect Delay (ms)", &effect_delay_ms);
  member("Duration (ms)", &duration_ms);
  member("Use Weapon Projectile?", &use_weapon_projectile_);
  member("Projectile", &projectile_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Projectile::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Projectile description;
  description.projectiles_per_stage = targets.getValue();
  description.stage_delay = period_ms.getValue();
  description.effect_delay = effect_delay_ms.getValue();
  if (duration_ms.getValue() == 0 || period_ms.getValue() == 0) {
    // this is a single-projectile effect, so base its dps on the rate at which
    // the player can repeatedly cast the effect.
    description.number_of_stages = 1;
    description.dps_to_damage_multiplier
      = dps_multiplier.getValue() * (use_weapon_projectile_.getValue() ? 1.0f : (total_action_delay / 1000.0));
  } else {
    // this is a multistage projectile, so base the dps on the stages
    description.number_of_stages
      = ((duration_ms.getValue() + period_ms.getValue() - 1) / period_ms.getValue());
    description.dps_to_damage_multiplier
      = dps_multiplier.getValue() * period_ms.getValue() / 1000.0 / description.projectiles_per_stage;
  }
  description.half_spread = D3DXToRadian(spread_.getValue() / 2.0f);
  description.use_weapon_projectile = use_weapon_projectile_.getValue();
  projectile_.compile(compiler->eventCompiler(), &description.projectile);
  return compiler->add(&description);
}



}
}
}

