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
#ifndef __SHARED_ACTOR_CONSTANTACTORANIMATION_H__
#define __SHARED_ACTOR_CONSTANTACTORANIMATION_H__
#pragma once


namespace Evidyon {
namespace Actor {



//----[  ConstantActorAnimation  ]---------------------------------------------
enum ConstantActorAnimation {
  ANIMATION_DEATH_1,
  ANIMATION_DEATH_2,
  ANIMATION_DEATH_3,
  ANIMATION_CAST_AREA_SPELL,
  ANIMATION_CAST_TARGETED_SPELL,
  ANIMATION_CAST_SELF_SPELL,
  ANIMATION_ATTACK_RUN,
  ANIMATION_LEAP,
  ANIMATION_LEAP_REPEATED,
  ANIMATION_RUN,
  ANIMATION_IDLE,
  ANIMATION_SWIM, // the only animation played while swimming
  NUMBER_OF_CONSTANT_ACTOR_ANIMATIONS,
  INVALID_CONSTANT_ACTOR_ANIMATION,
};



//----[  ConstantActorAnimationString  ]---------------------------------------
const char* ConstantActorAnimationString(ConstantActorAnimation value);



}
}

#endif