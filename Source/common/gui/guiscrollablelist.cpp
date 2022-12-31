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
#include "guiscrollablelist.h"
#include "guipictureelement.h"
#include "guislider.h"



GUIScrollableList::MouseSentinel::MouseSentinel(GUIScrollableList* owner) {
  owner_ = owner;
}

bool GUIScrollableList::MouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  bool pass_message = hasMouseFocus();
  if (!pass_message && owner_) {
    const GUIRect* area = owner_->getCanvas()->getArea();
    pass_message = area->contains(position) || area->contains(oldPosition);
  }

  return pass_message ? GUIMouseSentinel::onMouseMove(position, oldPosition, mouse) : false;
}


bool GUIScrollableList::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse) {
  bool pass_message = hasMouseFocus(); // always pass messages when the list has focus
  if (!pass_message && owner_) {
    pass_message = owner_->getCanvas()->getArea()->contains(position);
  }
  return pass_message ? GUIMouseSentinel::onMouseButtonAction(action, position, mouse) : false;
}


bool GUIScrollableList::MouseSentinel::onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse) {
  if (owner_->getCanvas()->getArea()->contains(position)) {
    GUISlider* slider = owner_->slider_;
    if (slider) {
      slider->scroll(amount, lines);
      mouse->generateMovementUpdate();
      //GUIMouseSentinel* sentinel = slider->getMouseSentinel();
      //sentinel->onMouseScroll(position, amount, lines);
    }
    return true;
  } else {
    releaseMouseFocus();
    return false;
  }
}


GUIScrollableList::GUIScrollableList() : mouse_sentinel_(this) {
  slider_ = NULL;
  rows_.setSourceCanvas(getCanvas());
  rows_.alignYTopFromTop(0);
  rows_.relativeWidth(0);
  rows_.alignXLeftFromLeft(0);
  rows_.relativeDepth(0.0f);
}


GUIScrollableList::~GUIScrollableList() {
  removeAllComponents(NULL);
}


void GUIScrollableList::setSlider(GUISlider* slider) {
  slider_ = slider;
}

void GUIScrollableList::setComponent(int id, GUIComponent* component) {
  Components::iterator i = components_.find(id);
  if (i != components_.end()) {
    GUIComponent* old_component = i->second.second; // replace this component
  if (old_component == component) return;
    old_component->setClippingArea(NULL);
    old_component->getCanvas()->setSourceCanvas(NULL);
    old_component->removeFromInputChain();
  } else {

    // Insert without assigning a row index (yet!)
    Components::_Pairib ib = components_.insert(
        Components::value_type(id,
          std::pair<unsigned int, GUIComponent*>(0, component)));

    // Move all of the rows after the location at which this was inserted
    // down the list.  At the same time, count the number of rows.
    Components::size_type rows_after_inserted_component = 0;
    for (Components::iterator j = ib.first; j != components_.end(); ++j) {
      ++j->second.first;
      ++rows_after_inserted_component;
    }

    // Calculate the row index of the row we just inserted by constraining
    // the row indices to be linear.  Note that the counter includes the line
    // that was just inserted.
    i = ib.first;
    i->second.first = components_.size() - rows_after_inserted_component;
  }

  component->setClippingArea(getCanvas()->getArea());
  component->getCanvas()->setSourceCanvas(rows_.getRowCanvas(i->second.first));
  component->setParentSentinels(&mouse_sentinel_, &keyboard_sentinel_);
}

GUIComponent* GUIScrollableList::getComponent(int id) {
  Components::iterator i = components_.find(id);
  if (i == components_.end()) return NULL;
  return i->second.second;
}

void GUIScrollableList::removeComponent(int id) {
  Components::iterator i = components_.find(id);
  if (i == components_.end()) return;
  for (Components::iterator j = i; j != components_.end(); ++j) {
    unsigned int line = j->second.first;
    if (line == 0) continue; // this should only happen for the first line (i==components.begin);
    --line;
    j->second.first = line;
    j->second.second->getCanvas()->setSourceCanvas(rows_.getRowCanvas(line));
  }
  GUIComponent* component = i->second.second;
  component->removeFromInputChain();
  component->setClippingArea(NULL);
  component->getCanvas()->setSourceCanvas(NULL);
  components_.erase(i);
}

