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
#ifndef __SHARED_PROJECTILE_PROJECTILE_H__
#define __SHARED_PROJECTILE_PROJECTILE_H__
#pragma once

#include "shared/time/milliseconds.h"
#include "shared/event/eventindex.h"

namespace Evidyon {
namespace Projectile {


//----[  Projectile  ]---------------------------------------------------------
// This structure is contained within projectile magic and projectile weapons,
// and describes the behavior of the projectiles that magic or weapon create.
struct Projectile {
  Event::EventIndex event_index;
  float speed;
  Time::Milliseconds duration;
  float radius;
  bool homing;
  float homing_rate;
  bool bouncing;
  bool piercing;
  Time::Milliseconds piercing_period;  // 1.0f / speed (once per square)
};



}
}

#endif