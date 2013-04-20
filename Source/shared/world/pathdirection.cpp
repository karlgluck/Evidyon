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
#include "pathdirection.h"
#include <assert.h>

namespace Evidyon {
namespace World {

  
//----[  RotatePathDirectionCW  ]----------------------------------------------
PathDirection RotatePathDirectionCW(PathDirection direction) {
  static const PathDirection ROTATION[] = {
    PATHDIRECTION_EAST,  // north
    PATHDIRECTION_SOUTH, // east
    PATHDIRECTION_WEST,  // south
    PATHDIRECTION_NORTH, // west
    PATHDIRECTION_NONE,
  };
  assert(direction >= 0);
  assert(direction < sizeof(ROTATION) / sizeof(PathDirection));
  return ROTATION[direction];
}


//----[  RotatePathDirectionCCW  ]---------------------------------------------
PathDirection RotatePathDirectionCCW(PathDirection direction) {
  static const PathDirection ROTATION[] = {
    PATHDIRECTION_WEST,  // north
    PATHDIRECTION_NORTH, // east
    PATHDIRECTION_EAST,  // south
    PATHDIRECTION_SOUTH, // west
    PATHDIRECTION_NONE,
  };
  assert(direction >= 0);
  assert(direction < sizeof(ROTATION) / sizeof(PathDirection));
  return ROTATION[direction];
}


//----[  OffsetByPathDirection  ]----------------------------------------------
bool OffsetByPathDirection(PathDirection direction, int* x, int* z) {
  int x_offset[] = {  0, +1,  0, -1, 0 }; // n E s W x
  int z_offset[] = { -1,  0, +1,  0, 0 }; // N e S w x
  assert(direction >= 0);
  assert(direction < sizeof(x_offset) / sizeof(int));
  *x += x_offset[direction];
  *z += z_offset[direction];
  return direction != PATHDIRECTION_NONE;
}


//----[  OffsetByPathDirection  ]----------------------------------------------
bool OffsetByPathDirection(PathDirection direction, Number* x, Number* z) {
  Number x_offset[] = {  0, +1,  0, -1, 0 }; // n E s W x
  Number z_offset[] = { -1,  0, +1,  0, 0 }; // N e S w x
  assert(direction >= 0);
  assert(direction < sizeof(x_offset) / sizeof(Number));
  *x += x_offset[direction];
  *z += z_offset[direction];
  return direction != PATHDIRECTION_NONE;
}




//----[  FillPathDirectionsCW  ]-----------------------------------------------
void FillPathDirectionsCW(PathDirection* _00,
                          PathDirection* _01,
                          PathDirection* _02,
                          PathDirection* _10,
                          PathDirection* _12,
                          PathDirection* _20,
                          PathDirection* _21,
                          PathDirection* _22) {
  *_00 = PATHDIRECTION_EAST;
  *_01 = PATHDIRECTION_EAST;
  *_02 = PATHDIRECTION_SOUTH;
  *_10 = PATHDIRECTION_NORTH;
  *_12 = PATHDIRECTION_SOUTH;
  *_20 = PATHDIRECTION_NORTH;
  *_21 = PATHDIRECTION_WEST;
  *_22 = PATHDIRECTION_WEST;
}


//----[  FillPathDirectionsCCW  ]----------------------------------------------
void FillPathDirectionsCCW(PathDirection* _00,
                           PathDirection* _01,
                           PathDirection* _02,
                           PathDirection* _10,
                           PathDirection* _12,
                           PathDirection* _20,
                           PathDirection* _21,
                           PathDirection* _22) {
  *_00 = PATHDIRECTION_SOUTH;
  *_01 = PATHDIRECTION_WEST;
  *_02 = PATHDIRECTION_WEST;
  *_10 = PATHDIRECTION_SOUTH;
  *_12 = PATHDIRECTION_NORTH;
  *_20 = PATHDIRECTION_EAST;
  *_21 = PATHDIRECTION_EAST;
  *_22 = PATHDIRECTION_NORTH;
}



}
}

