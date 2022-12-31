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
#ifndef __GUIGENERICCANVAS_H__
#define __GUIGENERICCANVAS_H__
#pragma once



#include "guirect.h"


// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);         \
  void operator=(const TypeName&)



class GUIGenericCanvas {
public:
  GUIGenericCanvas();
  virtual ~GUIGenericCanvas();

  GUIGenericCanvas* getSourceCanvas();

  // Returns 'true' if the canvas has children that reference it.  This can be
  // used to identify when the canvas is no longer being used.
  // Be aware that canvases do NOT reference count, and this cannot be used to
  // determine whether or not a standalone pointer or reference to this canvas
  // exists.
  bool isSourceCanvas() const;

  // Obtains the canvas's area.  This method will update the canvas's
  // alignment if necessary.  The return value is a reference to the
  // internal area, and is guaranteed to be valid as long as the
  // canvas itself exists.
  const GUIRect* getArea();

  // Updates the canvas's alignment (if necessary) and returns the
  // rendering (Z-buffer) depth of this canvas.
  float getDepth();

protected:

  // This data should be updated by the child class when realign() is
  // invoked.
  GUIRect area_;  // Pixel location of this canvas
  float depth_;   // Z-depth (visible in the range of 0.0-1.0)

protected:

  // Changes the canvas on which this this canvas bases its alignment.
  // Passing NULL is allowed.  For most cases, making this method public
  // is most useful; however, it is trivial to write a stub that calls
  // this method in a subclass--however, all of the canvas-multipliers
  // (canvas matrix, canvas array, etc.) are served by this being made
  // inaccessible.
  void changeSourceCanvas(GUIGenericCanvas* source_canvas);

  // Call this method when a change that affects area_ or depth_
  // occurs outside of realign().  This causes all child canvases
  // to need to be realigned.
  void changedAlignment();

  // Realigns all parents (if they need it) then invokes realign() for this
  // canvas and resets the alignment flag so that needsRealignment() returns
  // false.
  void realignIfNecessary();

private:

  // These methods modify the linked canvas tree.  However, they are NOT safe
  // to use on their own and are employed only as a tool to make the method
  // changeSourceCanvas() shorter.  They do not, for instance, update the
  // changedAlignment flag or change the source canvas reference.
  // addChildCanvas assumes that canvas->sibling_canvas_ == NULL.
  // removeChildCanvas assumes that child->sibling_canvas_ will be set to
  // NULL after the method is called (or subsequently overwritten)
  void addChildCanvas(GUIGenericCanvas* canvas);
  void removeChildCanvas(GUIGenericCanvas* child);

  // Invoked automatically so that the subclass can update area_ and
  // depth_.  It is guaranteed that the source canvas, if it exists,
  // will be properly aligned before this method is called.
  virtual void realign();

private:
  GUIGenericCanvas* source_canvas_;
  GUIGenericCanvas* first_child_canvas_;
  GUIGenericCanvas* sibling_canvas_;

private:
  bool aligned_;

private:
  DISALLOW_COPY_AND_ASSIGN(GUIGenericCanvas);
};



#endif