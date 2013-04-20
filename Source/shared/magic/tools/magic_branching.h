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
#ifndef __SHARED_MAGIC_TOOLS_MAGIC_BRANCHING_H__
#define __SHARED_MAGIC_TOOLS_MAGIC_BRANCHING_H__
#pragma once

#include "shared/magic/tools/magic.h"
#include "shared/specialfx/tools/specialfx.h"
#include <dc/boolean>
#include <dc/float>
#include <dc/integer>

namespace Evidyon {
namespace Magic {
struct Magic_Branching_Stage;
}
namespace Event {
namespace Tools {
class EventCompiler;
}
}
}

namespace Evidyon {
namespace Magic {
namespace Tools {



  
//----[  Magic_Branching_Stage  ]----------------------------------------------
class Magic_Branching_Stage : public dc::dcResource<Magic_Branching_Stage> {
public:
  static std::string staticTypeName() { return "Magic_Branching_Stage"; }
public:
  Magic_Branching_Stage();
  void compile(Evidyon::Event::Tools::EventCompiler* event_compiler,
               Evidyon::Magic::Magic_Branching_Stage* stage);
private:
  Evidyon::Tools::SpecialFXReference special_fx_;
  dc::dcFloat special_fx_duration_;
  dc::dcInteger targets_;
  dc::dcFloat radius_;
  dc::dcFloat delay_;
  dc::dcInteger min_damage_;
  dc::dcInteger max_damage_;
};

  
//----[  Magic_Branching  ]----------------------------------------------------
class Magic_Branching : public dc::dcResource<Magic_Branching, Evidyon::Magic::Tools::Magic> {
  static dc::dcClass<Magic_Branching>::Implements<Evidyon::Magic::Tools::Magic> implementsMagic;
public:
  static std::string staticTypeName() { return "Magic_Branching"; }
public:
  Magic_Branching();
  virtual MagicIndex compile(MagicCompiler* compiler);
private:
  Magic_Branching_Stage stages_[3];
  dc::dcBoolean find_enemies_;
  dc::dcBoolean find_friends_;
};



}
}
}

#endif