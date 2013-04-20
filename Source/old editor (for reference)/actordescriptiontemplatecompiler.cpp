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
#include "actordescriptiontemplatecompiler.h"
#include <dc/filestream>
#include "../shared/actor/actorid.h"

#include <dc/debug>


void ActorDescriptionTemplateCompiler::addActorDescriptionTemplate(
    const Evidyon::ClientActorDescriptionTemplate* client_description,
    const Evidyon::ServerActorDescriptionTemplate* server_description) {
  client_descriptions_.push_back(*client_description);
  server_descriptions_.push_back(*server_description);
}

int ActorDescriptionTemplateCompiler::getNextTemplateIndex() const {
  ASSERT(server_descriptions_.size() == client_descriptions_.size());
  return static_cast<int>(client_descriptions_.size());
}

bool ActorDescriptionTemplateCompiler::compileForClient(dc::dcStreamOut* stream) {
  ASSERT(server_descriptions_.size() == client_descriptions_.size()) else return false;
  {
    size_t elements = client_descriptions_.size();
    if (APP_FATAL(elements >= Evidyon::MAX_ACTOR_DESCRIPTION_TEMPLATES)
                 ("There are too many unique actor description templates to be correctly "\
                  "encoded into actors' ID numbers.  Increase MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS "\
                  "then rebuild Evidyon with more available templates.")) {
      return false;
    }
    CONFIRM(stream->write(&elements, sizeof(elements))) else return false;
  }
  {
    for (ClientDescriptions::iterator i = client_descriptions_.begin();
         i != client_descriptions_.end(); ++i) {
      const Evidyon::ClientActorDescriptionTemplate* description = &(*i);
      CONFIRM(stream->write(description,
                            sizeof(Evidyon::ClientActorDescriptionTemplate))) else return false;
    }
  }

  return true;
}

bool ActorDescriptionTemplateCompiler::compileForServer(dc::dcStreamOut* stream) {
  ASSERT(server_descriptions_.size() == client_descriptions_.size()) else return false;
  {
    size_t elements = server_descriptions_.size();
    if (APP_FATAL(elements >= Evidyon::MAX_ACTOR_DESCRIPTION_TEMPLATES)
                 ("There are too many unique actor description templates to be correctly "\
                  "encoded into actors' ID numbers.  Increase MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS "\
                  "then rebuild Evidyon with more available templates.")) {
      return false;
    }
    CONFIRM(stream->write(&elements, sizeof(elements))) else return false;
  }
  {
    for (ServerDescriptions::iterator i = server_descriptions_.begin();
         i != server_descriptions_.end(); ++i) {
      const Evidyon::ServerActorDescriptionTemplate* description = &(*i);
      CONFIRM(stream->write(description,
                            sizeof(Evidyon::ServerActorDescriptionTemplate))) else return false;
    }
  }

  return true;
}
