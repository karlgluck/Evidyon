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
#include "evidyonguiactionbuttonlist.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguikeyboundactionssentinel.h"


namespace Evidyon {

EvidyonGUIActionButtonList::EvidyonGUIActionButtonList() {
  button_font_ = NULL;
  tooltip_ = NULL;
}


GUICanvas* EvidyonGUIActionButtonList::getListCanvas() {
  return list_.getCanvas();
}


GUIFont* EvidyonGUIActionButtonList::getButtonFont() {
  return button_font_;
}


void EvidyonGUIActionButtonList::update(ActionTargeter* action_targeter) {
  if (!isActive()) return;

  int button_id;
  if (getClickedButton(&button_id)) {
    onButtonClicked(button_id, action_targeter);
  }

  if (!tooltip_) return;
  if (getMouseOverButton(&button_id)) {
    if (updateTooltipContent(button_id, tooltip_) &&
        tooltip_->becomeOwner(this)) {
      addTooltipContent(tooltip_);
      tooltip_->showContentAfter(0.0);
    }
  } else {
    tooltip_->releaseOwnership(this);
  }
}


EvidyonGUITooltipBox* EvidyonGUIActionButtonList::getTooltip() {
  return tooltip_;
}


void EvidyonGUIActionButtonList::createActionButtonList(
    GUIFont* button_font,
    EvidyonGUITooltipBox* tooltip) {
  button_font_ = button_font;
  tooltip_ = tooltip;
  addComponent(&list_);
  list_.setLineHeight(25);
}


void EvidyonGUIActionButtonList::destroyActionButtonList() {
  clearComponents();
  button_font_ = NULL;
  hide();
  clearButtons();
}


bool EvidyonGUIActionButtonList::getMouseOverButton(int* button_id) {
  if (!isActive()) return false;
  return list_.getMouseOverButton(button_id);
}


bool EvidyonGUIActionButtonList::getClickedButton(int* button_id) {
  if (!isActive()) return false;
  return list_.getClickedButton(button_id);
}



void EvidyonGUIActionButtonList::clearButtons() {
  list_.clear();
}



EvidyonGUIButton* EvidyonGUIActionButtonList::addButton(int id, const char* text) {
  EvidyonGUIButton* button = list_.button(id);
  button->setText(text);
  button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
  button->setFont(button_font_);
  button->enable();
  return button;
}


void EvidyonGUIActionButtonList::onButtonClicked(
    int button_id,
    ActionTargeter* action_targeter) {
}


bool EvidyonGUIActionButtonList::updateTooltipContent(int button_id, EvidyonGUITooltipBox* tooltip) {
  return false;
}


void EvidyonGUIActionButtonList::addTooltipContent(EvidyonGUITooltipBox* tooltip) {
}


}

