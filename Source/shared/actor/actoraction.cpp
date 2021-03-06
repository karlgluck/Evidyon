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
#include "actoraction.h"
#include "common/null.h"


namespace Evidyon {
namespace Actor {





//----[  ActorActionString  ]--------------------------------------------------
const char* ActorActionString(ActorAction value) {
  switch (value) {
  case ACTORACTION_DIE:   return "Die";
  case ACTORACTION_SPIN_ATTACK:   return "Spin Attack";
  case ACTORACTION_CAST_AREA_SPELL:   return "Cast Area Spell";
  case ACTORACTION_CAST_TARGETED_SPELL:   return "Cast Targeted Spell";
  case ACTORACTION_ATTACK:   return "Attack";
  case ACTORACTION_GOT_HIT:   return "Got Hit";
  case ACTORACTION_LEAP:  return "Leap";
  case ACTORACTION_NONE:   return "None";
  default: return NULL;
  }
}

}
}

