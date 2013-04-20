//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#include "evidyonquesttool.h"


int SelectItemReference(dc::dcGenericResource*, void*);
int SelectAtlasReference(dc::dcGenericResource*, void*);
int SelectLifeformAIReference(dc::dcGenericResource*, void*);

BEGIN_DEFINING_ACTIONS(Item)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Atlas)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<Item>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Item>::Element)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Item>::Reference)
  { "Select...", &SelectItemReference },
  { "Clear", &DirectCraftActions::ClearReference },
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<Atlas>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Atlas>::Element)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Atlas>::Reference)
  { "Select...", &SelectAtlasReference },
  { "Clear", &DirectCraftActions::ClearReference },
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(LifeformAI)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcInterface<LifeformAI>)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcTable<LifeformAI>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<LifeformAI>::Element)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<LifeformAI>::Reference)
  { "Select", &SelectLifeformAIReference },
  { "Clear", &DirectCraftActions::ClearReference },
END_DEFINING_ACTIONS()









//----[  AddQuestItemReference  ]----------------------------------------------
int AddQuestItemReference(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  QuestTool* tool = reinterpret_cast<QuestTool*>(dialog->getTool());
  HWND window = dialog->getWindowHandle();

  dc::dcGenericCollection* collection = 
    (dc::dcGenericCollection*)resource;

  dc::dcGenericResource::Array selection;
  const dc::dcGenericResource::Array& items = tool->getItems()->getMembers();
  if (DirectCraftActions::PromptSelectMultipleResources(
        dialog->getWindowHandle(),
        "Pick an Item",
        "",
        items,
        &selection,
        false)) {

    dc::dcGenericResource::Array::const_iterator i;
    for (i = selection.begin(); i != selection.end(); ++i) {

      dc::dcGenericResource* selected_resource = *i;
      dc::dcList<QuestItemReference>::Element* element =
        (dc::dcList<QuestItemReference>::Element*)(collection
          ->generateElement(selected_resource->getName()));

      std::string path_string("/Volucris/Items/");
      path_string.append(selected_resource->getName());

      std::string reference_string = "/Volucris/" +
        tool->translateQuestMemberToRealPath(
          element->getItemReference()->getPathString());

      element
          ->getItemReference()
          ->setGenericReferencedResourceByPathString(reference_string,
                                                     path_string);
    }

    return 1;
  }

  return 0;
}



//----[  dcList<QuestItemReference>  ]-----------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<QuestItemReference>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPERATOR,
  { "Add Item", &AddQuestItemReference },
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<QuestItemReference>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<QuestItemReference>::Reference)
END_DEFINING_ACTIONS()





//----[  SelectItemReference  ]------------------------------------------------
int SelectItemReference(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  QuestTool* tool = reinterpret_cast<QuestTool*>(dialog->getTool());
  HWND window = dialog->getWindowHandle();

  dc::dcList<Item>::Reference* reference =
    (dc::dcList<Item>::Reference*)resource;

  dc::dcGenericResource* selection = NULL;
  const dc::dcGenericResource::Array& items = tool->getItems()->getMembers();
  if (DirectCraftActions::PromptSelectSingleResource(dialog->getWindowHandle(),
                                                     "Pick an Item",
                                                     "",
                                                     items,
                                                     &selection)) {
    std::string path_string("Volucris/Items/");
    path_string.append(selection->getName());

    std::string reference_string = "Volucris/" +
      tool->translateQuestMemberToRealPath(resource->getPathString());

    reference->setGenericReferencedResourceByPathString(
      reference_string,
      path_string);

    return 1;
  }

  return 0;
}





//----[  SelectAtlasReference  ]-----------------------------------------------
int SelectAtlasReference(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  QuestTool* tool = reinterpret_cast<QuestTool*>(dialog->getTool());
  HWND window = dialog->getWindowHandle();

  dc::dcList<Item>::Reference* reference =
    (dc::dcList<Item>::Reference*)resource;

  dc::dcGenericResource* selection = NULL;
  const dc::dcGenericResource::Array& items = tool->getAtlases()->getMembers();
  if (DirectCraftActions::PromptSelectSingleResource(dialog->getWindowHandle(),
                                                     "Pick an Atlas",
                                                     "",
                                                     items,
                                                     &selection)) {
    std::string path_string("Volucris/World Atlases/");
    path_string.append(selection->getName());

    std::string reference_string = "Volucris/" +
      tool->translateQuestMemberToRealPath(resource->getPathString());

    reference->setGenericReferencedResourceByPathString(
      reference_string,
      path_string);

    return 1;
  }

  return 0;
}




//----[  SelectQuestReference  ]-----------------------------------------------
int SelectQuestReference(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  QuestTool* tool = reinterpret_cast<QuestTool*>(dialog->getTool());
  HWND window = dialog->getWindowHandle();

  dc::dcList<Quest>::Reference* reference =
    (dc::dcList<Quest>::Reference*)resource;
    Quest* quest = reference->getReferencedResource();

  dc::dcGenericResource* selection = NULL;
  const dc::dcGenericResource::Array& items = tool->getQuests()->getMembers();
  if (DirectCraftActions::PromptSelectSingleResource(dialog->getWindowHandle(),
                                                     "Pick a Quest",
                                                     "",
                                                     items,
                                                     &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}



//----[  SelectLifeformAIReference  ]------------------------------------------
int SelectLifeformAIReference(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  QuestTool* tool = reinterpret_cast<QuestTool*>(dialog->getTool());
  HWND window = dialog->getWindowHandle();

  dc::dcTable<LifeformAI>::Reference* reference =
    (dc::dcTable<LifeformAI>::Reference*)resource;

  dc::dcGenericResource* selection = NULL;
  dc::dcGenericCollection* collection = tool->getLifeforms();
  const dc::dcGenericResource::Array& items = collection->getMembers();
  if (DirectCraftActions::PromptSelectSingleResource(dialog->getWindowHandle(),
                                                     "Pick a Lifeform",
                                                     "",
                                                     items,
                                                     &selection)) {
    std::string path_string("Volucris/");
    path_string.append(collection->getName());
    path_string.append("/");
    path_string.append(selection->getName());

    std::string reference_string = "Volucris/" +
      tool->translateQuestMemberToRealPath(resource->getPathString());

    reference->setGenericReferencedResourceByPathString(
      reference_string,
      path_string);

    return 1;
  }

  return 0;
}



//----[  dcList<Quest>  ]------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Quest>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPERATOR,
  { "Add Quest", &DirectCraftActions::GenerateCollectionElement<dc::dcList<Quest>> },
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<Quest>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<Quest>::Reference)
  { "Select", &SelectQuestReference },
  { "Clear", &DirectCraftActions::ClearReference },
END_DEFINING_ACTIONS()








BEGIN_DEFINING_ACTIONS(QuestTool)
END_DEFINING_ACTIONS()



//----[  WinMain  ]------------------------------------------------------------
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int) {
  QuestTool quest_tool;
  return quest_tool.run(instance);
}