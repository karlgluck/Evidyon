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


