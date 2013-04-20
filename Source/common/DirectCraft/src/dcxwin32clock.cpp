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
//-----------------------------------------------------------------------------
// File:    dcxwin32clock.h
// Desc:    Provides a high-precision timer for the Win32 environment, using the
//          QueryPerformanceFrequency/QueryPerformanceTimer methods, if they are available.
//-----------------------------------------------------------------------------
#include <windows.h>
#include "../dcx/dcxclock.h"
#include "../dcx/dcxwin32clock.h"
#include "../dc/debug"


using namespace dcx;

// Resolve the frequency
double dcx::dcxWin32Clock::theQPFrequency = -1.0;
LARGE_INTEGER dcx::dcxWin32Clock::theStartTime = { 0 };


//-----------------------------------------------------------------------------
// Name:  getWallTime
// Desc:  Gets the current wall time using the preferred method of the implementing timer
//-----------------------------------------------------------------------------
double dcxWin32Clock::getWallTime() const
{
    return getAccurateSystemTime();
}


//-----------------------------------------------------------------------------
// Name:  getAccurateSystemTime
// Desc:  High-resolution (if available) global method for obtaining a time value
//-----------------------------------------------------------------------------
double dcxWin32Clock::getAccurateSystemTime()
{
    if( !theStartTime.QuadPart )
    {
        // Determine if high-performance timing is enabled
        LARGE_INTEGER qpFrequency;
        if( QueryPerformanceFrequency( &qpFrequency ) )
        {
            // Output information
            DEBUG_INFO( "Application using high-performance timer (freq = %I64i)\n", qpFrequency.QuadPart );

            // Cast the frequency into a double
            theQPFrequency = (double)qpFrequency.QuadPart;

            // Get the timer start time
            LARGE_INTEGER time;
            QueryPerformanceCounter( &time );
            theStartTime.QuadPart = time.QuadPart;
        }
        else
        {
            // Print information
            DEBUG_INFO( "Application NOT using high-performance timer\n" );

            // Disable high resolution timing
            theQPFrequency = -1.0;

            // Set up the system timer to have the maximum resolution possible
            TIMECAPS timeCaps;
            APP_WARNING( TIMERR_NOERROR != timeGetDevCaps( &timeCaps, sizeof(timeCaps) ) ||
                         TIMERR_NOERROR != timeBeginPeriod( timeCaps.wPeriodMin ) )
                 ( "Unable to adjust system timer for maximum resolution" );

            // Enable low-resolution timing
            theStartTime.QuadPart = 1;
        }

        // Get the time
        return getAccurateSystemTime();
    }
    else if( theStartTime.QuadPart == 1 )
    {
        // Use the default timer
        return dcxClock::getSystemTime();
    }
    else
    {
        // Get the current time
        LARGE_INTEGER time;
        QueryPerformanceCounter( &time );

        // Offset time from the application start to prevent double-precision overflow
        time.QuadPart -= theStartTime.QuadPart;

        // Calculate the number of seconds represented by this value
        double quadPart = (double)time.QuadPart;
        double currentTime = quadPart / theQPFrequency;

        // Return the difference
        return currentTime;
    }
}

