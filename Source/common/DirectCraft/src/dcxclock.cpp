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
//-----------------------------------------------------------------------------
// File:    dcxclock.cpp
// Desc:    Provides a generic interface for timing methods
//-----------------------------------------------------------------------------
#include "../dcx/dcxclock.h"

// TODO: get rid of reliance on windows!!!!
#include <windows.h>


using namespace dcx;


//-----------------------------------------------------------------------------
// Name:  getWallTime
// Desc:  Gets the current wall time in seconds
//-----------------------------------------------------------------------------
double dcxClock::getWallTime() const
{
    return getSystemTime() / 1000.0;
}



//-----------------------------------------------------------------------------
// Name:  getSystemTime
// Desc:  Low-resolution, global method for obtaining a time value in milliseconds
//-----------------------------------------------------------------------------
unsigned long dcxClock::getSystemTime()
{
    return timeGetTime();
}

