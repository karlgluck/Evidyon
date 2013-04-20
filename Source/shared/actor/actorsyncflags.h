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
#ifndef __ACTORSYNCFLAGS_H__
#define __ACTORSYNCFLAGS_H__
#pragma once


#include "shared/time/milliseconds.h"
#include "shared/actor/actoraction.h"

namespace Evidyon {
namespace Actor {

#pragma pack(push,1)


//----[  ActorSyncFlags  ]-----------------------------------------------------
union ActorSyncFlags {
  static const int PARAMETER_BITS = 10;
  static const Time::Milliseconds MINIMUM_DURATION = 300;
  static const Time::Milliseconds MAXIMUM_DURATION
    = MINIMUM_DURATION + (1<<PARAMETER_BITS);

  enum SyncState {
    // a movement state defines the base behavior of the actor
    // for movements, 'parameter' sets the direction the actor faces
    MOVEMENT_DEAD,
    MOVEMENT_DEAD_WADING,
    MOVEMENT_IDLE,
    MOVEMENT_WALK,
    MOVEMENT_ATTACK_RUN,
    MOVEMENT_TELEPORT,
    MOVEMENT_LEAPING,
    MOVEMENT_WADING,
    MOVEMENT_WADING_IDLE,
    MOVEMENT_WADING_ATTACK_RUN,
    MOVEMENT_SWIMMING,
    MOVEMENT_SWIMMING_IDLE,
    MOVEMENT_FALLING,
    // for actions, 'parameter' sets the duration of the animation
    ACTION_GOT_HIT,
    ACTION_SPIN_ATTACK,
    ACTION_CAST_AREA_SPELL,
    ACTION_CAST_TARGETED_SPELL,
    ACTION_ATTACK,
    ACTION_LEAP,    // sets length of the leap, starts MOVEMENT_LEAPING
  };

  struct {
    unsigned state      : 5;  // 
    unsigned blood      : 1;  // boolean; 1 if this actor was hit by a weapon
    unsigned parameter  : 10; // if moving=0, turn to pi/360/(1<<ACTORSYNC_ANGLE_BITS)*angle
  };

  inline void packAngle(float a);
  inline float unpackAngle() const;

  // Sets the parameter to be a time in milliseconds of the action that is
  // to be performed.  The unpack method returns the speed factor to apply
  // to that animation on the client's side.
  inline void packDuration(Time::Milliseconds a);
  inline float unpackSpeed() const;

  inline Actor::ActorAction unpackAction();

  inline void unpackMovement(
    bool* alive,
    bool* moving,
    bool* teleport,
    bool* leaping,
    bool* wading,
    bool* combat,
    bool* swimming,
    bool* falling,
    float* facing_angle);

  unsigned __int16 union_value;
};


#pragma pack(pop)



//----[  packAngle  ]----------------------------------------------------------
void ActorSyncFlags::packAngle(float a) {
  parameter
    = unsigned((((double)(1<<PARAMETER_BITS))) * (a / (2*3.14159265)));
}


//----[  unpackAngle  ]--------------------------------------------------------
float ActorSyncFlags::unpackAngle() const {
  return float(parameter * (2*3.14159265) / ((double)(1<<PARAMETER_BITS)));
}




//----[  packDuration  ]-------------------------------------------------------
inline void ActorSyncFlags::packDuration(Time::Milliseconds a) {
  a = a > MINIMUM_DURATION ? a : MINIMUM_DURATION;
  parameter = a - MINIMUM_DURATION;
}


//----[  unpackSpeed  ]--------------------------------------------------------
inline float ActorSyncFlags::unpackSpeed() const {
  return float(1000.0 / (MINIMUM_DURATION + parameter));
}



//----[  unpackAction  ]-------------------------------------------------------
Actor::ActorAction ActorSyncFlags::unpackAction() {
  switch (state) {
    case ACTION_GOT_HIT: return Actor::ACTORACTION_GOT_HIT;
    case ACTION_SPIN_ATTACK: return Actor::ACTORACTION_SPIN_ATTACK;
    case ACTION_CAST_AREA_SPELL: return Actor::ACTORACTION_CAST_AREA_SPELL;
    case ACTION_CAST_TARGETED_SPELL: return Actor::ACTORACTION_CAST_TARGETED_SPELL;
    case ACTION_ATTACK: return Actor::ACTORACTION_ATTACK;
    case ACTION_LEAP: return Actor::ACTORACTION_LEAP;
    default: return Actor::ACTORACTION_NONE;
  }
}


//----[  unpackMovement  ]-----------------------------------------------------
void ActorSyncFlags::unpackMovement(
    bool* alive,
    bool* moving,
    bool* teleport,
    bool* leaping,
    bool* wading,
    bool* combat,
    bool* swimming,
    bool* falling,
    float* facing_angle) {

  *alive = state != MOVEMENT_DEAD &&
           state != MOVEMENT_DEAD_WADING &&
           state != MOVEMENT_FALLING;
  *combat = //blood ||
            state == MOVEMENT_LEAPING ||
            state == MOVEMENT_ATTACK_RUN ||
            state == MOVEMENT_WADING_ATTACK_RUN;
            state == ACTION_GOT_HIT ||
            state == ACTION_SPIN_ATTACK ||
            state == ACTION_ATTACK ||
            state == ACTION_LEAP;

  // preserve the old variables if this is an action
  if (state >= ACTION_GOT_HIT) {
    if (state == ACTION_LEAP) *moving = true;
    return; 
  }

  *facing_angle = unpackAngle();
  *moving = state == MOVEMENT_WALK ||
            state == MOVEMENT_ATTACK_RUN ||
            state == MOVEMENT_WADING ||
            state == MOVEMENT_WADING_ATTACK_RUN ||
            state == MOVEMENT_SWIMMING ||
            state == MOVEMENT_LEAPING;
  *teleport = state == MOVEMENT_DEAD || state == MOVEMENT_TELEPORT;
  *leaping = state == MOVEMENT_LEAPING || state == ACTION_LEAP;
  *wading = state == MOVEMENT_WADING ||
            state == MOVEMENT_WADING_ATTACK_RUN ||
            state == MOVEMENT_WADING_IDLE ||
            state == MOVEMENT_DEAD_WADING;
  *swimming = state == MOVEMENT_SWIMMING ||
              state == MOVEMENT_SWIMMING_IDLE;
  *falling = state == MOVEMENT_FALLING;
}

}
}

#endif