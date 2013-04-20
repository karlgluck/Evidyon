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
#ifndef __SHARED_ZONE_ZONERULES_H__
#define __SHARED_ZONE_ZONERULES_H__
#pragma once

namespace Evidyon {
namespace Zone {


  
//----[  ZoneRules  ]----------------------------------------------------------
// Zone rules are a special set of rules that configure how the game is played
// in an area.  Chaos is NOT a zone rule:  it is an innate part of the world,
// not part of the meta-game.
// These are arranged in least -> most safe
enum ZoneRules {
  ZONERULES_NORMAL,

  // Wild geosid rules are PVP arena rules, but you don't auto-regenerate
  ZONERULES_WILD_GEOSID,

  // The PVP Arena Rules are:
  //  - can't teleport into or out of, but can teleport within
  //  - can't damage someone inside from outside or vice-versa
  //  - no items drop if you die
  //  - you don't respawn if you die (you die but regenerate to full health)
  //  - your pk counts & alignment aren't affected for killing players
  ZONERULES_PVP_ARENA,

  // For evil actors, town rules are normal rules.
  //  - can't damage any other actor while in the town
  //  - no items drop if you die
  ZONERULES_TOWN,
};



//----[  ZoneRulesString  ]----------------------------------------------------
const char* ZoneRulesString(ZoneRules value);


//----[  ZoneRulesDescription  ]-----------------------------------------------
const char* ZoneRulesDescription(ZoneRules value);

//----[  ZoneRules_PenalizeXPOnDeath  ]----------------------------------------
bool ZoneRules_PenalizeXPOnDeath(ZoneRules value);

//----[  ZoneRules_DropItemsOnDeath  ]-----------------------------------------
bool ZoneRules_DropItemsOnDeath(ZoneRules value);

}
}

#endif