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
#ifndef __SHARED_WORLD_PATHDIRECTION_H__
#define __SHARED_WORLD_PATHDIRECTION_H__
#pragma once

#include "common/number.h"

namespace Evidyon {
namespace World {


  
//----[  PathDirection  ]------------------------------------------------------
// A path direction aids the server's AI in navigating around objects on the
// map.  When the game file is compiled, an algorithm computes the path
// direction for each different type of basic navigation (walking, swimming...)
// and clockwise/counterclockwise avoidance for each.
enum PathDirection {
  PATHDIRECTION_NORTH,
  PATHDIRECTION_EAST,
  PATHDIRECTION_SOUTH,
  PATHDIRECTION_WEST,
  PATHDIRECTION_NONE,
};




//----[  RotatePathDirectionCW  ]----------------------------------------------
//----[  RotatePathDirectionCCW  ]---------------------------------------------
// Returns the next clockwise or counterclockwise path direction from the
// given parameter.  Passing PATHDIRECTION_NONE will return the same.
PathDirection RotatePathDirectionCW(PathDirection direction);
PathDirection RotatePathDirectionCCW(PathDirection direction);


//----[  OffsetByPathDirection  ]----------------------------------------------
// Changes the parameters to reference the next tile in the given direction.
// If the direction is PATHDIRECTION_NONE, the parameters are not modified
// and the method returns false.
bool OffsetByPathDirection(PathDirection direction, int* x, int* z);
bool OffsetByPathDirection(PathDirection direction, Number* x, Number* z);


//----[  FillPathDirectionsCW  ]-----------------------------------------------
// Writes path directions into the provided elements of a 3x3 grid in a
// clockwise order.  The central element (1,1) is ignored, because it is the
// point about which the rotation occurs.
//    0 1 2
//  0 E E S
//  1 N   S
//  2 N W W
void FillPathDirectionsCW(PathDirection* _00,
                          PathDirection* _01,
                          PathDirection* _02,
                          PathDirection* _10,
                          PathDirection* _12,
                          PathDirection* _20,
                          PathDirection* _21,
                          PathDirection* _22);


//----[  FillPathDirectionsCCW  ]----------------------------------------------
// Same as FillPathDirectionsCW, but writes directions in the other direction:
//    0 1 2
//  0 S W W
//  1 S   N
//  2 E E N
void FillPathDirectionsCCW(PathDirection* _00,
                           PathDirection* _01,
                           PathDirection* _02,
                           PathDirection* _10,
                           PathDirection* _12,
                           PathDirection* _20,
                           PathDirection* _21,
                           PathDirection* _22);

}
}

#endif