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
#include "shared/sound/tools/wavsound.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "shared/lifeformai/tools/lifeformai.h"
#include "shared/lifeformai/tools/lifeformai_simplemeleemonster.h"
#include "shared/lifeformai/tools/lifeformai_interactivenpc.h"
#include "shared/lifeformai/tools/lifeformai_simplemeleetownguard.h"
#include "shared/lifeformai/tools/lifeformai_geosidguardian.h"
#include "shared/lifeformai/tools/lifeformai_projectilemonster.h"
#include "shared/lifeformai/tools/lifeformai_directdamagemonster.h"

#include "shared/treasure/tools/monstertreasure.h"


BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::LifeformAI::LifeformAI_InteractiveNPC::Type>)
  ENUM_ACTIONS(Evidyon::LifeformAI::LifeformAI_InteractiveNPC::Type),
END_DEFINING_ACTIONS()



//----[  LifeformAI_SimpleMeleeMonster  ]--------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::LifeformAI::Tools::LifeformAI_SimpleMeleeMonster)
END_DEFINING_ACTIONS()



//----[  LifeformAI_InteractiveNPC  ]------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::LifeformAI::Tools::LifeformAI_InteractiveNPC)
END_DEFINING_ACTIONS()


//----[  LifeformAI_SimpleMeleeTownGuard  ]------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::LifeformAI::Tools::LifeformAI_SimpleMeleeTownGuard)
END_DEFINING_ACTIONS()



BEGIN_DEFINING_ACTIONS(Evidyon::LifeformAI::Tools::LifeformAI_GeosidGuardian)
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::LifeformAI::Tools::LifeformAI_ProjectileMonster)
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::LifeformAI::Tools::LifeformAI_DirectDamageMonster)
END_DEFINING_ACTIONS()





//----[  CreateLifeformAITableEntry  ]-----------------------------------------
template <typename T>
int CreateLifeformAITableEntry(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::LifeformAI::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<LifeformAI>, table);
  std::string name = "";
  if (!dcx::dcxWin32PromptTextInput(dialog->getWindowHandle(), 
                                   "Resource Name?",
                                   "Please enter a name for the lifeform:",
                                   &name)) {
    return 0;
  }
  T* entry = (T*)table->create<T>(name);
  return 1;
}




//----[  AutomaticallyAssignLifeformAITreasure  ]------------------------------
int AutomaticallyAssignLifeformAITreasure(dc::dcGenericResource* resource,
                                          void* param) {
  static const char* TITLE = "Automatically Assign Treasure";
  using namespace Evidyon::LifeformAI::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<LifeformAI>, table);

  dc::dcGenericResource::Array lifeforms;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        TITLE,
        "Pick the lifeforms to which you want to give treasure",
        tool->getLifeformAIs(),
        &lifeforms,
        false)) {
    return 0;
  }

  dc::dcGenericResource::Array items;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        TITLE,
        "Pick the items for which level-appropriate treasure will be generated",
        tool->getItems(),
        &items,
        false)) {
    return 0;
  }


  bool clear;
  switch (dcx::dcxYesNoCancelDialog(tool->topWindow(),
                                    TITLE,
                                    "Clear existing lists?")) {
  case IDYES: clear = true; break;
  case IDNO: clear = false; break;
  case IDCANCEL: return 0;
  }

  // do it!
  for (dc::dcGenericResource::Array::iterator i = lifeforms.begin();
                                              i != lifeforms.end();
                                            ++i) {
    Evidyon::Tools::LifeformAIElement* element
      = (Evidyon::Tools::LifeformAIElement*)(*i);
    Evidyon::LifeformAI::Tools::LifeformAI* lifeform
      = element->getImplementation();
    Evidyon::Treasure::Tools::MonsterTreasure* treasure = NULL;
    int intended_level;
    if (lifeform->getMonsterTreasure(&treasure, &intended_level)) {
      treasure->automaticallyAssignTreasure(clear, intended_level, items);
    }
  }

  return 1;
}


