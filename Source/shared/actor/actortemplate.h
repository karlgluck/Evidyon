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