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
#ifndef __DCXWIN32MOUSE_H__
#define __DCXWIN32MOUSE_H__


// TODO: test class functionality (it may not be complete) and unify naming conventions/cleanup
// the code inside to use the new debug system


#define HOTSPOT_OUTSIDE 0xFFFFFFFE /* a click occurred outside of the hotspots */
#define HOTSPOT_NOCLICK 0xFFFFFFFF /* nothing was clicked */


namespace dcx {

    /**
     * Manages input from the mouse.  Rendering the cursor and any other duties are the responsibility
     * of classes that use data from this class.
     */
    class dcxWin32Mouse
    {
	    public:

		    /**
		     * Defines accessible buttons on the mouse
		     */
		    enum Button
		    {
			    // Physical button references
			    BUTTON_LEFT = 0,
			    BUTTON_MIDDLE,
			    BUTTON_RIGHT,
			    BUTTON_XBUTTON1, // disabled
			    BUTTON_XBUTTON2, // disabled

			    // How many buttons are maintained
			    BUTTON_COUNT,

			    /// References ANY mouse button
			    BUTTON_ANY,

			    /// No mouse button
			    BUTTON_NONE,
		    };

		    /**
		     * Declares the different states that a button can be in.  This is used
		     * instead of a simple true/false flag because the structure will be the
		     * same size (so no memory is wasted) and it is more explicit.
		     */
		    enum ButtonState
		    {
			    /// The button is pressed
			    BUTTONSTATE_DOWN,

			    /// The button is not pressed
			    BUTTONSTATE_UP,
		    };

            struct ButtonClickRecord
            {
                bool PushedFlag;
                POINTS PushedPoint;
                bool ReleasedFlag;
                POINTS ReleasedPoint;
            };


        public:

            /**
             * Initializes the class
             */
            dcxWin32Mouse();

            /**
             * Set up the class for use
             *   @param screenWidth The pixel-width (horizontal resolution) of the screen
             *   @param screenHeight The pixel-height (vertical resolution) of the screen
             *   @return Result code
             */
            void initialize( unsigned long screenWidth, unsigned long screenHeight );

            /**
             * Updates the internal mouse status
             *   @return Result code
             */
            void query();

            /**
             * Erases all of the mouse's input information
             */
            void reset();

            /**
             * Get rid of any memory or allocated objects used by the program
             */
            void unloadResources();

            /**
             * Processes Windows messages
             */
            LRESULT handleWindowsMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult );

            /**
             * Obtains the area in which the user clicked the mouse; if the user has not clicked the
             * mouse then this method returns FALSE and the area referenced is not modified. This
             * method will continue to return the same mouse click throughout the frame, until the
             * next Query.
             *   @param Button The button to check for clicking
             *   @param pDown The starting coordinates of the click
             *   @param pUp The ending coordinates of the click
             *   @return Whether or not the mouse clicked and the returned points will be valid
             */
            bool getRealClick( Button button, POINTS* pDown, POINTS* pUp ) const;

            /**
             * Does the same thing as GetRealClick, but scales the coordinates into the standard
             * 800x600 screen.
             *   @param Button The button to check for clicking
             *   @param pDown The starting coordinates of the click
             *   @param pUp The ending coordinates of the click
             *   @return Whether or not the mouse clicked and the returned points will be valid
             */
            bool getScaledClick( Button button, POINTS* pDown, POINTS* pUp  ) const;

            /**
             * Determines whether or not the designated button is pressed.  If this is BUTTON_ANY,
             * if any of button is pressed the method will return 'true'.  If passed BUTTON_NONE,
             * it will return true only if none of the buttons are down.
             */
            bool isButtonDown( Button button ) const;

            /**
             * Determines whether or not the designated button is released.  If this is BUTTON_ANY,
             * it returns 'true' if any button is released.  If passed BUTTON_NONE, it returns 'true'
             * if none of the buttons are released.
             */
            bool isButtonUp( Button button ) const;

            /**
             * Checks to see if a button was pushed.  This works in a similar way for special
             * button indices as isButtonUp.
             */
            bool isButtonPushed( Button button, POINTS* realCoord, POINTS* scaledCoord ) const;

