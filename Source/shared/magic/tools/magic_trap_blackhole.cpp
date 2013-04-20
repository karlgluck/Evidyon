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
#include "shared/magic/tools/magic_trap_blackhole.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_trap_blackhole.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Trap_BlackHole>::Implements<Evidyon::Magic::Tools::Magic> Magic_Trap_BlackHole::implementsMagic;


//----[  Magic_Trap_BlackHole  ]-----------------------------------------------
Magic_Trap_BlackHole::Magic_Trap_BlackHole() {
  member("Duration", &duration_ms);
  member("Radius", &range);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Trap_BlackHole::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Trap_BlackHole description;
  description.duration = duration_ms.getValue();
  description.radius = range.getValue();
  return compiler->add(&description);
}



}
}
}

