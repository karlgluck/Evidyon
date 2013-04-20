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
#ifndef __SERVER_PLAYER_PLAYERDOTINSTANCE_H__
#define __SERVER_PLAYER_PLAYERDOTINSTANCE_H__
#pragma once

#include "server/actorpointer.h"
#include "shared/event/eventid.h"
#include "shared/time/timer.h"
#include "common/number.h"

namespace Evidyon {
namespace Magic {
struct Magic_DOT;
}
}

namespace Evidyon {
namespace Server {

//----[  PlayerDOTInstance  ]--------------------------------------------------
struct PlayerDOTInstance {

  // what this DOT does
  const Magic::Magic_DOT* data;

  // to whom this DOT is attached
  ActorPointer target;

  // when should we cancel the DOT event?
  Time::Timer<Time::RESOLUTION_100_MS> expiration_timer;

  // the event that is shown on the client.  this is kept around because the
  // event can be cancelled by pressing stop-action
  Event::EventID event_id;

  // next time to apply
  Time::Timer<Time::RESOLUTION_100_MS> next_hit_timer;

  // how much to change the target's hp each hit (negative = damage)
  Number target_hp_change;

  void zero();

  // enables timers
  void enable();

  // resets timers, sets data = NULL
  void disable();
};

}
}

#endif