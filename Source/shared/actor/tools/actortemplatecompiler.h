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
#ifndef __SHARED_ACTOR_TOOLS_ACTORTEMPLATECOMPILER_H__
#define __SHARED_ACTOR_TOOLS_ACTORTEMPLATECOMPILER_H__
#pragma once

#include "shared/actor/actortemplateindex.h"
#include <vector>


namespace Evidyon {
namespace Actor {
struct ActorTemplate;
}
}

namespace Evidyon {
namespace Actor {
namespace Tools {


  
//----[  ActorTemplateCompiler  ]----------------------------------------------
// When the game is being compiled, this structure is used to store the list
// of actor templates (which is dynamic and defined implicitly rather than
// in a list somewhere).  This compiler gives templates an index that the
// server can use to reference that template on the client.
// The actor template index is embedded in an actor's ID number so that an
// actors with constant descriptions can be instantiated without a request
// description packet having to be sent.
class ActorTemplateCompiler {
  typedef std::vector<Actor::ActorTemplate*> ActorTemplates;
public:
  ~ActorTemplateCompiler();
  void clear();
  Actor::ActorTemplateIndex addTemplate(
    const Actor::ActorTemplate* actor_template);
  size_t numberOfTemplates() const;
  void compile(size_t template_index, Actor::ActorTemplate* actor_template);

private:
  ActorTemplates actor_templates_;
};




}
}
}

#endif