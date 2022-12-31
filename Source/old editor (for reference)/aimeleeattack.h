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
#ifndef __AIMELEEATTACK_H__
#define __AIMELEEATTACK_H__
#pragma once


#include <dc/float>
#include <dc/enum>
#include <dc/integer>
#include "../shared/actor/evidyon_actor.h"
#include "leveldefinitions.h"


namespace Evidyon {
struct AIMeleeAttackDescription;
}


class AIMeleeAttack : public dc::dcResource<AIMeleeAttack> {
public:
  AIMeleeAttack();
  void compile(const LevelAttributes* intended_level_attributes,
               int intended_level, // the level for which this attack is configured
               float time_between_attacks, // specified by the AI lifeform
               const float attack_durations[Evidyon::ACTORATTACK_COUNT],
               Evidyon::AIMeleeAttackDescription* description) const;

public:
  static std::string staticTypeName();

private:
  dc::dcFloat range_;
  dc::dcEnum<Evidyon::ActorAttack> attack_;

  // Percent of target's HP dealt in damage per second (min & max) for
  // this attack.  The actual min/max damage values are calculated based
  // on the time between attacks and the average HP of a character
  // at the intended level.
  dc::dcInteger percent_dps_min_, percent_dps_max_;
};



#endif

