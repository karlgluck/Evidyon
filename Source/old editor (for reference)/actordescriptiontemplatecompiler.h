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
