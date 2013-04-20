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
#ifndef __SHARED_MAGIC_MAGIC_H__
#define __SHARED_MAGIC_MAGIC_H__
#pragma once


#include "magic_aoe.h"
#include "magic_spinattack.h"
#include "magic_attackrun.h"
#include "magic_leap.h"
#include "magic_buff.h"
#include "magic_aoebuff.h"
#include "magic_dot.h"
#include "magic_dotaoebuff.h"
#include "magic_directdamage.h"
#include "magic_projectile.h"
#include "magic_branching.h"
#include "magic_portal.h"
#include "magic_teleport.h"
#include "magic_mutate.h"
#include "magic_summon.h"
#include "magic_trap_dimensiondoor.h"
#include "magic_trap_blackhole.h"
#include "magic_disenchant.h"
#include "magic_invisibility.h"
#include "magic_heal.h"
#include "magic_enchantment_armor.h"
#include "magic_enchantment_shield.h"
#include "magic_enchantment_reactivehealing.h"
#include "magic_enchantment_thorns.h"


namespace Evidyon {
namespace Magic {

//----[  Magic  ]--------------------------------------------------------------
struct Magic {
  enum {
    AOE,
    SPIN_ATTACK,
    ATTACK_RUN,
    LEAP,
    DIRECT_DAMAGE,
    BUFF,
    AOE_BUFF,
    DOT,
    DOT_AOE_BUFF,
    PROJECTILE,
    BRANCHING,
    PORTAL,
    TELEPORT,
    MUTATE,
    SUMMON,
    TRAP_DIMENSION_DOOR,
    TRAP_BLACK_HOLE,
    DISENCHANT,
    INVISIBILITY,
    HEAL,
    ENCHANTMENT_ARMOR_0,
    ENCHANTMENT_ARMOR_1,
    ENCHANTMENT_ARMOR_2,
    ENCHANTMENT_SHIELD,
    ENCHANTMENT_REACTIVE_HEALING,
    ENCHANTMENT_THORNS,
  } type;

  union {
    Magic_AOE aoe;
    Magic_Buff buff;
    Magic_SpinAttack spin_attack;
    Magic_AttackRun attack_run;
    Magic_Leap leap;
    Magic_AOEBuff aoe_buff;
    Magic_DirectDamage direct_damage;
    Magic_DOT dot;
    Magic_DOTAOEBuff dot_aoe_buff;
    Magic_Projectile projectile;
    Magic_Branching branching;
    Magic_Portal portal;
    Magic_Teleport teleport;
    Magic_Mutate mutate;
    Magic_Summon summon;
    Magic_Trap_DimensionDoor trap_dimension_door;
    Magic_Trap_BlackHole trap_black_hole;
    Magic_Disenchant disenchant;
    Magic_Invisibility invisibility;
    Magic_Heal heal;
    Magic_Enchantment_Armor enchantment_armor;
    Magic_Enchantment_Shield enchantment_shield;
    Magic_Enchantment_ReactiveHealing enchantment_reactive_healing;
    Magic_Enchantment_Thorns enchantment_thorns;
  };
};

}
}

#endif



