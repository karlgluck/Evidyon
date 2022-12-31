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