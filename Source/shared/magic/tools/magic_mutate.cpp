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
#include "shared/magic/tools/magic_mutate.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_mutate.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Mutate>::Implements<Evidyon::Magic::Tools::Magic> Magic_Mutate::implementsMagic;


//----[  Magic_Mutate  ]-------------------------------------------------------
Magic_Mutate::Magic_Mutate() {
  member("Can Speak", &can_speak_);
  member("Can Cast Spells", &can_cast_spells_);
  member("Can Melee Attack", &can_melee_attack_);
  member("Can Be Seen By Monsters", &can_be_seen_by_monsters_);
  member("Levitating", &levitating_);
  member("Movement Speed Added", &movement_speed_added_);
  member("Attack Delay Multiplier", &attack_delay_multiplier_);
  member("Added Armor", &added_armor_);
  member("Duration", &duration_);
  member("Copy Target Avatar Level", &copy_target_avatar_level_);
  member("Mutated Form", &mutated_form_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Mutate::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Mutate description;
  description.can_speak = can_speak_.getValue();
  description.can_cast_spells = can_cast_spells_.getValue();
  description.can_melee_attack = can_melee_attack_.getValue();
  description.can_be_seen_by_monsters = can_be_seen_by_monsters_.getValue();
  description.levitating = levitating_.getValue();
  description.movement_speed_added = movement_speed_added_.getValue();
  description.attack_delay_multiplier = attack_delay_multiplier_.getValue();
  description.added_armor = added_armor_.getValue();
  description.duration = duration_.getValue();
  description.copy_target_avatar_level = copy_target_avatar_level_.getValue();
  mutated_form_.compile(&description.actor_profile, &description.mesh_form);
  return compiler->add(&description);
}



}
}
}

