//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#include "zonerules.h"
#include "common/null.h"
#include <assert.h>


namespace Evidyon {
namespace Zone {



//----[  ZoneRulesString  ]----------------------------------------------------
const char* ZoneRulesString(ZoneRules value) {
  switch (value) {
    default: return NULL;
    case ZONERULES_NORMAL:    return "Normal";
    case ZONERULES_TOWN:      return "Town";
    case ZONERULES_PVP_ARENA: return "PVP Arena";
    case ZONERULES_WILD_GEOSID: return "Wild Geosid";
  }
}


//----[  ZoneRulesDescription  ]-----------------------------------------------
const char* ZoneRulesDescription(ZoneRules value) {
  switch (value) {
  default: assert(false); return NULL;
  case ZONERULES_NORMAL:
    return "- You will drop items if you die\n"
           "- Good players drop fewer items than evil ones\n"
           "- Killing more than one non-evil player in a day causes your alignment to become more evil\n"
           "- The attacker in player-vs-player combat deals reduced damage until the attacked fights back";
  case ZONERULES_TOWN:
    return "- Non-evil players are protected from harm";
  case ZONERULES_PVP_ARENA:
    return "- No items will be dropped if you die\n"\
           "- There are no consequences for being killed"\
           "- Normal alignment rules are suspended\n"\
           "- Damage can not be dealt across this zone's boundaries\n"\
           "- Teleport does not work across the zone's boundaries\n";
  case ZONERULES_WILD_GEOSID:
    return "n/a";
  }
}



//----[  ZoneRules_PenalizeXPOnDeath  ]----------------------------------------
bool ZoneRules_PenalizeXPOnDeath(ZoneRules value) {
  switch (value) {
    default: return true;
    case ZONERULES_NORMAL:
    case ZONERULES_TOWN:
      return true;
    case ZONERULES_PVP_ARENA:
    case ZONERULES_WILD_GEOSID:
      return false;
  }
}


//----[  ZoneRules_DropItemsOnDeath  ]-----------------------------------------
bool ZoneRules_DropItemsOnDeath(ZoneRules value) {
  switch (value) {
    default: return true;
    case ZONERULES_NORMAL:
    case ZONERULES_TOWN:
      return true;
    case ZONERULES_PVP_ARENA:
    case ZONERULES_WILD_GEOSID:
      return false;
  }
}


}
}

