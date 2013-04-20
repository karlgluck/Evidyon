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
#ifndef __ACTORACTION_H__
#define __ACTORACTION_H__
#pragma once



namespace Evidyon {
namespace Actor {



//----[  ActorAction  ]--------------------------------------------------------
// These are in priority order; if an action comes along while another is
// being executed, the animation for that action is only applied if it has
// a higher priority than the current one.
enum ActorAction {
  ACTORACTION_DIE,
  ACTORACTION_SPIN_ATTACK,
  ACTORACTION_CAST_AREA_SPELL,
  ACTORACTION_CAST_TARGETED_SPELL,
  ACTORACTION_ATTACK,
  ACTORACTION_GOT_HIT,
  ACTORACTION_LEAP,
  NUMBER_OF_ACTOR_ACTIONS,
  ACTORACTION_NONE = NUMBER_OF_ACTOR_ACTIONS,
};




//----[  ActorActionString  ]--------------------------------------------------
const char* ActorActionString(ActorAction value);




}
}

#endif