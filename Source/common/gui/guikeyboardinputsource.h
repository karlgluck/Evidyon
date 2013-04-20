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