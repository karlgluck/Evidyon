//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#ifndef __EVIDYONGUIGAMEDIALOGMENU_H__
#define __EVIDYONGUIGAMEDIALOGMENU_H__
#pragma once


#include "common/gui/guicomponent.h"
#include "evidyonguibutton.h"
#include "common/gui/guicanvasmatrix.h"
#include "useractionqueue.h"


namespace Evidyon {
enum ActionType;

class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIGameDialogMenu : public GUIComponent {
public:
  enum Button {
    BUTTON_INVENTORY,
    BUTTON_AVATARSTATS,
    BUTTON_DROP,
    BUTTON_CAST,
    BUTTON_USEITEM,
    BUTTON_CHANGE_VIEWMODE,
    BUTTON_TOGGLE_NAMES,
    BUTTON_STOP_ACTION,
    BUTTON_TOGGLE_CHAT_LOG,
    BUTTON_CHAT,
    BUTTON_TAKE_SCREENSHOT,
    BUTTON_SHOW_HELP,
    NUMBER_OF_MENU_BUTTONS,
    BUTTON_INVALID,
    SEPARATOR_BUTTON = BUTTON_CHANGE_VIEWMODE
  };

public:
  EvidyonGUIGameDialogMenu();
  void setFont(GUIFont* font);
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);
  virtual GUIMouseSentinel* getMouseSentinel();

  void sync(EvidyonGUIKeyBoundActionsSentinel* key_bindings);

  static const char* getButtonText(Button button);
  static UserEventType getButtonEventType(Button button);

  static ActionType getButtonActionType(Button button);

  // Checks all of the selection buttons to see if one has been pressed
  Button pollSelectionButtons();

  // Detects if a button has been pressed, and performs the action for
  // that button.
  void updateSelectionButtons();

private:
  GUIMouseSentinel mouse_sentinel_;
  GUICanvasMatrix<NUMBER_OF_MENU_BUTTONS+1,1> menu_buttons_matrix_;
  EvidyonGUIButton menu_buttons_[NUMBER_OF_MENU_BUTTONS];
};



}



#endif