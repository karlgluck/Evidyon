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
#ifndef __SHARED_LIFEFORMAI_TOOLS_LIFEFORMAI_H__
#define __SHARED_LIFEFORMAI_TOOLS_LIFEFORMAI_H__
#pragma once

// needed for preview()... TODO: can we typedef these?
#include "shared/actor/tools/actorprofile.h"
#include "shared/skinned-mesh/tools/skinnedmeshform.h"

namespace Evidyon {
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
namespace LifeformAI {
struct LifeformAI;
}
namespace Treasure {
namespace Tools {
class MonsterTreasure;
class TreasureCompiler;
}
}
}



namespace Evidyon {
namespace LifeformAI {
namespace Tools {


  
//----[  LifeformAI  ]---------------------------------------------------------
class LifeformAI : public dc::dcGenericResource {
public:
  static std::string staticTypeName() { return "LifeformAI"; }

public:
  // Writes out the actor templates for the actors this lifeform AI uses as
  // well as the description of the lifeform for the server.
  virtual bool compile(
    Evidyon::Treasure::Tools::TreasureCompiler* treasure_compiler,
    Evidyon::Event::Tools::EventCompiler* event_compiler,
    Actor::Tools::ActorTemplateCompiler* actor_template_compiler,
    Evidyon::LifeformAI::LifeformAI* description) = 0;

  // Gets an actor profile and form that can represent a spawn-point for this
  // lifeform on the map.  Generally, the implementation can just pick from
  // any of its profiles & forms.  The returned values do not have to be
  // constant call-to-call; in fact, a better representation may be achieved if
  // the preview rotates between all variations of the lifeform.
  virtual bool preview(
    Evidyon::Tools::ActorProfileElement** actor_profile,
    Evidyon::Tools::SkinnedMeshFormElement** form) = 0;

  // If this AI is a monster, this method will return its treasure structure
  // and the avatar level at which the monster is designed to be hunted.
  virtual bool getMonsterTreasure(
    Treasure::Tools::MonsterTreasure** treasure,
    int* intended_level);
};

}
}
}

#include <dc/table>

namespace Evidyon {
namespace Tools {

  
//----[  LifeformAIElement  ]--------------------------------------------------
typedef dc::dcTable<Evidyon::LifeformAI::Tools::LifeformAI>::Element
  LifeformAIElement;


//----[  LifeformAIReference  ]------------------------------------------------
typedef dc::dcTable<Evidyon::LifeformAI::Tools::LifeformAI>::Reference
  LifeformAIReference;


}
}


#endif