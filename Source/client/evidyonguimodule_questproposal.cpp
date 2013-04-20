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
#include "evidyonguimodule_questproposal.h"
#include "shared/quest/quest-client.h"
#include "actiontargeter.h"
#include <d3d9.h> // ???
#include "common/gui/guifont.h"

namespace Evidyon {





  
//----[  EvidyonGUIModule_QuestProposal  ]-------------------------------------
EvidyonGUIModule_QuestProposal::EvidyonGUIModule_QuestProposal() {
  font_ = NULL;
  quests_ = NULL;
  number_of_quests_ = 0;

  addComponent(&panel_);
  addComponent(&quest_name_);
  addComponent(&quest_description_);
  addComponent(&accept_);
  addComponent(&decline_);

  { // Initialize the panel as much as possible
    panel_.setText("Quest");
    GUICanvas* canvas = panel_.getCanvas();
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
    canvas->absoluteDepth(0.4f);
    canvas->absoluteWidth(480);
    canvas->absoluteHeight(500);
  }

  { // Align the quest name
    GUICanvas* canvas = quest_name_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignYTop(10);
    canvas->alignXCenter(0);
    canvas->relativeDepth(-0.01f);
    canvas->relativeWidth(-20);
    canvas->absoluteHeight(30);
  }

  { // Align the quest description text
    GUICanvas* canvas = quest_description_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignYTop(10+20+5);
    canvas->alignXCenter(0);
    canvas->relativeDepth(-0.01f);
    canvas->relativeWidth(-40);
    canvas->relativeHeight(-(10+30+5+35+10));
  }

  { // accept button
    GUICanvas* canvas = accept_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignYBottom(-10);
    canvas->alignXLeft(10);
    canvas->relativeDepth(-0.01f);
    canvas->absoluteWidth(220);
    canvas->absoluteHeight(35);
  }

  { // decline button
    GUICanvas* canvas = decline_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignYBottom(-10);
    canvas->alignXRight(-10);
    canvas->relativeDepth(-0.01f);
    canvas->absoluteWidth(220);
    canvas->absoluteHeight(35);
  }
}


  
//----[  create  ]-------------------------------------------------------------
void EvidyonGUIModule_QuestProposal::create(
    GUICanvas* primary_canvas,
    GUIFont* font,
    const Evidyon::Quests::Client::Quest* quests,
    size_t number_of_quests) {
  font_ = font;
  panel_.getCanvas()->setSourceCanvas(primary_canvas);
  panel_.setFont(font);
  accept_.setFont(font);
  decline_.setFont(font);
  quest_description_.textLabel()->setFont(font);
  quest_description_.textLabel()->setAlignment(GUIALIGN_CENTER);
  quest_name_.textLabel()->setFont(font);
  quest_name_.textLabel()->setAlignment(GUIALIGN_CENTER);
  quests_ = quests;
  number_of_quests_ = number_of_quests;
}



//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIModule_QuestProposal::destroy() {
  hide();
  clearComponents();
}



//----[  displayQuestProposal  ]-----------------------------------------------
bool EvidyonGUIModule_QuestProposal::displayQuestProposal(
    unsigned int quest_index,
    Evidyon::Actor::ActorID npc_actor_id,
    int npc_quest_index,
    unsigned int validation_number) {
  if (quest_index >= number_of_quests_) return false;
  const Evidyon::Quests::Client::Quest* quest = &quests_[quest_index];
  setQuestDescription(quest);
  quest_index_ = quest_index;
  npc_actor_id_ = npc_actor_id;
  npc_quest_index_ = npc_quest_index;
  validation_number_ = validation_number;
  obtainFocus();
  panel_.setText("Quest Proposal");
  accept_.setText("Accept");
  decline_.setText("Decline");
  return true;
}




//----[  displayActiveQuest  ]-------------------------------------------------
bool EvidyonGUIModule_QuestProposal::displayActiveQuest(
    unsigned int quest_index,
    int client_quest_index,
    double time_remaining) {
  if (quest_index >= number_of_quests_) return false;
  const Evidyon::Quests::Client::Quest* quest = &quests_[quest_index];
  setQuestDescription(quest);
  quest_index_ = quest_index;
  client_quest_index_ = client_quest_index;
  panel_.setText("Active Quest");
  accept_.setText("Give Up");
  decline_.setText("Close Window");
  obtainFocus();
  return true;
}


