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
#ifndef __MAPVISIBILITYRECT_H__
#define __MAPVISIBILITYRECT_H__
#pragma once



namespace Evidyon {
namespace World {

  
//----[  MapVisibilityRect  ]--------------------------------------------------
class MapVisibilityRect {
public:
  struct Area {
    bool valid() const { return top <= bottom; }
    int left, top, right, bottom;
  };

public:

  // Rebuilds this visiblity rectangle for a viewer centered at
  // the given coordinates.
  void center(int x, int y, int half_width, int half_height);

  // Recenters the visibility area and returns the areas that are
  // newly in the region.  If this method is called without having
  // centered the rectangle at some point, the result is undefined
  // since the previous area has not been specified.
  void moveTo(int x,
              int y,
              int half_width,
              int half_height,
              Area* r1,
              Area* r2);

  // Calculates the limits for the set of tiles that are contained in
  // the new region but are not present in this one.  The caller can
  // iterate through the new tiles by iterating through all the
  // locations within the returned rectangles.  Note that the
  // rectangle coordinates are *inclusive*, so the right/bottom members
  // are actually part of the set.
  // This methid requires that the size of the two rectangles is exactly
  // the same, otherwise it will miss tiles.
  // If a returned area is invalid, it is guaranteed to fulfill
  // the condition (r->bottom < r->top) so iterating rows-first on the
  // results is fastest.  Furthermore, if this condition is not true,
  // then (r->left <= r->right) always.
  void newTilesIn(const MapVisibilityRect* other,
                  Area* r1, Area* r2);

  // If the given (x,y) coordinate is within this area, the output sub_*
  // parameters are set to (x - left_, y - top_) and 'true' is returned.
  bool subcoordinate(int x, int y, int* sub_x, int* sub_y);

  inline int left() const { return left_; }
  inline int top() const { return top_; }
  inline int right() const { return right_; }
  inline int bottom() const { return bottom_; }

private:
  int left_, top_, right_, bottom_;
};

}
}


#endif