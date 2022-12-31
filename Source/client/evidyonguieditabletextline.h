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