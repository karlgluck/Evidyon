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
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "shared/item/tools/item.h"
#include "shared/item/itemclientdescription.h"
#include "common/kg/gid/str.h"





//----[  Item  ]---------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Item::Tools::Item)
END_DEFINING_ACTIONS()


//----[  PreviewItemDescriptiveText  ]-----------------------------------------
int PreviewItemDescriptiveText(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::ItemElement, element);

  Evidyon::Item::ItemClientDescription client_description;
  element->compile(NULL, NULL, &client_description, NULL);

  MessageBox(tool->topWindow(), client_description.description, client_description.name, MB_OK);
  return 0;
}



//----[  AutomaticallyAssignItemScenery  ]-------------------------------------
int AutomaticallyAssignItemScenery(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Item::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Item>, list);
  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Automatically Assign Item Scenery",
        "Pick the items that need scenery assigned",
        tool->getItems(),
        &selection,
        false)) {
    return false;
  }

  const dc::dcGenericResource::Array& scenery = tool->getScenery();

  for (dc::dcGenericResource::Array::iterator i = selection.begin();
       i != selection.end(); ++i) {
    Evidyon::Tools::ItemElement* item =
      (Evidyon::Tools::ItemElement*)(*i);

    for (dc::dcGenericResource::Array::const_iterator t = scenery.begin();
         t != scenery.end(); ++t) {
      Evidyon::Tools::SceneryElement* scenery
       = (Evidyon::Tools::SceneryElement*)(*t);
      if (scenery->getName().find(item->getName()) != scenery->getName().npos) {
        item->setScenery(scenery);
      }
    }
  }

  return 1;
}



//----[  GetItemsWithoutScenery  ]---------------------------------------------
int GetItemsWithoutScenery(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Item::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Item>, list);

  std::string items = "";
  const dc::dcGenericResource::Array& members = list->getMembers();

  for (dc::dcGenericResource::Array::const_iterator i = members.begin();
       i != members.end(); ++i) {
    Evidyon::Tools::ItemElement* item =
      (Evidyon::Tools::ItemElement*)(*i);
    if (item->hasScenery()) continue;
    items += item->getName();
    items += "\n";
  }

  MessageBox(tool->topWindow(), items.c_str(), "Items Without Scenery", MB_OK);
  return 0;
}



//----[  AssignSceneryToItemsWithoutScenery  ]-----------------------------------
int AssignSceneryToItemsWithoutScenery(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Item::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Item>, list);

  const dc::dcGenericResource::Array& members = list->getMembers();

  for (dc::dcGenericResource::Array::const_iterator i = members.begin();
       i != members.end(); ++i) {
    Evidyon::Tools::ItemElement* item =
      (Evidyon::Tools::ItemElement*)(*i);
    if (item->hasScenery()) continue;

    dc::dcGenericResource* selection;
    if (!DirectCraftActions::PromptSelectSingleResource(
          tool->topWindow(),
          (std::string("Select Scenery for ") + item->getName()).c_str(),
          "Pick the scenery to reference",
          tool->getScenery(),
          &selection)) {
       break;
    }
    item->setScenery((Evidyon::Tools::SceneryElement*)selection);
  }
  return 1;
}


//----[  SelectItem  ]----------------------------------------------------------
int SelectItem(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::ItemReference, reference);

  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Item",
        "Pick the item to reference",
        tool->getItems(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}


//----[  ImportMasterDesignSpreadsheetItems  ]---------------------------------
int ImportMasterDesignSpreadsheetItems(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Item::Tools;
  BEGIN_ACTION_BODY();
  using namespace Evidyon::Tools;
  CAST_ACTION_RESOURCE(ItemList, list);

  char file[MAX_PATH];
  if (!dcx::dcxOpenFileDialog(
      tool->topWindow(),
      "Select Item Spreadsheet",
      "Tab-Delimited Spreadsheet File (*.TXT)\0*.TXT\0\0",
      file,
      MAX_PATH)) {
    return 0;
  }
  
  FILE* fp;
  fp = fopen(file, "rt");
  bool is_header_row = true;
  std::vector<std::string> headers;
  while (!feof(fp)) {
    char line[4096];
    fscanf_s(fp, "%[^\n]\n ", line, 4096);
    if (is_header_row) {
      split_str(line, '\t', headers);
      is_header_row = false;
    } else {
      std::vector<std::string> fields;
      split_str(line, '\t', fields);
      std::map<std::string, std::string> values;
      zip(headers, fields, values);
      const dc::dcGenericResource::Array& items = list->getMembers();
      dc::dcGenericResource::Array::const_iterator existing = items.begin();
      const std::string& name = values.find("Name")->second;
      if (name.empty() == false) {
        while (existing != items.end()) {
          if (0 == stricmp((*existing)->getName().c_str(), name.c_str())) break;
          ++existing;
        }
        ItemElement* item = (ItemElement*)(
          (existing != items.end()) ? (*existing) : list->generateElement(name));
        item->set(&values, tool->getRaces());
      }
    }
  }
  fclose(fp);
  return 1;
}

//----[  ItemList  ]-----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::ItemList)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Item...",
    &DirectCraftActions::GenerateCollectionElement<Evidyon::Tools::ItemList> },
{ "Automatically Assign Scenery", &AutomaticallyAssignItemScenery },
{ "Get Items Without Scenery", &GetItemsWithoutScenery },
{ "Assign Scenery To Items Without Scenery", &AssignSceneryToItemsWithoutScenery },
{ "ImportMasterDesignSpreadsheetItems", &ImportMasterDesignSpreadsheetItems },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::ItemElement)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Preview Descriptive Text", &PreviewItemDescriptiveText },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::ItemReference)
  { "Select...", &SelectItem },
  LIST_REFERENCE_ACTIONS(Evidyon::Item::Tools::Item),
END_DEFINING_ACTIONS()



//----[  ItemEquipmentType  ]--------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Item::ItemEquipmentType>)
  ENUM_ACTIONS(Evidyon::Item::ItemEquipmentType),
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Item::Tools::ItemIntendedUser>)
ENUM_ACTIONS(Evidyon::Item::Tools::ItemIntendedUser),
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Item::Tools::ItemIntendedUse>)
ENUM_ACTIONS(Evidyon::Item::Tools::ItemIntendedUse),
END_DEFINING_ACTIONS()

namespace Evidyon {
namespace Tools {
typedef dc::dcList<Evidyon::Tools::ItemReference> ItemReferenceList;
}
}


//----[  AddItemReference  ]----------------------------------------------------------
int AddItemReference(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::ItemReferenceList, list);

  Evidyon::Tools::ItemReferenceList::Element* reference =
    ((Evidyon::Tools::ItemReferenceList::Element*)list->generateElement("#"));
  if (SelectItem(reference, param)) {
    reference->setName(reference->getReferencedResource()->getName());
    return 1;
  }
  list->releaseElement(reference);
  return 0;
}

//----[  ItemReferenceList  ]-----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::ItemReferenceList)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Item...", &AddItemReference },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::ItemReferenceList::Element)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Select...", &SelectItem },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::ItemReferenceList::Reference)
END_DEFINING_ACTIONS()