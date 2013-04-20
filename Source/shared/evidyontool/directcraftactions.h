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
#ifndef __DIRECTCRAFTACTIONS_H__
#define __DIRECTCRAFTACTIONS_H__
#pragma once


#include <dc/list>
#include <dcx/win32>

#define BEGIN_DEFINING_ACTIONS(resource_class) \
  dc::dcAction dc::dcClass<resource_class>::actions[] = {


#define ACTION_LIST_SEPARATOR   { "-", NULL }

#define COLLECTION_ELEMENT_ACTIONS \
  { "Edit Name...", &DirectCraftActions::EditResourceName }, \
  { "Copy Members...", &DirectCraftActions::CopyCollectionElementMembers }

#define COLLECTION_ACTIONS \
  { "Sort", &DirectCraftActions::SortCollection }, \
  { "Sort by Name", &DirectCraftActions::SortCollectionByName }, \
  { "Bulk Rename...", &DirectCraftActions::BulkRename }, \
  { "Find/Replace in Names...", &DirectCraftActions::FindReplaceInNames }, \
  /*{ "Merge Duplicates", &DirectCraftActions::MergeDuplicatesInCollection },*/ \
  { "Duplicate Entries...", &DirectCraftActions::DuplicateCollectionEntries }, \
  { "Delete Entries...", &DirectCraftActions::DeleteCollectionEntries }

#define REFERENCE_ACTIONS(TYPE) \
  /*{ "Select...", &DirectCraftActions::SelectReference<dcTable<TYPE>::Reference> },*/\
  { "Clear", &DirectCraftActions::ClearReference }

#define LIST_REFERENCE_ACTIONS(LIST_TYPE) \
  { "Clear", &DirectCraftActions::ClearReference }


#define ENUM_ACTIONS(ENUM_TYPE) \
  { "Set...", &DirectCraftActions::EditEnumValue<dc::dcEnum<ENUM_TYPE>> }

#define END_DEFINING_ACTIONS() { 0, NULL } };

namespace DirectCraftActions {





// candidate for DCX
// Allows the user to pick a single resource from a list and returns the result
bool PromptSelectSingleResource(HWND parent,
                                const char* caption,
                                const char* prompt,
                                const dc::dcGenericResource::Array& resourceOptions,
                                dc::dcGenericResource** returnedSelection);

// candidate for DCX
// Allows the user to pick several resources from a list and returns the result
bool PromptSelectMultipleResources(HWND parent,
                                   const char* caption,
                                   const char* prompt,
                                   const dc::dcGenericResource::Array& resourceOptions,
                                   dc::dcGenericResource::Array* selection,
                                   bool initializeSelection);

  
//----[  Generic (Non-Templated) Actions  ]------------------------------------
int EditResourceName(dc::dcGenericResource* resource, void* param);
int CopyCollectionElementMembers(dc::dcGenericResource* resource, void* param);
int SortCollection(dc::dcGenericResource* resource, void* param);
int SortCollectionByName(dc::dcGenericResource* resource, void* param);
int ClearReference(dc::dcGenericResource* resource, void* param);
int BulkRename(dc::dcGenericResource* resource, void* param);
int FindReplaceInNames(dc::dcGenericResource* resource, void* param);
int DuplicateCollectionEntries(dc::dcGenericResource* resource, void* param);
int DeleteCollectionEntries(dc::dcGenericResource* resource, void* param);



//----[  GenerateCollectionElement  ]------------------------------------------
// Creates a new member in the collection of type T
template <typename T>
int GenerateCollectionElement(dc::dcGenericResource* resource,
                              void* param) {
  ::Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<::Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();
  T* collection = (T*)resource;

  std::string name = "";
  if (dcx::dcxWin32PromptTextInput(window, 
                                   "Resource Name?",
                                   "Please enter a name for this resource:",
                                   &name)) {
    dc::dcGenericResource* element = collection->generateElement(std::string("##") + name.c_str());
    collection->sortByName();
    element->setName(name, false);
    return 1;
  }

  return 0;
}



//----[  GenerateTableElement  ]-----------------------------------------------
// Creates a new member in the table with interface type I of element type E
template <typename I, typename E>
int GenerateTableElement(dc::dcGenericResource* resource,
                         void* param) {
  ::Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<::Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();
  dc::dcTable<I>* collection = (dc::dcTable<I>*)resource;

  std::string name = "";
  if (dcx::dcxWin32PromptTextInput(window, 
                                   "Resource Name?",
                                   "Please enter a name for this resource:",
                                   &name)) {
    dc::dcGenericResource* element
      = collection->create<E>(std::string("##") + name.c_str());
    collection->sortByName();
    element->getOwner()->setName(name, false);
    return 1;
  }

  return 0;
}



//----[  EditEnumValue  ]------------------------------------------------------
//  Edits an enumeration by displaying the string equivalents of the indices in a list
//  and allowing the user to choose among them.  This method makes the assumption that
//  the enumeration begins with a value of zero for the first element, the indices in
//  the enum are contiguous, and that the dc::dcEnum<{enum}> type (the T parameter) will
//  return 'null' from its getString method for the first integer value which does
//  not map to a valid enum type.
//  TODO: it might be better to pass the enum max value as a constant...somehow...?
template <typename T> int EditEnumValue(dc::dcGenericResource* resource,
                                        void* param) {
  ::Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<::Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  // Get the enum type
  T* enumResource = (T*)resource;

  // Build a prompt and caption
  std::string caption = "Edit " + resource->getTypeName();
  std::string prompt = "Select '" + resource->getName() + "' value:";

  // Add entries to the options table
  std::list<std::string> options;
  const char* stringValue = T::getString((T::BasicEnumType)0);
  int index = 1;
  for (index = 1; stringValue != 0; ++index)
  {
    options.push_back(stringValue);
    stringValue = T::getString((T::BasicEnumType)index);
  }

  // Set the current selection
  int selection = (int)enumResource->getValue();
  if (selection >= index) selection = -1;

  // Let the user pick the value
  if (dcx::dcxWin32PromptForSelection(window, caption.c_str(), prompt.c_str(),
                                  &options, &selection, selection >= 0))
  {
      enumResource->setValue((T::BasicEnumType)selection);
      return 1;
  }
  else
      return 0;
}



}



#endif