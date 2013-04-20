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
#include "actortemplatecompiler.h"
#include "common/safemacros.h"
#include "shared/actor/actortemplate.h"
#include <assert.h>

namespace Evidyon {
namespace Actor {
namespace Tools {





//----[  ~ActorTemplateCompiler  ]---------------------------------------------
ActorTemplateCompiler::~ActorTemplateCompiler() {
  clear();
}



//----[  clear  ]--------------------------------------------------------------
void ActorTemplateCompiler::clear() {
  for (ActorTemplates::iterator i = actor_templates_.begin();
        i != actor_templates_.end(); ++i) {
      Evidyon::Actor::ActorTemplate* actor_template = *i;
    SAFE_DELETE(actor_template);
  }
}
  
//----[  addTemplate  ]--------------------------------------------------------
Actor::ActorTemplateIndex ActorTemplateCompiler::addTemplate(
    const Actor::ActorTemplate* actor_template) {
  Actor::ActorTemplate* internal_template = new Actor::ActorTemplate;
  memcpy(internal_template, actor_template, sizeof(Actor::ActorTemplate));
  Actor::ActorTemplateIndex actor_template_index = actor_templates_.size();
  actor_templates_.push_back(internal_template);
  return actor_template_index;
}



//----[  numberOfTemplates  ]--------------------------------------------------
size_t ActorTemplateCompiler::numberOfTemplates() const {
  return actor_templates_.size();
}


//----[  compile  ]------------------------------------------------------------
void ActorTemplateCompiler::compile(size_t template_index,
                                    Actor::ActorTemplate* actor_template) {
  assert(template_index < actor_templates_.size());
  memcpy(actor_template,
         actor_templates_.at(template_index),
         sizeof(Actor::ActorTemplate));
}

}
}
}

