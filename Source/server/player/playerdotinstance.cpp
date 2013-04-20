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
#include "playerdotinstance.h"


namespace Evidyon {
namespace Server {



//----[  zero  ]---------------------------------------------------------------
void PlayerDOTInstance::zero() {
  data = NULL;
  target.reset();
  expiration_timer.reset();
  event_id = Event::INVALID_EVENT_ID;
  target_hp_change = 0;
  next_hit_timer.reset();
}



//----[  enable  ]-------------------------------------------------------------
void PlayerDOTInstance::enable() {
  expiration_timer.enable();
  next_hit_timer.enable();
}



//----[  disable  ]------------------------------------------------------------
void PlayerDOTInstance::disable() {
  data = NULL;
  target.reset();
  expiration_timer.disable();
  next_hit_timer.disable();
  target_hp_change = 0;
}




}
}

