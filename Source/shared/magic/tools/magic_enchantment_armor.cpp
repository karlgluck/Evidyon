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
#include "shared/magic/tools/magic_enchantment_armor.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_enchantment_armor.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Enchantment_Armor>::Implements<Evidyon::Magic::Tools::Magic> Magic_Enchantment_Armor::implementsMagic;


//----[  Magic_Enchantment_Armor  ]--------------------------------------------
Magic_Enchantment_Armor::Magic_Enchantment_Armor() {
  member("Special FX", &special_fx_);
  member("Defense", &defense_);
  member("Duration (ms)", &duration_ms);
  member("Armor Type (0, 1, or 2)", &armor_type_);
  duration_ms.setValue(5.0 * 60000);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Enchantment_Armor::compile(MagicCompiler* compiler) {
  special_fx_duration_.setValue(-1.0); // never expire
  Event::EventIndex event_index = compileTargetedSpecialFX(compiler);

  Evidyon::Magic::Magic_Enchantment_Armor description;
  description.defense = defense_.getValue();
  description.duration = duration_ms.getValue();
  return compiler->add(event_index, &description, armor_type_.getValue());
}



}
}
}

