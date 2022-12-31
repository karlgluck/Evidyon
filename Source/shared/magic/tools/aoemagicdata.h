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