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
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"





//-----------------------------------------------------------------------------
// Name: GUICanvas
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUICanvas::GUICanvas()
{
  zero();
}



//-----------------------------------------------------------------------------
// Name: ~GUICanvas
// Desc: Cleans up this class
//-----------------------------------------------------------------------------
GUICanvas::~GUICanvas()
{
}


void GUICanvas::setSourceCanvas(GUIGenericCanvas* source_canvas) {
  changeSourceCanvas(source_canvas);
}


//-----------------------------------------------------------------------------
// Name: alignXLeft
// Desc: Specifies this canvas's x-coordinate by defining the distance from the
//     source canvas's left edge to this canvas's left edge
//-----------------------------------------------------------------------------
void GUICanvas::alignXLeft(int x)
{
  myAlignmentFlags = GUICANVAS_ALIGNX_SET(myAlignmentFlags, GUICANVAS_ALIGNX_LEFT);
  changedAlignment();
  myAlignmentLocation.x = x;
}


/**
 * 
 */
void alignXLeftOutside(int x);



//-----------------------------------------------------------------------------
// Name: alignXLeftOutside
// Desc: Specifies this canvas's x-coordinate by defining the distance from the
//     source canvas's left edge to this canvas's right edge (0 places this
//     canvas to the left of the source canvas)
//-----------------------------------------------------------------------------
void GUICanvas::alignXLeftOutside(int x)
{
  myAlignmentFlags = GUICANVAS_ALIGNX_SET(myAlignmentFlags, GUICANVAS_ALIGNX_LEFT_OUTSIDE);
  changedAlignment();
  myAlignmentLocation.x = x;
}



//-----------------------------------------------------------------------------
// Name: alignXCenter
// Desc: Specifies this canvas's x-coordinate by defining the distance from the
//     source canvas's center to this canvas's center
//-----------------------------------------------------------------------------
void GUICanvas::alignXCenter(int x)
{
  myAlignmentFlags = GUICANVAS_ALIGNX_SET(myAlignmentFlags, GUICANVAS_ALIGNX_CENTER);
  changedAlignment();
  myAlignmentLocation.x = x;
}



//-----------------------------------------------------------------------------
// Name: alignXRight
// Desc: Specifies this canvas's x-coordinate by defining the distance from the
//     source canvas's right side to this canvas's right side
//-----------------------------------------------------------------------------
void GUICanvas::alignXRight(int x)
{
  myAlignmentFlags = GUICANVAS_ALIGNX_SET(myAlignmentFlags, GUICANVAS_ALIGNX_RIGHT);
  changedAlignment();
  myAlignmentLocation.x = x;
}


//-----------------------------------------------------------------------------
// Name: alignXRightOutside
// Desc: Specifies this canvas's x-coordinate by defining the distance from the
//     source canvas's right edge to this canvas's left edge (0 places this
//     canvas to the right of the source canvas)
//-----------------------------------------------------------------------------
void GUICanvas::alignXRightOutside(int x)
{
  myAlignmentFlags = GUICANVAS_ALIGNX_SET(myAlignmentFlags, GUICANVAS_ALIGNX_RIGHT_OUTSIDE);
  changedAlignment();
  myAlignmentLocation.x = x;
}



//-----------------------------------------------------------------------------
// Name: alignYTop
// Desc: Specifies this canvas's y-coordinate by defining the distance from the
//     source canvas's top to this canvas's top
//-----------------------------------------------------------------------------
void GUICanvas::alignYTop(int y)
{
  myAlignmentFlags = GUICANVAS_ALIGNY_SET(myAlignmentFlags, GUICANVAS_ALIGNY_TOP);
  changedAlignment();
  myAlignmentLocation.y = y;
}



//-----------------------------------------------------------------------------
// Name: alignYTopOutside
// Desc: Specifies this canvas's y-coordinate by defining the distance from the
//     source canvas's top to this canvas's bottom
//-----------------------------------------------------------------------------
void GUICanvas::alignYTopOutside(int y)
{
  myAlignmentFlags = GUICANVAS_ALIGNY_SET(myAlignmentFlags, GUICANVAS_ALIGNY_TOP_OUTSIDE);
  changedAlignment();
  myAlignmentLocation.y = y;
}



