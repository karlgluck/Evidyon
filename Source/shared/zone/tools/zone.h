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
#ifndef __SHARED_ZONE_TOOLS_ZONE_H__
#define __SHARED_ZONE_TOOLS_ZONE_H__
#pragma once

#include <dc/string>
#include <dc/boolean>
#include <dc/enum>
#include "shared/sound/tools/wavsound.h"
#include "shared/zone/zonerules.h"

namespace Evidyon {
namespace Zone {
struct Zone;
}
}

namespace Evidyon {
namespace Zone {
namespace Tools {


//----[  Zone  ]---------------------------------------------------------------
class Zone : public dc::dcResource<Zone> {
public:
  static std::string staticTypeName() { return "Zone"; }
public:
  Zone();
  void compile(Evidyon::Zone::Zone* zone);
  bool isChaosZone() const;
  ZoneRules getZoneRules() const;

private:
  dc::dcString name_;
  Evidyon::Tools::SoundReference background_sound_;
  dc::dcEnum<ZoneRules> rules_; // which set of rules apply here
  dc::dcBoolean chaos_; // is this a chaos zone?
};


}
}
}



#include <dc/list>

namespace Evidyon {
namespace Tools {

typedef dc::dcList<Evidyon::Zone::Tools::Zone> ZoneList;
typedef dc::dcList<Evidyon::Zone::Tools::Zone>::Element ZoneElement;
typedef dc::dcList<Evidyon::Zone::Tools::Zone>::Reference ZoneReference;

}
}



#endif