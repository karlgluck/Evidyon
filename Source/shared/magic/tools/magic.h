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
#ifndef __SHARED_MAGIC_TOOLS_MAGIC_H__
#define __SHARED_MAGIC_TOOLS_MAGIC_H__
#pragma once

#include "shared/magic/magicindex.h"
#include "shared/specialfx/tools/specialfx.h"
#include <dc/float>
#include <dc/integer>
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"

namespace Evidyon {
namespace Magic {
namespace Tools {
class MagicCompiler;
}
}
}

namespace Evidyon {
namespace Magic {
namespace Tools {


  
//----[  Magic  ]--------------------------------------------------------------
class Magic : public dc::dcGenericResource {
public:
  static std::string staticTypeName() { return "Magic"; }
public:
  Magic();
  virtual MagicIndex compile(MagicCompiler* compiler) = 0;

// These are common values used by many (but not all) of the magic types.
// Those that are used by a derived class are added as members by that class's
// constructor; those that are not can be safely ignored.  However, having
// these in the base class GREATLY reduces the complexity of importing the
// design spreadsheet data, and also reduces the amount of code that must be
// written and rewritten each time a new type of magic is created.
protected:
  Evidyon::Tools::SpecialFXReference special_fx_;
  dc::dcFloat special_fx_duration_;
  Event::EventIndex compileTargetedSpecialFX(MagicCompiler* compiler);
public:
  dc::dcInteger effect_delay_ms;  // time player spends in the "casting" state
  dc::dcInteger targets;
  dc::dcInteger period_ms;
  dc::dcInteger duration_ms;
  dc::dcFloat dps_multiplier;
  dc::dcFloat range;

  // This value is used to communicate the spell's delay down into the magic
  // when it is being compiled.  This is not elegant, but it's functional.
  // This value can be safely ignored; it is set to 1000 by default (which is
  // equivalent to multiplying dps by 1)
  Time::Milliseconds total_action_delay;
};


}
}
}

#include <dc/interface>

namespace Evidyon {
namespace Tools {
typedef dc::dcInterface<Evidyon::Magic::Tools::Magic> MagicInterface;
}
}

#endif