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
#ifndef __ACTORTEMPLATETYPE_H__
#define __ACTORTEMPLATETYPE_H__
#pragma once



namespace Evidyon {
namespace Actor {



//----[  ActorTemplate  ]------------------------------------------------------
enum ActorTemplateType {

  // These template types are incomplete.  The client should pull a full
  // description from the server when an actor of this type is instantiated.
  AVATAR_TEMPLATE,
  MONSTER_TEMPLATE,

  // These template types fully specify the way the actor looks, so the client
  // doesn't need to request an update to the description from the server.
  STATIC_TEMPLATE,
  //STATIC_MONSTER_TEMPLATE,
  //GENERIC_NPC_TEMPLATE,
  //STORAGE_NPC_TEMPLATE,
  //BAZAAR_NPC_TEMPLATE,
  //GEOSID_NPC_TEMPLATE,
  //QUEST_NPC_TEMPLATE,
};


//----[  ActorTemplateTypeString  ]--------------------------------------------
// This shouldn't be needed
//const char* ActorTemplateTypeString(ActorTemplateType value);



}
}

#endif