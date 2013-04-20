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
#include "newdesign.h"
#include "common/randint32.h"

#include <dc/debug>

namespace Evidyon {
namespace Design {



// Returns (table[index] * multiplier), clamping the index to [0, table size)
// and casting the return type to that specified.
template <typename T, const size_t TABLE_SIZE, typename R>
inline R indexAndMultiply(const T* table,
                          Avatar::AvatarLevel intended_level,
                          float multiplier) {
  intended_level = intended_level < 1 ? 0 : intended_level - 1;
  if (intended_level >= TABLE_SIZE) intended_level = TABLE_SIZE - 1;
  return R(table[intended_level] * multiplier);
}



//----[  MonsterHP  ]----------------------------------------------------------
int MonsterHP(Avatar::AvatarLevel intended_level, float hp_multiplier) {
  static const int TABLE[] = {
#include "monsterhptabledata.h"
  };
  return indexAndMultiply<int, sizeof(TABLE) / sizeof(int), int>(
    TABLE,
    intended_level,
    hp_multiplier);
}



//----[  MonsterXP  ]----------------------------------------------------------
Avatar::AvatarExperience MonsterXP(Avatar::AvatarLevel intended_level,
                                   float xp_multiplier) {
  static const int TABLE[] = {
#include "monsterxptabledata.h"
  };
  return indexAndMultiply<int,
                          sizeof(TABLE) / sizeof(int),
                          Avatar::AvatarExperience>(TABLE,
                                                    intended_level,
                                                    xp_multiplier);
}



//----[  MonsterAttackDamage  ]------------------------------------------------
int MonsterAttackDamage(Avatar::AvatarLevel intended_level,
                        float dps_to_damage_multiplier) {
  static const float TABLE[] = {
#include "monsterdpstabledata.h"
  };
  return indexAndMultiply<float,
                          sizeof(TABLE) / sizeof(float),
                          Avatar::AvatarExperience>(TABLE,
                                                    intended_level,
                                                    dps_to_damage_multiplier);
}



//----[  MonsterGold  ]--------------------------------------------------------
unsigned int MonsterGold(Avatar::AvatarLevel intended_level) {
  if (intended_level > Avatar::HIGHEST_AVATAR_LEVEL) {
    intended_level = Avatar::HIGHEST_AVATAR_LEVEL;
  }
  return 2 * intended_level + 10 + rand_uint32()%(1+intended_level);
}



//----[  BoostedAvatarStat  ]--------------------------------------------------
int BoostedAvatarStat(int stat_value) {
  static const int TABLE[] = {
#include "boostedstattabledata.h"
  };
  stat_value = (stat_value < 11) ? 0 : stat_value - 10;
  static const int TABLE_SIZE = sizeof(TABLE) / sizeof(float);
  if (stat_value >= TABLE_SIZE) stat_value = TABLE_SIZE - 1;
  return TABLE[stat_value];
}



//----[  ClassHP  ]------------------------------------------------------------
int ClassHP(Avatar::AvatarClass avatar_class) {
  static const int TABLE[] = {
    10, // adept
    11, // sorcerer
    //12, // leyshaper
    13, // hunter
    //14, // paladin
    15, // warrior
  };
  return TABLE[avatar_class];
}



//----[  RaceHP  ]-------------------------------------------------------------
int RaceHP(Avatar::AvatarRace avatar_race) {
  static const int TABLE[] = {
    32, // geonan
    46, // lucan
    50, // dantalion
    37, // volucris
    41, // human
  };
  return TABLE[avatar_race];
}



//----[  HPBonus  ]------------------------------------------------------------
int HPBonus(int boosted_str, int boosted_con) {
  float con_hp;
  if (boosted_con < 25) {
    con_hp = (boosted_con - 10) * 0.2700;
  } else {
    con_hp = (boosted_con - 25) * 13.000;
  }
  float str_hp;
  if (boosted_str < 26) {
    str_hp = (boosted_str - 10) * 0.13333;
  } else {
    str_hp = (boosted_str - 26) * 5.000;
  }
  return con_hp + str_hp;
}



//----[  AvatarHP  ]-----------------------------------------------------------
int AvatarHP(Avatar::AvatarRace avatar_race,
             Avatar::AvatarClass avatar_class,
             Avatar::AvatarLevel avatar_level,
             int strength,
             int constitution) {
  return avatar_level * ClassHP(avatar_class)
         + HPBonus(BoostedAvatarStat(strength), BoostedAvatarStat(constitution))
         + RaceHP(avatar_race);
}




//----[  ClassMP  ]------------------------------------------------------------
int ClassMP(Avatar::AvatarClass avatar_class) {
  static const int TABLE[] = {
    14, // adept
    10, // sorcerer
    //9, // leyshaper
    2, // hunter
    //4, // paladin
    2, // warrior
  };
  return TABLE[avatar_class];
}



//----[  RaceMP  ]-------------------------------------------------------------
int RaceMP(Avatar::AvatarRace avatar_race) {
  const int TABLE[] = {
    10, // geonan
    6, // lucan
    9, // dantalion
    5, // volucris
    7, // human
  };
  return TABLE[avatar_race];
}



//----[  MPBonus  ]------------------------------------------------------------
int MPBonus(int boosted_int, int boosted_wis) {
  float wis_hp;
  if (boosted_wis < 17) {
    wis_hp = (boosted_wis - 10) * 0.53;
  } else {
    wis_hp = (boosted_wis - 17) * 13.0;
  }
  float int_hp;
  if (boosted_int < 21) {
    int_hp = (boosted_int - 10) * 0.13333;
  } else {
    int_hp = (boosted_int - 21) * 6.00;
  }
  return int_hp + wis_hp;
}



//----[  AvatarMP  ]-----------------------------------------------------------
int AvatarMP(Avatar::AvatarRace avatar_race,
             Avatar::AvatarClass avatar_class,
             Avatar::AvatarLevel avatar_level,
             int intelligence,
             int wisdom) {
  return avatar_level * ClassMP(avatar_class)
    + MPBonus(BoostedAvatarStat(intelligence), BoostedAvatarStat(wisdom))
    + RaceMP(avatar_race);
}



//----[  AvatarHPRegen  ]------------------------------------------------------
int AvatarHPRegen(Avatar::AvatarRace avatar_race,
                  Avatar::AvatarClass avatar_class,
                  Avatar::AvatarLevel avatar_level,
                  int strength,
                  int constitution) {
  return 10; // in the new system (oct 2009) regen is purely from items
  //int current_hp
  //  = AvatarHP(avatar_race, avatar_class, avatar_level, strength, constitution);
  //int hp_no_str
  //  = AvatarHP(avatar_race, avatar_class, avatar_level, 10, constitution);
  //return 1 + ((hp_no_str - (current_hp - hp_no_str)) / 15);
}



//----[  AvatarMPRegen  ]------------------------------------------------------
int AvatarMPRegen(Avatar::AvatarRace avatar_race,
                  Avatar::AvatarClass avatar_class,
                  Avatar::AvatarLevel avatar_level,
                  int intelligence,
                  int wisdom) {
  return 10; // in the new system (oct 2009) regen is purely from items
  //int current_mp
  //  = AvatarMP(avatar_race, avatar_class, avatar_level, intelligence, wisdom);
  //int mp_no_int
  //  = AvatarMP(avatar_race, avatar_class, avatar_level, 10, wisdom);
  //return 1 + ((mp_no_int - (current_mp - mp_no_int)) / 10);
}




static const float AVATAR_DPS_TABLE[] = {
#include "avatardpstabledata.h"
};

//----[  AvatarSpellDamage  ]--------------------------------------------------
int AvatarSpellDamage(Avatar::AvatarLevel avatar_level,
                      float spell_dps_to_damage_multiplier,
                      int intelligence) {
  float expected_int = 18.0 + avatar_level / 3.5;
  float base_multiplier = 0.4 / (expected_int - 10);
  float multiplier
    = 0.5 + (BoostedAvatarStat(intelligence)-10) * base_multiplier;
  // I removed this 10/30/2009 because players were complaining that strength
  // didn't do anything useful.
  //if (multiplier > 1.22) multiplier = 1.22;
  multiplier *= spell_dps_to_damage_multiplier;
  return indexAndMultiply<float,
                          sizeof(AVATAR_DPS_TABLE)/sizeof(float),
                          int>
                         (AVATAR_DPS_TABLE, avatar_level, multiplier);
}




//----[  AvatarSpellDelay  ]---------------------------------------------------
Time::Milliseconds AvatarSpellDelay(Avatar::AvatarLevel avatar_level,
                                    Time::Milliseconds base_delay,
                                    int agility) {
  float expected_agi = 15 + avatar_level / 7;
  float base_multiplier = 0.2 / (expected_agi - 10);
  float multiplier = 1 - ((BoostedAvatarStat(agility)-10) * base_multiplier);
  if (multiplier < 0.60) multiplier = 0.60;
  return Time::Milliseconds(multiplier * base_delay);
}




//----[  AvatarMeleeDamage  ]--------------------------------------------------
int AvatarMeleeDamage(Avatar::AvatarLevel avatar_level,
                      int min_damage,
                      int max_damage,
                      int strength,
                      Avatar::AvatarClass avatar_class) {
  float expected_str = 15.0 + avatar_level / 3.5;
  float base_multiplier = 0.4 / (expected_str - 10);
  float multiplier = 0.25 + ((BoostedAvatarStat(strength)-10) * base_multiplier);
  // I removed this 9/30/2009 because players were complaining that strength
  // didn't do anything useful.
  //if (multiplier > 1.22) multiplier = 1.22;
  if (Avatar::AvatarClassIsMage(avatar_class)) multiplier /= 2.0;
  int damage_range = (max_damage - min_damage); // ensure it's at least 1
  damage_range = damage_range < 1 ? 1 : damage_range;
  return Time::Milliseconds(multiplier *
                            (min_damage + (rand_uint32()%damage_range)));

}



//----[  AvatarMeleeDelay  ]---------------------------------------------------
Time::Milliseconds AvatarMeleeDelay(Avatar::AvatarLevel avatar_level,
                                    Time::Milliseconds base_delay,
                                    int agility,
                                    Avatar::AvatarClass avatar_class) {
  int boosted_agi = BoostedAvatarStat(agility);
  float multiplier;
  if (boosted_agi < 25) {
    multiplier = 2.0 - ((boosted_agi - 10.0) * 1.0/(25.0 - 10.0));
  } else {
    float expected_agi = 22.0 + avatar_level / 2.5;
    float base_multiplier = 0.40 / (expected_agi - 10.0);
    multiplier = 1.0 - (boosted_agi - 25) * base_multiplier;
    if (multiplier < 0.7) { // reduce slope after 70% delay
      multiplier = (multiplier - 0.7) / 3.0 + 0.7;
    }
  }
  if (Avatar::AvatarClassIsMage(avatar_class)) multiplier *= 2.0;
  return Time::Milliseconds(multiplier * base_delay);
}





//----[  AvatarDamageTaken  ]--------------------------------------------------
int AvatarDamageTaken(Avatar::AvatarLevel avatar_level,
                      Avatar::AvatarClass avatar_class,
                      bool magical_damage,
                      int defense,
                      int damage) {

  // for every (this amount) of armor, damage taken is reduced by 20%.
  // level 1=50, level 10=104, level 50=344, level 100=644
  double twenty_percent_reduction_armor = (avatar_level*6.0) + 44.0;

  // amount of reduction added due to this being a mage class and the damage
  // is magical (mages take less magic damage naturally)
  double mage_reduction
    = (magical_damage && Avatar::AvatarClassIsMage(avatar_class)) ? 0.2 : 0.0;

  // how much % of damage is reduced
  double reduction = defense / twenty_percent_reduction_armor * 0.2
                     + mage_reduction;

  return damage - int(damage * reduction);
}



//----[  AvatarMovementSpeed  ]------------------------------------------------
float AvatarMovementSpeed(int agility) {
  static const float TABLE[] = {
#include "avatarmovementspeedtabledata.h"
  };
  agility -= 10;
  if (agility < 0) agility = 0;
  static const int SIZE = sizeof(TABLE)/sizeof(float);
  if (agility >= SIZE) agility = SIZE - 1;
  return TABLE[agility];
}


}
}

