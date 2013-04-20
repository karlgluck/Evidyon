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
#include "actorprofilespecializationtype.h"
#include "common/null.h"


namespace Evidyon {
namespace Actor {



//----[  ActorProfileSpecializationTypeString  ]-------------------------------
const char* ActorProfileSpecializationTypeString(
    ActorProfileSpecializationType value) {
  switch (value) {
  case SPECIALIZE_UNARMED: return "Unarmed";
  case SPECIALIZE_1HWEAPON_WITH_SHIELD: return "1H Weapon and Shield";
  case SPECIALIZE_1HWEAPON_NO_SHIELD:   return "1H Weapon (No Shield)";
  case SPECIALIZE_DUAL_WIELD: return "Dual Wield";
  case SPECIALIZE_2HWEAPON:   return "2H Weapon";
  case SPECIALIZE_STAFF:      return "Staff";
  case SPECIALIZE_BOW:        return "Bow";
  case SPECIALIZE_CROSSBOW:   return "Crossbow";
  case SPECIALIZE_JAVELIN:    return "Javelin";
  case SPECIALIZE_THROWN:     return "Thrown";
  case SPECIALIZE_SPEAR:      return "Spear";
  //case SPECIALIZE_SWIMMING:   return "Swimming";
  default: return NULL;
  }
}



}
}

