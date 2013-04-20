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
