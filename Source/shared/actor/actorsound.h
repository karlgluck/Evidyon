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
#ifndef __ACTORSOUND_H__
#define __ACTORSOUND_H__
#pragma once


namespace Evidyon {
namespace Actor {

//----[  ActorSound  ]---------------------------------------------------------
enum ActorSound {
  ACTORSOUND_ATTACK_1,
  ACTORSOUND_ATTACK_2,
  ACTORSOUND_ATTACK_3,
  ACTORSOUND_GET_HIT_1,
  ACTORSOUND_GET_HIT_2,
  ACTORSOUND_DIE_1,
  ACTORSOUND_DIE_2,
  NUMBER_OF_ACTOR_SOUNDS,
  INVALID_ACTOR_SOUND,
};



//----[  ActorSoundString  ]---------------------------------------------------
const char* ActorSoundString(ActorSound value);

}
}

#endif