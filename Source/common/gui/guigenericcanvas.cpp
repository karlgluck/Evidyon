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
#include "guigenericcanvas.h"

#ifndef NULL
#define NULL 0
#endif


GUIGenericCanvas::GUIGenericCanvas() {
  source_canvas_ = NULL;
  first_child_canvas_ = NULL;
  sibling_canvas_ = NULL;
  aligned_ = false;
}


GUIGenericCanvas::~GUIGenericCanvas() {
  changeSourceCanvas(NULL); // remove from canvas chain

  // Remove children from canvas chain.  We could just call
  // setSourceCanvas(NULL) for each of them, but this is more
  // efficient since it doesn't invoke any function calls.
  GUIGenericCanvas* child = first_child_canvas_;
  while (child != NULL) {
  GUIGenericCanvas* next = child->sibling_canvas_;
  child->source_canvas_ = NULL;
  child->sibling_canvas_ = NULL;
  child = next;
  }
}


GUIGenericCanvas* GUIGenericCanvas::getSourceCanvas() {
  return source_canvas_;
}

bool GUIGenericCanvas::isSourceCanvas() const {
  return first_child_canvas_ != NULL;
}


const GUIRect* GUIGenericCanvas::getArea() {
  realignIfNecessary();
  return &area_;
}


float GUIGenericCanvas::getDepth() {
  realignIfNecessary();
  return depth_;
}


void GUIGenericCanvas::changeSourceCanvas(GUIGenericCanvas* source_canvas) {
  if (source_canvas_) source_canvas_->removeChildCanvas(this);
  source_canvas_ = source_canvas;
  sibling_canvas_ = NULL; // make sure to get rid of any links
  if (source_canvas) source_canvas->addChildCanvas(this);
  changedAlignment();
}


void GUIGenericCanvas::changedAlignment() {
  if (!aligned_) return; // we don't have to invoke this more than once
  aligned_ = false;
  GUIGenericCanvas* child = first_child_canvas_;
  while (child != NULL) {
    child->changedAlignment();
    child = child->sibling_canvas_;
  }
}


void GUIGenericCanvas::realignIfNecessary() {
  // the behavior of changedAlignment() guarantees that if this canvas
  // doesn't need to be aligned, none of its parent canvases do, either
  if (aligned_) return;
  if (source_canvas_) source_canvas_->realignIfNecessary();
  realign();
  aligned_ = true;
}


void GUIGenericCanvas::realign() {
}


void GUIGenericCanvas::addChildCanvas(GUIGenericCanvas* canvas) {
  if (first_child_canvas_) {
  GUIGenericCanvas* last_first_child = first_child_canvas_;
  while (last_first_child->sibling_canvas_ != NULL) {
    last_first_child = last_first_child->sibling_canvas_;
  }
  last_first_child->sibling_canvas_ = canvas;
  }
  else
  first_child_canvas_ = canvas;
}


void GUIGenericCanvas::removeChildCanvas(GUIGenericCanvas* child) {
  if (first_child_canvas_ == child) {
  first_child_canvas_ = child->sibling_canvas_;
  } else {
  // remove the child from the linked list by finding the entry before it
  // and having that entry's sibling reference "hop" this one.
  GUIGenericCanvas* child_canvas = first_child_canvas_;
  while (child_canvas && child_canvas->sibling_canvas_ != child) {
    child_canvas = child_canvas->sibling_canvas_;
  }
  if (child_canvas) child_canvas->sibling_canvas_ = child->sibling_canvas_;
  }
}