//-----------------------------------------------------------------------------
// Name: alignYCenter
// Desc: Specifies this canvas's y-coordinate by defining the distance from the
//     source canvas's center to this canvas's center
//-----------------------------------------------------------------------------
void GUICanvas::alignYCenter(int y)
{
  myAlignmentFlags = GUICANVAS_ALIGNY_SET(myAlignmentFlags, GUICANVAS_ALIGNY_CENTER);
  changedAlignment();
  myAlignmentLocation.y = y;
}



//-----------------------------------------------------------------------------
// Name: alignYBottom
// Desc: Specifies this canvas's y-coordinate by defining the distance from the
//     source canvas's bottom to this canvas's bottom
//-----------------------------------------------------------------------------
void GUICanvas::alignYBottom(int y)
{
  myAlignmentFlags = GUICANVAS_ALIGNY_SET(myAlignmentFlags, GUICANVAS_ALIGNY_BOTTOM);
  changedAlignment();
  myAlignmentLocation.y = y;
}



//-----------------------------------------------------------------------------
// Name: alignYBottomOutside
// Desc: Specifies this canvas's y-coordinate by defining the distance from the
//     source canvas's bottom to this canvas's top
//-----------------------------------------------------------------------------
void GUICanvas::alignYBottomOutside(int y)
{
  myAlignmentFlags = GUICANVAS_ALIGNY_SET(myAlignmentFlags, GUICANVAS_ALIGNY_BOTTOM_OUTSIDE);
  changedAlignment();
  myAlignmentLocation.y = y;
}



//-----------------------------------------------------------------------------
// Name: absoluteWidth
// Desc: Sets this canvas's width to a definite pixel size
//-----------------------------------------------------------------------------
void GUICanvas::absoluteWidth(int pixels)
{
  myAlignmentFlags = GUICANVAS_WIDTH_SET(myAlignmentFlags, GUICANVAS_WIDTH_ABSOLUTE);
  changedAlignment();
  myAlignmentSize.width = pixels;
}



//-----------------------------------------------------------------------------
// Name: absoluteHeight
// Desc: Sets this canvas's height to a definite pixel size
//-----------------------------------------------------------------------------
void GUICanvas::absoluteHeight(int pixels)
{
  myAlignmentFlags = GUICANVAS_HEIGHT_SET(myAlignmentFlags, GUICANVAS_HEIGHT_ABSOLUTE);
  changedAlignment();
  myAlignmentSize.height = pixels;
}



//-----------------------------------------------------------------------------
// Name: relativeWidth
// Desc: Makes this canvas's width relative to its source canvas
//-----------------------------------------------------------------------------
void GUICanvas::relativeWidth(int pixelsBigger)
{
  myAlignmentFlags = GUICANVAS_WIDTH_SET(myAlignmentFlags, GUICANVAS_WIDTH_RELATIVE);
  changedAlignment();
  myAlignmentSize.width = pixelsBigger;
}



//-----------------------------------------------------------------------------
// Name: relativeHeight
// Desc: Makes this canvas's height relative to its source canvas
//-----------------------------------------------------------------------------
void GUICanvas::relativeHeight(int pixelsBigger)
{
  myAlignmentFlags = GUICANVAS_HEIGHT_SET(myAlignmentFlags, GUICANVAS_HEIGHT_RELATIVE);
  changedAlignment();
  myAlignmentSize.height = pixelsBigger;
}



