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
#ifndef __EVIDYONGUISLIDER_H__
#define __EVIDYONGUISLIDER_H__
#pragma once




/**
 * This slider is used to scroll through the list
 */
class EvidyonGUISlider : public GUISlider
{
    enum HandleGraphic
    {
        NEUTRAL,
        MOUSEOVER,
        DRAGGING
    };

public:

    /**
     * Changes the element being used to draw the slider handle
     */
    void setHandleGraphic(HandleGraphic graphic);

    /**
     * Called when the slider's state changes relative to user input.  If
     * the state change is that the slider's position changed, this method
     * updates the location of the list.
     */
    virtual void onStateChange(StateChange stateChange);


    /**
     * Initializes this slider
     */
    EvidyonGUISlider();

    /**
     * Adds this sentinel to the given input chain
     */
    void setParentMouseSentinel(GUIMouseSentinel* parentSentinel);

    /**
     * Removes this sentin from the input chain
     */
    void removeFromMouseInputChain();


protected:

    /**
     * Called when the handle moves
     */
    virtual void onChangedPosition(float position) = 0;
};


#endif