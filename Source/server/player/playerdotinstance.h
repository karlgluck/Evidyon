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