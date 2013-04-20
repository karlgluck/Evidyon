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
#include "shared/mesh/tools/mesh.h"
#include "shared/scenery/tools/scenery.h"
#include "shared/scenery/tools/scenerysubset.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "sceneryeditingdialog.h"


BEGIN_DEFINING_ACTIONS(Evidyon::Scenery::Tools::Scenery)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Scenery::Tools::ScenerySubset)
END_DEFINING_ACTIONS()





//----[  AddScenery  ]---------------------------------------------------------
int AddScenery(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Scenery::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Scenery>, list);

  // Let the user pick a set of meshes
  dc::dcGenericResource::Array selection;
  if (DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Scenery",
        "Pick meshes for which to create new scenery",
        tool->getMeshes(),
        &selection,
        false)) {

    tool->acquireMeshes();


    // Create scenery for each of these selections
    for (dc::dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i) {
      typedef dc::dcTable<Evidyon::Mesh::Tools::Mesh>::Element Element;
      Element* element = (Element*)(*i);
      Evidyon::Mesh::Tools::Mesh* mesh = element->getImplementation();

      Scenery* scenery
        = (Scenery*)list->generateElement(element->getName());

      size_t number_of_subsets = 
          tool->getMeshRenderer()->getNumberOfMeshSubsets(element->getIndex());
      scenery->setReferencedMesh(element,
                                 number_of_subsets);
    }
  }

  return 1;
}


//----[  SelectSceneryMesh  ]--------------------------------------------------
int SelectSceneryMesh(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Scenery::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Scenery>::Element, scenery);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = scenery->getReferencedMeshTableElement();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Mesh for Scenery",
        "Pick the mesh that this scenery should reference",
        tool->getMeshes(),
        &selection)) {
    tool->acquireMeshes();

    size_t number_of_subsets = 
        tool->getMeshRenderer()->getNumberOfMeshSubsets(selection->getIndex());
    scenery->setReferencedMesh(selection,
                               number_of_subsets);
  }

  return 1;
}



//----[  SelectSceneryReference  ]---------------------------------------------
int SelectSceneryReference(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Scenery::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Scenery>::Reference, reference);
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Scenery",
        "Pick the scenery to reference",
        tool->getScenery(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}




//----[  AddSceneryReferences  ]-----------------------------------------------
int AddSceneryReferences(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Scenery::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<dc::dcList<Scenery>::Reference>, list);
  dc::dcGenericResource::Array selection;
  if (DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Scenery References",
        "Pick the scenery to add to the list",
        tool->getScenery(),
        &selection,
        false)) {
    for (dc::dcGenericResource::Array::iterator i = selection.begin();
         i != selection.end(); ++i) {
      Evidyon::Tools::SceneryReference* scenery =
        (Evidyon::Tools::SceneryReference*)list->generateElement((*i)->getName());
      scenery->setGenericReferencedResource(*i);
    }
    return selection.empty() ? 0 : 1;
  }

  return 0;
}



//----[  EditScenery  ]--------------------------------------------------------
int EditScenery(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Scenery::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Scenery>, list);
  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Edit Scenery",
        "Pick the scenery to edit",
        tool->getScenery(),
        &selection,
        false)) {
    return false;
  }

  // Create the scenery dialog
  Evidyon::SceneryEditingDialog* scenery_editing_dialog
    = new Evidyon::SceneryEditingDialog();
  if (!scenery_editing_dialog->begin(tool, selection)) {
    delete scenery_editing_dialog;
    return 0;
  }

  // Dialog will be changing the scenery
  return 1;
}




//----[  AutomaticallyAssignSceneryTextures  ]---------------------------------
int AutomaticallyAssignSceneryTextures(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Scenery::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Scenery>, list);
  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Automatically Assign Scenery Textures",
        "Pick the scenery that needs textures assigned",
        tool->getScenery(),
        &selection,
        false)) {
    return false;
  }

  const dc::dcGenericResource::Array& textures = tool->getTextures();

  for (dc::dcGenericResource::Array::iterator i = selection.begin();
       i != selection.end(); ++i) {
    Evidyon::Tools::SceneryElement* scenery =
      (Evidyon::Tools::SceneryElement*)(*i);
    Evidyon::Scenery::Tools::ScenerySubset* subset = scenery->getSubset(0);
    if (!subset) continue;
    for (dc::dcGenericResource::Array::const_iterator t = textures.begin();
         t != textures.end(); ++t) {
      Evidyon::Tools::TextureElement* texture
       = (Evidyon::Tools::TextureElement*)(*t);
      if (texture->getName().find(scenery->getName()) != texture->getName().npos) {
        subset->setReferencedTexture(texture);
      }
    }
  }

  return 1;
}
//
//int ListSceneryWithMoreThanOneSubset(dc::dcGenericResource* resource, void* param) {
//
//  return 0;
//}

//----[  Scenery  ]------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Scenery::Tools::Scenery>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Scenery", &AddScenery },
  { "Edit Scenery", &EditScenery },
  { "Automatically Assign Textures...", &AutomaticallyAssignSceneryTextures },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Scenery::Tools::Scenery>::Element)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Select Mesh", &SelectSceneryMesh },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Scenery::Tools::Scenery>::Reference)
  { "Select...", &SelectSceneryReference },
  LIST_REFERENCE_ACTIONS(Evidyon::Scenery::Tools::Scenery),
END_DEFINING_ACTIONS()





BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Scenery::Tools::ScenerySubset>)
  COLLECTION_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Scenery::Tools::ScenerySubset>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Scenery::Tools::ScenerySubset>::Reference)
  LIST_REFERENCE_ACTIONS(Evidyon::Scenery::Tools::ScenerySubset),
END_DEFINING_ACTIONS()



BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::SceneryReference>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Scenery...", &AddSceneryReferences },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::SceneryReference>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::SceneryReference>::Reference)
END_DEFINING_ACTIONS()
