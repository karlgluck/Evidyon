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
#include "evidyonguimodule_activequestdisplay.h"
#include "actiontargeter.h"
#include "common/gui/guifont.h"

namespace Evidyon {




  
  //----[  EvidyonGUIModuleActiveQuestDisplay  ]-------------------------------
  EvidyonGUIModuleActiveQuestDisplay::EvidyonGUIModuleActiveQuestDisplay() {
  memset(quests_active_, 0, sizeof(quests_active_));
  action_targeter_ = NULL;
}



//----[  create  ]-------------------------------------------------------------
bool EvidyonGUIModuleActiveQuestDisplay::create(GUIFont* font,
                                                GUICanvas* primary_canvas,
                                                ActionTargeter* action_targeter) {
  { // Initialize the panel as much as possible
    panel_.setText("Quests");
    GUICanvas* canvas = panel_.getCanvas();
    panel_.setFont(font);
    canvas->setSourceCanvas(primary_canvas);
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
    canvas->absoluteDepth(0.8f);
    canvas->absoluteWidth(480);
    canvas->absoluteHeight(40*3+20);
  }
  for (int i = 0; i < 3; ++i) {
    quest_buttons_[i].setFont(font);
    GUICanvas* canvas = quest_buttons_[i].getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignXCenter(0);
    //canvas->alignYTop(+10+35*i);
    canvas->relativeDepth(-0.1f);
    canvas->relativeWidth(-20);
    canvas->absoluteHeight(35);
  }
  action_targeter_ = action_targeter;
  return true;
}



//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIModuleActiveQuestDisplay::destroy() {
  hide();
  clearComponents();
}



//----[  sync  ]---------------------------------------------------------------
void EvidyonGUIModuleActiveQuestDisplay::sync(const ActiveQuestData quest_data[3]) {
  GUILayer* layer = hide();
  clearComponents();
  addComponent(&panel_);
  for (int i = 0; i < 3; ++i) {
    bool active = !quest_data[i].name.empty();
    quests_active_[i] = active;
    if (active) {
      addComponent(&quest_buttons_[i]);
      quest_buttons_[i].enable();
      quest_names_[i] = quest_data[i].name;
      quest_expiration_times_[i] = quest_data[i].expiration_time;
      quest_indices_[i] = quest_data[i].quest_index;
    }
  }
  show(layer);
}



//----[  update  ]-------------------------------------------------------------
bool EvidyonGUIModuleActiveQuestDisplay::update(
    double time,
    int* clicked_client_quest_index,
    unsigned int* clicked_quest_index) {
  if (!isActive()) return false;
  int quest_index = 0;
  for (int i = 0; i < 3; ++i) {
    if (quests_active_[i] == false) continue;
    if (quest_expiration_times_[i] < time) {
      quest_buttons_[i].setText((quest_names_[i] + " (time expired)").c_str());
    } else if (quest_expiration_times_[i] < time + 60.0*60.0) {
      char name[256];
      int minutes = ((quest_expiration_times_[i]-time)/60.0);
      int seconds = ((quest_expiration_times_[i]-time) - (minutes * 60));
      sprintf_s(name, 256, "%s (%i:%02i left)", quest_names_[i].c_str(), minutes, seconds);
      quest_buttons_->setText(name);
    } else if (quest_expiration_times_[i] < time + 24.0 * 60.0 * 60.0) {
      char name[256];
      int hours = (quest_expiration_times_[i]-time) / (60.0 * 60.0);
      int minutes = ((quest_expiration_times_[i]-time)/60.0 - (hours * 60.0));
      int seconds = ((quest_expiration_times_[i]-time) - (hours * 60.0*60 + minutes * 60));
      sprintf_s(name, 256, "%s (%i:%02i:%02i left)", quest_names_[i].c_str(), hours, minutes, seconds);
      quest_buttons_[i].setText(name);
    } else {
      quest_buttons_[i].setText(quest_names_[i].c_str());
    }
    quest_buttons_[i].getCanvas()->alignYTop(+10+40*quest_index);
    quest_index++;

    if (quest_buttons_[i].pollPushed()) {
      *clicked_client_quest_index = i;
      *clicked_quest_index = quest_indices_[i];
      return true;
    }
  }
  return false;
}




//----[  onMouseMove  ]--------------------------------------------------------
bool EvidyonGUIModuleActiveQuestDisplay::onMouseMove(
    GUIPoint position,
    GUIPoint oldPosition,
    GUIMouseInputSource* mouse) {
  return true;
}


//----[  onMouseButtonAction  ]------------------------------------------------
bool EvidyonGUIModuleActiveQuestDisplay::onMouseButtonAction(
    GUIMouseButtonAction action,
    GUIPoint position,
    GUIMouseInputSource* mouse) {
  if (panel_.getCanvas()->getArea()->contains(position)) return true;
  if (action_targeter_) action_targeter_->userEvent(CLOSE_DIALOGS);
  return true;
}


}