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
#ifndef __DCXWIN32TIME_H__
#define __DCXWIN32TIME_H__


namespace dcx {

    /**
     * Implements the time interface for the Windows platform
     */
    class dcxWin32Clock : public dcxClock
    {
        public:

            /**
             * Gets the current wall time using the accurate Windows system timer
             */
            double getWallTime() const;


        public:

            /**
             * High-resolution (if available) global method for obtaining a time value.  The
             * time is returned in seconds, NOT milliseconds.
             * @return Current system time in seconds
             */
            static double getAccurateSystemTime();


        protected:

            /// Number of counts per second, if using high-resolution timing.
            static double theQPFrequency;

            /// The time at which getAccurateSystemTime() was first called.  All high-resolution timing
            /// values are offsets from this point.  If high-resolution timing is not available, this value
            /// is 1 and the default timing method will be used (which is not offset).  If the value is zero,
            /// the timer hasn't been initialized yet.
            static LARGE_INTEGER theStartTime;
    };
}




#endif