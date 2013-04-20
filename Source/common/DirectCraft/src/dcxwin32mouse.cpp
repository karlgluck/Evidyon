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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../dcx/dcxwin32mouse.h"


using namespace dcx;


//-----------------------------------------------------------------------------
// Name:  dcxWin32Mouse
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
dcxWin32Mouse::dcxWin32Mouse()
{
    zero();
}


//-----------------------------------------------------------------------------
// Name:  initialize
// Desc:  Set up the class for use
//-----------------------------------------------------------------------------
void dcxWin32Mouse::initialize( unsigned long screenWidth, unsigned long screenHeight )
{
    // Set up the scaling factors
    myScaleFactorX = 800.0f / screenWidth;
    myScaleFactorY = 600.0f / screenHeight;
}


//-----------------------------------------------------------------------------
// Name:  query
// Desc:  Updates the internal mouse status
//-----------------------------------------------------------------------------
void dcxWin32Mouse::query()
{
    // Copy the current state into the previous state
    memcpy_s( &myLastButtonsState, sizeof(myLastButtonsState),
           &myCurrentButtonsState, sizeof(myCurrentButtonsState) );

    // Copy the working state into the current state
    memcpy_s( &myCurrentButtonsState, sizeof(myCurrentButtonsState),
              &myWorkingButtonsState, sizeof(myWorkingButtonsState) );

    // Reset the working state
    //ZeroMemory( &myWorkingButtonsState, sizeof(myWorkingButtonsState) );

    // Move the mouse positions around
    myLastPosition = myCurrentPosition;
    myCurrentPosition = myWorkingPosition;
    //ZeroMemory( &myWorkingPosition, sizeof(myWorkingPosition) );

    // Go through all of the buttons and update their click states
    for( int btnIndex = 0; btnIndex < (int)BUTTON_COUNT; ++btnIndex )
    {
        // Reset the click flags after a frame so that a single click is doesn't span
        // multiple queries
        if( myButtonClickRecords[btnIndex].ReleasedFlag )
        {
            myButtonClickRecords[btnIndex].PushedFlag = false;
            myButtonClickRecords[btnIndex].ReleasedFlag = false;
        }

        // Check the click states and save them into the record
        if( isButtonPushed( (Button)btnIndex, 0, 0 ) )
        {
            myButtonClickRecords[btnIndex].PushedFlag = true;
            myButtonClickRecords[btnIndex].PushedPoint = myCurrentPosition;
        }
        else if( isButtonReleased( (Button)btnIndex, 0, 0 ) )
        {
            myButtonClickRecords[btnIndex].ReleasedFlag = true;
            myButtonClickRecords[btnIndex].ReleasedPoint = myCurrentPosition;
        }
    }
}


//-----------------------------------------------------------------------------
// Name:  reset
// Desc:  Erases all of the mouse's input information
//-----------------------------------------------------------------------------
void dcxWin32Mouse::reset()
{
    // We can't just call Zero here because that might get rid of variables we don't want to
    // change, such as the graphics pointer.
    myFirstButtonPressed = BUTTON_NONE;
    ZeroMemory( &myFirstButtonPressedPoint, sizeof(myFirstButtonPressedPoint) );
    myFirstButtonReleased = false;
    ZeroMemory( &myFirstButtonReleasedPoint, sizeof(myFirstButtonReleasedPoint) );
    ZeroMemory( &myWorkingButtonsState, sizeof(myWorkingButtonsState) );
    ZeroMemory( &myCurrentButtonsState, sizeof(myCurrentButtonsState) );
    ZeroMemory( &myLastButtonsState, sizeof(myLastButtonsState) );
    ZeroMemory( &myButtonClickRecords, sizeof(myButtonClickRecords) );
    ZeroMemory( &myWorkingPosition, sizeof(myWorkingPosition) );
    ZeroMemory( &myCurrentPosition, sizeof(myCurrentPosition) );
    ZeroMemory( &myLastPosition, sizeof(myLastPosition) );
}


//-----------------------------------------------------------------------------
// Name:  unloadResources
// Desc:  Get rid of any memory or allocated objects used by the program
//-----------------------------------------------------------------------------
void dcxWin32Mouse::unloadResources()
{
    // Reset the class
    zero();
}


