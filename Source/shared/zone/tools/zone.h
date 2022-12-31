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