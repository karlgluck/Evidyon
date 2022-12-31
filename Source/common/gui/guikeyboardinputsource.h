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
#ifndef __GUIKEYBOARDINPUTSOURCE_H__
#define __GUIKEYBOARDINPUTSOURCE_H__
#pragma once



/**
 * Defines the base class for an input source that provides keyboard input
 */
class GUIKeyboardInputSource
{
  public:

    /**
     * Initializes this class
     */
    GUIKeyboardInputSource();

    /**
     * Gets the primary sentinel to which messages are passed.  All elements of
     * the keyboard input chain should be children (at some point in the hierarchy)
     * of this sentinel.
     */
    static GUIKeyboardSentinel* getPrimarySentinel();

    /**
     * Determines whether or not the indicated key is pressed
     */
    bool isKeyDown(GUIKey key) const;


  protected:

    /**
     * This method is used by the derived class that implements this input source to
     * notify that the given key was pressed (down == true) or released (down == false)
     */
    void keyMoved(GUIKey key, bool down);

    /**
     * Used by the derived class to notify that the key for the designated character
     * was pressed and released ("typed")
     */
    void typedCharacter(char character);


  private:

    /// This sentinel is the root for all of the keyboard sentinels
    static GUIKeyboardSentinel myPrimarySentinel;

    /// This is the state of each of the keys
    bool myKeyDown[GUIKEY_COUNT];
};



#endif