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
#include "evidyonguidefs.h"


//----[  EvidyonGUISlider  ]---------------------------------------------------
EvidyonGUISlider::EvidyonGUISlider()
{
    getBackgroundElement()->setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
    getHandleElement()->setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
    setHandleGraphic(NEUTRAL);
}



//-----------------------------------------------------------------------------
// Name: setParentMouseSentinel
// Desc: Adds this sentinel to the given input chain
//-----------------------------------------------------------------------------
void EvidyonGUISlider::setParentMouseSentinel(GUIMouseSentinel* parentSentinel)
{
    getMouseSentinel()->setParentMouseSentinel(parentSentinel);
    getMouseSentinel()->obtainMouseFocus();
}



//-----------------------------------------------------------------------------
// Name: removeFromMouseInputChain
// Desc: Removes this sentin from the input chain
//-----------------------------------------------------------------------------
void EvidyonGUISlider::removeFromMouseInputChain()
{
    getMouseSentinel()->removeFromMouseSentinelChain();
}



//-----------------------------------------------------------------------------
// Name: setHandleGraphic
// Desc: Changes the element being used to draw the slider handle
//-----------------------------------------------------------------------------
void EvidyonGUISlider::setHandleGraphic(HandleGraphic graphic)
{
    // Make sure the background graphic is set
    {
        getBackgroundElement()->setSourceRegion(&EvidyonGUIDefs::SLIDER_BACKGROUND, 1, EvidyonGUIDefs::TEXTURE_SIZE);
    }

    // TODO: based on state
    {
        getHandleElement()->setSourceRegion(&EvidyonGUIDefs::SLIDER_HANDLE, EvidyonGUIDefs::TEXTURE_SIZE);
    }
}



//-----------------------------------------------------------------------------
// Name: onStateChange
// Desc: Called when the slider's state changes relative to user input.  If
//       the state change is that the slider's position changed, this method
//       updates the location of the list.
//-----------------------------------------------------------------------------
void EvidyonGUISlider::onStateChange(StateChange stateChange)
{
    switch(stateChange)
    {
        case STATECHANGE_MOUSEOVER_BACKGROUND:          break;
        case STATECHANGE_MOUSEOVERANDDOWN_HANDLE:       setHandleGraphic(DRAGGING);
        case STATECHANGE_MOUSEOVER_HANDLE:
        case STATECHANGE_MOUSEOVER_HANDLEANDBACKGROUND: setHandleGraphic(MOUSEOVER); break;
        case STATECHANGE_MOUSEOUT:                      setHandleGraphic(NEUTRAL);
        case STATECHANGE_HANDLEMOVED:
            onChangedPosition(calculateSliderPercentLocation());
            break;
    }
}