//----[  CreateLifeformAI_SimpleMeleeMonster  ]--------------------------------
int CreateLifeformAI_SimpleMeleeMonster(dc::dcGenericResource* resource,
                                        void* param) {
  static const char* TITLE = "Create Simple Melee Monster";
  using namespace Evidyon::LifeformAI::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<LifeformAI>, table);

  // pick the source actor profile
  dc::dcGenericResource* generic_actor_profile = NULL;
  if (!DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        TITLE,
        "Pick the source actor for this monster",
        tool->getActorProfiles(),
        &generic_actor_profile)) {
    return 0;
  }
  Evidyon::Tools::ActorProfileElement* actor_profile
    = (Evidyon::Tools::ActorProfileElement*)generic_actor_profile;
  Evidyon::SkinnedMesh::Tools::SkinnedMesh* skinned_mesh
    = actor_profile->getSkinnedMesh();
  if (!skinned_mesh) {
    MessageBox(tool->topWindow(),
               TITLE,
               "The selected actor doesn't have a skinned mesh selected--go pick one first",
               MB_OK);
    return -1;
  }

  // choose actor forms from the profile's skinned mesh
  dc::dcGenericResource::Array selected_forms;
  if (!DirectCraftActions::PromptSelectMultipleResources(
      tool->topWindow(),
      TITLE,
      "Choose one or more forms; a monster will be created for each",
      skinned_mesh->forms(),
      &selected_forms,
      false)) {
    return -1;
  }

  std::string user_text = "";
  if (!dcx::dcxWin32PromptTextInput(
      dialog->getWindowHandle(), 
      TITLE,
      selected_forms.size() == 1 ? "Please enter a name for the monster:"
                                 : "Type a prefix for the names of the monsters:",
      &user_text)) {
    return 0;
  }

  // Create each of the new elements
  for (dc::dcGenericResource::Array::iterator i = selected_forms.begin();
                                              i != selected_forms.end();
                                            ++i) {
    Evidyon::Tools::SkinnedMeshFormElement* form
      = (Evidyon::Tools::SkinnedMeshFormElement*)(*i);
    std::string name;
    if (selected_forms.size() == 1) {
      name = user_text;
    } else {
      name = user_text;
      name.append(form->getName());
    }
    LifeformAI_SimpleMeleeMonster* element
      = table->create<LifeformAI_SimpleMeleeMonster>(name);
    element->getActorProfile()->setReferencedResource(actor_profile);
    element->getSkinnedMeshForm()->setReferencedResource(form);
  }

  return 1;
}



//----[  CreateLifeformAI_InteractiveNPC  ]------------------------------------
int CreateLifeformAI_InteractiveNPC(
    dc::dcGenericResource* resource, void* param) {
  static const char* TITLE = "Create Interactive NPC";
  using namespace Evidyon::LifeformAI::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<LifeformAI>, table);

  // pick the source actor profile
  dc::dcGenericResource* generic_actor_profile = NULL;
  if (!DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        TITLE,
        "Pick the source actor for this npc",
        tool->getActorProfiles(),
        &generic_actor_profile)) {
    return 0;
  }
  Evidyon::Tools::ActorProfileElement* actor_profile
    = (Evidyon::Tools::ActorProfileElement*)generic_actor_profile;
  Evidyon::SkinnedMesh::Tools::SkinnedMesh* skinned_mesh
    = actor_profile->getSkinnedMesh();
  if (!skinned_mesh) {
    MessageBox(tool->topWindow(),
               TITLE,
               "The selected actor doesn't have a skinned mesh selected--go pick one first",
               MB_OK);
    return -1;
  }

  // choose actor forms from the profile's skinned mesh
  dc::dcGenericResource* selected_form = NULL;
  if (!DirectCraftActions::PromptSelectSingleResource(
      tool->topWindow(),
      TITLE,
      "Choose a form for this npc:",
      skinned_mesh->forms(),
      &selected_form)) {
    return -1;
  }

  std::string name = "";
  if (!dcx::dcxWin32PromptTextInput(
      dialog->getWindowHandle(), 
      TITLE,
      "Please enter a name for the npc:",
      &name)) {
    return 0;
  }

  LifeformAI_InteractiveNPC* element
    = table->create<LifeformAI_InteractiveNPC>(name);
  element->getActorProfile()->setReferencedResource(actor_profile);
  element->getSkinnedMeshForm()->setReferencedResource(
    (Evidyon::Tools::SkinnedMeshFormElement*)(selected_form));

  return 1;
}




