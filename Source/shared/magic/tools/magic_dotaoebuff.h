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
#ifndef __SHARED_MAGIC_TOOLS_MAGIC_DOTAOEBUFF_H__
#define __SHARED_MAGIC_TOOLS_MAGIC_DOTAOEBUFF_H__
#pragma once

#include "shared/magic/tools/magic.h"
#include "shared/magic/tools/aoemagicdata.h"
#include "shared/magic/tools/magic_buff.h"
#include <dc/float>
#include <dc/integer>

namespace Evidyon {
namespace Magic {
namespace Tools {


  
//----[  Magic_DOTAOEBuff  ]---------------------------------------------------
class Magic_DOTAOEBuff : public dc::dcResource<Magic_DOTAOEBuff, Evidyon::Magic::Tools::Magic> {
  static dc::dcClass<Magic_DOTAOEBuff>::Implements<Evidyon::Magic::Tools::Magic> implementsMagic;
public:
  static std::string staticTypeName() { return "Magic_DOTAOEBuff"; }
public:
  Magic_DOTAOEBuff();
  virtual MagicIndex compile(MagicCompiler* compiler);
private:
  Evidyon::Magic::Tools::AOEMagicData data_;  // basic targeting data
  Evidyon::Magic::Tools::Magic_Buff buff_;    // buff to apply
  dc::dcBoolean terminate_on_death_;          // ends when the player dies?
};



}
}
}

#endif