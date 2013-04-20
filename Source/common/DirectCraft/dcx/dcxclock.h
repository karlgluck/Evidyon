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
#ifndef __DCXCLOCK_H__
#define __DCXCLOCK_H__



namespace dcx {

/**
 * Interface implemented by a timing class
 */
class dcxClock {
public:

    /**
     * Gets the current wall time in seconds using the preferred method
     * of the implementing timer.  The default time interface uses the
     * CRT library's low-resolution timer.
     */
    virtual double getWallTime() const;


public:

    /**
     * Low-resolution, global method for obtaining a time value in milliseconds
     */
    static unsigned long getSystemTime();
};

}

#endif