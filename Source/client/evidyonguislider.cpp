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

