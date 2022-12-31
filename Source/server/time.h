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
