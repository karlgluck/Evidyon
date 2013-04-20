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