//----[  update  ]-------------------------------------------------------------
void EvidyonGUIModule_QuestProposal::updateQuestProposal(
    ActionTargeter* action_targeter) {
  if (!isActive()) return;
  UserEventQuestOfferResponse* response = NULL;
  if (accept_.pollPushed()) {
    response = action_targeter->questOfferResponse();
    response->accepted = true;
  } else if (decline_.pollPushed()) {
    response = action_targeter->questOfferResponse();
    response->accepted = false;
  }
  if (response != NULL) {
    response->quest_index = quest_index_;
    response->npc_actor_id = npc_actor_id_;
    response->npc_quest_index = npc_quest_index_;
    response->validation_key = validation_number_;
  }
  const GUIRect* rect = quest_description_.getCanvas()->getArea();
  rect = rect;
}






//----[  updateActiveQuest  ]--------------------------------------------------
void EvidyonGUIModule_QuestProposal::updateActiveQuest(
    ActionTargeter* action_targeter,
    double time) {
  if (!isActive()) return;
  if (accept_.pollPushed()) {
    action_targeter->questGiveUp()->client_quest_index = client_quest_index_;
  } else if (decline_.pollPushed()) {
    action_targeter->userEvent(CLOSE_DIALOGS);
  }
}



//----[  setQuestDescription  ]------------------------------------------------
// Decode the description into an internal buffer
// Find the width of the dialog,
// Until the quest's description has been entirely parsed:
//  - add 1 word to the current line
//  - is size greater than width of page?
//    then remove the last word, add a newline and continue
// set text of description
// find text's height
// set height of dialog window based on height of text + some margins
void EvidyonGUIModule_QuestProposal::setQuestDescription(
    const Evidyon::Quests::Client::Quest* quest) {
  using namespace Evidyon::Quests::Client;

  if (!font_) {
    panel_.getCanvas()->absoluteHeight(100);
    return;
  }

  const Evidyon::Quests::QuestEncryptionWord* key = NULL;

  char decrypted_name[Quest::QUEST_NAME_LENGTH];
  Quests::decryptQuestDescription(quest->name,
                                  key,
                                  decrypted_name,
                                  sizeof(decrypted_name));
  quest_name_.textLabel()->setText(decrypted_name);

  GUISize size;
  // = quest_description_.getCanvas()->getArea()->calculateSize();
  size.width = 400;

  char decrypted_description[Quest::QUEST_DESCRIPTION_LENGTH];
  Quests::decryptQuestDescription(quest->description,
                                  key,
                                  decrypted_description,
                                  sizeof(decrypted_description));
  font_->wrapTextToWidth(decrypted_description, size.width);
  /*
/////////////////////////////////////////////////////////////////////////////////
  char* line_start = decrypted_description;
  char* space_after_last_word = decrypted_description;
  char* read_point = decrypted_description;
  while (*read_point != '\0') {
    // move to to the next word
    while (*read_point != '\0' && *read_point != ' ') {
      ++read_point;
    }

    // Swap in an endline to make getTextExtent work
    char old_ch = *read_point;
    *read_point = '\0';

    // check the size of the word
    GUISize line_size;
    font_->getTextExtent(line_start, -1, &line_size);
    *read_point = old_ch; // restore the read point character
    if (line_size.width >= size.width) { // too wide?
      if (space_after_last_word != line_start) {
        *space_after_last_word = '\n'; // turn the space into a carriage return
        *read_point = old_ch; // restore the character at the read point
        line_start = space_after_last_word + 1; // begin after last line
        space_after_last_word = line_start;
        while (*line_start == ' ') ++line_start; // skip spaces
      } else {
        // turn this location into a return, even though its past the line
        if (read_point != '\0') {
          *read_point = '\n';
          line_start = read_point;
        }
      }
      read_point = line_start; // start reading
    } else {
      space_after_last_word = read_point;
      while (*read_point == ' ') ++read_point; // move up to the next word
    }
  }
/////////////////////////////////////////////////////////////////////////////////
*/
  // set the text
  quest_description_.textLabel()->setText(decrypted_description);

  // get the size of the descriptive text and set the panel's size
  font_->getTextExtent(decrypted_description, -1, &size);
  panel_.getCanvas()->absoluteHeight(size.height + (10 + 30 + 5 + 35 + 10 + 50));
}



//----[  onMouseMove  ]--------------------------------------------------------
bool EvidyonGUIModule_QuestProposal::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  return true; // catch all messages
}



//----[  onMouseButtonAction  ]------------------------------------------------
bool EvidyonGUIModule_QuestProposal::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse) {
  if (action == GUIMOUSE_CLICK_SINGLE &&
      !panel_.getCanvas()->getArea()->contains(position)) {
    decline_.push();
  }
  return true;
}




//----[  onKeyDown  ]----------------------------------------------------------
bool EvidyonGUIModule_QuestProposal::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) return true; // we're going to catch it on the way up
  return false;
}



//----[  onKeyUp  ]------------------------------------------------------------
bool EvidyonGUIModule_QuestProposal::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) {
    decline_.push();
    return true;
  } else {
    return false;
  }
}


}