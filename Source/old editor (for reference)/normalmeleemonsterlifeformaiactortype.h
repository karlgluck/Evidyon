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
#ifndef __NORMALMELEEMONSTERLIFEFORMAIACTORTYPE_H__
#define __NORMALMELEEMONSTERLIFEFORMAIACTORTYPE_H__
#pragma once


namespace Evidyon {
struct ClientActorDescriptionTemplate;
struct ServerActorDescriptionTemplate;
struct AIMonsterActorType;
}

#include "enchantment.h"
#include <dc/list>
#include "actortype.h"



class NormalMeleeMonsterLifeformAIActorType : public dc::dcResource<NormalMeleeMonsterLifeformAIActorType> {
public:
  NormalMeleeMonsterLifeformAIActorType();
  bool compileForServer(Evidyon::AIMonsterActorType* description);
  bool compileActorTemplate(int template_index,
                            Evidyon::ClientActorDescriptionTemplate* client_description,
                            Evidyon::ServerActorDescriptionTemplate* server_description);

public:
  static std::string staticTypeName();

private:
  dc::dcList<ActorType>::Reference actor_type_;
  dc::dcList<dc::dcReference<Enchantment>> enchantments_;

  // when compiled for the client, this is the index of this actor type's description
  int template_index_;
};



#endif