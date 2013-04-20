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
#include "constantactoranimation.h"
#include "common/null.h"

namespace Evidyon {
namespace Actor {


//----[  ConstantActorAnimationString  ]---------------------------------------
const char* ConstantActorAnimationString(ConstantActorAnimation value) {
  switch (value) {
    case ANIMATION_DEATH_1: return "Death Animation 1";
    case ANIMATION_DEATH_2: return "Death Animation 2";
    case ANIMATION_DEATH_3: return "Death Animation 3";
    case ANIMATION_CAST_AREA_SPELL: return "Cast Spell - Area";
    case ANIMATION_CAST_TARGETED_SPELL: return "Cast Spell - Targeted";
    case ANIMATION_CAST_SELF_SPELL: return "Cast Spell - Self";
    case ANIMATION_ATTACK_RUN: return "Attack Run";
    case ANIMATION_LEAP: return "Leap";
    case ANIMATION_LEAP_REPEATED: return "Leap (Repeated)";
    case ANIMATION_RUN: return "Run";
    case ANIMATION_IDLE: return "Idle";
    case ANIMATION_SWIM: return "Swim";
    default: return NULL;
  }
}



}
}

