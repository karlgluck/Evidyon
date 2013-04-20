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
#ifndef __GUIRECT_H__
#define __GUIRECT_H__
#pragma once



#include "guipoint.h"
#include "guisize.h"

/**
 * Defines a rectangular area
 */
struct GUIRect
{
  short left, top, right, bottom;

  /**
   * Sets up this rectangle to have the top-left coordinate at 'point' and the
   * bottom-right coordinate at point + size
   */
  void set(GUIPoint point, GUISize size);

  /**
   * Determines whether or not this rectangle contains the given point
   */
  bool contains(GUIPoint point) const;

  /**
   * Returns 'true' if the dimensions are non-negative
   */
  bool isValid() const;

  /**
   * Changes the coordinates of the given point such that contains(constrain(point)) == true
   * with the least change in x/y coordinates.
   */
  GUIPoint constrain(GUIPoint point) const;

  /**
   * Calculates the size of this rectangle
   */
  GUISize calculateSize() const;

  /**
   * Calculates the common rectangle between this rectangle and the other
   */
  void intersect(const GUIRect* other, GUIRect* intersection) const;

  /**
   * Determines whether or not this rectangle overlaps another
   */
  bool overlaps(const GUIRect* other) const;

  /**
   * Resets the members of this structure
   */
  void zero();
};



#endif