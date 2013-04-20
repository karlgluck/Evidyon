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
#include "magiccompiler.h"
#include "shared/magic/magic.h"
#include "common/safemacros.h"
#include <assert.h>


namespace Evidyon {
namespace Magic {
namespace Tools {


//----[  MagicCompiler  ]------------------------------------------------------
MagicCompiler::MagicCompiler(Event::Tools::EventCompiler* event_compiler)
    : event_compiler_(event_compiler) {
  clear();
}




//----[  ~MagicCompiler  ]-----------------------------------------------------
MagicCompiler::~MagicCompiler() {
  clear();
}




//----[  clear  ]--------------------------------------------------------------
void MagicCompiler::clear() {
  for (MagicList::iterator i = magics_.begin();
        i != magics_.end(); ++i) {
    Evidyon::Magic::Magic* magic = *i;
    SAFE_DELETE(magic);
  }
  magics_.clear();
  for (int i = 0; i < NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    enchantment_events_[i].clear();
  }
}



//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
    const Evidyon::Magic::Magic* description) {
  Evidyon::Magic::Magic* internal_magic = new Evidyon::Magic::Magic;
  memcpy(internal_magic, description, sizeof(*internal_magic));
  magics_.push_back(internal_magic);
  return magics_.size() - 1;
}


//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
    const Evidyon::Magic::Magic_Projectile* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::PROJECTILE;
  memcpy(&unioned_magic.projectile, description, sizeof(*description));
  return add(&unioned_magic);
}




//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
    const Evidyon::Magic::Magic_AttackRun* description)  {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::ATTACK_RUN;
  memcpy(&unioned_magic.attack_run, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_AOE* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::AOE;
  memcpy(&unioned_magic.aoe, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_AOEBuff* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::AOE_BUFF;
  memcpy(&unioned_magic.aoe_buff, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Branching* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::BRANCHING;
  memcpy(&unioned_magic.branching, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Buff* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::BUFF;
  memcpy(&unioned_magic.buff, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_DirectDamage* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::DIRECT_DAMAGE;
  memcpy(&unioned_magic.direct_damage, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Disenchant* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::DISENCHANT;
  memcpy(&unioned_magic.disenchant, description, sizeof(*description));
  return add(&unioned_magic);
}






//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(const Evidyon::Magic::Magic_DOT* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::DOT;
  memcpy(&unioned_magic.dot, description, sizeof(*description));
  return add(&unioned_magic);
}




//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_DOTAOEBuff* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::DOT_AOE_BUFF;
  memcpy(&unioned_magic.dot_aoe_buff, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Invisibility* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::INVISIBILITY;
  memcpy(&unioned_magic.invisibility, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Leap* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::LEAP;
  memcpy(&unioned_magic.leap, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Mutate* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::MUTATE;
  memcpy(&unioned_magic.mutate, description, sizeof(*description));
  return add(&unioned_magic);
}



//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Portal* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::PORTAL;
  memcpy(&unioned_magic.portal, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_SpinAttack* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::SPIN_ATTACK;
  memcpy(&unioned_magic.spin_attack, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Summon* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::SUMMON;
  memcpy(&unioned_magic.summon, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Teleport* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::TELEPORT;
  memcpy(&unioned_magic.teleport, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Trap_DimensionDoor* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::TRAP_DIMENSION_DOOR;
  memcpy(&unioned_magic.trap_dimension_door, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Trap_BlackHole* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::TRAP_BLACK_HOLE;
  memcpy(&unioned_magic.trap_black_hole, description, sizeof(*description));
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  const Evidyon::Magic::Magic_Heal* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::HEAL;
  memcpy(&unioned_magic.heal, description, sizeof(*description));
  return add(&unioned_magic);
}




//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  Event::EventIndex event_index,
    const Evidyon::Magic::Magic_Enchantment_Armor* description, int index) {
  Evidyon::Magic::Magic unioned_magic;
  memcpy(&unioned_magic.enchantment_armor, description, sizeof(*description));
  switch (index) {
    default: assert(false); return Evidyon::Magic::INVALID_MAGIC_INDEX;
    case 0:
      unioned_magic.enchantment_armor.enchantment_type
        = 1 + enchantment_events_[Evidyon::Magic::ENCHANTMENT_ARMOR_0].size();
      enchantment_events_[Evidyon::Magic::ENCHANTMENT_ARMOR_0].push_back(event_index);
      unioned_magic.type = Evidyon::Magic::Magic::ENCHANTMENT_ARMOR_0;
      break;
    case 1:
      unioned_magic.enchantment_armor.enchantment_type
        = 1 + enchantment_events_[Evidyon::Magic::ENCHANTMENT_ARMOR_1].size();
      enchantment_events_[Evidyon::Magic::ENCHANTMENT_ARMOR_1].push_back(event_index);
      unioned_magic.type = Evidyon::Magic::Magic::ENCHANTMENT_ARMOR_1;
      break;
    case 2:
      unioned_magic.enchantment_armor.enchantment_type
        = 1 + enchantment_events_[Evidyon::Magic::ENCHANTMENT_ARMOR_2].size();
      enchantment_events_[Evidyon::Magic::ENCHANTMENT_ARMOR_2].push_back(event_index);
      unioned_magic.type = Evidyon::Magic::Magic::ENCHANTMENT_ARMOR_2;
      break;
  }
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  Event::EventIndex event_index,
  const Evidyon::Magic::Magic_Enchantment_Shield* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::ENCHANTMENT_SHIELD;
  memcpy(&unioned_magic.enchantment_shield, description, sizeof(*description));
  unioned_magic.enchantment_shield.enchantment_type
    = 1 + enchantment_events_[Evidyon::Magic::ENCHANTMENT_SHIELD].size();
  enchantment_events_[Evidyon::Magic::ENCHANTMENT_SHIELD].push_back(event_index);
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  Event::EventIndex event_index,
  const Evidyon::Magic::Magic_Enchantment_ReactiveHealing* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::ENCHANTMENT_REACTIVE_HEALING;
  memcpy(&unioned_magic.enchantment_reactive_healing, description, sizeof(*description));
  unioned_magic.enchantment_reactive_healing.enchantment_type
    = 1 + enchantment_events_[Evidyon::Magic::ENCHANTMENT_REACTIVE_HEALING].size();
  enchantment_events_[Evidyon::Magic::ENCHANTMENT_REACTIVE_HEALING].push_back(event_index);
  return add(&unioned_magic);
}





//----[  add  ]----------------------------------------------------------------
MagicIndex MagicCompiler::add(
  Event::EventIndex event_index,
  const Evidyon::Magic::Magic_Enchantment_Thorns* description) {
  Evidyon::Magic::Magic unioned_magic;
  unioned_magic.type = Evidyon::Magic::Magic::ENCHANTMENT_THORNS;
  memcpy(&unioned_magic.enchantment_thorns, description, sizeof(*description));
  unioned_magic.enchantment_thorns.enchantment_type
    = 1 + enchantment_events_[Evidyon::Magic::ENCHANTMENT_THORNS].size();
  enchantment_events_[Evidyon::Magic::ENCHANTMENT_THORNS].push_back(event_index);
  return add(&unioned_magic);
}



//----[  eventCompiler  ]------------------------------------------------------
Event::Tools::EventCompiler* MagicCompiler::eventCompiler() {
  return event_compiler_;
}


//----[  numberOfMagics  ]-----------------------------------------------------
size_t MagicCompiler::numberOfMagics() const {
  return magics_.size();
}




//----[  compileMagic  ]-------------------------------------------------------
void MagicCompiler::compileMagic(size_t index,
                                 Evidyon::Magic::Magic* magic_out) {
  memcpy(magic_out, magics_.at(index), sizeof(*magic_out));
}




//----[  numberOfEnchantmentsOfType  ]-----------------------------------------
size_t MagicCompiler::numberOfEnchantmentsOfType(EnchantmentType type) {
  return enchantment_events_[type].size();
}


//----[  compileEnchantmentTypeEvents  ]---------------------------------------
void MagicCompiler::compileEnchantmentTypeEvents(EnchantmentType type,
                                                 Event::EventIndex* event_indices) {
  size_t index = 0;
  for (EventIndexList::iterator i = enchantment_events_[type].begin();
        i != enchantment_events_[type].end(); ++i, ++index) {
    event_indices[index] = *i;
  }
}




}
}
}

