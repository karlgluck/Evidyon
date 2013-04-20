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
#include "detailedmagic.h"


template<> const char* dc::dcEnum<Evidyon::MagicDescriptionStageTargeting::Type>::getString(
  Evidyon::MagicDescriptionStageTargeting::Type value) {
  using namespace Evidyon;
  switch (value) {
    case MagicDescriptionStageTargeting::SELF: return "Self";
    case MagicDescriptionStageTargeting::PROJECTILE: return "Projectile";
    case MagicDescriptionStageTargeting::ANY_TARGET_ACTOR: return "Any Target Actor";
    case MagicDescriptionStageTargeting::NONCASTER_TARGET_ACTOR: return "Non-Caster Target Actor";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER: return "AOE - Caster-Bound";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER_OFFENSIVE: return "Offensive AOE - Caster-Bound";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_OTHER_OFFENSIVE:  return "Offensive AOE - Actor-Bound";
    case MagicDescriptionStageTargeting::AOE_LOCATIONBOUND_OFFENSIVE: return "Offensive AOE - Location-Bound";
    case MagicDescriptionStageTargeting::PERIODIC_AOE_LOCATIONBOUND_OFFENSIVE: return "Periodic Offensive AOE - Location-Bound";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::MagicDescriptionStageTargeting::Type>::staticTypeName()
{
    return "Enum-MagicDescriptionStageTargetingType";
}

