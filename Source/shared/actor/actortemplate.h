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
#ifndef __ACTORTEMPLATE_H__
#define __ACTORTEMPLATE_H__
#pragma once



#include "actortemplatetype.h"
#include "actorprofileindex.h"
#include "actoralignment.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"
#include "shared/scenery/sceneryindex.h"
#include "actorprofilespecializationtype.h"
#include "actoraction.h"
#include "specializedactorattachmentpoint.h"
#include "actorequipmentslot.h"
#include "actorinteraction.h"


namespace Evidyon {
namespace Actor {


//----[  ActorTemplate  ]------------------------------------------------------
// This structure is saved in the game file and referenced by actors in the
// game.  When an actor is instantiated, the actor manager creates a local
// copy of this template type inside of the instance and updates it as
// requested by messages passed from the server.
struct ActorTemplate {
  static const int MAX_NAME_LENGTH = 64;

  ActorTemplateType template_type;

  // What to do when a client clicks on this actor.  The default setting is
  // to attack.
  ActorInteraction interaction;

  char name[MAX_NAME_LENGTH];
  ActorProfileIndex actor_profile;
  ActorAlignment alignment;
  Evidyon::SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form;

  // Equipped items
  Evidyon::Scenery::SceneryIndex
    equipped_items[NUMBER_OF_ACTOR_EQUIPMENT_SLOTS];
  ActorProfileSpecializationType profile_specialization;

};



}
}

#endif