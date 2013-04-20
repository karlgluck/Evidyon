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
#ifndef __SHARED_LIFEFORMAI_TOOLS_LIFEFORMAICOMPILER_H__
#define __SHARED_LIFEFORMAI_TOOLS_LIFEFORMAICOMPILER_H__
#pragma once

#include "common/kg/gid/structcollector.h"
#include "shared/lifeformai/lifeformaiindex.h"

namespace Evidyon {
namespace LifeformAI {
struct LifeformAI;
struct LifeformAI_GeosidGuardian;
struct LifeformAI_SimpleMeleeMonster;
struct LifeformAI_SimpleMeleeTownGuard;
struct LifeformAI_InteractiveNPC;
struct LifeformAI_DirectDamageMonster;
struct LifeformAI_ProjectileMonster;
}
namespace Treasure {
namespace Tools {
class TreasureCompiler;
}
}
namespace Event {
namespace Tools {
class EventCompiler;
}
}
namespace Actor {
namespace Tools {
class ActorTemplateCompiler;
}
}
}

namespace Evidyon {
namespace LifeformAI {
namespace Tools {


  
//----[  LifeformAICompiler  ]-------------------------------------------------
class LifeformAICompiler {
public:
  LifeformAICompiler(
    Treasure::Tools::TreasureCompiler* treasure_compiler,
    Event::Tools::EventCompiler* event_compiler,
    Actor::Tools::ActorTemplateCompiler* actor_template_compiler);

  LifeformAIIndex add(const Evidyon::LifeformAI::LifeformAI& description);
  size_t numberOfLifeforms();
  void compile(size_t index, Evidyon::LifeformAI::LifeformAI* description);

  Treasure::Tools::TreasureCompiler* treasureCompiler();
  Event::Tools::EventCompiler* eventCompiler();
  Actor::Tools::ActorTemplateCompiler* actorTemplateCompiler();

private:
  Treasure::Tools::TreasureCompiler* treasure_compiler_;
  Event::Tools::EventCompiler* event_compiler_;
  Actor::Tools::ActorTemplateCompiler* actor_template_compiler_;
  kg::gid::StructCollector<Evidyon::LifeformAI::LifeformAI> lifeforms_;
};



}
}
}

#endif