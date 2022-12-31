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

