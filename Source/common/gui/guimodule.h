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
#ifndef __GUIMODULE_H__
#define __GUIMODULE_H__
#pragma once


#include <set>
#include "guicomponentset.h"
#include "guimouse.h"
#include "guikeyboard.h"

/**
 * A module is a fully-functional, self-contained part of a GUI.  Examples of
 * modules could be dialog boxes, inventory lists and key-binding lists.
 */
class GUIModule {
public:
  GUIModule();

  // Registers the components and submodules with the given layer, and enables
  // the input sentinels.
  void show(GUILayer* layer);

  // Unregisters the visual elements and disables input sentinels.  This method
  // returns the the previous parameter to show() or NULL.
  GUILayer* hide();

  // Whether or not this module is being drawn and is receiving input
  bool isActive() const;

  // Gets the mouse and keyboard focus for this module
  void obtainFocus();

  // Clears mouse and keyboard focus from this module
  void releaseFocus();

  // Determines whether or not this module has focus
  bool hasFocus() const;

  // Initializes the sentinels that pass information to this module.  Unless it
  // is shown, this module will not interpret messages.
  void setParentSentinels(GUIMouseSentinel* mouse, GUIKeyboardSentinel* keyboard);

  // Clears the parent sentinels
  void removeFromInputChain();

protected:

  // Called by the implementing module class to add another module to this one.
  // This method automatically initializes the module's parent sentinels, and
  // will call show() if the current layer is valid.
  void addSubmodule(GUIModule* module);

  // Erases the given submodule from reference.  This automatically calls
  // hide() on the submodule and removes its parent sentinels.
  void removeSubmodule(GUIModule* module);
  void clearSubmodules();

  // Inserts the component into this module.  Until removed, its visual and
  // input aspects will be managed entirely by this class.
  void addComponent(GUIComponent* component);
  void removeComponent(GUIComponent* component);
  void clearComponents();

  void registerComponentIfActive(GUIComponent* component);

  // These input routines are invoked by the internal module sentinels.  For
  // those methods with a return value, 'true' indicates that the message has
  // been processed and should no longer propagate.
  // None of these methods will be called while the module is hidden.
  // These methods are called only after the message has been passed to the
  // members of this module.
protected:
  virtual void onGotMouseFocus();
  virtual void onLostMouseFocus();
  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
  virtual bool onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse);
  virtual void onGotKeyboardFocus();
  virtual void onLostKeyboardFocus();
  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);



private:

  // passes messages first to the owner module, then to children
  class ModuleKeyboardSentinel : public GUIKeyboardSentinel {
  public:
    ModuleKeyboardSentinel(GUIModule* owner);
    virtual void onGotKeyboardFocus();
    virtual void onLostKeyboardFocus();
    virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);
    virtual bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);
  private:
    GUIModule* owner_;
  };

  // passes messages first to the owner module, then to children
  class ModuleMouseSentinel : public GUIMouseSentinel {
  public:
    ModuleMouseSentinel(GUIModule* owner);
    virtual void onGotMouseFocus();
    virtual void onLostMouseFocus();
    virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
    virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
    virtual bool onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse);
  private:
    GUIModule* owner_;
  };

private:
  typedef std::set<GUIModule*> Submodules;

private:
  GUILayer* active_layer_;
  ModuleMouseSentinel module_mouse_sentinel_;
  ModuleKeyboardSentinel module_keyboard_sentinel_;
  Submodules submodules_;
  GUIComponentSet components_;
};




#endif
