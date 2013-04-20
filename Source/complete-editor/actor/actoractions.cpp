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
#include "shared/actor/tools/actorprofile.h"
#include "shared/actor/tools/actorprofilespecialization.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "actoreditingdialog.h"
#include "shared/actor/tools/actorformreference.h"
#include "shared/actor/actoraction.h"
#include "shared/actor/actorprofilespecializationtype.h"



//----[  ActorAction  ]--------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Actor::ActorAction>)
  ENUM_ACTIONS(Evidyon::Actor::ActorAction),
END_DEFINING_ACTIONS()


//----[  ActorAction  ]--------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Actor::ActorProfileSpecializationType>)
  ENUM_ACTIONS(Evidyon::Actor::ActorProfileSpecializationType),
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::Actor::Tools::ActorProfile)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::Actor::Tools::ActorProfileSpecialization)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::SkinnedMesh::SkinnedMeshAttachmentPoint>)
ENUM_ACTIONS(Evidyon::SkinnedMesh::SkinnedMeshAttachmentPoint),
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::Actor::Tools::ActorFormReference)
END_DEFINING_ACTIONS()



//----[  AddActorProfiles  ]---------------------------------------------------
int AddActorProfiles(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Actor::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<ActorProfile>, list);

  // Let the user pick a set of source animated meshes
  dc::dcGenericResource::Array selection;
  if (DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Actor Profiles",
        "Pick skinned meshes for which to create new actor profiles",
        tool->getSkinnedMeshes(),
        &selection,
        false)) {

    // Create skinned meshes for each of the selected elements
    for (dc::dcGenericResource::Array::iterator i = selection.begin();
                                                i != selection.end(); ++i) {
      Evidyon::Tools::SkinnedMeshElement* element
        = (Evidyon::Tools::SkinnedMeshElement*)(*i);

      ActorProfile* actor_profile
        = (ActorProfile*)list->generateElement(element->getName());

      actor_profile->setSkinnedMesh(element);
    }

    return 1;
  }

  return 0;
}




//----[  EditActorProfiles  ]--------------------------------------------------
int EditActorProfiles(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Scenery::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Scenery>, list);
  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Edit Actor Profiles",
        "Pick the actor profiles to edit",
        tool->getActorProfiles(),
        &selection,
        false)) {
    return false;
  }

  // Create the scenery dialog
  Evidyon::ActorEditingDialog* actor_profile_editing_dialog
    = new Evidyon::ActorEditingDialog();
  if (!actor_profile_editing_dialog->begin(tool, selection)) {
    delete actor_profile_editing_dialog;
    return 0;
  }

  // Dialog will be changing this skinned mesh array
  return 1;
}



//----[  SelectActorProfile  ]-------------------------------------------------
int SelectActorProfile(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::ActorProfileReference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Actor Profile",
        "Pick the actor profile to reference",
        tool->getActorProfiles(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}


//----[  dcList<ActorProfile>  ]------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Actor::Tools::ActorProfile>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Actor Profiles...", &AddActorProfiles },
  { "Edit...", &EditActorProfiles },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::ActorProfileElement)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::ActorProfileReference)
  { "Select...", &SelectActorProfile },
  LIST_REFERENCE_ACTIONS(Evidyon::Actor::Tools::ActorProfile),
END_DEFINING_ACTIONS()
