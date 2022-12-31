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
#include "mapvisibilityrect.h"


namespace Evidyon {
namespace World {


  
//----[  center  ]-----------------------------------------------------------
void MapVisibilityRect::center(int x,
                              int y,
                              int half_width,
                              int half_height) {
  left_ = x - half_width;
  right_ = x + half_width;
  top_ = y - half_height;
  bottom_ = y + half_height;
}

  
//----[  moveTo  ]-----------------------------------------------------------
void MapVisibilityRect::moveTo(int x,
                              int y,
                              int half_width,
                              int half_height,
                              Area* r1,
                              Area* r2) {
  MapVisibilityRect old(*this);
  center(x, y, half_width, half_height);
  old.newTilesIn(this, r1, r2);
}


//----[  newTilesIn  ]-------------------------------------------------------
void MapVisibilityRect::newTilesIn(const MapVisibilityRect* other,
                                  Area* r1,
                                  Area* r2) {
  if (other->left_ > right_ ||
      other->right_ < left_ ||
      other->top_ > bottom_ ||
      other->bottom_ < top_) {
    r1->left = other->left_;   // use the other rectangle completely
    r1->right = other->right_;
    r1->top = other->top_;
    r1->bottom = other->bottom_;
    r2->left = 1;   // invalidate
    r2->right = -1;
    r2->top = 1;
    r2->bottom = -1;
  } else { // the rectangles overlap
    r1->left = other->left_;
    r1->right = other->right_;

    if (other->top_ < top_) {
      r1->top = other->top_;
      r1->bottom = top_ - 1;
      r2->top = top_;
      r2->bottom = other->bottom_;
    } else if (other->top_ > top_) {
      r1->top = bottom_ + 1;
      r1->bottom = other->bottom_;
      r2->top = other->top_;
      r2->bottom = bottom_;
    } else {
      r1->top = +1;     // invalidate
      r1->bottom = -1;
      r2->top = top_;
      r2->bottom = bottom_;
    }

    if (other->left_ < left_) {
      r2->left = other->left_;
      r2->right = left_ - 1;
    } else if (other->left_ > left_) {
      r2->left = right_ + 1;
      r2->right = other->right_;
    } else {
      r2->top = +1;     // invalidate
      r2->bottom = -1;
    }
  }
}




//----[  subcoordinate  ]------------------------------------------------------
bool MapVisibilityRect::subcoordinate(int x, int y, int* sub_x, int* sub_y) {
  if (x <= right_ && y <= bottom_) {
    x -= left_;
    y -= top_;
    if (x > 0 && y > 0) {
      *sub_x = x;
      *sub_y = y;
      return true;
    }
  }
  return false;
}

}
}
