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
#ifndef __GUICANVAS_H__
#define __GUICANVAS_H__
#pragma once




#include "guipoint.h"
#include "guisize.h"


#define GUICANVAS_ALIGNX_SET(flags,flag)  (((~0x7)&flags)|(flag))
#define GUICANVAS_ALIGNX_GET(flags)     ((flags)&0x7)
#define GUICANVAS_ALIGNX_LEFT   0
#define GUICANVAS_ALIGNX_CENTER 1
#define GUICANVAS_ALIGNX_RIGHT  2

#define GUICANVAS_ALIGNX_LEFT_OUTSIDE   3
#define GUICANVAS_ALIGNX_CENTER_LEFT  4
#define GUICANVAS_ALIGNX_CENTER_RIGHT   5
#define GUICANVAS_ALIGNX_RIGHT_OUTSIDE  6

#define GUICANVAS_ALIGNY_SET(flags,flag)  (((~(0x7<<3))&flags)|((flag)<<3))
#define GUICANVAS_ALIGNY_GET(flags)     (((flags)>>3)&0x7)
#define GUICANVAS_ALIGNY_TOP  0
#define GUICANVAS_ALIGNY_CENTER 1
#define GUICANVAS_ALIGNY_BOTTOM 2

#define GUICANVAS_ALIGNY_TOP_OUTSIDE   3
#define GUICANVAS_ALIGNY_CENTER_TOP    4
#define GUICANVAS_ALIGNY_CENTER_BOTTOM   5
#define GUICANVAS_ALIGNY_BOTTOM_OUTSIDE  6

#define GUICANVAS_WIDTH_SET(flags,flag)   (((~(0x1<<6))&flags)|((flag)<<6))
#define GUICANVAS_WIDTH_GET(flags)      (((flags)>>6)&0x1)
#define GUICANVAS_WIDTH_ABSOLUTE  0
#define GUICANVAS_WIDTH_RELATIVE  1

#define GUICANVAS_HEIGHT_SET(flags,flag)  (((~(0x1<<7))&flags)|((flag)<<7))
#define GUICANVAS_HEIGHT_GET(flags)     (((flags)>>7)&0x1)
#define GUICANVAS_HEIGHT_ABSOLUTE   0
#define GUICANVAS_HEIGHT_RELATIVE   1

//#define GUICANVAS_NEEDS_REALIGNMENT_SET(flags)    (((~(0x1<<8))&flags)|(0x1<<8))
//#define GUICANVAS_NEEDS_REALIGNMENT_CLEAR(flags)  ((~(0x1<<8))&flags)
//#define GUICANVAS_NEEDS_REALIGNMENT_GET(flags)    (((flags)>>8)&0x1)

#define GUICANVAS_DEPTH_SET(flags,flag)  (((~(0x1<<9))&flags)|((flag)<<9))
#define GUICANVAS_DEPTH_GET(flags)     (((flags)>>9)&0x1)
#define GUICANVAS_DEPTH_ABSOLUTE   0
#define GUICANVAS_DEPTH_RELATIVE   1


#include "guigenericcanvas.h"


/**
 * The canvas structures of the GUI define how elements are positioned and rendered
 */
class GUICanvas : public GUIGenericCanvas
{
  public:

    /** 
     * Initializes this class
     */
    GUICanvas();

    /**
     * Cleans up linkage when this class disappears
     */
    ~GUICanvas();

    void setSourceCanvas(GUIGenericCanvas* source_canvas);

    /**
     * Specifies this canvas's x-coordinate by defining the distance from the
     * source canvas's left edge to this canvas's left edge
     */
    void alignXLeft(int x);

    /**
     * Specifies this canvas's x-coordinate by defining the distance from the
     * source canvas's left edge to this canvas's right edge (0 places this
     * canvas to the left of the source canvas)
     */
    void alignXLeftOutside(int x);

    /**
     * Specifies this canvas's x-coordinate by defining the distance from the
     * source canvas's center to this canvas's center
     */
    void alignXCenter(int x);

    /**
     * Specifies this canvas's x-coordinate by defining the distance from the
     * source canvas's right side to this canvas's right side
     */
    void alignXRight(int x);

    /**
     * Specifies this canvas's x-coordinate by defining the distance from the
     * source canvas's right edge to this canvas's left edge (0 places this
     * canvas to the right of the source canvas)
     */
    void alignXRightOutside(int x);

    /**
     * Specifies this canvas's y-coordinate by defining the distance from the
     * source canvas's top to this canvas's top
     */
    void alignYTop(int y);

    /**
     * Specifies this canvas's y-coordinate by defining the distance from the
     * source canvas's top to this canvas's bottom
     */
    void alignYTopOutside(int y);

    /**
     * Specifies this canvas's y-coordinate by defining the distance from the
     * source canvas's center to this canvas's center
     */
    void alignYCenter(int y);

    /**
     * Specifies this canvas's y-coordinate by defining the distance from the
     * source canvas's bottom to this canvas's bottom
     */
    void alignYBottom(int y);

    /**
     * Specifies this canvas's y-coordinate by defining the distance from the
     * source canvas's bottom to this canvas's top
     */
    void alignYBottomOutside(int y);

    /**
     * Sets this canvas's width to a definite pixel size
     */
    void absoluteWidth(int pixels);

    /**
     * Sets this canvas's height to a definite pixel size
     */
    void absoluteHeight(int pixels);

    /**
     * Makes this canvas's width relative to its source canvas
     */
    void relativeWidth(int pixelsBigger);

    /**
     * Makes this canvas's height relative to its source canvas
     */
    void relativeHeight(int pixelsBigger);

    /**
     * Sets the depth of this canvas to be the parent's depth + this depth value
     */
    void relativeDepth(float depth);

    /**
     * Sets the absolute depth of this canvas
     */
    void absoluteDepth(float depth);

    /**
     * Determines whether or not the given point is inside of this canvas
     */
    bool pointInside(GUIPoint point);


  protected:

    /**
     * Resets all of the members of this class
     */
    void zero();


  protected:

    /// The relative coordinates for aligning this canvas
    GUIPoint myAlignmentLocation;

    /// The actual or relative size of this canvas (as specified by the flags)
    GUISize myAlignmentSize;

    /// How the alignment X/Y and width/height values should be interpreted when realigning
    unsigned long myAlignmentFlags;

    /// The actual or relative depth of this canvas
    float myAlignmentDepth;


  private:

    /**
     * Recalculates this canvas's coordinates
     */
    virtual void realign();
};




#endif