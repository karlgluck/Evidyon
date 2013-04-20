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
#ifndef __SHARED_LIFEFORMAI_LIFEFORMAI_GEOSIDGUARDIAN_H__
#define __SHARED_LIFEFORMAI_LIFEFORMAI_GEOSIDGUARDIAN_H__
#pragma once


#include "shared/actor/actortemplateindex.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/treasure/treasureclassindex.h"
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"
#include "shared/geosid/geosidindex.h"


namespace Evidyon {
namespace LifeformAI {


 
//----[  LifeformAI_GeosidGuardian  ]------------------------------------------
struct LifeformAI_GeosidGuardian {
  Event::EventIndex despawn_event;
  Time::Milliseconds despawn_delay;

  // which geosid is this guardian tied to?
  Geosid::GeosidIndex geosid;

  Actor::ActorTemplateIndex actor_template_index;
  float size, reach;
  float speed;
  Time::Milliseconds attack_period, attack_duration;

  // how many times would an attacker have killed a normal avatar by the time
  // this guardian is killed?
  // this weird wording is because the guardian takes damage as a %.  It starts
  // with this amount, and takes damage equal to the % of the attacker's hp
  // the attacker's current attack dealt.
  float hp_times_average;

  // what amount of damage taken is reflected on the source?
  float thorns_reflected_damage_multiplier;

  // this value is multiplied by the dps of a character at the *target's* level,
  // so that the guardian adjusts damage to ANY character attacking it!
  float dps_to_damage_multiplier;
};



}
}

#endif