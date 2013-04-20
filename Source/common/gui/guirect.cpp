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
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"



//-----------------------------------------------------------------------------
// Name: set
// Desc: Sets up this rectangle to have the top-left coordinate at 'point' and the
//     bottom-right coordinate at point + size
//-----------------------------------------------------------------------------
void GUIRect::set(GUIPoint point, GUISize size)
{
  left = point.x;
  top = point.y;
  right = left + size.width;
  bottom = top + size.height;
}



//-----------------------------------------------------------------------------
// Name: contains
// Desc: Determines whether or not this rectangle contains the given point
//-----------------------------------------------------------------------------
bool GUIRect::contains(GUIPoint point) const
{
  return point.x >= left && point.x < right && point.y >= top && point.y < bottom;
}



//-----------------------------------------------------------------------------
// Name: isValid
// Desc: Returns 'true' if the dimensions are non-negative
//-----------------------------------------------------------------------------
bool GUIRect::isValid() const
{
  GUISize size = calculateSize();
  return size.width >= 0 && size.height >= 0;
}




//-----------------------------------------------------------------------------
// Name: constrain
// Desc: Changes the coordinates of the given point so that the point lies within this rectangle
//-----------------------------------------------------------------------------
GUIPoint GUIRect::constrain(GUIPoint point) const
{
  point.x = point.x > left   ? point.x : left;
  point.y = point.y > top  ? point.y : top;
  point.x = point.x < right  ? point.x : right;
  point.y = point.y < bottom ? point.y : bottom;

  return point;
}



//-----------------------------------------------------------------------------
// Name: calculateSize
// Desc: Calculates the size of this rectangle
//-----------------------------------------------------------------------------
GUISize GUIRect::calculateSize() const
{
  GUISize size = { right - left, bottom - top };
  return size;
}



//-----------------------------------------------------------------------------
// Name: intersect
// Desc: Calculates the common rectangle between this rectangle and the other
//-----------------------------------------------------------------------------
void GUIRect::intersect(const GUIRect* other, GUIRect* intersection) const
{
#define min(a,b) ((a)<(b))?(a):(b);
#define max(a,b) ((a)>(b))?(a):(b);
  intersection->left   = max(left, other->left);
  intersection->right  = min(right, other->right);
  intersection->top  = max(top, other->top);
  intersection->bottom = min(bottom, other->bottom);
}



//-----------------------------------------------------------------------------
// Name: overlaps
// Desc: Determines whether or not this rectangle overlaps another
//-----------------------------------------------------------------------------
bool GUIRect::overlaps(const GUIRect* other) const
{
  GUIRect intersection;
  intersect(other, &intersection);
  return intersection.isValid();
}



//-----------------------------------------------------------------------------
// Name: zero
// Desc: Resets the members of this structure
//-----------------------------------------------------------------------------
void GUIRect::zero()
{
  left = 0;
  right = 0;
  top = 0;
  bottom = 0;
}