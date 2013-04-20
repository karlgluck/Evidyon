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
#ifndef __EVIDYIONGUIMODULE_QUESTPROPOSAL_H__
#define __EVIDYIONGUIMODULE_QUESTPROPOSAL_H__
#pragma once


#include "common/gui/guimodule.h"
#include "common/gui/guitextlabelcomponent.h"
#include "evidyonguibutton.h"
#include "evidyonguititledpanel.h"
#include "shared/actor/actorid.h"


namespace Evidyon {
class ActionTargeter;

namespace Quests {
namespace Client {
struct Quest;
}
}

                /*-------------------------------*\
                |                                 |
                | QUEST NAME                      |
                |                                 |
                | Description text goes here, and |
                | is formatted to wrap around     |
                | inside the box.  The box gets   |
                | longer to accomodate larger     |
                | amounts of text.                |
                |                                 |
                |  +----------+   +-----------+   |
                |  |  Accept  |   |  Decline  |   |
                |  +----------+   +-----------+   |
                \*-------------------------------*/


//----[  EvidyonGUIModule_QuestProposal  ]-------------------------------------
class EvidyonGUIModule_QuestProposal : public GUIModule {
public:
  EvidyonGUIModule_QuestProposal();

  // Initializes the module to display quests
  void create(GUICanvas* primary_canvas,
              GUIFont* font,
              const Evidyon::Quests::Client::Quest* quests,
              size_t number_of_quests);

  // Cleans up internal objects
  void destroy();

  // Updates the quest that this dialog should be displaying.  If this method
  // returns 'false', the provided quest_index is invalid.
  bool displayQuestProposal(unsigned int quest_index,
                            Evidyon::Actor::ActorID npc_actor_id,
                            int npc_quest_index,
                            unsigned int validation_number);

  // Displays the same dialog, but with buttons named "Continue" and
  // "Give Up Quest" instead.  The actions for this module are also
  // different, 
  bool displayActiveQuest(unsigned int quest_index,
                          int client_quest_index,
                          double time_remaining);

  // Polls the dialog to see if the user has selected anything.  The server is
  // sent a network message if the user cancels or confirms the dialog.  If the
  // dialog is closed implicitly due to the user opening another dialog, nothing
  // happens.
  void updateQuestProposal(ActionTargeter* action_targeter);

  // Polls the dialog for action when the dialog is being used to display an
  // active quest.
  void updateActiveQuest(ActionTargeter* action_targeter,
                         double time);


public:
  // Overrides these methods to disable passing messages to all other components
  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

  // Catch the ESCAPE key so that pressing it only hits the cancel button
  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);


private:

  // Reformats the quest's description with endlines so that it
  // fits properly into the panel display.  Will set the height
  // of the panel to accomadate the text.
  void setQuestDescription(const Evidyon::Quests::Client::Quest* quest);


private:
  GUIFont* font_;
  const Evidyon::Quests::Client::Quest* quests_;
  size_t number_of_quests_;
  EvidyonGUITitledPanel panel_;
  GUITextLabelComponent quest_name_;
  GUITextLabelComponent quest_description_;
  EvidyonGUIButton accept_, decline_;

  // When offered a quest, these parameters hold data about it
  unsigned int quest_index_;
  Evidyon::Actor::ActorID npc_actor_id_;
  union {
    int npc_quest_index_;
    int client_quest_index_;
  };
  unsigned int validation_number_;
};


}



#endif
