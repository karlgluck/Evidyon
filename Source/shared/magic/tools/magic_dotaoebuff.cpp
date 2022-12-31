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
#include "shared/magic/tools/magic_dotaoebuff.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_dotaoebuff.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_DOTAOEBuff>::Implements<Evidyon::Magic::Tools::Magic> Magic_DOTAOEBuff::implementsMagic;


//----[  Magic_DOTAOEBuff  ]---------------------------------------------------
Magic_DOTAOEBuff::Magic_DOTAOEBuff() {
  member("Data", &data_);
  member("Period (ms)", &period_ms);
  member("Duration (ms)", &duration_ms);
  member("Buff", &buff_);
  member("DPS Multiplier", &dps_multiplier);
  member("Terminate On Death?", &terminate_on_death_);
  period_ms.setValue(1000);
  dps_multiplier.setValue(0.1f);
  terminate_on_death_.setValue(true);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_DOTAOEBuff::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_DOTAOEBuff description;
  data_.setDurationMillis(duration_ms.getValue());
  data_.compile(compiler->eventCompiler(), &description.data);
  description.period = period_ms.getValue();
  description.duration = duration_ms.getValue();
  buff_.compile(compiler, &description.buff);
  description.dps_to_damage_multiplier
    = (description.period / 1000.0) * dps_multiplier.getValue();
  description.terminate_on_death = terminate_on_death_.getValue();
  return compiler->add(&description);
}



}
}
}

