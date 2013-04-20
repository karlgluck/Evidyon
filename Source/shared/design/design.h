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
#ifndef __SHARED_DESIGN_DESIGN_H__
#define __SHARED_DESIGN_DESIGN_H__
#pragma once

#include "common/number.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarrace.h"

namespace Evidyon {
namespace Design {

//
//static const int NUMBER_OF_STAT_BONUSES = 20;
//
//
//  
////----[  DesignParameter  ]----------------------------------------------------
//enum DesignParameter {
//  HP_CON_BELOWTHRESHOLD_FACTOR,
//  HP_CON_THRESHOLD,
//  HP_CON_ABOVETHRESHOLD_FACTOR,
//  HP_STR_BELOWTHRESHOLD_FACTOR,
//  HP_STR_THRESHOLD,
//  HP_STR_ABOVETHRESHOLD_FACTOR,
//  MP_WIS_BELOWTHRESHOLD_FACTOR,
//  MP_WIS_THRESHOLD,
//  MP_WIS_ABOVETHRESHOLD_FACTOR,
//  MP_INT_BELOWTHRESHOLD_FACTOR,
//  MP_INT_THRESHOLD,
//  MP_INT_ABOVETHRESHOLD_FACTOR,
//  HP_REGEN_AVERAGE_OFFSET,
//  HP_REGEN_AVERAGE_MULTIPLIER,
//  HP_REGEN_DIFFERENCE_DIVISOR,
//  MP_REGEN_AVERAGE_OFFSET,
//  MP_REGEN_AVERAGE_MULTIPLIER,
//  MP_REGEN_DIFFERENCE_DIVISOR,
//  ABILITY_EFFECT_PERCENTAGE_MULTIPLIER,
//  ABILITY_EFFECT_BASE_MULTIPLIER,
//  ABILITY_EFFECT_BONUS_OFFSET,
//  ABILITY_EFFECT_BONUS_PERCENTAGE_MULTIPLIER,
//  MOVEMENT_SPEED_AGI_THRESHOLD,
//  MOVEMENT_SPEED_BELOW_FACTOR,
//  MOVEMENT_SPEED_ABOVE_OFFSET,
//  MOVEMENT_SPEED_ABOVE_FACTOR,
//  MOVEMENT_SPEED_MIN,
//  MOVEMENT_SPEED_MAX,
//  EFFECT_DELAY_MULTIPLIER_AGI_THRESHOLD,
//  EFFECT_DELAY_MULTIPLIER_AGI_ABOVE_MULTIPLIER,
//  EFFECT_DELAY_MULTIPLIER_MIN,
//  ACTION_DELAY_AGI_THRESHOLD,
//  ACTION_DELAY_AGI_BELOW_MULTIPLIER,
//  ACTION_DELAY_AGI_BELOW_OFFSET,
//  ACTION_DELAY_AGI_ABOVE_OFFSET,
//  ACTION_DELAY_AGI_ABOVE_MULTIPLIER,
//  ARMOR_MIN_DAMAGE_TAKEN,
//  ARMOR_PERCENT_FACTOR,
//  STATBOOST_BEGIN,
//  STATBOOST_END = STATBOOST_BEGIN + NUMBER_OF_STAT_BONUSES,
//  STATBOOST_AMOUNT_BEGIN,
//  STATBOOST_AMOUNT_END = STATBOOST_AMOUNT_BEGIN + NUMBER_OF_STAT_BONUSES,
//
//  NUMBER_OF_PARAMETERS
//};
//
//
////----[  DesignParameterValues  ]----------------------------------------------
//struct DesignParameterValues {
//  static const int HIGHEST_POSSIBLE_NATURAL_STAT = 200;
//  Number parameter[NUMBER_OF_PARAMETERS];
//
//  struct {
//    Number hp, mp, base_speed, base_action_speed;
//    Number magic_armor_bonus_percent; // 20% for mages at the moment
//  } classes[Avatar::NUMBER_OF_AVATAR_CLASSES];
//
//  struct {
//    Number base_hp, base_mp, base_speed, base_action_speed;
//  } races[Avatar::NUMBER_OF_AVATAR_RACES];
//
//  // Look up stat - 10 in this array to find out what stat value to use in the
//  // equations.
//  Number boosted_stats[HIGHEST_POSSIBLE_NATURAL_STAT];
//};
//
//
//
//void SetParameters(const DesignParameterValues* design_parameters);
//void FillDefaultParameters(DesignParameterValues* design_parameters);
//void CalculateBoostedStats(DesignParameterValues* design_parameters);
//
//Number BoostedStat(Number stat);
//Number AvatarHP(Avatar::AvatarLevel level,
//                Avatar::AvatarClass _class,
//                Avatar::AvatarRace race,
//                Number strength,
//                Number constitution);
//Number AvatarMP(Avatar::AvatarLevel level,
//                Avatar::AvatarClass _class,
//                Avatar::AvatarRace race,
//                Number intelligence,
//                Number wisdom);
//Number AvatarHP(Avatar::AvatarLevel level,
//                Number class_hp,
//                Number strength,
//                Number constitution,
//                Number base_race_hp);
//Number AvatarMP(Avatar::AvatarLevel level,
//                Number class_mp,
//                Number intelligence,
//                Number wisdom,
//                Number base_race_mp);
//Number AvatarHPRegen(Number strength,
//                     Number constitution);
//Number AvatarMPRegen(Number intelligence,
//                     Number wisdom);
//Number AvatarMovementSpeed(Number agility,
//                           Avatar::AvatarClass _class,
//                           Avatar::AvatarRace race);
//Number AvatarActionDelay(Number agility,
//                         Avatar::AvatarClass _class,
//                         Avatar::AvatarRace race);
//Number AvatarMovementSpeed(Number agility,
//                           Number base_race_speed,
//                           Number base_class_speed);
//Number AvatarActionDelay(Number agility,
//                         Number base_race_action_delay,
//                         Number base_class_action_delay);
//Number AvatarEffectDelayMultiplier(Number agility);
//void AvatarAbilityEffectParameters(Number primary_stat,
//                                   Number secondary_stat,
//                                   Number* base,
//                                   Number* critical,
//                                   Number* bonus);
////
////// Mages get a bonus to their magic armor factors
////void AvatarArmorFactors(Number defense,
////                        Avatar::AvatarClass avatar_class,
////                        Number* melee_armor_factor,
////                        Number* magic_armor_factor);
////
//
//Number AvatarAbilityEffect(Number random,       // 0->1
//                           Number base,         // from parameters
//                           Number critical,     // from parameters
//                           Number bonus,        // from parameters
//                           Number range_lower,      // min
//                           Number range_magnitude); // max - min
////
////Number AvatarDamageTaken(Number armor_factor,
////                         Number attacker_level,
////                         Number damage);
//
//
//// Mages get a bonus to their magic armor factors
//void AvatarArmorFactors2(Number defense,
//                         Avatar::AvatarClass avatar_class,
//                         Avatar::AvatarLevel level,
//                         Number* melee_armor_factor,
//                         Number* magic_armor_factor);
//
//Number AvatarDamageTaken2(Number armor_factor,
//                          Number damage);
//
//
//Number AvatarMeleeAttackDelayMultiplier(Number level, Number agility);
//Number AvatarMagicDelayMultiplier(Number level, Number agility);


}
}

#endif