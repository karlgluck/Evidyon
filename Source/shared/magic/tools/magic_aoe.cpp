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
#include "shared/magic/tools/magic_aoe.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_aoe.h"
#include "shared/magic/magiclimits.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_AOE>::Implements<Evidyon::Magic::Tools::Magic> Magic_AOE::implementsMagic;


//----[  Magic_AOE  ]----------------------------------------------------------
Magic_AOE::Magic_AOE() {
  member("Data", &data_);
  member("Effect Delay", &effect_delay_ms);
  member("DPS Multiplier", &dps_multiplier);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_AOE::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_AOE description;
  data_.compile(compiler->eventCompiler(), &description.data);
  description.effect_delay = effect_delay_ms.getValue();
  description.dps_to_damage_multiplier =
    dps_multiplier.getValue() * (total_action_delay / 1000.0);
  return compiler->add(&description);
}



}
}
}

