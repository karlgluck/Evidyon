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