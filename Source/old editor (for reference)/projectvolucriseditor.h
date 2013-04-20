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

#ifndef __PROJECTVOLUCRISEDITOR_H__
#define __PROJECTVOLUCRISEDITOR_H__
#pragma once


// Forward-declare the editor
class ProjectVolucrisEditor;


/**
* Gets the global instance of the PV editor.  If none exists, it is created.  The editor will
* automatically be deallocated when the program exists.
* @return Global instance of the PV editor
*/
ProjectVolucrisEditor* globalEditor();


/**
* Handles all of the editing operations of this application
*/
class ProjectVolucrisEditor
{
public:

  /**
   * Initializes the editor class
   */
  ProjectVolucrisEditor();

  /**
   * Executes the editor.  The return value is passed back as the return code from
   * the entry-point WinMain.
   * @return Exit code
   */
  int Run();

  /**
   * Gets the window handle of the top-most editing window.  If there are no windows in
   * the list, this will return the desktop window.
   */
  HWND getTopmostWindowHandle();

  /**
   * Adds a new window to the end of the window handle list.  This indicates that the
   * window is a child of whatever window was before it.
   * @param hWindow New window to add
   */
  void pushWindowHandle(HWND hWindow);

  /**
   * This method will pop the last window off of the end of the internal list.  The
   * incoming parameter is simply used to make sure that the windows haven't gotten
   * mixed up; if they aren't the same, an error will be logged but the last window
   * will still be removed.
   * @param hConfirmWindow The window that should be on the end of the list
   */
  void popWindowHandle(HWND hConfirmWindow);

  /**
   * Asks the user to edit a given string value.  The parent of the modal dialog is
   * assumed to be the last entry in the current window chain.
   * @param Caption The caption to display in the title bar
   * @param Prompt The question to ask the user
   * @param Text The text to put in the window (in) and the user's changes (out)
   * @return 'true' if the user clicked OK; 'false' otherwise.
   */
  //bool promptForText(const char* caption, const char* prompt, std::string* text);

  /**
   * Prompts the user to create a new project file
   * @return 'true' if saved, 'false' if canceled
   */
  bool newProject();

  /**
   * Asks the user for a file, and loads it if one is selected
   * @return 'true' if a project was loaded, 'false' if canceled
   */
  bool openProject();

  /**
   * Handles the actions that occur when the user desires to close the current project
   * (prompt to save, escape if cancel...etc).  Returns 'true' if the project was
   * closed, 'false' if canceled.
   */
  bool closeProject();

  /**
   * Saves the current file, unless a project is not open, in which case the user is
   * prompted to choose a file name.
   * @return 'true' if saved, 'false' if canceled
   */
  bool saveProject();

  /**
   * Asks the user where to save the current file
   * @return 'true' if saved, 'false' if canceled
   */
  bool saveProjectAs();

  /**
   * Creates a resource pack from the current project
   * @return 'true' if the pack could be created
   */
  bool createResourcePack();

  /** 
   * Runs the import algorithm
   */
  bool importProject();

  /** 
   * Runs the export algorithm
   */
  bool exportProject();

  /**
   * Lets the editor know that something about the project has changed
   */
  void projectChanged();

  /**
   * Opens another project and merges it with this one
   */
  bool mergeProject();

  /**
   * Lets the user compile this project file and output it
   */
  void compileProject();

  /**
   * Gets the root DirectCraft resource
   */
  ProjectVolucris* getRootResource() const;

  /**
   * Get the Direct3D device
   */
  LPDIRECT3DDEVICE9 getD3DDevice();


protected:

  /**
   * Closes all of the editor dialogs that are currently active
   */
  void closeAllWindows();

  /**
   * When the entire project is being unloaded (on new project, close, or open) this method
   * asks the user to save changes to the current project if it has been changed.
   * @return 'true' if the procedure calling this method should continue; 'false' otherwise
   */
  bool promptSaveChanges();

  /**
   * Sets the current project file name
   */
  void setProjectFile(const char* file);

  /**
   * Determines whether or not a project is currently open
   */
  bool projectIsOpen();


public:

  /**
   * Initialize the Direct3D objects
   */
  bool initializeD3DObjects();

  /**
   * Erases the Direct3D objects
   */
  void releaseD3DObjects();


protected:

  /// The root editor
  EditResourceDialog myRootDialog;

  /// This resource type holds all of the resources for currently-active PV config file
  ProjectVolucris myConfiguration;

  /// List of the current chain of active windows
  std::list<HWND> myWindows;

  /// The current project file
  char myProjectFile[MAX_PATH];

  /// Whether or not this project has been edited
  bool myProjectChangedFlag;


protected:

  /// Direct3D controller
  LPDIRECT3D9 myD3D;

  /// Direct3D device object (internal reference)
  LPDIRECT3DDEVICE9 myD3DDevice;

  /// The parameters with which the device was created
  D3DPRESENT_PARAMETERS myD3DParams;

  /// The vertex shader used to render meshes
  LPDIRECT3DVERTEXSHADER9 myMeshVertexShader;

  /// Vertex declaration used when rendering meshes
  LPDIRECT3DVERTEXDECLARATION9 myMeshVertexDeclaration;

  /// The most number of bones that can be rendered simultaneously
  int myMaxBonesPerSet;
};





#endif