void GUIScrollableList::removeAllComponents(ComponentCallback callback) {
  for (Components::iterator i = components_.begin(); i != components_.end(); ++i) {
    GUIComponent* component = i->second.second;;
    component->removeFromInputChain();
    if (callback != NULL) {
      (*callback)(component);
    }
  }
  rows_.clearAllRowCanvases();
  components_.clear();
}

void GUIScrollableList::setLineHeight(int pixels) {
  rows_.setRowHeight(pixels);
  if (slider_) {
    slider_->syncSliderPosition(getPercentScrollLocation());
  }
}



//----[  scrollToLine  ]-------------------------------------------------------
void GUIScrollableList::scrollToLine(int line, int pixel_offset) {
  int line_height = rows_.getRowHeight();
  rows_.alignYTopFromTop(pixel_offset - line * line_height);
  if (slider_) {
    slider_->syncSliderPosition(getPercentScrollLocation());
  }
}




//----[  getLineScrollLocation  ]----------------------------------------------
//void GUIScrollableList::getLineScrollLocation(int* line, int* pixel_offset) {
//  int pixels_down = (rows_.getSourceCanvas()->getArea()->top - rows_.getArea()->top);
//  int row_height = rows_.getRowHeight();
//  *line = pixels_down / row_height;
//  *pixel_offset = pixels_down % row_height;
//}



//----[  scrollToPercent  ]----------------------------------------------------
void GUIScrollableList::scrollToPercent(float percent) {
  sliderScrollToPercent(percent);
  if (slider_) {
    slider_->syncSliderPosition(getPercentScrollLocation());
  }
}



//----[  getPercentScrollLocation  ]-------------------------------------------
float GUIScrollableList::getPercentScrollLocation() {
  int scrollable_row_height =
    rows_.getArea()->calculateSize().height - rows_.getRowHeight();
  float position = (getCanvas()->getArea()->top - rows_.getArea()->top) / (float)scrollable_row_height;
  return (position > 1.0f) ? 1.0f : position;
}



//----[  scrollToComponent  ]--------------------------------------------------
void GUIScrollableList::scrollToComponent(int id, int pixel_offset) {
  Components::iterator i = components_.find(id);
  if (i == components_.end()) return;
  scrollToLine(i->second.first, pixel_offset);
}



//----[  sliderScrollToPercent  ]----------------------------------------------
void GUIScrollableList::sliderScrollToPercent(float percent) {
  int scrollable_row_height =
    rows_.getArea()->calculateSize().height - rows_.getRowHeight();
  rows_.alignYTopFromTop(-percent * scrollable_row_height);
}


//----[  scrollToEnd  ]--------------------------------------------------------
void GUIScrollableList::scrollToEnd() {
  if (rows_.getArea()->calculateSize().height > getCanvas()->getArea()->calculateSize().height) {
    rows_.alignYBottomFromBottom(0);
  } else {
    rows_.alignYTopFromTop(0);
  }
  if (slider_) {
    slider_->syncSliderPosition(getPercentScrollLocation());
  }
}


GUIMouseSentinel* GUIScrollableList::getMouseSentinel() {
  return &mouse_sentinel_;
}

GUIKeyboardSentinel* GUIScrollableList::getKeyboardSentinel() {
  return &keyboard_sentinel_;
}

void GUIScrollableList::registerElements(GUILayer* layer) {
  for (Components::iterator i = components_.begin(); i != components_.end(); ++i) {
    i->second.second->registerElements(layer);
  }
}

void GUIScrollableList::unregisterElements(GUILayer* layer) {
  for (Components::iterator i = components_.begin(); i != components_.end(); ++i) {
    i->second.second->unregisterElements(layer);
  }
}







