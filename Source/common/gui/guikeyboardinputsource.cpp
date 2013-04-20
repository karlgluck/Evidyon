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
#include "guikeyboard.h"



GUIKeyboardSentinel GUIKeyboardInputSource::myPrimarySentinel;


//-----------------------------------------------------------------------------
// Name: GUIKeyboardInputSource
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIKeyboardInputSource::GUIKeyboardInputSource()
{
  for (int i = 0; i < (int)GUIKEY_COUNT; ++i)
    myKeyDown[i] = false;
}


//-----------------------------------------------------------------------------
// Name: getPrimarySentinel
// Desc: Gets the primary sentinel to which messages are passed
//-----------------------------------------------------------------------------
GUIKeyboardSentinel* GUIKeyboardInputSource::getPrimarySentinel()
{
  return &myPrimarySentinel;
}




//-----------------------------------------------------------------------------
// Name: isKeyDown
// Desc: Determines whether or not the indicated key is pressed
//-----------------------------------------------------------------------------
bool GUIKeyboardInputSource::isKeyDown(GUIKey key) const
{
  return myKeyDown[key];
}



//-----------------------------------------------------------------------------
// Name: keyMoved
// Desc: This method is used by the derived class that implements this input source to
//     notify that the given key was pressed (down == true) or released (down == false)
//-----------------------------------------------------------------------------
void GUIKeyboardInputSource::keyMoved(GUIKey key, bool down)
{
  // Don't change key states if it's unnecessary
  if (myKeyDown[key] == down) return;

  // Set the key state
  myKeyDown[key] = down;

  // Notify the sentinels of the change in key state
  if (down)
    myPrimarySentinel.onKeyDown(key, this);
  else
    myPrimarySentinel.onKeyUp(key, this);
}



//-----------------------------------------------------------------------------
// Name: typedCharacter
// Desc: Used by the derived class to notify that the key for the designated character
//     was pressed and released ("typed")
//-----------------------------------------------------------------------------
void GUIKeyboardInputSource::typedCharacter(char character)
{
  // Notify the sentinels that the character was typed
  myPrimarySentinel.onTypedCharacter(character, this);
}

