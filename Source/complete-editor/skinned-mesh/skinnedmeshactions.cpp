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
#include "shared/skinned-mesh/tools/skinnedmeshform.h"
#include "shared/skinned-mesh/tools/skinnedmeshattachmentpointoffset.h"
#include "shared/skinned-mesh/tools/skinnedmesh.h"
#include "shared/skinned-mesh/tools/skinnedmeshanimation.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "skinnedmesheditingdialog.h"




//----[  SkinnedMeshForm  ]--------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::SkinnedMesh::Tools::SkinnedMeshForm)
END_DEFINING_ACTIONS()





//----[  AddSkinnedMeshForms  ]------------------------------------------------
int AddSkinnedMeshForms(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::SkinnedMesh::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<SkinnedMeshForm>, list);

  // Let the user pick a set of source animated meshes
  dc::dcGenericResource::Array selection;
  if (DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Skinned Mesh Forms",
        "Pick animated meshes for which to create new skinned mesh forms",
        tool->getAnimatedMeshes(),
        &selection,
        false)) {

    // Create skinned meshes for each of the selected elements
    for (dc::dcGenericResource::Array::iterator i = selection.begin();
                                                i != selection.end(); ++i) {
      Evidyon::Tools::AnimatedMeshElement* element
        = (Evidyon::Tools::AnimatedMeshElement*)(*i);

      SkinnedMeshForm* skinned_mesh
        = (SkinnedMeshForm*)list->generateElement(element->getName());

      skinned_mesh->setAnimatedMeshReference(element);
    }

    return 1;
  }

  return 0;
}





//----[  SelectSkinnedMeshFormReference  ]-------------------------------------
#include "shared/lifeformai/tools/lifeformai.h"
#include "shared/lifeformai/tools/lifeformai_simplemeleemonster.h"
int SelectSkinnedMeshFormReference(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::SkinnedMesh::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<SkinnedMeshAnimation>::Reference, reference);

  // Let the user pick a source form from the skinned mesh the
  // owner of this structure references
  SkinnedMesh* skinned_mesh = NULL;
  dc::dcGenericResource* owner = reference->getOwner();
  if (owner->getTypeID() == dc::dcClass<Evidyon::Actor::Tools::ActorProfileSpecialization>::staticResourceManager()->uniqueID()) {
    skinned_mesh = ((Evidyon::Actor::Tools::ActorProfileSpecialization*)owner)->getSkinnedMesh();
  } else if (owner->getTypeID() == dc::dcClass<dc::dcList<Evidyon::Actor::Tools::ActorProfile>::Element>::staticResourceManager()->uniqueID()) {
    skinned_mesh = ((Evidyon::Actor::Tools::ActorProfile*)owner)->getSkinnedMesh();
  } else if (owner->getTypeID() == dc::dcClass<Evidyon::Actor::Tools::ActorFormReference>::staticResourceManager()->uniqueID()) {
    Evidyon::Actor::Tools::ActorProfile* actor_profile
      = ((Evidyon::Actor::Tools::ActorFormReference*)owner)->getActorProfile()->getReferencedResource();
    skinned_mesh = actor_profile ? actor_profile->getSkinnedMesh() : NULL;
  }
  if (!skinned_mesh) {
    MessageBox(tool->topWindow(),
               "Select a skinned mesh first",
               "Can't pick form",
               MB_OK);
    return 0;
  }
  dc::dcGenericResource* selection = reference->getReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Animation",
        "Pick the form to reference",
        skinned_mesh->forms(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}



//----[  dcList<SkinnedMeshForm>  ]------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshForm>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Skinned Mesh Forms...", &AddSkinnedMeshForms },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshForm>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshForm>::Reference)
  { "Select...", &SelectSkinnedMeshFormReference },
  LIST_REFERENCE_ACTIONS(Evidyon::SkinnedMesh::Tools::SkinnedMeshForm),
END_DEFINING_ACTIONS()



//----[  SkinnedMesh  ]---------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::SkinnedMesh::Tools::SkinnedMesh)
END_DEFINING_ACTIONS()



//----[  EditSkinnedMeshes  ]--------------------------------------------------
int EditSkinnedMeshes(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Scenery::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Scenery>, list);
  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Edit Skinned Meshes",
        "Pick the skinned meshes to edit",
        tool->getSkinnedMeshes(),
        &selection,
        false)) {
    return false;
  }

  // Create the skinned mesh dialog
  Evidyon::SkinnedMeshEditingDialog* skinned_mesh_editing_dialog
    = new Evidyon::SkinnedMeshEditingDialog();
  if (!skinned_mesh_editing_dialog->begin(tool, selection)) {
    delete skinned_mesh_editing_dialog;
    return 0;
  }

  // Dialog will be changing this skinned mesh array
  return 1;
}



