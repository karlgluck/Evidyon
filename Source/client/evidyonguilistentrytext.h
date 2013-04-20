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
#ifndef __EVIDYONGUILISTENTRYTEXT_H__
#define __EVIDYONGUILISTENTRYTEXT_H__
#pragma once



/**
 * Renders a single line of text for a list entry
 */
class EvidyonGUIListEntryText
{
    public:

        /**
         * Initializes the class
         */
        EvidyonGUIListEntryText(GUIList::ListEntry* owner);

        /**
         * Obtains the canvas for this element
         */
        GUICanvas* getCanvas();

        /**
         * Changes the font used to render this text.  The font must remain
         * valid.
         */
        void setFont(GUIFont* font);

        /**
         * Changes the displayed text
         */
        void setText(const std::string& text);

        /**
         * Changes this text's color
         */
        void setTextColor(D3DCOLOR color);

        /**
         * Registers the visible elements with the screen
         */
        void registerElements(GUILayer* layer);

        /**
         * Removes elements from the given layer 
         */
        void unregisterElements(GUILayer* layer);

        /**
         * Sets the clipping region of the internal elements
         */
        void setClippingArea(const GUIRect* clippingArea);

        GUIFont* getFont() { return myTextLabelElement.getFont(); }
        const std::string& getText() const { return myTextLabelElement.getText(); }
        D3DCOLOR getTextColor() const { return myTextLabelElement.getTextColor(); }
        void setAlignment(Alignment alignment) { myTextLabelElement.setAlignment(alignment); }

    protected:

        /// Canvas onto which to draw this label
        GUICanvas myTextLabelCanvas;

        /// The element with which to render the text
        GUITextLabelElement myTextLabelElement;
};




#endif