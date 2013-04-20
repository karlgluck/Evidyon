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
#include "evidyontool.h"
#include "evidyontooldialog.h"
#include <dcx/win32>
#include <dc/filename>
#include <dc/dcresourcestorage2.h>
#include <dc/dcreference.h>


namespace Evidyon {
namespace Tools {

const char GAME_FILE_FILTER[] = "Evidyon Configuration (*.evidyon)\0*.evidyon\0\0";
const char GAME_FILE_EXTENSION[] = ".evidyon";



//----[  EvidyonTool  ]--------------------------------------------------------
EvidyonTool::EvidyonTool() {
  project_changed_ = false;
  project_file_[0] = '\0';
  root_dialog_ = NULL;
}




//----[  executeRootDialog  ]--------------------------------------------------
bool EvidyonTool::executeRootDialog(dc::dcGenericResource* resource,
                                    WORD menu_resource_id) {
  if (root_dialog_) return false;
  dcx::dcxInitCommonControls();
  root_dialog_ = new EvidyonToolDialog(this);
  bool return_value = root_dialog_->execute(resource, NULL, menu_resource_id);
  return return_value;
}



//----[  beginRootDialog  ]----------------------------------------------------
bool EvidyonTool::beginRootDialog(dc::dcGenericResource* resource,
                                  WORD menu_resource_id) {
  if (root_dialog_) return false;
  dcx::dcxInitCommonControls();
  root_dialog_ = new EvidyonToolDialog(this);
  bool return_value = root_dialog_->begin(resource, NULL, menu_resource_id);
  return return_value;
}



//----[  importResourceFromFile  ]---------------------------------------------
void EvidyonTool::importResourceFromFile(dc::dcGenericResource* resource) {
  char file[MAX_PATH];
  if (dcx::dcxOpenFileDialog(topWindow(), "Import Resource",
      "Evidyon Exported Resources (*.evxp)\0*.evxp\0\0",
      file, sizeof(file))) {
    dc::dcResourceStorage2 storage;
    bool succeeded = false;
    if (storage.openFile(file, "import_export")) {
      std::string original_name = resource->getName();
      if (resource->setName("##import_export##", false)) {
        succeeded = storage.load("", resource, true);
        resource->setName(original_name, false);
      }
      storage.close();
    }
    if (!succeeded) {
      MessageBox(topWindow(),
                 "Unable to import resource",
                 "Export Resource",
                 MB_OK);
    }
  }
}



//----[  exportResourceToFile  ]-----------------------------------------------
void EvidyonTool::exportResourceToFile(dc::dcGenericResource* resource) {
  char file[MAX_PATH];
  if (dcx::dcxSaveFileDialog(topWindow(), "Export Resource",
      "Evidyon Exported Resources (*.evxp)\0*.evxp\0\0",
      file, sizeof(file))) {
    dcx::dcxWin32SetFileExtension(file, MAX_PATH, ".evxp");
    dc::dcResourceStorage2 storage;
    bool succeeded = false;
    if (storage.openFile(file, "import_export")) {
      std::string original_name = resource->getName();
      if (resource->setName("##import_export##", false)) {
        succeeded = storage.save("", resource);
        resource->setName(original_name, false);
      }
      storage.close();
    }
    if (!succeeded) {
      MessageBox(topWindow(),
                 "Unable to export resource",
                 "Export Resource",
                 MB_OK);
    }
  }
}




//----[  findInvalidReferences  ]----------------------------------------------
void EvidyonTool::findInvalidReferences(dc::dcGenericResource* resource) {
  using namespace dc;

  dcGenericResource::Array resources;
  resources.push_back(resource);
  std::string missing_references;

  // Go through the list
  while (!resources.empty()) {

    // Obtain the last entry in the list
    dcGenericResource* resource = resources.back();

    // Add any of this element's children to the start of the list
    const dcGenericResource::Array& members = resource->getMembers();
    if (!members.empty()) {
      resources.insert(resources.begin(), members.begin(), members.end());
    }

    // If this resource is a reference, update it
    if (resource->isReference()) {
      dcGenericReference* reference = (dcGenericReference*)resource;
      dcGenericResource* target
        = reference->getGenericReferencedResource();
      if (reference->isMissingReference()) {
        missing_references += reference->getPathString() + "\n\t" + reference->toString() + "\n";
      } else if (target &&
                 (reference->getReferenceableTypeID() != target->getTypeID())) {
        missing_references += reference->getPathString() + "\n\t";
        missing_references += "invalid target type:  ";
        missing_references += reference->toString() + "\n";
      }
    }

    // Remove this resource from the list, since we've processed it
    resources.pop_back(); 
  }

  MessageBox(topWindow(), missing_references.c_str(), "Missing References", MB_OK);
}


//----[  replaceInReferencePaths  ]--------------------------------------------
void EvidyonTool::replaceInReferencePaths(dc::dcGenericResource* resource) {
  using namespace dc;

  // Enter the find and replace strings
  std::string findString, replaceString;
  if (!dcx::dcxWin32PromptTextInput(
                topWindow(),
                "Find/Replace In Reference Paths",
                "Enter the string to search for in the paths:",
                &findString) ||
      !dcx::dcxWin32PromptTextInput(
                topWindow(), "Find/Replace In Reference Paths",
                "Enter the replacement string for the text, if it's found:", &replaceString)) {
    return;
  }

  dcGenericResource::Array resources;
  resources.push_back(resource);

  // Replace everything
  int number_replaced = 0;
  while (!resources.empty()) {

    // Obtain the last entry in the list
    dcGenericResource* resource = resources.back();

    // Add any of this element's children to the start of the list
    const dcGenericResource::Array& members = resource->getMembers();
    if (!members.empty()) {
      resources.insert(resources.begin(), members.begin(), members.end());
    }

    // If this resource is a reference, update it
    if (resource->isReference()) {
      dcGenericReference* reference = (dcGenericReference*)resource;
      if (reference->replaceInPath(findString, replaceString)) {
        ++number_replaced;
      }
    }

    // Remove this resource from the list, since we've processed it
    resources.pop_back(); 
  }

  char buffer[256];
  sprintf_s(buffer, 256, "Made %i replacements.", number_replaced);
  MessageBox(topWindow(), buffer, "Find/Replace In Reference Paths", MB_OK);
}


//----[  dialogMenuNotification  ]---------------------------------------------
void EvidyonTool::dialogMenuNotification(EvidyonToolDialog* dialog,
                                         WORD item_id,
                                         WORD item_index) {
}



//----[  dialogOpening  ]------------------------------------------------------
void EvidyonTool::dialogOpening(EvidyonToolDialog* dialog) {
}





//----[  dialogClosing  ]------------------------------------------------------
void EvidyonTool::dialogClosing(EvidyonToolDialog* dialog) {
  if (dialog == root_dialog_) {
    promptSaveChanges();

    // TODO: since I wrote DC, I know this is how the dialog controller works.
    // this hack is to make it so that I can deallocate the root dialog and
    // force this thing to exit properly...
    SetWindowLong(dialog->getWindowHandle(), GWL_USERDATA, NULL);
    delete root_dialog_;
    root_dialog_ = NULL;
  }
}




//----[  projectChanged  ]-----------------------------------------------------
void EvidyonTool::projectChanged() {
  project_changed_ = true;
}





//----[  topWindow  ]----------------------------------------------------------
HWND EvidyonTool::topWindow() const {
  return root_dialog_ ? root_dialog_->getWindowHandle() : NULL;
}





//----[  hasRootDialog  ]------------------------------------------------------
bool EvidyonTool::hasRootDialog() const {
  return root_dialog_ != NULL;
}



//----[  promptSaveChanges  ]--------------------------------------------------
bool EvidyonTool::promptSaveChanges() {
  if (!project_changed_) return true;

  // Ask the user what to do
  int r = dcx::dcxYesNoCancelDialog(topWindow(),
                                    "Evidyon Editor Tool",
                                    "Save changes to current project?");

  // Handle the user's response
  switch(r) {
    case IDYES: return saveProject();
    case IDNO:  return true;
    default:    return false;
  }
}




//----[  projectIsOpen  ]------------------------------------------------------
bool EvidyonTool::projectIsOpen() const {
  return project_file_[0] != '\0';
}





//----[  newProject  ]---------------------------------------------------------
void EvidyonTool::newProject() {
  if (promptSaveChanges()) {
    setProjectFile("");
    toolReset();
    synchronizeRootDialog();
  }
}





//----[  saveProject  ]--------------------------------------------------------
bool EvidyonTool::saveProject() {
  if (projectIsOpen()) {
    if (toolWriteTo(project_file_)) {
      project_changed_ = false;
      return true;
    } else {
      MessageBox(topWindow(),
                 "There was some sort of problem saving this project.  "\
                 "Try picking a different destination name.",
                 "Error",
                 MB_OK);
      return false;
    }
  } else {
    return saveProjectAs();
  }
}





//----[  saveProjectAs  ]------------------------------------------------------
bool EvidyonTool::saveProjectAs() {
  char file[MAX_PATH];
  if (!dcx::dcxSaveFileDialog(topWindow(),
                             "Save File As",
                             GAME_FILE_FILTER,
                             file,
                             sizeof(file))) {
    return false;
  }

  // Make sure the extension is correct
  int len = (int)strlen(file);
  if (0 != _stricmp(file + len - sizeof(GAME_FILE_EXTENSION) + 1,
                    GAME_FILE_EXTENSION)) {
    strcat_s(file,
             sizeof(file),
             GAME_FILE_EXTENSION);
  }

  // Set the current project file
  setProjectFile(file);

  // Save the project
  return saveProject();
}





//----[  openProject  ]--------------------------------------------------------
bool EvidyonTool::openProject() {

  // Ask the user to save the current project
  if (false == promptSaveChanges()) return false;

  // Get the user to pick a file
  char file[MAX_PATH];
  if (false == dcx::dcxOpenFileDialog(topWindow(),
                                      "Open File",
                                      GAME_FILE_FILTER,
                                      file,
                                      sizeof(file))) {
    return false;
  }

  // Change the current project file
  setProjectFile(file);

  // Open this file
  toolReset();
  if (!toolReadFrom(file)) {
    toolReset();
    synchronizeRootDialog();
    MessageBox(topWindow(),
               "There was a problem loading this project; the information "\
               "displayed could be corrupted.",
               "Error",
               MB_OK);
    setProjectFile(""); // Reset the current project file
    project_changed_ = true;  // This project has been modified
    return false;
  }
  synchronizeRootDialog();

  // Success
  return true;
}




//----[  synchronizeRootDialog  ]----------------------------------------------
void EvidyonTool::synchronizeRootDialog() {
  if (root_dialog_) {
    root_dialog_->synchronizeDisplay();
  }
}




//----[  setProjectFile  ]-----------------------------------------------------
void EvidyonTool::setProjectFile(const char* file) {
  strcpy_s(project_file_, MAX_PATH, file);
  dc::dcFileName::setCurrentPathFromFileDirectory(file);
}


}
}