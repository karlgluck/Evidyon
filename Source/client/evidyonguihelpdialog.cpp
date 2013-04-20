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
#include "evidyonguihelpdialog.h"


namespace Evidyon {
namespace Client {


static const char HELP_TEXT[]
  = "For complete directions, visit www.evidyon.com.\n"\
    "Movement: Use mouse pointer and left-click on desired location.\n"\
    "Key Bindings: Keys can be bound to opening menus, casting spells or using items.\n"
    "Change what a key is bound to by by right-clicking on a button or list item and\n"
    "pressing the shortcut key you wish to use\n"\
    "\n"\
    "Entering Text\n"\
    "To speak, hit Enter and type your text, press enter again to send text. Text will\n"\
    "appear above your character’s head\n"\
    "For now (this is not a permanent feature) type > followed by text to speak to everyone online\n"\
    "\n"\
    "Geosids\n"\
    "You can sacrifice items to a Geosid. If you open your drop menu (default key is D)\n"\
    "near a town Geosid the menu will appear in red to acknowledge that any items dropped in\n"\
    "this location will be sacrificed. Eventually, sacrificing items to a Geosid causes it to\n"\
    "perform different events, such as generating items or creating a portal to one of the worlds\n"\
    "that was linked in the Evidyon. Anyone can sacrifice to white geosids, but red, blue and\n"\
    "yellow geosids must be owned by a guild.\n\n"
    "Guilds\n"\
    "Guilds are collection of members who can own Geosids and are not disbanded\n"\
    "when you log off. Say \"g/create GuildName\" to try to start a guild.  When\n"\
    "someone else says \"g/join\" to join you, your guild will be created.  To\n"\
    "bring more people into the guild, use \"g/lead\".  More guild commands are online.\n"\
    "Membership in a guild is denoted under the character’s name in brackets";

  
//----[  EvidyonGUIHelpDialog  ]-----------------------------------------------
EvidyonGUIHelpDialog::EvidyonGUIHelpDialog() {
  addComponent(&panel_);
  addComponent(&help_text_);
  addComponent(&ok_button_);
}



//----[  create  ]-------------------------------------------------------------
void EvidyonGUIHelpDialog::create(GUICanvas* source_canvas, GUIFont* font) {
  {
    panel_.setColor(0xC0FFFFFF);
    panel_.setFont(font);
    panel_.setText("Help - How to Play Evidyon");
    GUICanvas* canvas = panel_.getCanvas();
    canvas->setSourceCanvas(source_canvas);
    canvas->absoluteWidth(740);
    canvas->absoluteHeight(500);
    canvas->absoluteDepth(0.5f);
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
  }
  {
    help_text_.textLabel()->setText(HELP_TEXT);
    help_text_.textLabel()->setFont(font);
    GUICanvas* canvas = help_text_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->relativeWidth(-30);
    canvas->relativeHeight(-60);
    canvas->alignXCenter(0);
    canvas->alignYCenter(-30);
    canvas->relativeDepth(-0.01);
  }
  {
    ok_button_.setText("OK");
    ok_button_.setFont(font);
    GUICanvas* canvas = ok_button_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->absoluteWidth(180);
    canvas->absoluteHeight(35);
    canvas->alignXCenter(0);
    canvas->alignYBottom(-15);
    canvas->relativeDepth(-0.01);
  }
}



//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIHelpDialog::destroy() {
  hide();
}

//----[  show  ]---------------------------------------------------------------
void EvidyonGUIHelpDialog::show(GUILayer* layer) {
  GUIModule::show(layer);
  obtainFocus();
}

//----[  hide  ]---------------------------------------------------------------
void EvidyonGUIHelpDialog::hide() {
  GUIModule::hide();
  releaseFocus();
}



//----[  onMouseMove  ]--------------------------------------------------------
bool EvidyonGUIHelpDialog::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  return true; // catch all messages
}



//----[  onMouseButtonAction  ]------------------------------------------------
bool EvidyonGUIHelpDialog::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse) {
  // catch all actions in the log
  if (panel_.getCanvas()->getArea()->contains(position)) {
    if (ok_button_.pollPushed()) {
      hide();
    }
  } else {
    hide();
  }
  return false;
}




//----[  onKeyDown  ]----------------------------------------------------------
bool EvidyonGUIHelpDialog::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) return true; // we're going to catch it on the way up
  return false;
}



//----[  onKeyUp  ]------------------------------------------------------------
bool EvidyonGUIHelpDialog::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) {
    hide();
    return true;
  } else {
    return false;
  }
}


}
}

