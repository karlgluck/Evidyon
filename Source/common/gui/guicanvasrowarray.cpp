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
#include "guicanvasrowarray.h"



GUICanvasRowArray::RowCanvas::RowCanvas(GUICanvasRowArray* source_canvas, int row_index) {
  row_index_ = row_index;
  changeSourceCanvas(source_canvas);
}

void GUICanvasRowArray::RowCanvas::realign() {
  GUICanvasRowArray* source_canvas = (GUICanvasRowArray*)getSourceCanvas();
  if (source_canvas) {
    const GUIRect* source_area = source_canvas->getArea();
    area_.left = source_area->left;
    area_.right = source_area->right;

    int row_height = source_canvas->row_height_;
    int top = source_area->top + row_height * row_index_;
    area_.top = top;
    area_.bottom = top + row_height;

    depth_ = source_canvas->getDepth();
  }
}

GUICanvasRowArray::GUICanvasRowArray() {
  row_height_ = 0;
  ///////////// TODO: INITIALIZE OTHER VALUES ///////////////////
}

GUICanvasRowArray::~GUICanvasRowArray() {
  clearAllRowCanvases();
}

void GUICanvasRowArray::clearAllRowCanvases() {
  for (Rows::iterator row = rows_.begin(); row != rows_.end(); ++row) {
    delete row->second; // this will unlink the row both from this canvas and its children
  }
  rows_.clear();
  changedAlignment();
}

GUIGenericCanvas* GUICanvasRowArray::getRowCanvas(unsigned int index) {
  Rows::iterator row = rows_.find(index);
  RowCanvas* row_canvas = NULL;
  if (row == rows_.end()) {
    row_canvas = new RowCanvas(this, index);
    rows_.insert(Rows::value_type(index, row_canvas));
  } else {
    row_canvas = row->second;
  }

  return (GUIGenericCanvas*)row_canvas;
}

void GUICanvasRowArray::setRowHeight(int height) {
  if (row_height_ != height) {
    row_height_ = height;
    changedAlignment();
  }
}

void GUICanvasRowArray::setSourceCanvas(GUIGenericCanvas* source_canvas) {
  changeSourceCanvas(source_canvas);
}

void GUICanvasRowArray::alignXLeftFromLeft(int x) {
  alignment_flags_.horizontal_offset = 1;
  alignment_flags_.horizontal_reference = 1;
  alignment_location_.x = x;
  changedAlignment();
}

void GUICanvasRowArray::alignXCenterFromCenter(int x) {
  alignment_flags_.horizontal_offset = 2;
  alignment_flags_.horizontal_reference = 2;
  alignment_location_.x = x;
  changedAlignment();
}

void GUICanvasRowArray::alignXRightFromRight(int x) {
  alignment_flags_.horizontal_offset = 3;
  alignment_flags_.horizontal_reference = 3;
  alignment_location_.x = x;
  changedAlignment();
}

void GUICanvasRowArray::alignYTopFromTop(int y) {
  alignment_flags_.vertical_offset = 1;
  alignment_flags_.vertical_reference = 1;
  alignment_location_.y = y;
  changedAlignment();
}

void GUICanvasRowArray::alignYCenterFromCenter(int y) {
  alignment_flags_.vertical_offset = 2;
  alignment_flags_.vertical_reference = 2;
  alignment_location_.y = y;
  changedAlignment();
}

void GUICanvasRowArray::alignYBottomFromBottom(int y) {
  alignment_flags_.vertical_offset = 3;
  alignment_flags_.vertical_reference = 3;
  alignment_location_.y = y;
  changedAlignment();
}

void GUICanvasRowArray::absoluteWidth(int pixels) {
  alignment_flags_.width = 0;
  alignment_width_ = pixels;
  changedAlignment();
}


void GUICanvasRowArray::relativeWidth(int pixelsBigger) {
  alignment_flags_.width = 1;
  alignment_width_ = pixelsBigger;
  changedAlignment();
}

void GUICanvasRowArray::absoluteDepth(float depth) {
  alignment_flags_.depth = 0;
  alignment_depth_ = depth;
  changedAlignment();
}


void GUICanvasRowArray::relativeDepth(float depth) {
  alignment_flags_.depth = 1;
  alignment_depth_ = depth;
  changedAlignment();
}

void GUICanvasRowArray::realign() {

  // Clean entries from the end that have no references and calculate the
  // height of the rows in the canvas.  Don't erase the canvases in the
  // middle because it is likely that they will be used in the future.
  int height = 0;
  for (Rows::reverse_iterator row = rows_.rbegin(); row != rows_.rend();) {
  RowCanvas* row_canvas = row->second;
  unsigned int row_index = row->first;
  height = row_index;
  ++row;
  if (!row_canvas->isSourceCanvas()) {
    rows_.erase(row_index);
    row = rows_.rbegin();
  } else {
    break;
  }
  }
  height = (height + 1) * row_height_;

  GUIGenericCanvas* source_canvas = getSourceCanvas();
  const GUIRect* source_area = source_canvas ? source_canvas->getArea() : NULL;

  if (!source_canvas) {
  alignment_flags_.width = 0;
  alignment_flags_.depth = 0;
  alignment_flags_.vertical_offset = 0;
  alignment_flags_.vertical_reference = 0;
  alignment_flags_.horizontal_offset = 0;
  alignment_flags_.horizontal_reference = 0;
  }

  int width;
  switch (alignment_flags_.width) {
  case 0: width = alignment_width_; break;
  case 1: width = alignment_width_ + source_area->calculateSize().width; break;
  }

  int reference_x, offset_x = alignment_location_.x;
  switch(alignment_flags_.horizontal_offset) {
  case 0:
  case 1: break;
  case 2: offset_x -= width / 2; break;
  case 3: offset_x -= width; break;
  }
  switch(alignment_flags_.horizontal_reference) {
  case 0: reference_x = 0; break;
  case 1: reference_x = source_area->left; break;
  case 2: reference_x = (source_area->left + source_area->right) / 2; break;
  case 3: reference_x = source_area->right; break;
  }

  int x = reference_x + offset_x;

  int reference_y, offset_y = alignment_location_.y;
  switch(alignment_flags_.vertical_offset) {
  case 0:
  case 1: break;
  case 2: offset_y -= height / 2; break;
  case 3: offset_y -= height; break;
  }
  switch(alignment_flags_.vertical_reference) {
  case 0: reference_y = 0; break;
  case 1: reference_y = source_area->top; break;
  case 2: reference_y = (source_area->top + source_area->bottom) / 2; break;
  case 3: reference_y = source_area->bottom; break;
  }

  int y = reference_y + offset_y;

  area_.left = x;
  area_.right = x + width;
  area_.top = y;
  area_.bottom = y + height;

  switch (alignment_flags_.depth) {
  case 0: depth_ = alignment_depth_; break;
  case 1: depth_ = source_canvas->getDepth() + alignment_depth_;
  }
}




































