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
#ifndef __SHARED_MAGIC_TOOLS_MAGIC_MUTATE_H__
#define __SHARED_MAGIC_TOOLS_MAGIC_MUTATE_H__
#pragma once

#include "shared/magic/tools/magic.h"
#include "shared/actor/tools/actorformreference.h"
#include <dc/float>
#include <dc/integer>

namespace Evidyon {
namespace Magic {
namespace Tools {


  
//----[  Magic_Mutate  ]-------------------------------------------------------
class Magic_Mutate : public dc::dcResource<Magic_Mutate, Evidyon::Magic::Tools::Magic> {
  static dc::dcClass<Magic_Mutate>::Implements<Evidyon::Magic::Tools::Magic> implementsMagic;
public:
  static std::string staticTypeName() { return "Magic_Mutate"; }
public:
  Magic_Mutate();
  virtual MagicIndex compile(MagicCompiler* compiler);
private:
  dc::dcBoolean can_speak_;
  dc::dcBoolean can_cast_spells_;
  dc::dcBoolean can_melee_attack_;
  dc::dcBoolean can_be_seen_by_monsters_;
  dc::dcBoolean flying, levitating_;
  dc::dcFloat movement_speed_added_;
  dc::dcFloat attack_delay_multiplier_;
  dc::dcInteger added_armor_;
  dc::dcFloat duration_;

  // ? take target's profile & form if avatar and < level
  dc::dcInteger copy_target_avatar_level_;
  Evidyon::Actor::Tools::ActorFormReference mutated_form_;
};



}
}
}

#endif