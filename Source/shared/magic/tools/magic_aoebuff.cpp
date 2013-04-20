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
#include "shared/magic/tools/magic_aoebuff.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_aoebuff.h"
#include "shared/magic/magiclimits.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_AOEBuff>::Implements<Evidyon::Magic::Tools::Magic> Magic_AOEBuff::implementsMagic;


//----[  Magic_AOEBuff  ]------------------------------------------------------
Magic_AOEBuff::Magic_AOEBuff() {
  member("AOE Data", &data_);
  member("Buff", &buff_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_AOEBuff::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_AOEBuff description;
  data_.compile(compiler->eventCompiler(), &description.data);
  buff_.compile(compiler, &description.buff);
  return compiler->add(&description);
}



}
}
}

