//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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