//-----------------------------------------------------------------------------
// Name: realign
// Desc: Recalculates this canvas's coordinates
//-----------------------------------------------------------------------------
void GUICanvas::realign()
{
  // These are the calculated dimensions of this canvas
  GUIPoint point = myAlignmentLocation;
  GUISize size = myAlignmentSize;
  float depth = myAlignmentDepth;

  // Recalculate the coordinates for this canvas based on the source if it exists
  GUIGenericCanvas* mySourceCanvas = getSourceCanvas();
  if (mySourceCanvas)
  {
    const GUIRect* sourceCanvasArea = mySourceCanvas->getArea();
    float sourceCanvasDepth = mySourceCanvas->getDepth();

    // Adjust the width, if it is relative
    if (GUICANVAS_WIDTH_GET(myAlignmentFlags) == GUICANVAS_WIDTH_RELATIVE)
      size.width += sourceCanvasArea->calculateSize().width;

    // Adjust the height, if it is relative
    if (GUICANVAS_HEIGHT_GET(myAlignmentFlags) == GUICANVAS_HEIGHT_RELATIVE)
      size.height += sourceCanvasArea->calculateSize().height;

    // Adjust the depth, if it is relative
    if (GUICANVAS_DEPTH_GET(myAlignmentFlags) == GUICANVAS_DEPTH_RELATIVE)
      depth += sourceCanvasDepth;

    // Calculate the x-coordinate of the canvas
    switch(GUICANVAS_ALIGNX_GET(myAlignmentFlags))
    {
      default:
      case GUICANVAS_ALIGNX_LEFT:   point.x +=  sourceCanvasArea->left; break;
      case GUICANVAS_ALIGNX_CENTER:   point.x += (sourceCanvasArea->left + sourceCanvasArea->right - size.width) / 2; break;
      case GUICANVAS_ALIGNX_RIGHT:  point.x +=  sourceCanvasArea->right - size.width; break;
      case GUICANVAS_ALIGNX_LEFT_OUTSIDE:  point.x += sourceCanvasArea->left - size.width; break;
      case GUICANVAS_ALIGNX_RIGHT_OUTSIDE: point.x += sourceCanvasArea->right; break;
    }

    // Calculate the y-coordinate of this canvas
    switch(GUICANVAS_ALIGNY_GET(myAlignmentFlags))
    {
      default:
      case GUICANVAS_ALIGNY_TOP:    point.y +=  sourceCanvasArea->top; break;
      case GUICANVAS_ALIGNY_CENTER:   point.y += (sourceCanvasArea->top + sourceCanvasArea->bottom - size.height) / 2; break;
      case GUICANVAS_ALIGNY_BOTTOM:   point.y +=  sourceCanvasArea->bottom - size.height; break;
      case GUICANVAS_ALIGNY_TOP_OUTSIDE:    point.y +=  sourceCanvasArea->top - size.height; break;
      case GUICANVAS_ALIGNY_BOTTOM_OUTSIDE:   point.y +=  sourceCanvasArea->bottom; break;
    }
  }

  // Set the properties of this canvas
  area_.set(point, size);
  depth_ = depth;
}


//-----------------------------------------------------------------------------
// Name: relativeDepth
// Desc: Sets the depth of this canvas to be the parent's depth + this depth value
//-----------------------------------------------------------------------------
void GUICanvas::relativeDepth(float depth)
{
  myAlignmentFlags = GUICANVAS_DEPTH_SET(myAlignmentFlags, GUICANVAS_DEPTH_RELATIVE);
  changedAlignment();
  myAlignmentDepth = depth;
}



//-----------------------------------------------------------------------------
// Name: absoluteDepth
// Desc: Sets the absolute depth of this canvas
//-----------------------------------------------------------------------------
void GUICanvas::absoluteDepth(float depth)
{
  myAlignmentFlags = GUICANVAS_DEPTH_SET(myAlignmentFlags, GUICANVAS_DEPTH_ABSOLUTE);
  changedAlignment();
  myAlignmentDepth = depth;
}


//-----------------------------------------------------------------------------
// Name: pointInside
// Desc: Determines whether or not the given point is inside of this canvas
//-----------------------------------------------------------------------------
bool GUICanvas::pointInside(GUIPoint point)
{
  return getArea()->contains(point);
}




//-----------------------------------------------------------------------------
// Name: zero
// Desc: Finds the distance from one of the edges of this canvas to the point.  If this
//     point is inside of the canvas, this method returns 0.
//int minimumDistanceToPoint(GUIPoint point);
//     Resets all of the members of this class
//-----------------------------------------------------------------------------
void GUICanvas::zero()
{
  myAlignmentLocation.zero();
  myAlignmentSize.zero();
  myAlignmentFlags = 0;
  myAlignmentDepth = 0.0f;
}

