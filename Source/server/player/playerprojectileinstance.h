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
#ifndef __SERVER_PLAYER_PLAYERPROJECTILEINSTANCE_H__
#define __SERVER_PLAYER_PLAYERPROJECTILEINSTANCE_H__
#pragma once

#include "shared/time/timer.h"
#include "server/actorpointer.h"
#include "server/player/playerattackeffect.h"
#include "shared/event/eventid.h"
#include "common/number.h"


namespace Evidyon {
namespace Projectile {
struct Projectile;
}
}



namespace Evidyon {
namespace Server {


//----[  PlayerProjectileInstance  ]-------------------------------------------
// This is an instance of a projectile spell or attack initiated by a player
struct PlayerProjectileInstance {
  const Evidyon::Projectile::Projectile* data;
  Event::EventID event_id;
  Number x, z;
  Number dx, dz;

  PlayerAttackEffect effect;
  Time::Timer<Time::RESOLUTION_10_MS> expiration_timer;
  
  struct {
    Number target_x, target_z;
    ActorPointer target_actor;
  } homing;
  
  struct {
    Time::Timer<Time::RESOLUTION_10_MS> next_pierce_hit_timer;
  } piercing;

  // resets the contents of this object
  void zero();

  // enables timers
  void enable();

  // resets timers, sets data = NULL
  void disable();
};



}
}

#endif