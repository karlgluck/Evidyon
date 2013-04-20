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
#ifndef __EVIDYONTOOL_H__
#define __EVIDYONTOOL_H__
#pragma once


#include <dc/dcresource.h>
#include <windows.h>


namespace Evidyon {
namespace Tools {

extern const char GAME_FILE_FILTER[];
extern const char GAME_FILE_EXTENSION[];

class EvidyonToolDialog;



//----[  EvidyonTool  ]--------------------------------------------------------
class EvidyonTool {
public:
  EvidyonTool();

  // Runs the given dialog as the first dialog in the stac.  If another root
  // dialog already exists, this method returns and nothing happens.  This
  // method initializes the common controls library automatically.
  // beginRootDialog behaves the same way as execute*, but it returns after
  // the window has been created.
  bool executeRootDialog(dc::dcGenericResource* resource,
                         WORD menu_resource_id);
  bool beginRootDialog(dc::dcGenericResource* resource,
                       WORD menu_resource_id);

public:
  // Saves or loads a resource and all of its children into a file.  This
  // method saves the resource as the root, and the resource can only be
  // opened again with an import.
  void importResourceFromFile(dc::dcGenericResource* resource);
  void exportResourceToFile(dc::dcGenericResource* resource);

  // Used to manipulate references
  void findInvalidReferences(dc::dcGenericResource* resource);
  void replaceInReferencePaths(dc::dcGenericResource* resource);

public:

  // Invoked by the topmost tool dialog's menu when the user picks something
  virtual void dialogMenuNotification(EvidyonToolDialog* dialog,
                                      WORD item_id,
                                      WORD item_index);

  // Called when the given dialog has just been created
  virtual void dialogOpening(EvidyonToolDialog* dialog);

  // Called when the given dialog is about to be destroyed
  virtual void dialogClosing(EvidyonToolDialog* dialog);

  // Invoked by an executing dialog when it performs an action that
  // modifys the project in some way.
  void projectChanged();

  // Gets the top window
  HWND topWindow() const;

  // Returns whether or not this tool's root dialog is open.  If it is not,
  // the tool either hasn't started up yet or (as is its intended use) is
  // going to close.
  bool hasRootDialog() const;


protected:

  // Display a prompt to the user asking them whether or not to save the
  // changes that they made to the project.  Will return 'false' only if
  // the user cancels the prompt instead of selecting yes or no.
  bool promptSaveChanges();

  // Whether or not a project mirrored on disk is currently active
  bool projectIsOpen() const;

  // Prompts to save changes, then closes the current project and resets
  // any modified data structures in the tool.
  void newProject();

  // Invoked when the user wants to save a project under its current name.
  // Will prompt the user for a project file if none is selected.  If this
  // method returns 'false', the user canceled the dialog.
  bool saveProject();

  // Invoke this when the user wants to save a project under a file name
  // that is different from the one that is currently open.  If this method
  // returns 'false', the user canceled the dialog.
  bool saveProjectAs();

  // Asks the user for a file to open.  If the selected files is valid,
  // this will prompt to save changes to the current project or cancel
  // the load.
  bool openProject();

  // Closes all dialogs, clears the root dialog of its contents and
  // readds the elements it contains.
  void synchronizeRootDialog();


private:

  // Invoked internally to scan data from the file.  Global parameters can
  // be assumed to be valid.
  virtual bool toolReadFrom(const char* file) = 0;

  // Invoked internally so that the implementing tool class can actually
  // write its data into the given file.  Global parameters such as the
  // dcFileName's current path will have already been set.
  virtual bool toolWriteTo(const char* file) = 0;

  // Called when the implementing tool should erase all of the editable
  // data it contains as well as anything specific to a given project.
  virtual void toolReset() = 0;


private:

  // Internal method to update the current project file and the dcFileName
  // current path structure for interpreting relative paths.
  void setProjectFile(const char* file);


private:

  // Set when the EvidyonToolDialog reports that a resource has been
  // modified.
  bool project_changed_;

  // The currently-open Evidyon data storage file
  char project_file_[MAX_PATH];

  // The first dialog created.  When this dialog closes, the program
  // is about to exit.
  EvidyonToolDialog* root_dialog_;


private:
  EvidyonTool(const EvidyonTool&);
  void operator =(const EvidyonTool&);
};



}
}

#endif