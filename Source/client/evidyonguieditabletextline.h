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
#ifndef __EVIDYONGUIEDITABLETEXTLINE_H__
#define __EVIDYONGUIEDITABLETEXTLINE_H__
#pragma once





#include "common/gui/guieditabletextline.h"
#include "common/gui/guiborderedfillelement.h"

/**
 * Implements a Evidyon-specific editable text line
 */
class EvidyonGUIEditableTextLine : public GUIEditableTextLine
{
    public:

        /**
         * Initializes this class
         */
        EvidyonGUIEditableTextLine();

        /**
         * Calls the methods to set many text line parameters all at once
         */
        void configure(GUIFont* font,
                        bool isPassword, bool restrictToNumbers, D3DCOLOR textColor);

        /**
         * Calls the methods to set many text line parameters all at once
         */
        void configure(GUIFont* font,
                        bool isPassword, bool restrictToNumbers, D3DCOLOR textColor, int textLengthLimit);

        /**
         * Sets the number of characters this line can accept
         */
        void setTextLengthLimit(int characters);

        /**
         * Makes this text line accept any number of characters (default behavior)
         */
        void removeTextLengthLimit();

        /**
         * Changes the flag that restricts input so that only numbers are accepted
         */
        void numbersOnly(bool restrictToNumbers);

        /**
         * Registers this component's visible elements with the given layer.  This method calls
         * the superclass method.
         */
        void registerElements(GUILayer* layer);

        /**
         * Unregisters this component's visible elements.  This method calls the superclass method.
         */
        void unregisterElements(GUILayer* layer);

        void setBackgroundColor(D3DCOLOR color);


    protected:

        /**
         * Invoked when the user enters a valid character that should be added to the internal buffer
         */
        virtual void onTypedPrintableCharacter(char printableCharacter);


    protected:

        /// Whether or not this line only accepts numbers
        bool myNumbersOnlyFlag;

        /// The maximum number of characters (excluding the null-terminator) this text line accepts
        size_t myTextLengthLimit;

        /// The background for this text element.  Uses the superclass's canvas as the source.
        GUIBorderedFillElement myBackground;

        /// The area where this text element is drawn.  This is used to override the source canvas for
        /// the text in the superclass GUIEditableTextLine.  That canvas will instead be the background
        /// canvas for the whole element.
        GUICanvas myTextCanvas;
};


#endif