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

