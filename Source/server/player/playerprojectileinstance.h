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