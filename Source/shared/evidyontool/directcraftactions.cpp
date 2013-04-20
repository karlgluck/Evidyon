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
#include "directcraftactions.h"

#include <dc/string>
#include <dc/integer>
#include <dc/boolean>
#include <dc/list>

#include <dc/debug>

#include "evidyontooldialog.h"

#include <dc/bufferstream>
#include <dc/filename>
#include <dc/index16>
#include <dc/index32>
#include <dc/byte>
#include <dc/float>
#include <dc/interface>


namespace DirectCraftActions {


//----[  PromptSelectSingleResource  ]-----------------------------------------
bool PromptSelectSingleResource(HWND parent,
                                const char* caption, const char* prompt,
                                const dc::dcGenericResource::Array& resourceOptions,
                                dc::dcGenericResource** returnedSelection) {
  // Make sure the parameters are valid
  if (APP_ERROR(!caption || !prompt || !returnedSelection)("Invalid parameter to PromptSelectSingleResource"))
      return false;

  // Reset the selection
  *returnedSelection = 0;

  // Holds the end of the resource options list
  dc::dcGenericResource::Array::const_iterator end = resourceOptions.end();

  // Add options to the list
  std::list<std::string> selectionOptions;
  for (dc::dcGenericResource::Array::const_iterator i = resourceOptions.begin();
       i != end; ++i) {
     selectionOptions.push_back((*i)->getName());
  }

  // Ask the user to pick a resource
  int selection;
  if (!dcx::dcxWin32PromptForSelection(parent,
                                       caption,
                                       prompt,
                                       &selectionOptions,
                                       &selection))
      return false;

  // Set the returned resource
  *returnedSelection = resourceOptions.at(selection);

  // Success
  return true;
}




//----[  PromptSelectMultipleResources  ]--------------------------------------
bool PromptSelectMultipleResources(HWND parent,
                                   const char* caption,
                                   const char* prompt,
                                   const dc::dcGenericResource::Array& resourceOptions,
                                   dc::dcGenericResource::Array* resourceSelection,
                                   bool initializeSelection) {
  // Holds the end of the resource options list
  dc::dcGenericResource::Array::const_iterator end = resourceOptions.end();

  // The indices of the entries the user picked
  std::list<int> selection;

  // Add options to the list
  std::list<std::string> selectionOptions;
  int index = 0;
  for (dc::dcGenericResource::Array::const_iterator i = resourceOptions.begin();
       i != end; ++i, ++index) {

    // Get the resource from the options list
    dc::dcGenericResource* resource = (*i);

    // Add this resource's name
    selectionOptions.push_back((*i)->getName());

    // If necessary, see if this entry is in the current selection
    if (initializeSelection) {
      dc::dcGenericResource::Array::const_iterator selectionEnd = resourceSelection->end();
      for (dc::dcGenericResource::Array::const_iterator j = resourceSelection->begin();
           j != selectionEnd; ++j) {
         if ((*j) == resource) { selection.push_back(index); break; }
      }
    }
  }

  // Reset the returned selection list
  resourceSelection->clear();

  // Ask the user to pick some resources
  if (!dcx::dcxWin32PromptForSelection(parent,
                                       caption,
                                       prompt,
                                       &selectionOptions,
                                       &selection,
                                       initializeSelection))
    return false;

  // Build a list of the resource pointers
  for (std::list<int>::iterator i = selection.begin(); i != selection.end(); ++i)
  {
    // Add this entry to the returned selection
    resourceSelection->push_back(resourceOptions.at(*i));
  }

  // Success
  return resourceSelection->empty() == false;
}



//----[  EditResourceName  ]---------------------------------------------------
int EditResourceName(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  // Get the current name
  std::string name = resource->getName();

  // Prompt the resource's current name
  if (dcx::dcxWin32PromptTextInput(window,
                                  "Edit Name",
                                  "Please enter a new name for this resource:",
                                  &name)) {
    // Assign the new name
    if (!resource->setName(name)) {
      // Display a message to the user
      MessageBox(window,
                 "Edit Name - Failed",
                 "A resource with that name already exists.",
                 MB_OK|MB_ICONEXCLAMATION);

      // Nothing happened
      return 0;
    }
    else
        return 1; // Something changed
  }
  else
      return 0; // Nothing
}





//----[  CopyCollectionElementMembers  ]---------------------------------------
int CopyCollectionElementMembers(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  dc::dcGenericCollection* collection =
    (dc::dcGenericCollection*)resource->getOwner();
  if (APP_ERROR(!collection->isCollection())("Specified resource is not a member of a collection")) return 0;

  const dc::dcGenericResource::Array& resource_members =
    resource->isInterface() ? ((dc::dcGenericInterface*)resource)->getGenericImplementation()->getMembers() : resource->getMembers();

  // Select the members of this resource to copy
  dc::dcGenericResource::Array members;
  if (!PromptSelectMultipleResources(window,
                                     "Copy Members",
                                     "Pick the members to copy",
                                     resource_members,
                                     &members,
                                     false))
      return 0;

  // Select the target elements to which to copy the members
  dc::dcGenericResource::Array elements;
  if (!PromptSelectMultipleResources(window,
                                     "Copy Members",
                                     "Pick the elements to which to copy the members",
                                     collection->getMembers(),
                                     &elements,
                                     false))
      return 0;

  // Copy the values over
  for (dc::dcGenericResource::Array::iterator
       elementIterator = elements.begin();
       elementIterator != elements.end(); ++elementIterator) {
    dc::dcGenericResource* targetElement = *elementIterator;
    for (dc::dcGenericResource::Array::iterator
         memberIterator = members.begin();
         memberIterator != members.end(); ++memberIterator) {
      dc::dcBufferStream bufferStream;
      dc::dcGenericResource* member = (dc::dcGenericResource*)(*memberIterator);
      CONFIRM(member->save(&bufferStream)) else continue;

      //
      const dc::dcGenericResource::Array& target_members =
        targetElement->isInterface()
        ? ((dc::dcGenericInterface*)targetElement)->getGenericImplementation()->getMembers()
        : targetElement->getMembers();

      // Find the element by name
      for (dc::dcGenericResource::Array::const_iterator
           tgt = target_members.begin();
           tgt != target_members.end(); ++tgt) {
        dc::dcGenericResource* targetMember = *tgt;
        if (targetMember->getName().compare(member->getName()) == 0) {
          bufferStream.seek(0);
          targetMember->load(&bufferStream);
        }
      }
    }
  }

  return 1;
}


//----[  SortCollection  ]-----------------------------------------------------
int SortCollection(dc::dcGenericResource* resource, void* param) {
  dc::dcGenericCollection* collection = (dc::dcGenericCollection*)resource;
  collection->sort();
  return 1;
}



//----[  SortCollectionByName  ]-----------------------------------------------
int SortCollectionByName(dc::dcGenericResource* resource, void* param) {
  dc::dcGenericCollection* collection = (dc::dcGenericCollection*)resource;
  collection->sortByName();
  return 1;
}



//----[  ClearReference  ]-----------------------------------------------------
int ClearReference(dc::dcGenericResource* resource, void* param) {
  dc::dcGenericReference* reference = (dc::dcGenericReference*)resource;
  reference->setGenericReferencedResource(NULL);
  return 1;
}





//----[  ApplyBulkRenamePattern  ]---------------------------------------------
void ApplyBulkRenamePattern(std::string* name,
                            const std::string& nameTemplate,
                            const std::string& oldName,
                            size_t index) {

    // Create an index string
    char indexString[32];
    sprintf_s(indexString, sizeof(indexString), "%lu", index);
    size_t indexStringLen = strlen(indexString);

    // Initialize the resource name
    name->assign(nameTemplate);

    // Save the index
    for (size_t offset = 0; offset < name->npos;)
    {
        offset = name->find("##", offset);
        if (offset < name->npos)
        {
            // Replace this token
            name->replace(offset, 2, indexString);

            // Advance the offset
            offset += indexStringLen;
        }
    }

    // Save the name
    for (size_t offset = 0; offset < name->npos;)
    {
        offset = name->find("$$", offset);
        if (offset < name->npos)
        {
            // Replace this token with the old name
            name->replace(offset, 2, oldName);

            // Advance the offset
            offset += oldName.length();
        }
    }
}



//----[  BulkRenameSelection  ]------------------------------------------------
bool BulkRenameSelection(dc::dcGenericResource::Array& selection,
                         const std::string& nameTemplate,
                         bool updateReferences) {

    // Repeat for each of the elements
    size_t index = 0;
    for (dc::dcGenericResource::Array::iterator
           i = selection.begin();
         i != selection.end(); ++i, ++index)
    {
        // Get the resource reference
        dc::dcGenericResource* resource = *i;

        std::string name;
        ApplyBulkRenamePattern(&name, nameTemplate, resource->getName(), index);

        // Change the resource's name
        resource->setName(name, updateReferences);
    }

    // Success
    return true;
}




//----[  BulkRename  ]---------------------------------------------------------
int BulkRename(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  dc::dcGenericResource::Array selection;
  if (!PromptSelectMultipleResources(window,
                                     "Bulk Rename",
                                     "Select resources to rename",
                                     resource->getMembers(),
                                     &selection,
                                     false))
      return 0;

// If the user enters a malformed template, we jump back here
LBL_ENTERNAMETEMPLATE:

  // Enter the new name
  std::string nameTemplate;
  if (!dcx::dcxWin32PromptTextInput(window,
          "Bulk Rename",
          "Enter the name template for the selected resources, using ## to indicate\n"
          "where to place the number, and $$ to notate the resource's old name", &nameTemplate))
      return false;

  // Check to make sure the name template has some sort of key in it
  if (nameTemplate.find("##") >= nameTemplate.npos && nameTemplate.find("$$") >= nameTemplate.npos)
  {
      // Let the user know what's up
      MessageBox(window, "The name template must contain either ## or $$", "Bulk Rename - Error", MB_OK|MB_ICONEXCLAMATION);

      // Yep, it's a goto.  Sorry kids.
      goto LBL_ENTERNAMETEMPLATE;
  }

  // Find out whether or not we should update references
  bool updateReferences = true;

  // Execute the renaming
  return BulkRenameSelection(selection, nameTemplate, updateReferences) ? 1 : 0;
}






//----[  FindReplaceInNames  ]-------------------------------------------------
int FindReplaceInNames(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  dc::dcGenericResource::Array selection;
  selection.assign(resource->getMembers().begin(),
                   resource->getMembers().end());
  if (!PromptSelectMultipleResources(window,
                  "Find/Replace In Names",
                  "Select resources whose names you want to modify:",
                  resource->getMembers(),
                  &selection,
                  false))
      return 0;

  // Enter the find and replace strings
  std::string findString, replaceString;
  if (!dcx::dcxWin32PromptTextInput(
                window,
                "Find/Replace In Names",
                "Enter the string to search for in the names:",
                &findString) ||
      !dcx::dcxWin32PromptTextInput(
                window, "Find/Replace In Names",
      "Enter the replacement string for the text, if it's found:", &replaceString))
      return 0;

  // If the find string is empty, don't do anything
  if (findString.empty()) return 0;

  // Find out whether or not we should update references
  bool updateReferences = true;

  // Repeat for each of the elements
  size_t index = 0;
  for (dc::dcGenericResource::Array::iterator i = selection.begin();
       i != selection.end(); ++i, ++index) {
    // Get the resource reference
    dc::dcGenericResource* resource = *i;

    // Obtain a copy of the resource's name
    std::string name(resource->getName());

    // Look for the string in this name
    std::string::size_type location = name.find(findString);

    // Check to see if the string was found
    if (location != name.npos) {
      // Replace the string
      name.replace(location, findString.length(), replaceString);

      // Assign the new name
      resource->setName(name, updateReferences);
    }
  }

  // Success
  return 1;
}





//----[  DuplicateCollectionEntries  ]-----------------------------------------
int DuplicateCollectionEntries(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  // Obtain the collection
  dc::dcGenericCollection* collection = (dc::dcGenericCollection*)resource;

  // Ask the user to select elements
  dc::dcGenericResource::Array selection;
  if (!PromptSelectMultipleResources(
        window,
        "Duplicate Collection Elements",
        "Pick some elements of this collection to duplicate",
        collection->getMembers(),
        &selection,
        false))
      return 0;

  if (selection.size() == 1) {
    std::string name;
    if (!dcx::dcxWin32PromptTextInput(window,
                                      "Duplicate Collection Elements",
                                      "Enter the name for the new element",
                                      &name))
        return false;
    dc::dcGenericResource* source = selection.front();
    dc::dcBufferStream buffer;
    buffer.seek(0);
    CONFIRM(source->save(&buffer)) else return -1;
    buffer.seek(0);
    dc::dcGenericResource* duplicate = collection->generateElement(name);
    CONFIRM(duplicate && duplicate->load(&buffer)) else return 1;
    if (duplicate->getName().compare(name) != 0) { // did we have to rename b/c of a conflict?
      char message[512];
      sprintf_s(message,
                512,
                "Name '%s' already exists, so '%s' was used instead",
                name.c_str(),
                duplicate->getName().c_str());
      MessageBox(window,
                 message,
                 "Duplicate Collection Elements",
                 MB_OK);
    }

  } else {

// If the user enters a malformed template, we jump back here
LBL_ENTERNAMETEMPLATE:

    // Enter the new name
    std::string nameTemplate;
    if (!dcx::dcxWin32PromptTextInput(
            window,
            "Duplicate Collection Elements",
            "Enter the name template for the selected resources, using ## to indicate\n"
            "where to place the number, and $$ to notate the resource's old name",
            &nameTemplate))
        return false;

    // Check to make sure the name template has some sort of key in it
    if (nameTemplate.find("##") >= nameTemplate.npos &&
        nameTemplate.find("$$") >= nameTemplate.npos)
    {
      // Let the user know what's up
      MessageBox(window,
                 "The name template must contain either ## or $$", "Bulk Rename - Error",
                 MB_OK|MB_ICONEXCLAMATION);

      // Yep, it's a goto.  Sorry kids.
      goto LBL_ENTERNAMETEMPLATE;
    }

    // Use this buffer to duplicate resources
    dc::dcBufferStream buffer;

    // Duplicate the resources, and add the created ones to a new list
    dc::dcGenericResource::Array createdResources;
    dc::dcGenericResource::Array::iterator i = selection.begin();
    for (size_t index = 0; i != selection.end(); ++i, ++index) {
      // Obtain the original resource
      dc::dcGenericResource* source = (*i);

      // Save the resource to the buffer stream
      buffer.seek(0);
      if (APP_ERROR(!source || !source->save(&buffer))("There was a problem duplicating %s", source ? source->getName().c_str() : "???"))
          continue;

      // Generate the new name for this resource
      std::string name;
      ApplyBulkRenamePattern(&name, nameTemplate, source->getName(), index);

      // Create a new resource
      dc::dcGenericResource* duplicate = collection->generateElement(name);

      // Load this resource's data
      buffer.seek(0);
      if (APP_ERROR(!duplicate || !duplicate->load(&buffer))
          ("There was a problem creating %s from %s", name.c_str(), source->getName().c_str()))
          continue;
    }
  }

  // Success
  return 1;
}



//----[  DeleteCollectionEntries  ]--------------------------------------------
int DeleteCollectionEntries(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  dc::dcGenericCollection* collection = (dc::dcGenericCollection*)resource;

  // Build a prompt
  std::string prompt = "Select entries to delete from " + resource->getName();

  // Add options to the list
  std::list<std::string> options;
  dc::dcGenericResource::Array::const_iterator i, end;
  end = resource->getMembers().end();
  for (i = resource->getMembers().begin(); i != end; ++i) {
    options.push_back((*i)->getName());
  }

  // Ask the user to pick some resources
  std::list<int> selection;
  if (!dcx::dcxWin32PromptForSelection(window,
                                       "Pick Entries",
                                       prompt.c_str(),
                                       &options,
                                       &selection)) return 0;

  // Build a list of the resource pointers
  dc::dcGenericResource::Array resourceSelection;
  for (std::list<int>::iterator
        i = selection.begin();
       i != selection.end(); ++i) {
    dc::dcGenericResource::Array::const_iterator r =
      resource->getMembers().begin();

    // Add the entry with the given selection index.  Can't just use a lookup here
    // because this is a list...ugh.
    for (int j = 0; j < *i; ++j, ++r);
    resourceSelection.push_back(*r);
  }

  // Delete the resources.  We can't do this in a single stage (without looking
  // up all of the pointers before) because as soon as the collection is modified,
  // all of the indices change.  We can't delete by index anyway, so it's not a
  // big deal.
  for (dc::dcGenericResource::Array::iterator
        res = resourceSelection.begin();
       res != resourceSelection.end(); ++res) {
    collection->releaseElement(*res);
  }

  return 1;
}






















  
//----[  EditString  ]---------------------------------------------------------
int EditString(dc::dcGenericResource* generic_resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();
  dc::dcString* resource = (dc::dcString*)generic_resource;

  std::string caption = "Edit " + resource->getName();
  std::string value = resource->getValue();
  if (dcx::dcxWin32PromptTextInput(window,
                                   caption.c_str(),
                                   "Type a new value:",
                                   &value)) {
    resource->setValue(value);
    return 1;
  } else {
    return 0;
  }
}








//----[  EditNumberResource  ]-------------------------------------------------
// Edits the value of a dc::dcObject-implementing resource, masking its
// basic type as V instead.
template <typename T,typename V>
int EditNumberResource(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  T* numberResource = (T*)resource;
  V value = (V)numberResource->getValue();

  // Create a caption and prompt
  std::string caption = "Edit " + resource->getTypeName();
  std::string prompt = "Type a new value for '" + resource->getName() + "'";

  // Get the user input
  if (dcx::dcxWin32PromptNumericInput(window,
                                      caption.c_str(),
                                      prompt.c_str(),
                                      &value)) {
    numberResource->setValue((T::BasicType)value);
    return 1;
  }

  return 0;
}





//----[  EditFileName  ]-------------------------------------------------------
int EditFileName(dc::dcGenericResource* resource, void* param) {
  Evidyon::Tools::EvidyonToolDialog* dialog =
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);
  HWND window = dialog->getWindowHandle();

