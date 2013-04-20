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
#ifndef __NAVIGABILITY_H__
#define __NAVIGABILITY_H__
#pragma once



namespace Evidyon {
namespace World {

//----[  Navigability  ]-------------------------------------------------------
// Defines how players can interact with map locations.
enum Navigability {
  NAVIGABILITY_NORMAL,            // walk, teleport, fly, etc.
  NAVIGABILITY_NO_TELEPORTING,    // can't teleport on, from or over
  NAVIGABILITY_WADE,              // move character down
  NAVIGABILITY_SWIM,              // water is too deep to wade; must swim
  NAVIGABILITY_PIT,               // fly over, otherwise player falls & dies
  NAVIGABILITY_LOWWALL,           // fly over but can't walk through
  NAVIGABILITY_HIGHWALL,          // must teleport over this location
  NAVIGABILITY_SAFEZONE,          // NORMAL, plus safe-zone behavior ///////////// TODO: ERASE, REPLACED w/ ZONE RULES
  NAVIGABILITY_IMPASSABLE,        // high wall + no teleporting
  NAVIGABILITY_INVALID,
};



//----[  NavigabilityString  ]-------------------------------------------------
const char* NavigabilityString(Navigability navigability);

//----[  NavigabilitySolidToWalking  ]-----------------------------------------
// Can a tile with the given navigability be moved over by an entity that
// cannot fly or swim?
bool NavigabilitySolidToWalking(Navigability navigability);

//----[  NavigabilitySolidToFlying  ]------------------------------------------
// Can a tile with the given navigability be flown over?
bool NavigabilitySolidToFlying(Navigability navigability);

//----[  NavigabilitySolidToSwimming  ]----------------------------------------
// Can an entity that is only able to swim move over the given navigability?
bool NavigabilitySolidToSwimming(Navigability navigability);

}
}

#endif