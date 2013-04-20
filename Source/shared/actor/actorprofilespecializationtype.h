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
#ifndef __ACTORPROFILESPECIALIZATIONTYPE_H__
#define __ACTORPROFILESPECIALIZATIONTYPE_H__
#pragma once



namespace Evidyon {
namespace Actor {




//----[  ActorProfileSpecializationType  ]-------------------------------------
enum ActorProfileSpecializationType {
  SPECIALIZE_UNARMED,
  SPECIALIZE_1HWEAPON_WITH_SHIELD,
  SPECIALIZE_1HWEAPON_NO_SHIELD,
  SPECIALIZE_DUAL_WIELD,
  SPECIALIZE_2HWEAPON,
  SPECIALIZE_STAFF,
  SPECIALIZE_BOW,
  SPECIALIZE_CROSSBOW,
  SPECIALIZE_JAVELIN,
  SPECIALIZE_THROWN,
  SPECIALIZE_SPEAR,
  //SPECIALIZE_SWIMMING,
  NUMBER_OF_ACTOR_PROFILE_SPECIALIZATION_TYPES,
  INVALID_ACTOR_PROFILE_SPECIALIZATION,
};




//----[  ActorProfileSpecializationTypeString  ]-------------------------------
const char* ActorProfileSpecializationTypeString(
  ActorProfileSpecializationType value);


}
}

#endif