  // Get the file-name resource
  dc::dcFileName* fileName = (dc::dcFileName*)resource;

  // Get the file
  char file[512];
  strcpy_s(file, sizeof(file), fileName->getValue().c_str());

  // Run the file dialog
  if (dcx::dcxOpenFileDialog(window,
                             "Select File", "All Files (*.*)\0*.*\0\0",
                             file,
                             sizeof(file))) {

    fileName->setValue(file);
    return 1;
  }

  return 0;
}



//----[  ToggleBooleanValue  ]-------------------------------------------------
int ToggleBooleanValue(dc::dcGenericResource* resource, void* data) {
  dc::dcBoolean* boolean = (dc::dcBoolean*)resource;
  boolean->toggle();
  return 1;
}

}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//...........................................................................//
//                                                                           //
///////////////////////////////////////////////////////////////////////////////







//----[  dcByte  ]-------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcByte)
  { "Edit", &DirectCraftActions::EditNumberResource<dc::dcByte, int> },
END_DEFINING_ACTIONS()




//----[  dcFileName  ]---------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcFileName)
  { "Edit", &DirectCraftActions::EditFileName },
END_DEFINING_ACTIONS()



//----[  dcBoolean  ]----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcBoolean)
  { "Toggle", &DirectCraftActions::ToggleBooleanValue },
END_DEFINING_ACTIONS()



//----[  dcInteger  ]----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInteger)
  { "Edit", &DirectCraftActions::EditNumberResource<dc::dcInteger,int> },
END_DEFINING_ACTIONS()



//----[  dcFloat  ]------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcFloat)
  { "Edit", &DirectCraftActions::EditNumberResource<dc::dcFloat,float> },
END_DEFINING_ACTIONS()



//----[  dcString  ]-----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcString)
  { "Edit", &DirectCraftActions::EditString },
END_DEFINING_ACTIONS()



//----[  dcIndex16  ]----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcIndex16)
  { "Edit", &DirectCraftActions::EditNumberResource<dcIndex16,size_t> },
END_DEFINING_ACTIONS()



//----[  dcIndex32  ]----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcIndex32)
  { "Edit", &DirectCraftActions::EditNumberResource<dcIndex32,size_t> },
END_DEFINING_ACTIONS()
