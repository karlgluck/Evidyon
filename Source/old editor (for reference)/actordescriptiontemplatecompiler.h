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
#ifndef __ACTORDESCRIPTIONTEMPLATECOMPILER_H__
#define __ACTORDESCRIPTIONTEMPLATECOMPILER_H__
#pragma once


#include "../shared/actor/actordescriptiontemplate.h"

namespace dc {
class dcStreamOut;
}

#include <vector>

class ActorDescriptionTemplateCompiler {
  typedef std::vector<Evidyon::ClientActorDescriptionTemplate> ClientDescriptions;
  typedef std::vector<Evidyon::ServerActorDescriptionTemplate> ServerDescriptions;
public:

  // Resets all of the descriptions, then adds the default description:  index 0 is always
  // a blank description with default entries that indicates "get this data from the server"
  // unnecessary?
  //void initialize();

  // Appends a new description template
  void addActorDescriptionTemplate(const Evidyon::ClientActorDescriptionTemplate* client_description,
                                   const Evidyon::ServerActorDescriptionTemplate* server_description);

  // Returns the array index at which the next call to addActorDescriptionTemplate will put
  // its parameter.  This is used to reference the description.
  int getNextTemplateIndex() const;

  // writes out these descriptions into the stream
  bool compileForClient(dc::dcStreamOut* stream);
  bool compileForServer(dc::dcStreamOut* stream);


private:
  ClientDescriptions client_descriptions_;
  ServerDescriptions server_descriptions_;
};



#endif
