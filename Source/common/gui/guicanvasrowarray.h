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
#ifndef __GUICANVASROWARRAY_H__
#define __GUICANVASROWARRAY_H__
#pragma once



#include "guigenericcanvas.h"
#include <map>


// A canvas row array is used to create a set of sub-canvases that represent
// lines contained by the row array.  The rows have the same width as the row
// array; however, the row array has the same height as the row height times
// the number of rows.
class GUICanvasRowArray : public GUIGenericCanvas {
  class RowCanvas : public GUIGenericCanvas {
  public:
    RowCanvas(GUICanvasRowArray* source_canvas, int row_index);

  private:
    virtual void realign();

  private:
    unsigned int row_index_;
  };

public:
  GUICanvasRowArray();

  // Clears row canvases and frees internal memory.  The preconditions for
  // calling clearAllRowCanvases() should be met.
  virtual ~GUICanvasRowArray();

  // Gets rid of all of the rows from this class.  All canvases returned by
  // getRowCanvas() will be invalid, so before this method is called the
  // caller should be sure that no outstanding references exist.  Canvases
  // that have a row canvas as a source do not need to be cleared; their
  // references will be updated automatically.
  void clearAllRowCanvases();

  // Erases the given index from the row array.  No outstanding references
  // to the canvas at this row can exist, except as a source canvas (these
  // will be erased automatically).
  //void deleteRowCanvas(unsigned int index);

  // Obtains a canvas that represents the given row in this array.  There are
  // any number of rows.  It is not necessary to linearly index rows; however,
  // a row is positioned based on its index and the height of the row array
  // is based on the highest outstanding index.
  // Be aware that when a row canvas is unused (as identified by
  // !isSourceCanvas()), it is automatically erased from the row array--so
  // don't save pointers returned by this method unless you are sure that
  // a canvas is using it as a reference as well.
  GUIGenericCanvas* getRowCanvas(unsigned int index);

  // All rows have the same height.  This method causes this canvas and
  // the canvases for all rows to become invalid and require updating.
  void setRowHeight(int height);
  inline int getRowHeight() const;

  // Normal positioning methods
  void setSourceCanvas(GUIGenericCanvas* source_canvas);
  void alignXLeftFromLeft(int x);
  void alignXCenterFromCenter(int x);
  void alignXRightFromRight(int x);
  void alignYTopFromTop(int y);
  void alignYCenterFromCenter(int y);
  void alignYBottomFromBottom(int y);
  void absoluteWidth(int pixels);
  void relativeWidth(int pixelsBigger);
  void absoluteDepth(float depth);
  void relativeDepth(float depth);

private:
  typedef std::map<unsigned int,RowCanvas*> Rows;
  Rows rows_;

  GUIPoint alignment_location_;
  float alignment_depth_;
  int alignment_width_;
  int row_height_;

  struct {
    unsigned horizontal_reference : 2;  // 0: absolute, 1: left, 2: center, 3: right
    unsigned horizontal_offset  : 2;  // 0: absolute, 1: left, 2: center, 3: right
    unsigned vertical_reference   : 2;  // 0: absolute, 1: top, 2: center, 3: bottom
    unsigned vertical_offset    : 2;  // 0: absolute, 1: top, 2: center, 3: bottom
    unsigned width        : 1;  // 0 = absolute, 1 = relative
    unsigned depth        : 1;  // 0 = absolute, 1 = relative
  } alignment_flags_;


private:
  virtual void realign();
};



int GUICanvasRowArray::getRowHeight() const { return row_height_; }


#endif
