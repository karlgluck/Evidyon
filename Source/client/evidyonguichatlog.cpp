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
#include "evidyonguichatlog.h"
#include "common/gui/guitextlabelcomponent.h"
#include "common/gui/guifont.h"

namespace Evidyon {
namespace Client {


static const D3DCOLOR GUILD_TEXT_COLOR = D3DCOLOR_XRGB(96,238,99);
static const D3DCOLOR GLOBAL_TEXT_COLOR = D3DCOLOR_XRGB(240,232,70);
  
//----[  EvidyonGUIChatLog  ]--------------------------------------------------
EvidyonGUIChatLog::EvidyonGUIChatLog() {
  font_ = NULL;
  list_.setLineHeight(22);
  next_line_id_ = 0;
}




//----[  create  ]-------------------------------------------------------------
void EvidyonGUIChatLog::create(GUICanvas* source_canvas, GUIFont* font) {
  addComponent(&list_);
  list_.setBackgroundColor(D3DCOLOR_ARGB(128,255,255,255));
  list_.getCanvas()->setSourceCanvas(source_canvas);
  list_.getCanvas()->relativeWidth(-32);
  list_.getCanvas()->absoluteHeight(300);
  list_.getCanvas()->alignXCenter(0);
  list_.getCanvas()->alignYTop(16);
  list_.getCanvas()->absoluteDepth(1.0f); // put this at the very back
  font_ = font;
}




//----[  DeleteGUITextLabelComponent  ]----------------------------------------
void DeleteGUITextLabelComponent(GUIComponent* component) {
  delete (GUITextLabelComponent*)component;
}


//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIChatLog::destroy() {
  hide();
  list_.removeAllComponents(&DeleteGUITextLabelComponent);
  font_ = NULL;
}




//----[  show  ]---------------------------------------------------------------
void EvidyonGUIChatLog::show(GUILayer* layer) {
  GUIModule::show(layer);
  obtainFocus();
}


//----[  hide  ]---------------------------------------------------------------
void EvidyonGUIChatLog::hide() {
  GUIModule::hide();
  releaseFocus();
}

//----[  addChatText  ]--------------------------------------------------------
void EvidyonGUIChatLog::addChatText(Actor::ActorAlignment alignment,
                                    const std::string& actor_name,
                                    const std::string& text) {
  GUITextLabelComponent* line = new GUITextLabelComponent;
  line->textLabel()->setAlignment(GUIALIGN_LEFT_TOP);
  line->textLabel()->setFont(font_);
  line->textLabel()->setText(actor_name + ":  " + text);
  line->textLabel()->setTextColor(Actor::ActorAlignmentColor(alignment));
  line->getCanvas()->relativeDepth(0.0);
  line->getCanvas()->relativeWidth(0);
  line->getCanvas()->relativeHeight(0);
  line->getCanvas()->alignXLeft(0);
  line->getCanvas()->alignYTop(0);
  list_.setComponent(++next_line_id_, line);
  list_.scrollToEnd();
  registerComponentIfActive(line);
}




//----[  addGuildText  ]-------------------------------------------------------
void EvidyonGUIChatLog::addGuildText(const std::string& actor_name,
                                     const std::string& text) {
  GUITextLabelComponent* line = new GUITextLabelComponent;
  line->textLabel()->setAlignment(GUIALIGN_LEFT_TOP);
  line->textLabel()->setFont(font_);
  line->textLabel()->setText(actor_name + " [guild]:  " + text);
  line->textLabel()->setTextColor(GUILD_TEXT_COLOR);
  line->getCanvas()->relativeDepth(0.0);
  line->getCanvas()->relativeWidth(0);
  line->getCanvas()->relativeHeight(0);
  line->getCanvas()->alignXLeft(0);
  line->getCanvas()->alignYTop(0);
  list_.setComponent(++next_line_id_, line);
  list_.scrollToEnd();
  registerComponentIfActive(line);
}



//----[  addGlobalText  ]------------------------------------------------------
void EvidyonGUIChatLog::addGlobalText(const std::string& actor_name,
                                      const std::string& text) {
  GUITextLabelComponent* line = new GUITextLabelComponent;
  line->textLabel()->setAlignment(GUIALIGN_LEFT_TOP);
  line->textLabel()->setFont(font_);
  line->textLabel()->setText(actor_name + " [global]:  " + text);
  line->textLabel()->setTextColor(GLOBAL_TEXT_COLOR);
  line->getCanvas()->relativeDepth(0.0);
  line->getCanvas()->relativeWidth(0);
  line->getCanvas()->relativeHeight(0);
  line->getCanvas()->alignXLeft(0);
  line->getCanvas()->alignYTop(0);
  list_.setComponent(++next_line_id_, line);
  list_.scrollToEnd();
  registerComponentIfActive(line);
}



//----[  onMouseMove  ]--------------------------------------------------------
bool EvidyonGUIChatLog::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  return true; // catch all messages
}



//----[  onMouseButtonAction  ]------------------------------------------------
bool EvidyonGUIChatLog::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse) {
  // catch all actions in the log
  if (list_.getCanvas()->getArea()->contains(position)) {
    return true;
  } else {
    hide();
    return false;
  }
}




//----[  onKeyDown  ]----------------------------------------------------------
bool EvidyonGUIChatLog::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) return true; // we're going to catch it on the way up
  return false;
}



//----[  onKeyUp  ]------------------------------------------------------------
bool EvidyonGUIChatLog::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) {
    hide();
    return true;
  } else {
    return false;
  }
}


}
}

