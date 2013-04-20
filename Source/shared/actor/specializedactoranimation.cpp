//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#include "specializedactoranimation.h"
#include "common/null.h"

namespace Evidyon {
namespace Actor {




//----[  SpecializedActorAnimationString  ]------------------------------------
const char* SpecializedActorAnimationString(SpecializedActorAnimation value) {
  switch (value) {
  case ANIMATION_COMBAT_IDLE: return "Combat Idle";
  case ANIMATION_ATTACK_1: return "Normal Attack";
  case ANIMATION_ATTACK_2: return "Alternate Attack";
  case ANIMATION_SPIN_ATTACK: return "Spin Attack";
  case ANIMATION_GET_HIT: return "Get Hit";
  default:
    return NULL;
  }
}


}
}
