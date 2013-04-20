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
#include "playerprojectileinstance.h"
#include "common/null.h"


namespace Evidyon {
namespace Server {


//----[  zero  ]---------------------------------------------------------------
void PlayerProjectileInstance::zero() {
  data = NULL;
  event_id = Event::INVALID_EVENT_ID;
  effect.zero();
  expiration_timer.trigger();
  homing.target_actor.reset();
}


//----[  enable  ]-------------------------------------------------------------
void PlayerProjectileInstance::enable() {
  expiration_timer.enable();
  expiration_timer.reset();
  piercing.next_pierce_hit_timer.enable();
  piercing.next_pierce_hit_timer.trigger();
}




//----[  disable  ]------------------------------------------------------------
void PlayerProjectileInstance::disable() {
  data = NULL;
  expiration_timer.disable();
  expiration_timer.trigger();
  homing.target_actor.reset();
  piercing.next_pierce_hit_timer.disable();
  piercing.next_pierce_hit_timer.reset();
}


}
}