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
#ifndef __GUIMOUSEINPUTSOURCE_H__
#define __GUIMOUSEINPUTSOURCE_H__
#pragma once



#include "guipoint.h"

/**
 * Defines the base class for an input source that provides keyboard input
 */
class GUIMouseInputSource
{
  public:

    /**
     * Initializes this class
     */
    GUIMouseInputSource();

    /**
     * Determines whether or not the primary button is down
     */
    bool isPrimaryButtonDown() const;

    /**
     * Determines whether or not the alternate button is down
     */
    bool isAlternateButtonDown() const;

    /**
     * Gets the mouse's current position
     */
    GUIPoint getMousePosition() const;

    /**
     * Gets the primary sentinel to which messages are passed.  All elements of
     * the mouse input chain should be children (at some point in the hierarchy)
     * of this sentinel.
     */
    static GUIMouseSentinel* getPrimarySentinel();

    /**
     * Forces this input source to propogate a mouse-movement message with the
     * current position as both the old and new locations.  This is used to
     * update the GUI when changes to dynamic components could cause the mouse
     * location to become out of sync with the GUI's display.
     */
    void generateMovementUpdate();

    /**
     * Forces the mouse to move to the given location.
     */
    //virtual void jumpTo(GUIPoint point) = 0;

    // Forces a primary-button mouse click to propagate
    void click();


  protected:

    /**
     * Called by the derived class to update the mouse's location on the screen
     */
    void mouseMovedTo(GUIPoint position);

    /**
     * Called by the derived class to update the mouse with action
     */
    void mouseButtonAction(GUIMouseButtonAction action);

    /**
     * Called when the mouse moves.  'amount' is the value / value per line
     */
    void mouseScroll(float amount);


  private:

    /// This sentinel is the root for all of the keyboard sentinels
    static GUIMouseSentinel myPrimarySentinel;

    /// The mouse's position
    GUIPoint myCurrentPosition;

    /// The mouse's button states
    bool myPrimaryButtonDown, myAlternateButtonDown;

    /// Once this reaches +/- 1.0f, it sends a "scroll line" message
    float myScrollWheelAccumulator;
};



#endif