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
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guilayer.h"
#include "guifont.h"
#include "guitextelement.h"
#include "guitextlabelelement.h"
#include "guikeyboard.h"
#include "guimouse.h"
#include "guicomponent.h"
#include "guibutton.h"


//-----------------------------------------------------------------------------
// Name: GUIButton
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIButton::GUIButton() : myMouseSentinel(this)
{
  myPushedFlag = false;
  myEnabledFlag = true;
  myText.setSourceCanvas(getCanvas());
}



//-----------------------------------------------------------------------------
// Name: ~GUIButton
// Desc: Allow the subclass to clean up
//-----------------------------------------------------------------------------
GUIButton::~GUIButton()
{
}



//-----------------------------------------------------------------------------
// Name: push
// Desc: Pushes this button
//-----------------------------------------------------------------------------
void GUIButton::push()
{
  // If this button isn't enabled, it can't be pushed
  if (!myEnabledFlag) return;

  // Set the pushed state
  myPushedFlag = true;

  // Call the 'pushed' routine
  onStateChange(STATECHANGE_PUSHED);
}



//-----------------------------------------------------------------------------
// Name: wasPushed
// Desc: Determines whether or not this button was pushed
//-----------------------------------------------------------------------------
bool GUIButton::wasPushed() const
{
  return myPushedFlag;
}



//-----------------------------------------------------------------------------
// Name: resetPushed
// Desc: Resets the button's pushed state to false.  This method should be called after
//     examining wasPushed() and the information is no longer needed.
//-----------------------------------------------------------------------------
void GUIButton::resetPushed()
{
  if (myPushedFlag)
  {
    myPushedFlag = false;
    onStateChange(STATECHANGE_RESET);
  }
}



//-----------------------------------------------------------------------------
// Name: pollPushed
// Desc: 
//-----------------------------------------------------------------------------
bool GUIButton::pollPushed() {
  if (myPushedFlag) {
    myPushedFlag = false;
    onStateChange(STATECHANGE_RESET);
    return true;
  } else {
    return false;
  }
}



//-----------------------------------------------------------------------------
// Name: enable
// Desc: Allows this button to respond to user input
//-----------------------------------------------------------------------------
void GUIButton::enable()
{
  if (!myEnabledFlag)
  {
    resetPushed();
    myEnabledFlag = true;
  }

  onStateChange(STATECHANGE_ENABLED);
}



//-----------------------------------------------------------------------------
// Name: disable
// Desc: Prevents this button from responding to user input
//-----------------------------------------------------------------------------
void GUIButton::disable()
{
  if (myEnabledFlag)
  {
    resetPushed();
    myEnabledFlag = false;
  }

  onStateChange(STATECHANGE_DISABLED);
}



bool GUIButton::isEnabled() const {
  return myEnabledFlag;
}



//-----------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers this button's visible elements with the given layer
//-----------------------------------------------------------------------------
void GUIButton::registerElements(GUILayer* layer)
{
  layer->registerElement(&myText);
}


//-----------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Unregisters this button's visible elements
//-----------------------------------------------------------------------------
void GUIButton::unregisterElements(GUILayer* layer)
{
  layer->unregisterElement(&myText);
}


/*
//-----------------------------------------------------------------------------
// Name: getCanvas
// Desc: Gets the canvas for this button so that it can be positioned
//-----------------------------------------------------------------------------
GUICanvas* GUIButton::getCanvas()
{
  return &myCanvas;
}*/



//-----------------------------------------------------------------------------
// Name: getMouseSentinel
// Desc: Gets this button's mouse sentinel so that it can be added to something which triggers its events
//-----------------------------------------------------------------------------
GUIMouseSentinel* GUIButton::getMouseSentinel()
{
  return &myMouseSentinel;
}


GUITextLabelElement* GUIButton::getTextLabelElement() {
  return &myText;
}



//-----------------------------------------------------------------------------
// Name: onPushed
// Desc: This method is called when the button is pushed
//-----------------------------------------------------------------------------
void GUIButton::onStateChange(GUIButton::StateChange stateChange)
{
}

void GUIButton::setClippingArea(const GUIRect* area) {
  myText.setClippingArea(area);
}


//-----------------------------------------------------------------------------
// Name: MouseSentinel
// Desc: Initializes this button sentinel
//-----------------------------------------------------------------------------
GUIButton::MouseSentinel::MouseSentinel(GUIButton* button)
{
  myButton = button;
  myMouseOverFlag = false;
}



//-----------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIButton::MouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
  // If the button is disabled or if its canvas it doesn't exist, don't do anything
  GUICanvas* buttonCanvas = myButton->getCanvas();
  if (!myButton->myEnabledFlag || !buttonCanvas) return false;

  bool insideOld = buttonCanvas->pointInside(oldPosition);
  bool insideNew = buttonCanvas->pointInside(position);

  const GUIRect* clipping_area = myButton->myText.getClippingArea();
  if (clipping_area) {
    insideOld = insideOld && clipping_area->contains(oldPosition);
    insideNew = insideNew && clipping_area->contains(position);
  }

  // Trigger state changes
  if ((!myMouseOverFlag || !insideOld) && insideNew) {
    myButton->onStateChange(mouse->isPrimaryButtonDown() ? GUIButton::STATECHANGE_MOUSEOVERANDDOWN : GUIButton::STATECHANGE_MOUSEOVER);
    myMouseOverFlag = true;
  }
  else if ((myMouseOverFlag || insideOld) && !insideNew && !hasMouseFocus()) { // only free the mouse if we don't have focus
    myButton->onStateChange(GUIButton::STATECHANGE_MOUSEOUT);
    myMouseOverFlag = false;
  }

  // Capture if the button has focus
  return hasMouseFocus();
}



//-----------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIButton::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)
{
  // Get the button's canvas; if it doesn't exist, don't do anything
  GUICanvas* buttonCanvas = myButton->getCanvas();
  if (!myButton->myEnabledFlag || !buttonCanvas) return false;

  const GUIRect* clipping_area = myButton->myText.getClippingArea ();

  // If this point is inside the button, do something with it
  if (buttonCanvas->pointInside(position) && (clipping_area == NULL || clipping_area->contains(position)))
  {
    switch(action)
    {
      case GUIMOUSE_DOWN:     myButton->onStateChange(GUIButton::STATECHANGE_MOUSEOVERANDDOWN); obtainMouseFocus(); return true;
      case GUIMOUSE_UP:       myButton->onStateChange(GUIButton::STATECHANGE_MOUSEOVER); return true;
      case GUIMOUSE_CLICK_SINGLE: myButton->push(); return true;
    }
  } else if (hasMouseFocus()) {
    //if (action == GUIMOUSE_UP) {
    myButton->onStateChange(GUIButton::STATECHANGE_MOUSEOUT);
    myMouseOverFlag = false;
    releaseMouseFocus();
    return true;
    //}
  }

  // Don't capture this action
  return false;
}

