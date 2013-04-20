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
#include <dc/enum>
#include "shared/magic/magic_portal.h"
#include "common/null.h"


//----[  getString  ]----------------------------------------------------------
template<>
const char* dc::dcEnum<Evidyon::Magic::Magic_Portal::Type>
    ::getString(Evidyon::Magic::Magic_Portal::Type value) {
  using namespace Evidyon::Magic;
  switch (value) {
    case Magic_Portal::SELF_ONLY_MARK1: return "Self-Only to Mark 1";
    case Magic_Portal::SELF_ONLY: return "Self-Only";
    case Magic_Portal::OPEN_PORTAL_MARK1: return "Open Portal to Mark 1";
    case Magic_Portal::OPEN_PORTAL: return "Open Portal";
    case Magic_Portal::DIMENSION_DOOR: return "Dimension Door";
    case Magic_Portal::REVERSE_DIMENSION_DOOR: return "Reverse Dimension Door";
    case Magic_Portal::OPEN_PORTAL_TO_DESTINATION: return "Open Portal to Destination";
    case Magic_Portal::SELF_ONLY_TO_SPAWN_POINT: return "Self-Only to Spawn Point";
    default:  return NULL;
  }
}



//----[  staticTypeName  ]-----------------------------------------------------
std::string dc::dcEnum<Evidyon::Magic::Magic_Portal::Type>::staticTypeName()
{
    return "Enum-Magic-Magic_Portal-Type";
}