//-----------------------------------------------------------------------------
// Name:  handleWindowsMessage
// Desc:  Processes Windows messages
//-----------------------------------------------------------------------------
LRESULT dcxWin32Mouse::handleWindowsMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult )
{
    // By default, we don't need to return a result
    *returnResult = false;

    // Handle the message
    switch( uMsg )
    {
        case WM_MOUSEMOVE:
        {
            // Get the point structure
            POINTS pt = MAKEPOINTS(lParam);

            // Save the location of the mouse
            myWorkingPosition.x = pt.x;
            myWorkingPosition.y = pt.y;

        } break;

        case WM_LBUTTONDOWN: onButtonDown( hWnd, BUTTON_LEFT ); break;
        case WM_LBUTTONUP:   onButtonUp( BUTTON_LEFT );         break;

        case WM_MBUTTONDOWN: onButtonDown( hWnd, BUTTON_MIDDLE ); break;
        case WM_MBUTTONUP:   onButtonUp( BUTTON_MIDDLE );         break;

        case WM_RBUTTONDOWN: onButtonDown( hWnd, BUTTON_RIGHT ); break;
        case WM_RBUTTONUP:   onButtonUp( BUTTON_RIGHT );         break;

        // todo: extend this to XBUTTON2 (using wparam).  note: potentially, we can just have all of these
        // cases lead to the same place and use the wParam to determine states.  That might be nifty.
        //case WM_XBUTTONDOWN: myWorkingButtonsState[BUTTON_XBUTTON1] = BUTTONSTATE_DOWN; SetCapture(hWnd); break;
        //case WM_XBUTTONUP:   myWorkingButtonsState[BUTTON_XBUTTON1] = BUTTONSTATE_UP; ReleaseCapture(); break;
    }

    // Success
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name:  getRealClick
// Desc:  Obtains the area in which the user clicked the mouse
//-----------------------------------------------------------------------------
bool dcxWin32Mouse::getRealClick( Button button, POINTS* pushedReal, POINTS* releasedReal ) const
{
    // Check to see if this is a special button
    if( button == BUTTON_ANY )
    {
        // Used to iterate through the buttons
        int btnIndex = 0;

        // Look through all of the buttons to see if any was clicked
        for( btnIndex = 0; btnIndex < (int)BUTTON_COUNT; ++btnIndex )
        {
            // Check the release flag for the button
            if( myButtonClickRecords[btnIndex].PushedFlag &&
                myButtonClickRecords[btnIndex].ReleasedFlag )
            {
                // Exit the loop
                break;
            }
        }

        // Save the index of the button that was clicked (if this is invalid, the next statement
        // below catches it)
        button = (Button)btnIndex;
    }

    // Make sure the index is valid
    if( button >= BUTTON_COUNT )
        return false;

    // Check the designated button
    if( myButtonClickRecords[button].PushedFlag && myButtonClickRecords[button].ReleasedFlag )
    {
        if( pushedReal )   *pushedReal   = myButtonClickRecords[button].PushedPoint;
        if( releasedReal ) *releasedReal = myButtonClickRecords[button].ReleasedPoint;

        // There was a click
        return true;
    }
    else
        return false;
}


//-----------------------------------------------------------------------------
// Name:  getScaledClick
// Desc:  Does the same thing as GetRealClick, but scales the coordinates
//-----------------------------------------------------------------------------
bool dcxWin32Mouse::getScaledClick( Button button, POINTS* pDown, POINTS* pUp ) const
{
//	todo:  check ptrs

    POINTS down, up;
    if( getRealClick( button, &down, &up ) )
    {
        // Scale these coordinates
        scaleToDefaultResolution( &down );
        scaleToDefaultResolution( &up );

        // Save the coordinates
        *pDown = down;
        *pUp = up;

        // There was a click
        return true;
    }
    else
        return false;
}


//-----------------------------------------------------------------------------
// Name:  isButtonDown
// Desc:  
//-----------------------------------------------------------------------------
bool dcxWin32Mouse::isButtonDown( Button button ) const
{
    // First, if the button is a valid real button, just check its state
    if( button < BUTTON_COUNT )
        return myCurrentButtonsState[button] == BUTTONSTATE_DOWN;

    // Check any of the buttons; if any of them is down return
    else if( button == BUTTON_ANY )
    {
        for( int i = 0; i < (int)BUTTON_COUNT; ++i )
            if( myCurrentButtonsState[i] == BUTTONSTATE_DOWN ) return true;
        return false;
    }

    // Check to make sure none of the buttons are down
    else if( button == BUTTON_NONE )
    {
        for( int i = 0; i < (int)BUTTON_COUNT; ++i )
            if( myCurrentButtonsState[i] == BUTTONSTATE_DOWN ) return false;
        return true;
    }

    // This is an invalid button
    else
        return false;
}


//-----------------------------------------------------------------------------
// Name:  isButtonUp
// Desc:  
//-----------------------------------------------------------------------------
bool dcxWin32Mouse::isButtonUp( Button button ) const
{
    // First, if the button is a valid real button, just check its state
    if( button < BUTTON_COUNT )
        return myCurrentButtonsState[button] == BUTTONSTATE_UP;

    // Check any of the buttons; if any of them is up return
    else if( button == BUTTON_ANY )
    {
        for( int i = 0; i < (int)BUTTON_COUNT; ++i )
            if( myCurrentButtonsState[i] == BUTTONSTATE_UP ) return true;
        return false;
    }

    // Check to make sure none of the buttons are up
    else if( button == BUTTON_NONE )
    {
        for( int i = 0; i < (int)BUTTON_COUNT; ++i )
            if( myCurrentButtonsState[i] == BUTTONSTATE_UP ) return false;
        return true;
    }

    // This is an invalid button
    else
        return false;
}


//-----------------------------------------------------------------------------
// Name:  isButtonPushed
// Desc:  
//-----------------------------------------------------------------------------
bool dcxWin32Mouse::isButtonPushed( Button button, POINTS* realCoord, POINTS* scaledCoord ) const
{
    // First, if the button is a valid real button, just check its state
    if( button < BUTTON_COUNT )
    {
        // Check the push flag
        bool pushed = (myLastButtonsState[button] == BUTTONSTATE_UP) && 
                      (myCurrentButtonsState[button] == BUTTONSTATE_DOWN);

        // Save the coordinate, if necessary
        if( pushed ) getCoordinates( realCoord, scaledCoord );

        // Return the result
        return pushed;
    }

    // Check any of the buttons; if any of them is up return
    else if( button == BUTTON_ANY )
    {
        for( int i = 0; i < (int)BUTTON_COUNT; ++i )
            if( isButtonPushed( (Button)i, realCoord, scaledCoord ) ) return true;
        return false;
    }

    // Check to make sure none of the buttons are up
    else if( button == BUTTON_NONE )
    {
        for( int i = 0; i < (int)BUTTON_COUNT; ++i )
            if( isButtonPushed( (Button)i, realCoord, scaledCoord ) ) return false;
        return true;
    }

    // This is an invalid button
    else
        return false;
}


//-----------------------------------------------------------------------------
// Name:  isButtonReleased
// Desc:  
//-----------------------------------------------------------------------------
bool dcxWin32Mouse::isButtonReleased( Button button, POINTS* realCoord, POINTS* scaledCoord ) const
{
    // First, if the button is a valid real button, just check its state
    if( button < BUTTON_COUNT )
    {
        // Check to see if the button is released
        bool released = (myLastButtonsState[button] == BUTTONSTATE_DOWN) && 
                        (myCurrentButtonsState[button] == BUTTONSTATE_UP);

        // Save the coordinate, if necessary
        if( released ) getCoordinates( realCoord, scaledCoord );

        // Return the result
        return released;
    }

    // Check any of the buttons; if any of them is up return
    else if( button == BUTTON_ANY )
    {
        for( int i = 0; i < (int)BUTTON_COUNT; ++i )
            if( isButtonReleased( (Button)i, realCoord, scaledCoord ) ) return true;
        return false;
    }

    // Check to make sure none of the buttons are up
    else if( button == BUTTON_NONE )
    {
        for( int i = 0; i < (int)BUTTON_COUNT; ++i )
            if( isButtonReleased( (Button)i, realCoord, scaledCoord ) ) return false;
        return true;
    }

    // This is an invalid button
    else
        return false;
}




//-----------------------------------------------------------------------------
// Name:  isMoved
// Desc:  Determines whether or not the mouse moved between the last frame and this frame
//-----------------------------------------------------------------------------
bool dcxWin32Mouse::isMoved() const
{
    return (myCurrentPosition.x != myLastPosition.x) || (myCurrentPosition.y != myLastPosition.y);
}




//-----------------------------------------------------------------------------
// Name:  getCoordinates
// Desc:  Obtains the location of the mouse
//-----------------------------------------------------------------------------
void dcxWin32Mouse::getCoordinates( POINTS* realCoord, POINTS* scaledCoord ) const
{
    // Save the coordinate, if necessary
    if( realCoord )
        *realCoord = myCurrentPosition;

    // Save the scaled coordinate
    if( scaledCoord )
    {
        *scaledCoord = myCurrentPosition;
        scaleToDefaultResolution( scaledCoord );
    }
}



//-----------------------------------------------------------------------------
// Name:  getCursorInScaledHotspot
// Desc:  Determines which hotspot the cursor is inside
//-----------------------------------------------------------------------------
unsigned long dcxWin32Mouse::getCursorInScaledHotspot( const RECT* hotspotArray, unsigned long hotspotCount )
{
//todo: check ptr
    // Get the scaled mouse coordinates
    POINTS ptShortMouse;
    getCoordinates( 0, &ptShortMouse );
	POINT pointMouse = { ptShortMouse.x, ptShortMouse.y };

    // Go through the list of hotspots
    for( DWORD hotspot = 0; hotspot < hotspotCount; ++hotspot )
    {
        // Return the index if the mouse is in the hotspot
        if( PtInRect( &hotspotArray[hotspot], pointMouse ) )
            return hotspot;
    }

    // The mouse was outside of all of the hotspots
    return HOTSPOT_OUTSIDE;
}


//-----------------------------------------------------------------------------
// Name:  getClickInScaledHotspot
// Desc:  Changes the cursor depending on where it is
//-----------------------------------------------------------------------------
unsigned long dcxWin32Mouse::getClickInScaledHotspot( Button button, const RECT* hotspotArray, unsigned long hotspotCount )
{
//todo: check ptr
    // Scan for clicks
    POINTS ptShortDown, ptShortUp;
    if( getScaledClick( button, &ptShortDown, &ptShortUp ) )
    {
		// Convert to the POINT structure, which uses LONG instead of SHORT for its values.
		POINT ptDown = { ptShortDown.x, ptShortDown.y },
				ptUp = { ptShortUp.x,   ptShortUp.y   };

        // Iterate through all of the hotspots in the list
        for( DWORD hotspot = 0; hotspot < hotspotCount; ++hotspot )
        {
            // The area to check
            const RECT* pCheckRect = &(hotspotArray[hotspot]);

            // If both the press and the release are inside of the hotspot, then we have
            // a click here!
            if( PtInRect( pCheckRect, ptDown ) && PtInRect( pCheckRect, ptUp ) )
                return hotspot;
        }

        // An area outside of all of the hotspots was clicked
        return HOTSPOT_OUTSIDE;
    }
    else
        // Nothing was clicked
        return HOTSPOT_NOCLICK;
}



//-----------------------------------------------------------------------------
// Name:  getMousePositionReference
// Desc:  Gets a reference to the internal mouse position variable
//-----------------------------------------------------------------------------
const POINTS* dcxWin32Mouse::getMousePositionReference()
{
    return &myCurrentPosition;
}



//-----------------------------------------------------------------------------
// Name:  zero
// Desc:  Resets all internal variables
//-----------------------------------------------------------------------------
void dcxWin32Mouse::zero()
{
    myScaleFactorX = 1.0f;
    myScaleFactorY = 1.0f;
    myFirstButtonPressed = BUTTON_NONE;
    ZeroMemory( &myFirstButtonPressedPoint, sizeof(myFirstButtonPressedPoint) );
    myFirstButtonReleased = false;
    ZeroMemory( &myFirstButtonReleasedPoint, sizeof(myFirstButtonReleasedPoint) );
    ZeroMemory( &myWorkingButtonsState, sizeof(myWorkingButtonsState) );
    ZeroMemory( &myCurrentButtonsState, sizeof(myCurrentButtonsState) );
    ZeroMemory( &myLastButtonsState, sizeof(myLastButtonsState) );
    ZeroMemory( &myButtonClickRecords, sizeof(myButtonClickRecords) );
    ZeroMemory( &myWorkingPosition, sizeof(myWorkingPosition) );
    ZeroMemory( &myCurrentPosition, sizeof(myCurrentPosition) );
    ZeroMemory( &myLastPosition, sizeof(myLastPosition) );
}



//-----------------------------------------------------------------------------
// Name:  scaleToDefaultResolution
// Desc:  Scales the given point to the equivalent on an 800x600 display
//-----------------------------------------------------------------------------
void dcxWin32Mouse::scaleToDefaultResolution( POINTS* ppt ) const
{
    ppt->x = (short)(ppt->x * myScaleFactorX);
    ppt->y = (short)(ppt->y * myScaleFactorY);
}


//-----------------------------------------------------------------------------
// Name:  onButtonDown
// Desc:  Called when a button is pressed on the mouse
//-----------------------------------------------------------------------------
void dcxWin32Mouse::onButtonDown( HWND hWnd, Button button )
{
    myWorkingButtonsState[button] = BUTTONSTATE_DOWN;
    SetCapture( hWnd );
}


//-----------------------------------------------------------------------------
// Name:  onButtonUp
// Desc:  Called when a button on the mouse is released
//-----------------------------------------------------------------------------
void dcxWin32Mouse::onButtonUp( Button button )
{
    myWorkingButtonsState[button] = BUTTONSTATE_UP;
    ReleaseCapture();
}