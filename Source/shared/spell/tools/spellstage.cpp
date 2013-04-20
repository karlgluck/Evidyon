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
#include "spellstage.h"
#include "shared/spell/spellstageserverdescription.h"

#include "shared/magic/tools/aoemagicdata.h"
#include "shared/magic/tools/magic.h"
#include "shared/magic/tools/magic_spinattack.h"
#include "shared/magic/tools/magic_attackrun.h"
#include "shared/magic/tools/magic_leap.h"
#include "shared/magic/tools/magic_buff.h"
#include "shared/magic/tools/magic_aoe.h"
#include "shared/magic/tools/magic_aoebuff.h"
#include "shared/magic/tools/magic_dot.h"
#include "shared/magic/tools/magic_dotaoebuff.h"
#include "shared/magic/tools/magic_directdamage.h"
#include "shared/magic/tools/magic_projectile.h"
#include "shared/magic/tools/magic_branching.h"
#include "shared/magic/tools/magic_portal.h"
#include "shared/magic/tools/magic_teleport.h"
#include "shared/magic/tools/magic_mutate.h"
#include "shared/magic/tools/magic_summon.h"
#include "shared/magic/tools/magic_trap_dimensiondoor.h"
#include "shared/magic/tools/magic_trap_blackhole.h"
#include "shared/magic/tools/magic_disenchant.h"
#include "shared/magic/tools/magic_invisibility.h"
#include "shared/magic/tools/magic_heal.h"
#include "shared/magic/tools/magic_enchantment_armor.h"
#include "shared/magic/tools/magic_enchantment_shield.h"
#include "shared/magic/tools/magic_enchantment_reactivehealing.h"
#include "shared/magic/tools/magic_enchantment_thorns.h"




namespace Evidyon {
namespace Spell {
namespace Tools {




  
//----[  SpellStage  ]---------------------------------------------------------
SpellStage::SpellStage() {
  member("Magic", &magic_);
  for (int class_index = 0; class_index < Avatar::NUMBER_OF_AVATAR_CLASSES; ++class_index) {
    std::string class_name
      = Avatar::AvatarClassString((Avatar::AvatarClass)class_index);
    member(class_name, &level_obtained_[class_index]);
  }
  member("Improvement Level Limit", &improvement_level_limit_);
  member("Action Delay (ms)", &action_delay_ms_);
}



//----[  compile  ]------------------------------------------------------------
void SpellStage::compile(Magic::Tools::MagicCompiler* magic_compiler,
                         Evidyon::Spell::SpellStageServerDescription* description) {
  for (int class_index = 0; class_index < Avatar::NUMBER_OF_AVATAR_CLASSES; ++class_index) {
    description->level_obtained[class_index]
      = level_obtained_[class_index].getValue();
    if (!description->level_obtained[class_index]) {
      // invalidate this spell's level
      description->level_obtained[class_index] = 9999;
    }
  }
  description->improvement_level_limit = improvement_level_limit_.getValue();
  Evidyon::Magic::Tools::Magic* magic = magic_.getImplementation();
  if (magic) magic->total_action_delay = action_delay_ms_.getValue();
  description->magic_index
    = magic ? magic->compile(magic_compiler) : Magic::INVALID_MAGIC_INDEX;
  description->action_delay = action_delay_ms_.getValue();
}


//----[  set  ]----------------------------------------------------------------
void SpellStage::set(std::map<std::string,std::string>* values) {
  for (int class_index = 0; class_index < Avatar::NUMBER_OF_AVATAR_CLASSES; ++class_index) {
    if (level_obtained_[class_index].getName().compare(values->find("Class Name")->second) == 0) {
      level_obtained_[class_index].interpret(values->find("Get at Level")->second.c_str());
    }
  }

  // try to figure out the type
  if (!magic_.isImplemented()) {
    const std::string& type_str = values->find("Type")->second;
#define IF_SET_TYPE(text, type)\
    if (stricmp(type_str.c_str(),text) == 0) magic_.implement<Magic::Tools::type>();
    IF_SET_TYPE("AOE", Magic_AOE);
    IF_SET_TYPE("Direct Damage", Magic_DirectDamage);
    IF_SET_TYPE("AOE-DOT", Magic_DOTAOEBuff);
    IF_SET_TYPE("Heal", Magic_Heal);
    IF_SET_TYPE("DOT", Magic_DOT);
    IF_SET_TYPE("Projectile", Magic_Projectile);
    IF_SET_TYPE("Armor Enchantment", Magic_Enchantment_Armor);
    IF_SET_TYPE("Buff", Magic_Buff);
    IF_SET_TYPE("AOE Buff", Magic_AOEBuff);
    IF_SET_TYPE("Teleport", Magic_Teleport);
    IF_SET_TYPE("Portal", Magic_Portal);
    IF_SET_TYPE("Mutate", Magic_Mutate);
    IF_SET_TYPE("Leap", Magic_Leap);
    IF_SET_TYPE("Attack Run", Magic_AttackRun);
    IF_SET_TYPE("Shoot", Magic_Projectile);
    IF_SET_TYPE("Spin Attack", Magic_SpinAttack);
  }
  if (magic_.isImplemented()) {
    Evidyon::Magic::Tools::Magic* magic = magic_.getImplementation();
    magic->effect_delay_ms.interpret(values->find("Effect Delay (ms)")->second.c_str());
    magic->dps_multiplier.interpret(values->find("DPS Modifier")->second.c_str());
    if (magic->dps_multiplier.getValue() > 5.0) {      // correct for exported %s
      // this multiplier is probably a %
      magic->dps_multiplier.setValue(magic->dps_multiplier.getValue() / 100.0f); // -> %
    }
    magic->targets.interpret(values->find("Targets")->second.c_str());
    magic->period_ms.interpret(values->find("Period")->second.c_str());
    magic->duration_ms.interpret(values->find("Duration")->second.c_str());

    const std::string& range_str = values->find("Range")->second;
#define IF_SET_RANGE(text, value)\
  if (stricmp(range_str.c_str(), text) == 0) magic->range.setValue(value);
    IF_SET_RANGE("very small", 2.25);
    IF_SET_RANGE("small", 4.0);
    IF_SET_RANGE("medium", 5.5);
    IF_SET_RANGE("large", 8.5);
  }

  {
    dc::dcInteger effect_delay_ms, repeat_delay_ms;
    improvement_level_limit_.interpret(values->find("Improves Until")->second.c_str());
    effect_delay_ms.interpret(values->find("Effect Delay (ms)")->second.c_str());
    repeat_delay_ms.interpret(values->find("Repeat Delay (ms)")->second.c_str());
    action_delay_ms_.setValue(effect_delay_ms.getValue() + repeat_delay_ms.getValue());
  }
}



//----[  getLevelAcquired  ]---------------------------------------------------
bool SpellStage::getLevelAcquired(Avatar::AvatarClass avatar_class,
                                  Avatar::AvatarLevel* level) {
  *level = level_obtained_[avatar_class].getValue();
  return *level > 0;
}



}
}
}

