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
#include "guicanvas.h"
#include "guimouse.h"
#include <math.h>


GUIMouseSentinel GUIMouseInputSource::myPrimarySentinel;


//-----------------------------------------------------------------------------
// Name: GUIMouseInputSource
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIMouseInputSource::GUIMouseInputSource()
{
  myCurrentPosition.zero();
  myPrimaryButtonDown = 0.0f;
  myAlternateButtonDown = 0.0f;
  myScrollWheelAccumulator = 0.0f;
}



//-----------------------------------------------------------------------------
// Name: isPrimaryButtonDown
// Desc: Determines whether or not the primary button is down
//-----------------------------------------------------------------------------
bool GUIMouseInputSource::isPrimaryButtonDown() const
{
  return myPrimaryButtonDown;
}



//-----------------------------------------------------------------------------
// Name: isAlternateButtonDown
// Desc: Determines whether or not the alternate button is down
//-----------------------------------------------------------------------------
bool GUIMouseInputSource::isAlternateButtonDown() const
{
  return myAlternateButtonDown;
}


//-----------------------------------------------------------------------------
// Name: getMousePosition
// Desc: Gets the mouse's current position
//-----------------------------------------------------------------------------
GUIPoint GUIMouseInputSource::getMousePosition() const
{
  return myCurrentPosition;
}


//-----------------------------------------------------------------------------
// Name: getPrimarySentinel
// Desc: Gets the primary sentinel to which messages are passed
//-----------------------------------------------------------------------------
GUIMouseSentinel* GUIMouseInputSource::getPrimarySentinel() {
  return &myPrimarySentinel;
}



//-----------------------------------------------------------------------------
void GUIMouseInputSource::generateMovementUpdate() {
  GUIPoint invalid_old_position = { -1, -1 };
  myPrimarySentinel.onMouseMove(myCurrentPosition, invalid_old_position, this);
}


//-----------------------------------------------------------------------------
void GUIMouseInputSource::click() {
  myPrimarySentinel.onMouseButtonAction(GUIMOUSE_CLICK_SINGLE, myCurrentPosition, this);
}





//-----------------------------------------------------------------------------
// Name: mouseMovedTo
// Desc: Called by the derived class to update the mouse's location on the screen
//-----------------------------------------------------------------------------
void GUIMouseInputSource::mouseMovedTo(GUIPoint position)
{
  // Update the mouse's position
  GUIPoint oldPosition = myCurrentPosition;
  myCurrentPosition = position;

  // Update the sentinels
  myPrimarySentinel.onMouseMove(position, oldPosition, this);
}



//-----------------------------------------------------------------------------
// Name: mouseButtonAction
// Desc: Called by the derived class to update the mouse with action
//-----------------------------------------------------------------------------
void GUIMouseInputSource::mouseButtonAction(GUIMouseButtonAction action)
{
  // Change the button states
  switch(action)
  {
    case GUIMOUSE_DOWN:   myPrimaryButtonDown = true;  break;
    case GUIMOUSE_UP:     myPrimaryButtonDown = false;   break;
    case GUIMOUSE_ALT_DOWN: myAlternateButtonDown = true;  break;
    case GUIMOUSE_ALT_UP:   myAlternateButtonDown = false; break;
  }

  // Update the sentinels
  myPrimarySentinel.onMouseButtonAction(action, myCurrentPosition, this);
}



//-----------------------------------------------------------------------------
// Name: mouseScroll
// Desc: Called when the mouse moves.  'amount' is the value / value per line
//-----------------------------------------------------------------------------
void GUIMouseInputSource::mouseScroll(float amount) {
  float new_amount = myScrollWheelAccumulator + amount;
  int lines = 0;
  float extra = 0.0f;
  if (new_amount < 0.0f) {
    extra = floor(-new_amount);
    lines = (int)(new_amount - extra);
  } else {
    extra = floor(new_amount);
    lines = (int)(new_amount - extra);
  }
  myScrollWheelAccumulator = new_amount;
  myPrimarySentinel.onMouseScroll(myCurrentPosition, amount, lines, this);
}

