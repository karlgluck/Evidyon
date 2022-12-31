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
#include "evidyonguiscrollablelist.h"
#include "common/gui/guilayer.h"
#include "evidyonguidefs.h"

namespace Evidyon {



EvidyonGUIScrollableList::MouseSentinel::MouseSentinel(EvidyonGUIScrollableList* owner) {
  owner_ = owner;
}

void EvidyonGUIScrollableList::MouseSentinel::onGotMouseFocus() {
}

void EvidyonGUIScrollableList::MouseSentinel::onLostMouseFocus() {
}

bool EvidyonGUIScrollableList::MouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  const GUIRect* rect = owner_->list_.getCanvas()->getArea();
  if (hasMouseFocus() || rect->contains(position) || rect->contains(oldPosition)) {
    GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
    return true;
  } else {
    return false;
  }
}

bool EvidyonGUIScrollableList::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse) {
  const GUIRect* rect = owner_->list_.getCanvas()->getArea();
  bool contains = rect->contains(position);
  if (hasMouseFocus() || contains) {
    GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
    if (contains) {
      obtainMouseFocus();
    } else {
      releaseMouseFocus();
    }
    return true;
  } else {
    return false;
  }
}

bool EvidyonGUIScrollableList::MouseSentinel::onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse) {
  const GUIRect* rect = owner_->list_.getCanvas()->getArea();
  releaseMouseFocus();
  if (rect->contains(position)) {
    //obtainMouseFocus();
    GUIMouseSentinel::onMouseScroll(position, amount, lines, mouse);
    return true;
  } else {
    return false;
  }
}






EvidyonGUIScrollableList::EvidyonGUIScrollableList() : slider_(&list_) {
  static const int BACKGROUND_BORDER_SIZE = 8;
  static const int SLIDER_WIDTH = 16;
  static const float RELATIVE_COMPONENT_DEPTH = -0.01f;
  background_.setSourceCanvas(getCanvas());
  background_.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
  background_.setSourceRegion(&EvidyonGUIDefs::CONTENT_COMPONENT_BACKGROUND,
                              BACKGROUND_BORDER_SIZE,
                              EvidyonGUIDefs::TEXTURE_SIZE);
  { // set up the slider background
    GUICanvas* slider_canvas = slider_.getCanvas();
    slider_canvas->setSourceCanvas(getCanvas());
    slider_canvas->relativeDepth(RELATIVE_COMPONENT_DEPTH);
    slider_canvas->relativeHeight(-2*BACKGROUND_BORDER_SIZE);
    slider_canvas->alignXRight(-BACKGROUND_BORDER_SIZE);
    slider_canvas->alignYCenter(0);
    slider_canvas->absoluteWidth(SLIDER_WIDTH);
  }
  { // align the body of the list
    GUICanvas* list_canvas = list_.getCanvas();
    list_canvas->setSourceCanvas(getCanvas());
    list_canvas->relativeWidth(-SLIDER_WIDTH-2*BACKGROUND_BORDER_SIZE);
    list_canvas->alignXLeft(BACKGROUND_BORDER_SIZE);
    list_canvas->alignYCenter(0);
    list_canvas->relativeHeight(-2*BACKGROUND_BORDER_SIZE);
    list_canvas->relativeDepth(RELATIVE_COMPONENT_DEPTH);
  }

  // Set up the sentinels
  slider_.setParentMouseSentinel(&mouse_sentinel_);
  list_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  list_.setSlider(&slider_);
}


void EvidyonGUIScrollableList::setComponent(int id, GUIComponent* component) {
  list_.setComponent(id, component);
}

GUIComponent* EvidyonGUIScrollableList::getComponent(int id) {
  return list_.getComponent(id);
}

void EvidyonGUIScrollableList::removeComponent(int id) {
  list_.removeComponent(id);
}

void EvidyonGUIScrollableList::removeAllComponents(GUIScrollableList::ComponentCallback callback) {
  list_.removeAllComponents(callback);
}

void EvidyonGUIScrollableList::setLineHeight(int pixels) {
  list_.setLineHeight(pixels);
}

void EvidyonGUIScrollableList::scrollToLine(int line, int pixel_offset) {
  list_.scrollToLine(line, pixel_offset);
}

void EvidyonGUIScrollableList::setBackgroundColor(D3DCOLOR color) {
  background_.setColor(color);
}


//----[  getLineScrollLocation  ]----------------------------------------------
//void EvidyonGUIScrollableList::getLineScrollLocation(int* line, int* pixel_offset) {
//  list_.getLineScrollLocation(line, pixel_offset);
//}

void EvidyonGUIScrollableList::scrollToPercent(float percent) {
  list_.scrollToPercent(percent);
}

float EvidyonGUIScrollableList::getPercentScrollLocation() {
  return list_.getPercentScrollLocation();
}

void EvidyonGUIScrollableList::scrollToComponent(int id, int pixel_offset) {
  list_.scrollToComponent(id, pixel_offset);
}

void EvidyonGUIScrollableList::scrollToEnd() {
  list_.scrollToEnd();
}

GUIKeyboardSentinel* EvidyonGUIScrollableList::getKeyboardSentinel() {
  return list_.getKeyboardSentinel();
}

GUIMouseSentinel* EvidyonGUIScrollableList::getMouseSentinel() {
  return &mouse_sentinel_;
}

void EvidyonGUIScrollableList::registerElements(GUILayer* layer) {
  layer->registerElement(&background_);
  slider_.registerElements(layer);
  list_.registerElements(layer);
}

void EvidyonGUIScrollableList::unregisterElements(GUILayer* layer) {
  layer->unregisterElement(&background_);
  slider_.unregisterElements(layer);
  list_.unregisterElements(layer);
}

}