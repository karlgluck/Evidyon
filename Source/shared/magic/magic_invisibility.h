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
#ifndef __SHARED_MAGIC_MAGIC_INVISIBILITY_H__
#define __SHARED_MAGIC_MAGIC_INVISIBILITY_H__
#pragma once

namespace Evidyon {
namespace Magic {

//----[  Magic_Invisibility  ]-------------------------------------------------
// Client sends destroy actor packets to all clients other than itself, and no
// longer sends slow-sync updates.  This means that the client's avatar
// can't be directly targeted by players, but is still susceptible to
// projectile attacks.  If the player is targeted by any spell that requires
// them to be present on the client for the effect to work (an AOE, for
// example, or any spell they cast) invisibility is removed.
struct Magic_Invisibility {
  float duration;
};

}
}

#endif