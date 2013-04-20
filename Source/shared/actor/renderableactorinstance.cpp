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
#include "renderableactorinstance.h"
#include <memory.h>


namespace Evidyon {
namespace Actor {


  
  
//----[  reset  ]--------------------------------------------------------------
void RenderableActorInstance::reset() {
  memset(this, 0, sizeof(RenderableActorInstance));
  base_animation_state = INVALID_BASE_ANIMATION_STATE;
  current_action = ACTORACTION_NONE;
  lower_priority_action = ACTORACTION_NONE;
  last_sync.alive = true;
  binding.reset();
}

}
}