//----[  CreateLifeformAI_SimpleMeleeTownGuard ]------------------------------------
int CreateLifeformAI_SimpleMeleeTownGuard(
    dc::dcGenericResource* resource, void* param) {
  static const char* TITLE = "Create Simple Melee Town Guard NPC";
  using namespace Evidyon::LifeformAI::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<LifeformAI>, table);

  // pick the source actor profile
  dc::dcGenericResource* generic_actor_profile = NULL;
  if (!DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        TITLE,
        "Pick the source actor for this npc",
        tool->getActorProfiles(),
        &generic_actor_profile)) {
    return 0;
  }
  Evidyon::Tools::ActorProfileElement* actor_profile
    = (Evidyon::Tools::ActorProfileElement*)generic_actor_profile;
  Evidyon::SkinnedMesh::Tools::SkinnedMesh* skinned_mesh
    = actor_profile->getSkinnedMesh();
  if (!skinned_mesh) {
    MessageBox(tool->topWindow(),
               TITLE,
               "The selected actor doesn't have a skinned mesh selected--go pick one first",
               MB_OK);
    return -1;
  }

  // choose actor forms from the profile's skinned mesh
  dc::dcGenericResource* selected_form = NULL;
  if (!DirectCraftActions::PromptSelectSingleResource(
      tool->topWindow(),
      TITLE,
      "Choose a form for this npc:",
      skinned_mesh->forms(),
      &selected_form)) {
    return -1;
  }

  std::string name = "";
  if (!dcx::dcxWin32PromptTextInput(
      dialog->getWindowHandle(), 
      TITLE,
      "Please enter a name for the npc:",
      &name)) {
    return 0;
  }

  LifeformAI_SimpleMeleeTownGuard* element
    = table->create<LifeformAI_SimpleMeleeTownGuard>(name);
  element->getActorProfile()->setReferencedResource(actor_profile);
  element->getSkinnedMeshForm()->setReferencedResource(
    (Evidyon::Tools::SkinnedMeshFormElement*)(selected_form));

  return 1;
}




//----[  SelectLifeformAI  ]---------------------------------------------------
int SelectLifeformAI(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(LifeformAIReference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select LifeformAI",
        "Pick the lifeform to reference",
        tool->getLifeformAIs(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}



//----[  LifeformAI  ]---------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::LifeformAI::Tools::LifeformAI>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::LifeformAI::Tools::LifeformAI>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Create Simple Melee Monster",
    &CreateLifeformAI_SimpleMeleeMonster },
  { "Create Interactive NPC",
    &CreateLifeformAI_InteractiveNPC },
  { "Create Simple Melee Town Guard NPC",
    &CreateLifeformAI_SimpleMeleeTownGuard },
  { "Create Projectile Monster",
    &DirectCraftActions::GenerateTableElement<Evidyon::LifeformAI::Tools::LifeformAI, Evidyon::LifeformAI::Tools::LifeformAI_ProjectileMonster> },
  { "Create Direct Damage Monster",
    &DirectCraftActions::GenerateTableElement<Evidyon::LifeformAI::Tools::LifeformAI, Evidyon::LifeformAI::Tools::LifeformAI_DirectDamageMonster> },
  ACTION_LIST_SEPARATOR,
  { "Automatically Assign Treasure", &AutomaticallyAssignLifeformAITreasure },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::LifeformAI::Tools::LifeformAI>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::LifeformAI::Tools::LifeformAI>::Reference)
  { "Select...", &SelectLifeformAI },
  LIST_REFERENCE_ACTIONS(NULL),
END_DEFINING_ACTIONS()





//----[  AddEntryToLifeformAIReferenceList  ]----------------------------------
int AddEntryToLifeformAIReferenceList(dc::dcGenericResource* resource,
                                      void* param) {
  static const char* TITLE = "Add Lifeform AI Reference";
  using namespace Evidyon::LifeformAI::Tools;
  using namespace Evidyon::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<LifeformAIReference>, list);

  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
      tool->topWindow(),
      TITLE,
      "Select lifeforms to add:",
      tool->getLifeformAIs(),
      &selection,
      false)) {
    return 0;
  }

  // Create each of the new elements
  for (dc::dcGenericResource::Array::iterator i = selection.begin();
                                              i != selection.end();
                                            ++i) {
    Evidyon::Tools::LifeformAIElement* lifeform_ai
      = (Evidyon::Tools::LifeformAIElement*)(*i);
    typedef dc::dcList<LifeformAIReference>::Element Element;
    Element* element
      = (Element*)list->generateElement(lifeform_ai->getName());
    element->setReferencedResource(lifeform_ai);
  }

  return 1;
}



//----[  LifeformAI  ]---------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::LifeformAIReference>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add...",
    &AddEntryToLifeformAIReferenceList },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::LifeformAIReference>::Element)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::LifeformAIReference>::Reference)
END_DEFINING_ACTIONS()
