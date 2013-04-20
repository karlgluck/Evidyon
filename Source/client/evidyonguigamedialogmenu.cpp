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
#include "evidyonguigamedialogmenu.h"
#include "evidyonguikeyboundactionssentinel.h"
#include "client/actionqueue.h"



namespace Evidyon {


EvidyonGUIGameDialogMenu::EvidyonGUIGameDialogMenu() {

  menu_buttons_matrix_.setSourceCanvas(getCanvas());
  menu_buttons_matrix_.alignXLeft(+16);
  menu_buttons_matrix_.alignYTop(+16);
  menu_buttons_matrix_.relativeDepth(0.0f);
  menu_buttons_matrix_.absoluteWidth(180);
  menu_buttons_matrix_.absoluteHeight((35)*(NUMBER_OF_MENU_BUTTONS + 1));

  for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
    menu_buttons_[i].getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
    GUICanvas* canvas = menu_buttons_[i].getCanvas();
    int index = i >= SEPARATOR_BUTTON ? (i + 1) : i;
    canvas->setSourceCanvas(menu_buttons_matrix_.getCellCanvas(index, 0));
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
    canvas->relativeWidth(0);
    canvas->relativeHeight(-8);
    canvas->relativeDepth(0.0f);
  }

  sync(NULL);
  mouse_sentinel_.disable();
}


void EvidyonGUIGameDialogMenu::setFont(GUIFont* font) {
  for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
    menu_buttons_[i].setFont(font);
  }
}


void EvidyonGUIGameDialogMenu::registerElements(GUILayer* layer) {
  for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
    menu_buttons_[i].registerElements(layer);
  }
  mouse_sentinel_.enable();
}




void EvidyonGUIGameDialogMenu::unregisterElements(GUILayer* layer) {
  mouse_sentinel_.disable();
  for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
    menu_buttons_[i].unregisterElements(layer);
  }
}




GUIMouseSentinel* EvidyonGUIGameDialogMenu::getMouseSentinel() {
  return &mouse_sentinel_;
}

void EvidyonGUIGameDialogMenu::sync(EvidyonGUIKeyBoundActionsSentinel* key_bindings) {
  if (key_bindings == NULL) {
    for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
      menu_buttons_[i].setText(getButtonText((Button)i));
    }
  } else {
    for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
      GUIKey bound_key = key_bindings->getKeyForCommand(getButtonActionType((Button)i));
      if (bound_key == GUIKEY_NULL) {
        menu_buttons_[i].setText(getButtonText((Button)i));
      } else {
        std::string text = getButtonText((Button)i);
        text.append(" (");
        text.append(GUIKeyboardKeyName(bound_key));
        text.append(")");
        menu_buttons_[i].setText(text.c_str());
      }
    }
  }
}

const char* EvidyonGUIGameDialogMenu::getButtonText(Button button) {
  switch (button) {
  case EvidyonGUIGameDialogMenu::BUTTON_INVENTORY:    return "inventory";
  case EvidyonGUIGameDialogMenu::BUTTON_AVATARSTATS:  return "stats";
  case EvidyonGUIGameDialogMenu::BUTTON_DROP:         return "drop item";
  case EvidyonGUIGameDialogMenu::BUTTON_CAST:         return "cast";
  case EvidyonGUIGameDialogMenu::BUTTON_USEITEM:      return "use item";
  case EvidyonGUIGameDialogMenu::BUTTON_CHANGE_VIEWMODE: return "change view mode";
  case EvidyonGUIGameDialogMenu::BUTTON_TOGGLE_NAMES: return "toggle names";
  case EvidyonGUIGameDialogMenu::BUTTON_STOP_ACTION: return "stop action";
  case EvidyonGUIGameDialogMenu::BUTTON_TOGGLE_CHAT_LOG: return "toggle chat log";
  case EvidyonGUIGameDialogMenu::BUTTON_CHAT:         return "chat";
  case EvidyonGUIGameDialogMenu::BUTTON_TAKE_SCREENSHOT: return "take screenshot";
  case EvidyonGUIGameDialogMenu::BUTTON_SHOW_HELP: return "show help";
  }
  return NULL;
}


ActionType EvidyonGUIGameDialogMenu::getButtonActionType(Button button) {
  switch (button) {
  case EvidyonGUIGameDialogMenu::BUTTON_INVENTORY:    return ACTION_OPEN_INVENTORY;
  case EvidyonGUIGameDialogMenu::BUTTON_AVATARSTATS:  return ACTION_OPEN_STAT_DISPLAY;
  case EvidyonGUIGameDialogMenu::BUTTON_DROP:         return ACTION_OPEN_DROP;
  case EvidyonGUIGameDialogMenu::BUTTON_CAST:         return ACTION_OPEN_CAST;
  case EvidyonGUIGameDialogMenu::BUTTON_USEITEM:      return ACTION_OPEN_USEITEM;
  case EvidyonGUIGameDialogMenu::BUTTON_CHANGE_VIEWMODE:  return ACTION_TOGGLE_VIEW_ANGLE;
  case EvidyonGUIGameDialogMenu::BUTTON_TOGGLE_NAMES: return ACTION_TOGGLE_NAME_DISPLAY;
  case EvidyonGUIGameDialogMenu::BUTTON_STOP_ACTION: return ACTION_STOP_ACTION;
  case EvidyonGUIGameDialogMenu::BUTTON_TOGGLE_CHAT_LOG: return ACTION_TOGGLE_CHAT_LOG;
  case EvidyonGUIGameDialogMenu::BUTTON_CHAT:         return ACTION_OPEN_CHAT;
  case EvidyonGUIGameDialogMenu::BUTTON_TAKE_SCREENSHOT: return ACTION_TAKE_SCREENSHOT;
  case EvidyonGUIGameDialogMenu::BUTTON_SHOW_HELP: return ACTION_SHOW_HELP;
  default: assert(!"Button has no action type"); return INVALID_ACTION_TYPE;
  }
}


EvidyonGUIGameDialogMenu::Button EvidyonGUIGameDialogMenu::pollSelectionButtons() {
  for (int check = 0; check < NUMBER_OF_MENU_BUTTONS; ++check) {
    if (menu_buttons_[check].pollPushed()) {
      return (Button)check;
    }
  }

  return BUTTON_INVALID;
}


void EvidyonGUIGameDialogMenu::updateSelectionButtons() {
  for (int check = 0; check < NUMBER_OF_MENU_BUTTONS; ++check) {
    if (menu_buttons_[check].pollPushed()) {
      Client::ActionQueue::singleton()->addCommand(getButtonActionType(
        (EvidyonGUIGameDialogMenu::Button)check));
    }
  }
}


}

