//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
//                                                                           //
//  Permission is hereby granted, free of charge, to any person obtaining a  //
//  copy of this software and associated documentation files (the            //
//  "Software"), to deal in the Software without restriction, including      //
//  without limitation the rights to use, copy, modify, merge, publish,      //
//  distribute, sublicense, and/or sell copies of the Software, and to       //
//  permit persons to whom the Software is furnished to do so, subject to    //
//  the following conditions:                                                //
//                                                                           //
//  The above copyright notice and this permission notice shall be included  //
//  in all copies or substantial portions of the Software.                   //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  //
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               //
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     //
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     //
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   //
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