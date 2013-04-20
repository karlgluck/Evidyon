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
#ifndef __SHARED_MAGIC_TOOLS_AOEMAGICDATA_H__
#define __SHARED_MAGIC_TOOLS_AOEMAGICDATA_H__
#pragma once

#include "shared/specialfx/tools/specialfx.h"
#include <dc/float>
#include <dc/boolean>
#include "shared/magic/aoemagicdata.h"
#include <dc/enum>
#include <dc/integer>
#include "shared/time/milliseconds.h"

namespace Evidyon {
namespace Event {
namespace Tools {
class EventCompiler;
}
}
}

namespace Evidyon {
namespace Magic {
namespace Tools {


  
//----[  AOEMagicData  ]-------------------------------------------------------
class AOEMagicData : public dc::dcResource<AOEMagicData> {
public:
  static std::string staticTypeName() { return "AOEMagicData"; }
public:
  AOEMagicData();
  void compile(Evidyon::Event::Tools::EventCompiler* event_compiler,
               Evidyon::Magic::AOEMagicData* description);
  Time::Milliseconds getDurationMillis();
  void setDurationMillis(Time::Milliseconds time);
private:
  dc::dcEnum<Evidyon::Magic::AOEMagicData::Location> source_;
  dc::dcEnum<Evidyon::Magic::AOEMagicData::Location> effect_;
  Evidyon::Tools::SpecialFXReference special_fx_;
  dc::dcFloat special_fx_duration_;
  dc::dcFloat radius_;
  dc::dcBoolean target_caster_;
  dc::dcBoolean target_friends_;
  dc::dcBoolean target_enemies_;
  dc::dcInteger max_targets_;       // highest number of simultaneous targets
};



}
}
}

#endif