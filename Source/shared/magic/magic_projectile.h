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
#ifndef __SHARED_MAGIC_MAGIC_PROJECTILE_H__
#define __SHARED_MAGIC_MAGIC_PROJECTILE_H__
#pragma once

#include "shared/projectile/projectile.h"
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"

namespace Evidyon {
namespace Magic {

//----[  Magic_Projectile  ]---------------------------------------------------
// Stage 0: release X projectiles, ++stage_counter < number_of_stages ? next
// Stage 1: release X projectiles, ...
// ...
// Stage N
struct Magic_Projectile {
  int projectiles_per_stage;        // # to release at once
  Time::Milliseconds stage_delay;   // time between projectiles
  Time::Milliseconds effect_delay;  // time until first projectile
  int number_of_stages;             // # of waves to release
  float half_spread;                // radians to spread in both directions
  float dps_to_damage_multiplier;
  Projectile::Projectile projectile;
  // flag is set? ignore 'projectile', only cast if projectile weapon
  bool use_weapon_projectile;
};

}
}

#endif