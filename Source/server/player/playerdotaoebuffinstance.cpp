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
#include "playerdotaoebuffinstance.h"
#include "common/null.h"

namespace Evidyon {
namespace Server {

  
//----[  zero  ]---------------------------------------------------------------
void PlayerDOTAOEBuffInstance::zero() {
  data = NULL;
  next_hit_timer.reset();
  x = 0;
  z = 0;
  expiration_timer.reset();
  event_id = Event::INVALID_EVENT_ID;
  local_region = NULL;
}




//----[  enable  ]-------------------------------------------------------------
void PlayerDOTAOEBuffInstance::enable() {
  next_hit_timer.enable();
  expiration_timer.enable();
}



//----[  disable  ]------------------------------------------------------------
void PlayerDOTAOEBuffInstance::disable() {
  data = NULL;
  event_id = Event::INVALID_EVENT_ID;
  local_region = NULL;
  next_hit_timer.disable();
  expiration_timer.disable();
}



}
}

