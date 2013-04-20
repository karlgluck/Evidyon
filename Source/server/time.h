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
#ifndef __TIME_H__
#define __TIME_H__
#pragma once


namespace Evidyon {
namespace Time {
extern double current_frame;
static const double MAIN_LOOP_PERIOD = 1.0 / 500.0;

// Increments once per minute (30 second resolution).  Never loops.  This
// can be treated as a reliable source for timing events.
extern unsigned long minute_counter;

// Increments once per hour (10 minute resolution).  Never loops.  This
// can be treated as a reliable source for timing long events, such
// as duration between PKs and character account time.
extern unsigned long hour_counter;

// Day, week and month indices that go along with the shared/Calendar
// spec.  Updated by updateMinuteHourCounters.
//  calendar_weekday: 0 = sunday, 7 = saturday
//  calendar_week: day of month / 7, rounded down, maximum value is 3.
//  calendar_month: 0-january, 11-december
extern unsigned long calendar_weekday, calendar_week, calendar_month;

// Called every 30 seconds or so by the main loop to set the value of
// the global 'hour_counter' and 'minute_counter' variables.
void updateMinuteHourCounters();

}
}

#endif
