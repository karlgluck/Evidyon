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
#ifndef __SHARED_DESIGN_NEWDESIGN_H__
#define __SHARED_DESIGN_NEWDESIGN_H__
#pragma once

#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/time/milliseconds.h"

namespace Evidyon {
namespace Design {


int MonsterHP(Avatar::AvatarLevel intended_level, float hp_multiplier);

// Calculates the amount of experience to give those who slayed the monster
Avatar::AvatarExperience MonsterXP(Avatar::AvatarLevel intended_level,
                                   float xp_multiplier);

// Returns a negative number indicating how much to change the target's HP
// when a monster lands an attack.
int MonsterAttackDamage(Avatar::AvatarLevel intended_level,
                        float dps_to_damage_multiplier);

// Obtains the quantity of gold dropped when a monster dies and generates gold
unsigned int MonsterGold(Avatar::AvatarLevel intended_level);

int BoostedAvatarStat(int stat_value);

int AvatarHP(Avatar::AvatarRace avatar_race,
             Avatar::AvatarClass avatar_class,
             Avatar::AvatarLevel avatar_level,
             int strength,
             int constitution);

int AvatarMP(Avatar::AvatarRace avatar_race,
             Avatar::AvatarClass avatar_class,
             Avatar::AvatarLevel avatar_level,
             int intelligence,
             int wisdom);

int AvatarHPRegen(Avatar::AvatarRace avatar_race,
                  Avatar::AvatarClass avatar_class,
                  Avatar::AvatarLevel avatar_level,
                  int strength,
                  int constitution);

int AvatarMPRegen(Avatar::AvatarRace avatar_race,
                  Avatar::AvatarClass avatar_class,
                  Avatar::AvatarLevel avatar_level,
                  int intelligence,
                  int wisdom);

int AvatarSpellDamage(Avatar::AvatarLevel avatar_level,
                      float spell_dps_to_damage_multiplier,
                      int intelligence);

Time::Milliseconds AvatarSpellDelay(Avatar::AvatarLevel avatar_level,
                                    Time::Milliseconds base_delay,
                                    int agility);

// Mage classes get their melee damage reduced 50%
int AvatarMeleeDamage(Avatar::AvatarLevel avatar_level,
                      int min_damage,
                      int max_damage,
                      int strength,
                      Avatar::AvatarClass avatar_class);

// Mage classes get their melee delays increased 50%
Time::Milliseconds AvatarMeleeDelay(Avatar::AvatarLevel avatar_level,
                                    Time::Milliseconds base_delay,
                                    int agility,
                                    Avatar::AvatarClass avatar_class);

// reduces damage based on the armor; damage can be negative or
// positive (the sign is preserved)
int AvatarDamageTaken(Avatar::AvatarLevel avatar_level,
                      Avatar::AvatarClass avatar_class,
                      bool magical_damage,
                      int defense,
                      int damage);

float AvatarMovementSpeed(int agility);

}
}

#endif