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
#ifndef __WALLTYPE_H__
#define __WALLTYPE_H__
#pragma once



namespace Evidyon {
namespace World {



//----[  WallType  ]-----------------------------------------------------------
// Defines different types of walls that can be on a location.  To specify lack
// of a wall, reference an invalid texture index.
enum WallType {
  WALLTYPE_LOW,
  WALLTYPE_MEDIUM,
  WALLTYPE_HIGH,

  // Extend the ground plane for this wall to the bottom of a pit
  WALLTYPE_FLOOR,

  // Masks to create a wall that is angled; always combine two (NE, SE, ...)
  WALLTYPE_COLLAPSE_N=0,
  WALLTYPE_COLLAPSE_E=0,
  WALLTYPE_COLLAPSE_S=0,
  WALLTYPE_COLLAPSE_W=0,

  // Masks to create a wall that is bumped/angled; this only pulls the vertex
  // halfway to the center instead of all the way.  Like full-collapse, always
  // mask 2 together.
  WALLTYPE_HALFCOLLAPSE_N=0,
  WALLTYPE_HALFCOLLAPSE_E=0,
  WALLTYPE_HALFCOLLAPSE_S=0,
  WALLTYPE_HALFCOLLAPSE_W=0,
};



//----[  WallTypeString  ]-----------------------------------------------------
const char* WallTypeString(WallType wall_type);


}
}

#endif