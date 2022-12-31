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
#include "evidyonguitooltipbox.h"
#include "evidyonguidefs.h"
#include "common/gui/guicomponent.h"
#include "common/gui/guilayer.h"


namespace Evidyon {

EvidyonGUITooltipBox::Canvas::Canvas(EvidyonGUITooltipBox* owner) {
  owner_ = owner;
  content_size_.zero();
}


void EvidyonGUITooltipBox::Canvas::setContentSize(GUISize size) {
  content_size_ = size;
}

void EvidyonGUITooltipBox::Canvas::setSourceCanvas(GUIGenericCanvas* canvas) {
  changeSourceCanvas(canvas);
  changedAlignment();
}

void EvidyonGUITooltipBox::Canvas::realign() {
  getSourceCanvas();
  const GUIRect* mouse = owner_->mouse_cursor_canvas_->getArea();
  GUIPoint pt = { mouse->right + 5, mouse->bottom + 5 };
  area_.set(pt, content_size_);
  const GUIRect* boundaries = owner_->boundaries_.getArea();
  if (area_.right > boundaries->right) {
    pt.x = mouse->left - 5 - content_size_.width;
  }
  if (area_.bottom > boundaries->bottom) {
    pt.y = mouse->top - 5 - content_size_.height;
  }
  area_.set(pt, content_size_);
  depth_ = 0.1f;
}


EvidyonGUITooltipBox::EvidyonGUITooltipBox() : canvas_(this) {
  owner_ = NULL;
  last_update_time_ = 0.0;
  next_visible_time_ = -1.0;
  primary_layer_ = NULL;
  mouse_cursor_canvas_ = NULL;

  boundaries_.relativeWidth(0);
  boundaries_.relativeHeight(0);
  boundaries_.relativeDepth(0.0f);
  boundaries_.alignXLeft(0);
  boundaries_.alignYTop(0);

  background_.setSourceCanvas(&canvas_);
  background_.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
  background_.setSourceRegion(&EvidyonGUIDefs::WINDOW_BACKGROUND, 8, EvidyonGUIDefs::TEXTURE_SIZE);
}


EvidyonGUITooltipBox::~EvidyonGUITooltipBox() {
  destroy();
}


void EvidyonGUITooltipBox::create(GUILayer* primary_layer,
                                  int pixels_tolerance,
                                  GUIGenericCanvas* mouse_cursor_canvas) {
  primary_layer_ = primary_layer;
  canvas_.setSourceCanvas(mouse_cursor_canvas); 
  boundaries_.setSourceCanvas(primary_layer->getPrimaryCanvas());
  boundaries_.relativeWidth(pixels_tolerance);
  boundaries_.relativeHeight(pixels_tolerance);
  mouse_cursor_canvas_ = mouse_cursor_canvas;
}


void EvidyonGUITooltipBox::destroy() {
  clearContent();
}


bool EvidyonGUITooltipBox::becomeOwner(void* unique_ptr) {
  if (owner_ != unique_ptr) {
    clearContent();
    owner_ = unique_ptr;
    return true;
  } else {
    return false;
  }
}


void EvidyonGUITooltipBox::releaseOwnership(void* unique_ptr) {
  if (owner_ == unique_ptr) {
    clearContent();
    owner_ = NULL;
  }
}

bool EvidyonGUITooltipBox::hasOwnership(void* unique_ptr) const {
  return owner_ == unique_ptr;
}


void EvidyonGUITooltipBox::setContentSize(GUISize size) {
  canvas_.setContentSize(size);
}

void EvidyonGUITooltipBox::addContent(GUIComponent* component) {
  component->getCanvas()->setSourceCanvas(&canvas_);
  components_.add(component);
}


void EvidyonGUITooltipBox::removeContent(GUIComponent* component) {
  components_.remove(component);
  component->getCanvas()->setSourceCanvas(NULL);
}


void EvidyonGUITooltipBox::showContentAfter(double seconds) {
  next_visible_time_ = last_update_time_ + seconds;
}


void EvidyonGUITooltipBox::update(double time) {
  if (last_update_time_ <= next_visible_time_ &&
      time >= next_visible_time_) {
    primary_layer_->registerElement(&background_);
    components_.registerElements(primary_layer_);
    next_visible_time_ = -1.0; // reset (so this can't be done multiple times)
  }

  last_update_time_ = time;
}


void EvidyonGUITooltipBox::clearContent() {
  primary_layer_->unregisterElement(&background_);
  components_.clear();

  // this ordering makes update() not try to re-register components, so
  // the components never become visible!
  next_visible_time_ = -1.0;
}




}


