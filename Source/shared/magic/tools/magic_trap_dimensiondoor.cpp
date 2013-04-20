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
#include "shared/magic/tools/magic_trap_dimensiondoor.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_trap_dimensiondoor.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Trap_DimensionDoor>::Implements<Evidyon::Magic::Tools::Magic> Magic_Trap_DimensionDoor::implementsMagic;


//----[  Magic_Trap_DimensionDoor  ]-------------------------------------------
Magic_Trap_DimensionDoor::Magic_Trap_DimensionDoor() {
  member("Duration", &duration_ms);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Trap_DimensionDoor::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Trap_DimensionDoor description;
  description.duration = duration_ms.getValue();
  return compiler->add(&description);
}



}
}
}