            /**
             * Checks to see if a button was released.  This works in a similar way for special
             * button indices as isButtonDown.
             */
            bool isButtonReleased( Button button, POINTS* realCoord, POINTS* scaledCoord ) const;

            /**
             * Determines whether or not the mouse moved between the last frame and this frame
             */
            bool isMoved() const;

            /**
             * Obtains the location of the mouse.  This method will only store values in the
             * parameters if they are valid.
             *   @param realCoord Current mouse in pixels from the top-left corner of the screen
             *   @param scaledCoord Coordinate of the mouse on an 800x600 screen
             */
            void getCoordinates( POINTS* realCoord, POINTS* scaledCoord ) const;

            /**
             * When provided with a list of rectangles, this method will search the list and find
             * the hotspot that the cursor is inside.  Note that the hotspots should be specified
             * as if they were on an 800x600 screen.
             * TODO: at some point, add a "unsigned long startIndex = 0" to index the start of the hotspot array
             *   @param hotspotArray The list of rectangles to check
             *   @param hotspotCount How many rectangles are in the list
             *   @return The index of the hotspot that the mouse is inside, or HOTSPOT_OUTSIDE
             */
            unsigned long getCursorInScaledHotspot( const RECT* hotspotArray, unsigned long hotspotCount );

            /**
             * Checks to see whether or not the mouse has been clicked inside of one of the given
             * regions.  Note that the hotspots should be specified as if they were on an 800x600
             * screen.
             *   @param button The button for which to check for clicks
             *   @param hotspotArray How many rectangles are in the list
             *   @param hotspotCount The cursor type to set if the mouse is in a hotspot
             *   @return The index of the hotspot that was clicked or HOTSPOT_OUTSIDE if an area
             *           outside of all of the hotspots was clicked, or HOTSPOT_NOCLICK if the
             *           mouse was not clicked.
             */
            unsigned long getClickInScaledHotspot( Button button, const RECT* hotspotArray, unsigned long hotspotCount );

            /**
             * Gets a reference to the internal mouse position variable.  This is used so that other
             * classes can reference the mouse location without explicitly referencing this class.
             * The return value should be treated as read-only, even though C++ can't enforce this.
             * @return Reference to the mouse's location (updated whenever Query is called)
             */
            const POINTS* getMousePositionReference();


        protected:

            /**
             * Resets all internal variables
             */
            void zero();

            /**
             * Scales the given point to the equivalent on an 800x600 display
             */
            void scaleToDefaultResolution( POINTS* ppt ) const;

            /**
             * Called when a button is pressed on the mouse.  This will activate mouse capture and log the
             * state of the button and the coordinates at which the button was pressed.
             */
            void onButtonDown( HWND hWnd, Button button );

            /**
             * Called when a button on the mouse is released.  This will release mouse capture, change the
             * state of the button and log the coordinates at which it was released.
             */
            void onButtonUp( Button button );


        protected:

            /// Multiplier for horizontal coordinates that transforms from the screen to 800x600 resolution
            float myScaleFactorX;

            /// Multiplier for vertical coordinates that transforms from the screen to 800x600 resolution
            float myScaleFactorY;

            /// This is the ID of the first mouse button that was pressed when no other
            /// buttons were down.  It is used to keep track of mouse clicks.
            Button myFirstButtonPressed;

            /// The real location at which the button was pressed
            POINTS myFirstButtonPressedPoint;

            /// Whether or not the button that was pressed has been released (i.e. clicked)
            bool myFirstButtonReleased;

            /// The real coordinate at which the button was released
            POINTS myFirstButtonReleasedPoint;

            /// A bank of mouse button states that is actively updated
            ButtonState myWorkingButtonsState[16];

            /// A bank of current mouse button states for this frame
            ButtonState myCurrentButtonsState[16];

            /// The last frame's mouse button states (for comparison)
            ButtonState myLastButtonsState[16];

            /// Records button states in order to handle mouse clicks
            ButtonClickRecord myButtonClickRecords[16];

            /// The mouse's position (updated between frames)
            POINTS myWorkingPosition;

            /// The mouse position as of the most recent frame
            POINTS myCurrentPosition;

            /// The mouse position from the last frame
            POINTS myLastPosition;
    };
}


#endif