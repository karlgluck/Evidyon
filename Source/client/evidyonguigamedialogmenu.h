//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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