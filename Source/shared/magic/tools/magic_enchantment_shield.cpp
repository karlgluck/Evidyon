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
#include "shared/magic/tools/magic_enchantment_shield.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_enchantment_shield.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Enchantment_Shield>::Implements<Evidyon::Magic::Tools::Magic> Magic_Enchantment_Shield::implementsMagic;


//----[  Magic_Enchantment_Shield  ]-------------------------------------------
Magic_Enchantment_Shield::Magic_Enchantment_Shield() {
  member("Special FX", &special_fx_);
  member("Absorbtion", &absorbtion_);
  member("Damage Points Absorbed", &damage_points_absorbed_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Enchantment_Shield::compile(MagicCompiler* compiler) {
  Event::Event_TargetedSpecialFX event_description;
  event_description.special_fx = special_fx_.getReferencedResourceIndex();
  event_description.duration = -1.0; // lasts until all points are used
  Event::EventIndex event_index = compiler->eventCompiler()->add(&event_description);

  Evidyon::Magic::Magic_Enchantment_Shield description;
  description.absorbtion = absorbtion_.getValue();
  description.damage_points_absorbed = damage_points_absorbed_.getValue();
  return compiler->add(event_index, &description);
}



}
}
}

