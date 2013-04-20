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
#include "design.h"


namespace Evidyon {
namespace Design {
//
//#define PARAMS EVIDYON_DESIGN_PARAMETERS
//#define BOOSTED_STATS EVIDYON_BOOSTED_STATS
//
//static const Number* PARAMS = NULL;
//static const Number* BOOSTED_STATS = NULL;
//static const DesignParameterValues* VALUES = NULL;
//#define RACE VALUES->races
//#define CLASS VALUES->classes
//
//#ifdef min
//#undef min
//#endif
//#define min FixedPoint::minfast<16>
//#ifdef max
//#undef max
//#endif
//#define max FixedPoint::maxfast<16>
//  
////----[  SetParameters  ]------------------------------------------------------
//void SetParameters(const DesignParameterValues* design_parameters) {
//  PARAMS = design_parameters->parameter;
//  BOOSTED_STATS = design_parameters->boosted_stats;
//  VALUES = design_parameters;
//}
//
//
////----[  FillDefaultParameters  ]----------------------------------------------
//void FillDefaultParameters(DesignParameterValues* design_parameter_values) {
//  using namespace Evidyon::Avatar;
//  design_parameter_values->classes[AVATARCLASS_ADEPT].hp    = 14;
//  design_parameter_values->classes[AVATARCLASS_SORCERER].hp = 19;
//  design_parameter_values->classes[AVATARCLASS_HUNTER].hp   = 26;
//  design_parameter_values->classes[AVATARCLASS_WARRIOR].hp  = 30;
//
//  design_parameter_values->classes[AVATARCLASS_ADEPT].mp    = 25;
//  design_parameter_values->classes[AVATARCLASS_SORCERER].mp = 22;
//  design_parameter_values->classes[AVATARCLASS_HUNTER].mp   = 9;
//  design_parameter_values->classes[AVATARCLASS_WARRIOR].mp  = 4;
//
//  design_parameter_values->classes[AVATARCLASS_ADEPT].base_speed    = 0.04;
//  design_parameter_values->classes[AVATARCLASS_SORCERER].base_speed = 0.07;
//  design_parameter_values->classes[AVATARCLASS_HUNTER].base_speed   = 0.10;
//  design_parameter_values->classes[AVATARCLASS_WARRIOR].base_speed  = 0.08;
//  
//  design_parameter_values->classes[AVATARCLASS_ADEPT].base_action_speed    =  0.01;
//  design_parameter_values->classes[AVATARCLASS_SORCERER].base_action_speed = -0.01;
//  design_parameter_values->classes[AVATARCLASS_HUNTER].base_action_speed   =  0.05;
//  design_parameter_values->classes[AVATARCLASS_WARRIOR].base_action_speed  =  0.04;
//  
//  design_parameter_values->classes[AVATARCLASS_ADEPT].magic_armor_bonus_percent    = 0.20;
//  design_parameter_values->classes[AVATARCLASS_SORCERER].magic_armor_bonus_percent = 0.20;
//  design_parameter_values->classes[AVATARCLASS_HUNTER].magic_armor_bonus_percent   = 0;
//  design_parameter_values->classes[AVATARCLASS_WARRIOR].magic_armor_bonus_percent  = 0;
//
//
//  ///////////// RACES /////////////////
//  design_parameter_values->races[AVATARRACE_GEONAN].base_hp    =  6;
//  design_parameter_values->races[AVATARRACE_LUCAN].base_hp     = 13;
//  design_parameter_values->races[AVATARRACE_DANTALION].base_hp =  8;
//  design_parameter_values->races[AVATARRACE_VOLUCRIS].base_hp  = 15;
//
//  design_parameter_values->races[AVATARRACE_GEONAN].base_mp    = 15;
//  design_parameter_values->races[AVATARRACE_LUCAN].base_mp     = 12;
//  design_parameter_values->races[AVATARRACE_DANTALION].base_mp = 14;
//  design_parameter_values->races[AVATARRACE_VOLUCRIS].base_mp  = 11;
//
//  design_parameter_values->races[AVATARRACE_GEONAN].base_speed    = 0.02;
//  design_parameter_values->races[AVATARRACE_LUCAN].base_speed     = 0.05;
//  design_parameter_values->races[AVATARRACE_DANTALION].base_speed = 0.01;
//  design_parameter_values->races[AVATARRACE_VOLUCRIS].base_speed  = 0.04;
//
//  design_parameter_values->races[AVATARRACE_GEONAN].base_action_speed    =  0.03;
//  design_parameter_values->races[AVATARRACE_LUCAN].base_action_speed     = -0.04;
//  design_parameter_values->races[AVATARRACE_DANTALION].base_action_speed =  0.05;
//  design_parameter_values->races[AVATARRACE_VOLUCRIS].base_action_speed  = -0.02;
//
//
//  Number* design_parameters = design_parameter_values->parameter;
//  design_parameters[STATBOOST_BEGIN + 0] = 13;
//  design_parameters[STATBOOST_BEGIN + 1] = 15;
//  design_parameters[STATBOOST_BEGIN + 2] = 17;
//  design_parameters[STATBOOST_BEGIN + 3] = 19;
//  design_parameters[STATBOOST_BEGIN + 4] = 21;
//  design_parameters[STATBOOST_BEGIN + 5] = 25;
//  design_parameters[STATBOOST_BEGIN + 6] = 29;
//  design_parameters[STATBOOST_BEGIN + 7] = 34;
//  design_parameters[STATBOOST_BEGIN + 8] = 39;
//  design_parameters[STATBOOST_BEGIN + 9] = 45;
//  design_parameters[STATBOOST_BEGIN + 10] = 51;
//  design_parameters[STATBOOST_BEGIN + 11] = 57;
//  design_parameters[STATBOOST_BEGIN + 12] = 60;
//  design_parameters[STATBOOST_BEGIN + 13] = 66;
//  design_parameters[STATBOOST_BEGIN + 14] = 72;
//  design_parameters[STATBOOST_BEGIN + 15] = 78;
//  design_parameters[STATBOOST_BEGIN + 16] = 84;
//  design_parameters[STATBOOST_BEGIN + 17] = 90;
//  design_parameters[STATBOOST_BEGIN + 18] = 96;
//  design_parameters[STATBOOST_BEGIN + 19] = 103;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 0] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 1] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 2] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 3] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 4] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 5] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 6] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 7] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 8] = 1;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 9] = 2;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 10] = 2;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 11] = 2;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 12] = 2;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 13] = 2;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 14] = 3;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 15] = 3;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 16] = 3;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 17] = 4;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 18] = 4;
//  design_parameters[STATBOOST_AMOUNT_BEGIN + 19] = 5;
//  design_parameters[HP_CON_BELOWTHRESHOLD_FACTOR] = 8.0 / 15.0;
//  design_parameters[HP_CON_THRESHOLD] = 25;
//  design_parameters[HP_CON_ABOVETHRESHOLD_FACTOR] = 21.0;
//  design_parameters[HP_STR_BELOWTHRESHOLD_FACTOR] = 2.0 / 15.0;
//  design_parameters[HP_STR_THRESHOLD] = 25;
//  design_parameters[HP_STR_ABOVETHRESHOLD_FACTOR] = 5.0;
//  design_parameters[MP_WIS_BELOWTHRESHOLD_FACTOR] = 8.0 / 15.0;
//  design_parameters[MP_WIS_THRESHOLD] = 25;
//  design_parameters[MP_WIS_ABOVETHRESHOLD_FACTOR] = 17.0;
//  design_parameters[MP_INT_BELOWTHRESHOLD_FACTOR] = 2.0 / 15.0;
//  design_parameters[MP_INT_THRESHOLD] = 25;
//  design_parameters[MP_INT_ABOVETHRESHOLD_FACTOR] = 9.0;
//  design_parameters[HP_REGEN_AVERAGE_OFFSET] = 4;
//  design_parameters[HP_REGEN_AVERAGE_MULTIPLIER] = 1.0; /////////////////
//  design_parameters[HP_REGEN_DIFFERENCE_DIVISOR] = 15.0;
//  design_parameters[MP_REGEN_AVERAGE_OFFSET] = 4;
//  design_parameters[MP_REGEN_AVERAGE_MULTIPLIER] = 1.0; /////////////////
//  design_parameters[MP_REGEN_DIFFERENCE_DIVISOR] = 15.0;
//  design_parameters[MOVEMENT_SPEED_AGI_THRESHOLD] = 25;
//  design_parameters[MOVEMENT_SPEED_BELOW_FACTOR] = 1.0 / 25.0;
//  design_parameters[MOVEMENT_SPEED_ABOVE_OFFSET] = 0.6;
//  design_parameters[MOVEMENT_SPEED_ABOVE_FACTOR] = 1.0 / 17.0;
//  design_parameters[MOVEMENT_SPEED_MIN] = 2.5;
//  design_parameters[MOVEMENT_SPEED_MAX] = 5.3;
//  design_parameters[EFFECT_DELAY_MULTIPLIER_AGI_THRESHOLD] = 50;
//  design_parameters[EFFECT_DELAY_MULTIPLIER_AGI_ABOVE_MULTIPLIER] = 1.0 / 133.0;
//  design_parameters[EFFECT_DELAY_MULTIPLIER_MIN] = 0.20;
//  design_parameters[ABILITY_EFFECT_PERCENTAGE_MULTIPLIER] = 0.10 / 200.0;
//  design_parameters[ABILITY_EFFECT_BASE_MULTIPLIER] = 500.0 / 200.0;
//  design_parameters[ABILITY_EFFECT_BONUS_OFFSET] = 0.5;
//  design_parameters[ABILITY_EFFECT_BONUS_PERCENTAGE_MULTIPLIER] = 5.0;
//  design_parameters[ACTION_DELAY_AGI_THRESHOLD] = 25;
//  design_parameters[ACTION_DELAY_AGI_BELOW_MULTIPLIER] = 1.0 / 25.0;
//  design_parameters[ACTION_DELAY_AGI_BELOW_OFFSET] = 1.35;
//  design_parameters[ACTION_DELAY_AGI_ABOVE_OFFSET] = 0.75;
//  design_parameters[ACTION_DELAY_AGI_ABOVE_MULTIPLIER] = 1.0 / 85.0;
//  design_parameters[ARMOR_MIN_DAMAGE_TAKEN] = 0.20;
//  design_parameters[ARMOR_PERCENT_FACTOR] = 0.03;
//
//  CalculateBoostedStats(design_parameter_values);
//}
//
//
//
////----[  CalculateBoostedStats  ]----------------------------------------------
//void CalculateBoostedStats(DesignParameterValues* design_parameter_values) {
//  Number* parameter_values = design_parameter_values->parameter;
//  Number boost_accumulator = 0.0;
//  int current_boost = 0;
//  for (int i = 0; i < DesignParameterValues::HIGHEST_POSSIBLE_NATURAL_STAT; ++i) {
//    design_parameter_values->boosted_stats[i] = 10 + i + boost_accumulator;
//    if (parameter_values[STATBOOST_BEGIN + current_boost] <= 10 + i) {
//      boost_accumulator += parameter_values[STATBOOST_AMOUNT_BEGIN + current_boost];
//      ++current_boost;
//      if (current_boost >= NUMBER_OF_STAT_BONUSES) {
//        current_boost = NUMBER_OF_STAT_BONUSES - 1;
//      }
//    }
//  }
//}
//
//
//
//
////----[  BoostedStat  ]--------------------------------------------------------
//Number BoostedStat(Number stat) {
//  return BOOSTED_STATS[(stat - 10).wholePart()];
//}
//
//
//
////----[  AvatarHP  ]-----------------------------------------------------------
//Number AvatarHP(Avatar::AvatarLevel level,
//                Avatar::AvatarClass _class,
//                Avatar::AvatarRace race,
//                Number strength,
//                Number constitution) {
//  return AvatarHP(
//    level,
//    VALUES->classes[_class].hp,
//    strength,
//    constitution,
//    VALUES->races[race].base_hp);
//}
//
//
//
//
////----[  AvatarMP  ]-----------------------------------------------------------
//Number AvatarMP(Avatar::AvatarLevel level,
//                Avatar::AvatarClass _class,
//                Avatar::AvatarRace race,
//                Number intelligence,
//                Number wisdom) {
//  return AvatarMP(
//    level,
//    VALUES->classes[_class].mp,
//    intelligence,
//    wisdom,
//    VALUES->races[race].base_mp);
//
//}
//
//
////----[  AvatarHP  ]-----------------------------------------------------------
//Number AvatarHP(Avatar::AvatarLevel level,
//                Number class_hp,
//                Number strength,
//                Number constitution,
//                Number base_race_hp) {
//  Number hp_bonus_con =
//    (constitution <= PARAMS[HP_CON_THRESHOLD])
//    ? ((constitution - 10) * PARAMS[HP_CON_BELOWTHRESHOLD_FACTOR])
//    : ((constitution - PARAMS[HP_CON_THRESHOLD]) * PARAMS[HP_CON_ABOVETHRESHOLD_FACTOR]);
//  Number hp_bonus_str =
//    (strength <= PARAMS[HP_STR_THRESHOLD])
//    ? ((strength - 10) * PARAMS[HP_STR_BELOWTHRESHOLD_FACTOR])
//    : ((strength - PARAMS[HP_STR_THRESHOLD]) * PARAMS[HP_STR_ABOVETHRESHOLD_FACTOR]);
// // float f [] = { (level * class_hp).toFloat(), base_race_hp.toFloat(), hp_bonus_con.toFloat(), hp_bonus_str.toFloat() };
//  return
//    level * class_hp +
//    base_race_hp +
//    hp_bonus_con +
//    hp_bonus_str;
//}
//
//
//
//
////----[  AvatarMP  ]-----------------------------------------------------------
//Number AvatarMP(Avatar::AvatarLevel level,
//                Number class_mp,
//                Number intelligence,
//                Number wisdom,
//                Number base_race_mp) {
//  Number mp_bonus_wis =
//    (wisdom <= PARAMS[MP_WIS_THRESHOLD]
//    ? ((wisdom - 10) * PARAMS[MP_WIS_BELOWTHRESHOLD_FACTOR])
//    : ((wisdom - PARAMS[MP_WIS_THRESHOLD]) * PARAMS[MP_WIS_ABOVETHRESHOLD_FACTOR]));
//  Number mp_bonus_int =
//    ((intelligence <= PARAMS[MP_INT_THRESHOLD])
//    ? ((intelligence - 10) * PARAMS[MP_INT_BELOWTHRESHOLD_FACTOR])
//    : ((intelligence - PARAMS[MP_INT_THRESHOLD]) * PARAMS[MP_INT_ABOVETHRESHOLD_FACTOR]));
//  float f[] = { (level * class_mp).toFloat(), base_race_mp.toFloat(), mp_bonus_wis.toFloat(), mp_bonus_int.toFloat() };
//  return
//    level * class_mp +
//    base_race_mp +
//    mp_bonus_wis +
//    mp_bonus_int;
//}
//
//
//
//
////----[  AvatarHPRegen  ]------------------------------------------------------
//Number AvatarHPRegen(Number strength, Number constitution) {
//  Number difference = FixedPoint::abs(strength - constitution);
//  return
//    ((strength + constitution) / 2 - PARAMS[HP_REGEN_AVERAGE_OFFSET])
//      * ((difference > PARAMS[HP_REGEN_DIFFERENCE_DIVISOR])
//          ? (PARAMS[HP_REGEN_AVERAGE_MULTIPLIER]
//              / PARAMS[HP_REGEN_DIFFERENCE_DIVISOR])
//          :  PARAMS[HP_REGEN_AVERAGE_MULTIPLIER]);
//}
//
//
//
////----[  AvatarMPRegen  ]------------------------------------------------------
//Number AvatarMPRegen(Number intelligence, Number wisdom) {
//  Number difference = FixedPoint::abs(intelligence - wisdom);
//  return
//    ((intelligence + wisdom) / 2 - PARAMS[MP_REGEN_AVERAGE_OFFSET])
//      * ((difference > PARAMS[MP_REGEN_DIFFERENCE_DIVISOR])
//          ? (PARAMS[MP_REGEN_AVERAGE_MULTIPLIER]
//              / PARAMS[MP_REGEN_DIFFERENCE_DIVISOR])
//          :  PARAMS[MP_REGEN_AVERAGE_MULTIPLIER]);
//}
//
//
//
////----[  AvatarMovementSpeed  ]------------------------------------------------
//Number AvatarMovementSpeed(Number agility,
//                           Avatar::AvatarClass _class,
//                           Avatar::AvatarRace race) {
//  return AvatarMovementSpeed(agility,
//    VALUES->races[race].base_speed,
//    VALUES->classes[_class].base_speed);
//}
//
//
////----[  AvatarActionDelay  ]--------------------------------------------------
//Number AvatarActionDelay(Number agility,
//                         Avatar::AvatarClass _class,
//                         Avatar::AvatarRace race) {
//  return AvatarActionDelay(agility,
//    VALUES->races[race].base_action_speed,
//    VALUES->classes[_class].base_action_speed);
//}
//
//
//
//
////----[  AvatarMovementSpeed  ]------------------------------------------------
//Number AvatarMovementSpeed(Number agility,
//                           Number base_race_speed,
//                           Number base_class_speed) {
//  Number speed =
//    (agility <= PARAMS[MOVEMENT_SPEED_AGI_THRESHOLD])
//    ? (agility - 10) * PARAMS[MOVEMENT_SPEED_BELOW_FACTOR]
//  : (PARAMS[MOVEMENT_SPEED_ABOVE_OFFSET]
//      + (agility - PARAMS[MOVEMENT_SPEED_AGI_THRESHOLD])
//        * PARAMS[MOVEMENT_SPEED_ABOVE_FACTOR]);
//  speed += PARAMS[MOVEMENT_SPEED_MIN]
//          + (base_race_speed + base_class_speed) * (PARAMS[MOVEMENT_SPEED_MAX] - PARAMS[MOVEMENT_SPEED_MIN]);
//  return min(speed, PARAMS[MOVEMENT_SPEED_MAX]);
//}
//
//
//
//
////----[  AvatarActionDelay  ]--------------------------------------------------
//Number AvatarActionDelay(Number agility,
//                         Number base_race_action_delay,
//                         Number base_class_action_delay) {
//  return 0.5;/*
//  Number delay
//    = (agility <= PARAMS[ACTION_DELAY_AGI_THRESHOLD])
//    ? (PARAMS[ACTION_DELAY_AGI_BELOW_OFFSET] -
//        (agility - 10) * PARAMS[ACTION_DELAY_AGI_BELOW_MULTIPLIER])
//    : (PARAMS[ACTION_DELAY_AGI_ABOVE_OFFSET] -
//        (agility - PARAMS[ACTION_DELAY_AGI_THRESHOLD])
//          * PARAMS[ACTION_DELAY_AGI_ABOVE_MULTIPLIER]);
//  return max(delay, 0) * (1 + base_race_action_delay) * (1 + base_class_action_delay);(*/
//}
//
//
//
//
////----[  AvatarEffectDelayMultiplier  ]----------------------------------------
//Number AvatarEffectDelayMultiplier(Number agility) {
//  return
//    (agility <= PARAMS[EFFECT_DELAY_MULTIPLIER_AGI_THRESHOLD])
//      ? 1.0 
//      : max(1.0 - (agility - PARAMS[EFFECT_DELAY_MULTIPLIER_AGI_THRESHOLD])
//                    * PARAMS[EFFECT_DELAY_MULTIPLIER_AGI_ABOVE_MULTIPLIER],
//            PARAMS[EFFECT_DELAY_MULTIPLIER_MIN]);
//}
//
//
//
//
////----[  AvatarAbilityEffectParameters  ]--------------------------------------
//void AvatarAbilityEffectParameters(Number primary_stat,
//                                   Number secondary_stat,
//                                   Number* base,
//                                   Number* critical,
//                                   Number* bonus) {
//  *base = PARAMS[ABILITY_EFFECT_BASE_MULTIPLIER] * (primary_stat - 10);
//  *critical = FixedPoint::sqrt<16>(
//    PARAMS[ABILITY_EFFECT_PERCENTAGE_MULTIPLIER] * (primary_stat - 10));
//  *bonus = PARAMS[ABILITY_EFFECT_BONUS_OFFSET]
//    + PARAMS[ABILITY_EFFECT_PERCENTAGE_MULTIPLIER]
//      * FixedPoint::sqrt<16>(
//    PARAMS[ABILITY_EFFECT_BONUS_PERCENTAGE_MULTIPLIER] * (primary_stat / 2 + secondary_stat - 10));;
//}
//
//
////
//////----[  AvatarArmorFactor  ]--------------------------------------------------
////Number AvatarArmorFactor(Number defense) {
////  return PARAMS[ARMOR_PERCENT_FACTOR] * defense;
////}
////
////
//////----[  AvatarArmorFactors  ]-------------------------------------------------
////void AvatarArmorFactors(Number defense,
////                        Avatar::AvatarClass avatar_class,
////                        Number* melee_armor_factor,
////                        Number* magic_armor_factor) {
////  *melee_armor_factor = defense * PARAMS[ARMOR_PERCENT_FACTOR];
////  *magic_armor_factor =
////    defense
////      * PARAMS[ARMOR_PERCENT_FACTOR]
////      * (1 + CLASS[avatar_class].magic_armor_bonus_percent);
////}
//
////
//////----[  AvatarDamageTaken  ]--------------------------------------------------
////Number AvatarDamageTaken(Number armor_factor,
////                         Number attacker_level,
////                         Number damage) {
////  Number percentage = Number(1.0) - armor_factor / attacker_level;
////  return damage * max(percentage, PARAMS[ARMOR_MIN_DAMAGE_TAKEN]);
////}
//
//
//
//
//
////----[  AvatarArmorFactors2  ]------------------------------------------------
//void AvatarArmorFactors2(Number defense,
//                         Avatar::AvatarClass avatar_class,
//                         Avatar::AvatarLevel level,
//                         Number* melee_armor_factor,
//                         Number* magic_armor_factor) {
//  // this revised version is not dependent on the attacker's level, and
//  // works hand-in-hand with the (experimental) adjusted PVP damage method
//  double armor_factor = 0;//(0.2 * defense.toFloat()) / (20.0 * level);
//  *melee_armor_factor
//    = 1.0 - armor_factor;
//  *magic_armor_factor
//    = 1.0 - armor_factor - CLASS[avatar_class].magic_armor_bonus_percent;
//}
//
//
//
////----[  AvatarDamageTaken2  ]-------------------------------------------------
//Number AvatarDamageTaken2(Number armor_factor,
//                          Number damage) {
//  return damage * max(armor_factor, PARAMS[ARMOR_MIN_DAMAGE_TAKEN]);
//}
//
//
////----[  AvatarAbilityEffect  ]------------------------------------------------
//Number AvatarAbilityEffect(Number random,
//                           Number base,
//                           Number critical,
//                           Number bonus,
//                           Number range_lower,
//                           Number range_magnitude) {
//  return base + (range_lower + ((random < critical) ? range_magnitude : (random * range_magnitude))) * bonus;
//}
//
//
//
//
//
////----[  AvatarMeleeAttackDelayMultiplier  ]-----------------------------------
//Number AvatarMeleeAttackDelayMultiplier(Number level, Number agility) {
//  float expected_agility = (20.0 + level.toFloat() / 3.0);
//  float multiplier = 1.5 - (agility.toFloat() - 10.0) * (0.5 / (expected_agility - 10.0));
//  return Number(multiplier > 0.5 ? multiplier : 0.5);
//}
//
//
////----[  AvatarMagicDelayMultiplier  ]-----------------------------------------
//Number AvatarMagicDelayMultiplier(Number level, Number agility) {
//  float expected_agility = (15.0 + level.toFloat() / 7.0);
//  float multiplier = 1.0 - (agility.toFloat() - 10.0) * (0.2 / (expected_agility - 10.0));
//  return Number(multiplier > 0.5 ? multiplier : 0.5);
//}
//
//
//
//

}
}