//----[  SelectSkinnedMeshReference  ]-----------------------------------------
int SelectSkinnedMeshReference(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::SkinnedMesh::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<SkinnedMesh>::Reference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Skinned Mesh",
        "Pick the skinned mesh to reference",
        tool->getSkinnedMeshes(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}


//----[  dcList<SkinnedMeshAnimation>  ]---------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMesh>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Skinned Mesh",
    &DirectCraftActions::GenerateCollectionElement<dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMesh>> },
  ACTION_LIST_SEPARATOR,
  { "Edit...", &EditSkinnedMeshes },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMesh>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMesh>::Reference)
  { "Select...", &SelectSkinnedMeshReference },
  LIST_REFERENCE_ACTIONS(Evidyon::SkinnedMesh::Tools::SkinnedMesh),
END_DEFINING_ACTIONS()


//----[  SkinnedMeshAttachmentPointOffset  ]-----------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::SkinnedMesh::Tools::SkinnedMeshAttachmentPointOffset)
END_DEFINING_ACTIONS()



//----[  SkinnedMeshAnimation  ]-----------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::SkinnedMesh::Tools::SkinnedMeshAnimation)
END_DEFINING_ACTIONS()




//----[  AddSkinnedMeshAnimations  ]-------------------------------------------
int AddSkinnedMeshAnimations(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::SkinnedMesh::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<SkinnedMeshAnimation>, list);

  // Let the user pick a set of source animated meshes
  dc::dcGenericResource::Array selection;
  if (DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Skinned Mesh Animations",
        "Pick skeletal animations for which to create new skinned mesh animations",
        tool->getSkeletalAnimations(),
        &selection,
        false)) {

    // Create skinned meshes for each of the selected elements
    for (dc::dcGenericResource::Array::iterator i = selection.begin();
                                                i != selection.end(); ++i) {
      Evidyon::Tools::SkeletalAnimationElement* element
        = (Evidyon::Tools::SkeletalAnimationElement*)(*i);

      SkinnedMeshAnimation* skinned_mesh_animation
        = (SkinnedMeshAnimation*)list->generateElement(element->getName());
      skinned_mesh_animation->setSkeletalAnimationReference(element);
    }

    return 1;
  }

  return 0;
}

#include "shared/actor/tools/actorprofile.h"
#include "shared/actor/tools/actorprofilespecialization.h"

//----[  SelectSkinnedMeshAnimationReference  ]--------------------------------
int SelectSkinnedMeshAnimationReference(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::SkinnedMesh::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<SkinnedMeshAnimation>::Reference, reference);

  // Let the user pick a source animation from the skinned mesh the
  // owner of this structure references
  SkinnedMesh* skinned_mesh = NULL;
  dc::dcGenericResource* owner = reference->getOwner();
  if (owner->getTypeID() == dc::dcClass<Evidyon::Actor::Tools::ActorProfileSpecialization>::staticResourceManager()->uniqueID()) {
    skinned_mesh = ((Evidyon::Actor::Tools::ActorProfileSpecialization*)owner)->getSkinnedMesh();
  } else if (owner->getTypeID() == dc::dcClass<dc::dcList<Evidyon::Actor::Tools::ActorProfile>::Element>::staticResourceManager()->uniqueID()) {
    skinned_mesh = ((Evidyon::Actor::Tools::ActorProfile*)owner)->getSkinnedMesh();
  }

  if (!skinned_mesh) {
    MessageBox(tool->topWindow(),
               "Select a skinned mesh first",
               "Can't pick animation",
               MB_OK);
    return 0;
  }
  dc::dcGenericResource* selection = reference->getReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Animation",
        "Pick the animation to reference",
        skinned_mesh->animations(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}

//----[  dcList<SkinnedMeshAnimation>  ]---------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshAnimation>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Animations...", &AddSkinnedMeshAnimations },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshAnimation>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshAnimation>::Reference)
  { "Select...", &SelectSkinnedMeshAnimationReference },
  LIST_REFERENCE_ACTIONS(Evidyon::SkinnedMesh::Tools::SkinnedMeshAnimation),
END_DEFINING_ACTIONS()