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
#include "shared/magic/tools/magic_buff.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_buff.h"
#include "shared/event/tools/eventcompiler.h"
#include "shared/event/event_targetedspecialfx.h"


namespace Evidyon {
namespace Magic {
namespace Tools {

dc::dcClass<Magic_Buff>::Implements<Evidyon::Magic::Tools::Magic> Magic_Buff::implementsMagic;


//----[  Magic_Buff  ]---------------------------------------------------------
Magic_Buff::Magic_Buff() {
  member("Special FX", &special_fx_);
  member("Special FX Duration", &special_fx_duration_);
  member("Added Max HP", &max_hp_);
  member("Added Max MP", &max_mp_);
  member("Added Defense", &defense_);
  member("Melee Damage Bonus", &melee_damage_);
  member("Magic Damage Bonus", &magic_damage_);
  member("Steal HP %", &hp_steal_);
  member("Steal MP %", &mp_steal_);
  member("Action Speed Modifier", &action_speed_modifier_);
  member("Added Movement Speed", &movement_speed_);
  member("HP Regen (negative = weaken)", &hp_regen_change_);
  member("MP Regen (negative = leech)", &mp_regen_change_);
  member("Duration (ms)", &duration_ms);
  member("Target Self?", &target_self_);

  target_self_.setValue(true);
  duration_ms.setValue(60000.0 * 4);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Buff::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Buff description;
  compile(compiler, &description);
  return compiler->add(&description);
}



//----[  compile  ]------------------------------------------------------------
void Magic_Buff::compile(MagicCompiler* magic_compiler,
                         Evidyon::Magic::Magic_Buff* description) {
  memset(description, 0, sizeof(*description));

  description->event_index = compileTargetedSpecialFX(magic_compiler);
  description->duration = duration_ms.getValue();

  if (max_hp_.getValue() != 0) {
    description->enabled_mask.set(BUFF_MAX_HP, true);
    description->buff_values[BUFF_MAX_HP] = max_hp_.getValue();
  }
  if (max_mp_.getValue() != 0) {
    description->enabled_mask.set(BUFF_MAX_MP, true);
    description->buff_values[BUFF_MAX_MP] = max_mp_.getValue();
  }
  if (defense_.getValue() != 0) {
    description->enabled_mask.set(BUFF_DEFENSE, true);
    description->buff_values[BUFF_DEFENSE] = defense_.getValue();
  }
  if (melee_damage_.getValue() != 0.0) {
    description->enabled_mask.set(BUFF_MELEE_DAMAGE, true);
    description->buff_values[BUFF_MELEE_DAMAGE] = melee_damage_.getValue();
  }
  if (magic_damage_.getValue() != 0.0) {
    description->enabled_mask.set(BUFF_MAGIC_DAMAGE, true);
    description->buff_values[BUFF_MAGIC_DAMAGE] = magic_damage_.getValue();
  }
  if (hp_steal_.getValue() != 0.0) {
    description->enabled_mask.set(BUFF_HP_STEAL, true);
    description->buff_values[BUFF_HP_STEAL] = (hp_steal_.getValue()/100.0);
  }
  if (mp_steal_.getValue() != 0.0) {
    description->enabled_mask.set(BUFF_MP_STEAL, true);
    description->buff_values[BUFF_MP_STEAL] = (mp_steal_.getValue()/100.0);
  }
  if (action_speed_modifier_.getValue() != 0.0) {
    description->enabled_mask.set(BUFF_ACTION_SPEED_MODIFIER, true);
    description->buff_values[BUFF_ACTION_SPEED_MODIFIER] = action_speed_modifier_.getValue();
  }
  if (movement_speed_.getValue() != 0.0) {
    description->enabled_mask.set(BUFF_MOVEMENT_SPEED, true);
    description->buff_values[BUFF_MOVEMENT_SPEED] = movement_speed_.getValue();
  }
  if (hp_regen_change_.getValue() > 0) {
    description->enabled_mask.set(BUFF_HP_REGEN, true);
    description->buff_values[BUFF_HP_REGEN] = hp_regen_change_.getValue();
  }
  if (hp_regen_change_.getValue() < 0) {
    description->enabled_mask.set(BUFF_POISON, true);
    description->buff_values[BUFF_POISON] = -hp_regen_change_.getValue();
  }
  if (mp_regen_change_.getValue() > 0) {
    description->enabled_mask.set(BUFF_MP_REGEN, true);
    description->buff_values[BUFF_MP_REGEN] = mp_regen_change_.getValue();
  }
  if (mp_regen_change_.getValue() < 0) {
    description->enabled_mask.set(BUFF_LEECH, true);
    description->buff_values[BUFF_LEECH] = -mp_regen_change_.getValue();
  }

  description->target_self = target_self_.getValue();
}



}
}
}

