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