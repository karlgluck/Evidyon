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
#ifndef __EVIDYONGUIMODULE_ACTIVEQUESTDISPLAY_H__
#define __EVIDYONGUIMODULE_ACTIVEQUESTDISPLAY_H__
#pragma once


#include "common/gui/guimodule.h"
#include "evidyonguibutton.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguititledpanel.h"
#include "common/gui/guitextlabelcomponent.h"

namespace Evidyon {




//----[  ActiveQuestData  ]----------------------------------------------------
struct ActiveQuestData {
  std::string name, description;
  double expiration_time;
  unsigned int quest_index;
};


class ActionTargeter;


//----[  EvidyonGUIModuleActiveQuestDisplay  ]---------------------------------
class EvidyonGUIModuleActiveQuestDisplay : public GUIModule {
public:
  EvidyonGUIModuleActiveQuestDisplay();
  bool create(GUIFont* font,
              GUICanvas* primary_canvas,
              ActionTargeter* action_targeter);
  void destroy();
  void sync(const ActiveQuestData quest_data[3]);

  // If this method returns 'true', the client has clicked the quest button
  // and would like to see details about the quest.
  bool update(double time,
              int* clicked_client_quest_index,
              unsigned int* clicked_quest_index);

public:
  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

private:
  bool quests_active_[3];
  std::string quest_names_[3];
  EvidyonGUIButton quest_buttons_[3];
  double quest_expiration_times_[3];
  ActionTargeter* action_targeter_;
  EvidyonGUITitledPanel panel_;
  unsigned int quest_indices_[3];
};



}





#endif