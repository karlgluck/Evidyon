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
#include "stdafx.h"



//-----------------------------------------------------------------------------
// Name: registerElements
// Desc: Invokes the method of the same name for all members of this list.  This method
//       assumes that the pointer returned by getListEntry can be cast to a EvidyonListEntry.
//-----------------------------------------------------------------------------
void EvidyonGUIList::registerElements(GUILayer* layer)
{
   unsigned int lines = getNumberOfLines();
   for (unsigned int i = 0; i < lines; ++i)
   {
       EvidyonListEntry* listEntry = (EvidyonListEntry*)getListEntry(i);
       CONFIRM(listEntry) else continue;
       listEntry->registerElements(layer);
   }
}



//-----------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Invokes the method of the same name for all members of this list.  This method
//       assumes that the pointer returned by getListEntry can be cast to a EvidyonListEntry.
//-----------------------------------------------------------------------------
void EvidyonGUIList::unregisterElements(GUILayer* layer)
{
   unsigned int lines = getNumberOfLines();
   for (unsigned int i = 0; i < lines; ++i)
   {
       EvidyonListEntry* listEntry = (EvidyonListEntry*)getListEntry(i);
       CONFIRM(listEntry) else continue;
       listEntry->unregisterElements(layer);
   }
}




//-----------------------------------------------------------------------------
// Name: ListSlider
//-----------------------------------------------------------------------------
EvidyonGUIList::ListSlider::ListSlider(EvidyonGUIList* owner)
{
    myOwner = owner;
}



//-----------------------------------------------------------------------------
// Name: syncToList
// Desc: Synchronizes this slider's position with the list to which it
//       is attached.  This method is safe to call at any time and will
//       not produce an infinite loop.
//-----------------------------------------------------------------------------
void EvidyonGUIList::ListSlider::syncToList()
{
    EvidyonGUIList* owner = myOwner;
    myOwner = 0;    // prevent recursive calling of any sort (future prevention, doesn't do anything now)
    setSliderPosition(owner->getScrollIndexPercent());
    myOwner = owner;
}



//-----------------------------------------------------------------------------
// Name: setListCanvases
// Desc: Initializes this and the list canvas such that the slider
//       and list share the area of the background canvas (both will
//       be linked to this canvas.)  The slider will be positioned
//       on the right.
//-----------------------------------------------------------------------------
void EvidyonGUIList::ListSlider::setListCanvases(GUICanvas* backgroundCanvas)
{
    ASSERT(myOwner) else return;

    getCanvas()->setSourceCanvas(backgroundCanvas);
    getCanvas()->absoluteWidth(16);
    getCanvas()->relativeHeight(0);
    getCanvas()->alignXRight(0);
    getCanvas()->alignYTop(0);
    getCanvas()->relativeDepth(-0.001f);

    myOwner->myTargetCanvas.setSourceCanvas(backgroundCanvas);
    myOwner->myTargetCanvas.relativeDepth(-0.001f);
    myOwner->myTargetCanvas.relativeHeight(0);
    myOwner->myTargetCanvas.relativeWidth(- 16);
    myOwner->myTargetCanvas.alignXLeft(0);
    myOwner->myTargetCanvas.alignYTop(0);

}



//-----------------------------------------------------------------------------
// Name: onChangedPosition
// Desc: Called by the compositor when the handle moves
//-----------------------------------------------------------------------------
void EvidyonGUIList::ListSlider::onChangedPosition(float percentPosition)
{
    if (!myOwner) return;
    myOwner->scrollToPercent(percentPosition);
}


//-----------------------------------------------------------------------------
// Name: EvidyonListEntryWithMouseInput
// Desc: 
//-----------------------------------------------------------------------------
EvidyonGUIList::EvidyonListEntryWithMouseInput::EvidyonListEntryWithMouseInput()
    : myStateMouseSentinel(this)
{
    addChildMouseSentinel(&myStateMouseSentinel);
}



//-----------------------------------------------------------------------------
// Name: StateMouseSentinel
// Desc: Initializes this sentinel with its owner entry
//-----------------------------------------------------------------------------
EvidyonGUIList::EvidyonListEntryWithMouseInput::StateMouseSentinel::StateMouseSentinel(EvidyonListEntryWithMouseInput* owner)
{
    myOwner = owner;
}



//-----------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves
//       @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool EvidyonGUIList::EvidyonListEntryWithMouseInput::StateMouseSentinel::onMouseMove(
    GUIPoint position,
    GUIPoint oldPosition,
    GUIMouseInputSource* mouse)
{
    CONFIRM(myOwner) else return false;

    const GUIRect* lineArea = myOwner->getLineArea();
    const GUIRect* clipArea = myOwner->getClippingArea();

    bool insideOld = lineArea->contains(oldPosition) && clipArea->contains(oldPosition);
    bool insideNew = lineArea->contains(position)    && clipArea->contains(position);

    // Trigger state changes
    if (!insideOld &&  insideNew) myOwner->onMouseAction(mouse->isPrimaryButtonDown() ? MOUSEOVERANDDOWN : MOUSEOVER);
    else if ( insideOld && !insideNew) myOwner->onMouseAction(MOUSEOUT);

    return false;
}



//-----------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state
//       @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool EvidyonGUIList::EvidyonListEntryWithMouseInput::StateMouseSentinel::onMouseButtonAction(
    GUIMouseButtonAction action,
    GUIPoint position,
    GUIMouseInputSource* mouse)
{
    const GUIRect* lineArea = myOwner->getLineArea();
    const GUIRect* clipArea = myOwner->getClippingArea();

    // If this point is inside the list entry, do something with it
    if (lineArea->contains(position) && clipArea->contains(position))
    {
        switch(action)
        {
            case GUIMOUSE_DOWN:         myOwner->onMouseAction(MOUSEOVERANDDOWN);  break;
            case GUIMOUSE_UP:           myOwner->onMouseAction(MOUSEOVER); break;
            case GUIMOUSE_CLICK_SINGLE: myOwner->onMouseAction(CLICKED); break;
        }
    }

    // Don't capture this action
    return false;
}