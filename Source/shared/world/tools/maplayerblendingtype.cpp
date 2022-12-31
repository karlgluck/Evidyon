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
#include "maplayerblendingtype.h"
#include "common/null.h"


namespace Evidyon {
namespace World {
namespace Tools {

//----[  MapLayerBlendingTypeString  ]-----------------------------------------
const char* MapLayerBlendingTypeString(MapLayerBlendingType blending_type) {
  switch (blending_type) {
  case NORTH: return "North";
  case EAST:  return "East";
  case SOUTH: return "South";
  case WEST:  return "West";
  case NORTHEAST_OUTER_CORNER: return "North-East Outer Corner";
  case SOUTHEAST_OUTER_CORNER: return "South-East Outer Corner";
  case SOUTHWEST_OUTER_CORNER: return "South-West Outer Corner";
  case NORTHWEST_OUTER_CORNER: return "North-West Outer Corner";
  case NORTHEAST_INNER_CORNER: return "North-East Inner Corner";
  case SOUTHEAST_INNER_CORNER: return "South-East Inner Corner";
  case SOUTHWEST_INNER_CORNER: return "South-West Inner Corner";
  case NORTHWEST_INNER_CORNER: return "North-West Inner Corner";
  default:    return NULL;
  }
}


}
}
}