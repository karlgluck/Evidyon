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
#include "navigability.h"
#include "common/null.h"


namespace Evidyon {
namespace World {




//----[  NavigabilityString  ]-------------------------------------------------
const char* NavigabilityString(Navigability navigability) {
  switch (navigability) {
  case NAVIGABILITY_NORMAL:           return "Normal";
  case NAVIGABILITY_NO_TELEPORTING:   return "No Teleporting";
  case NAVIGABILITY_WADE:             return "Wade";
  case NAVIGABILITY_SWIM:             return "Swim";
  case NAVIGABILITY_PIT:              return "Pit";
  case NAVIGABILITY_LOWWALL:          return "Low Wall";
  case NAVIGABILITY_HIGHWALL:         return "High Wall";
  case NAVIGABILITY_SAFEZONE:         return "Safe Zone";
  case NAVIGABILITY_IMPASSABLE:       return "Impassable";
  default:                            return NULL;
  }
}

//----[  NavigabilitySolidToWalking  ]-----------------------------------------
bool NavigabilitySolidToWalking(Navigability navigability) {
  switch (navigability) {
  case NAVIGABILITY_NORMAL:
  case NAVIGABILITY_NO_TELEPORTING:
  case NAVIGABILITY_WADE:
  case NAVIGABILITY_SAFEZONE:
    return false;
  default:
    return true;
  }
}

//----[  NavigabilitySolidToFlying  ]------------------------------------------
bool NavigabilitySolidToFlying(Navigability navigability) {
  switch (navigability) {
  default:
    return false;
  case NAVIGABILITY_HIGHWALL:
  case NAVIGABILITY_IMPASSABLE:
    return true;
  }
}

//----[  NavigabilitySolidToSwimming  ]----------------------------------------
bool NavigabilitySolidToSwimming(Navigability navigability) {
  return navigability != NAVIGABILITY_SWIM;
}

  
}  
}

