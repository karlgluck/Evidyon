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
#include "time.h"
#include <windows.h>

namespace Evidyon {
namespace Time {


double current_frame = 0.0;
unsigned long minute_counter = 0;
unsigned long hour_counter = 0;
unsigned long calendar_weekday = 0, calendar_week = 0, calendar_month = 0;

//----[  updateMinuteHourCounters  ]-------------------------------------------
void updateMinuteHourCounters() {
  SYSTEMTIME st;
  GetSystemTime(&st);

  calendar_weekday = st.wDayOfWeek;
  calendar_week = st.wDay >= 22 ? 3 : ((st.wDay-1) / 7); // divide into 4 weeks
  calendar_month = st.wMonth - 1;

  //algorithm:  from RtlTimeToSecondsSince1970
  //Call SystemTimeToFileTime to copy the system time to a FILETIME structure. Call GetSystemTime to get the current system time to pass to SystemTimeToFileTime.
  //Copy the contents of the FILETIME structure to a ULARGE_INTEGER structure.
  //Initialize a SYSTEMTIME structure with the date and time of the first second of January 1, 1970.
  //Call SystemTimeToFileTime, passing the SYSTEMTIME structure initialized in Step 3 to the call.
  //Copy the contents of the FILETIME structure returned by SystemTimeToFileTime in Step 4 to a second ULARGE_INTEGER. The copied value should be greater than or equal to the value copied in Step 2.
  //Subtract the 64-bit value in the ULARGE_INTEGER structure initialized in Step 2 from the 64-bit value of the ULARGE_INTEGER structure initialized in Step 5. This produces a value in 100-nanosecond intervals since January 1, 1970. To convert this value to seconds, divide by 10,000,000.

  FILETIME ft;
  SystemTimeToFileTime(&st, &ft);

  ULARGE_INTEGER uli;
  memcpy(&uli, &ft, sizeof(ft));

  // the base time is Tuesday, May 26 2009 at about 12:04 PM
  ULARGE_INTEGER base_time;
  base_time.QuadPart = 128878273603120000L;

  ULONGLONG ull = uli.QuadPart - base_time.QuadPart;

  // this conversion has to be two steps
  ull /= (10000000L); // convert from hundreds of nanoseconds to seconds...
  minute_counter = unsigned long(ull / 60L);
  hour_counter = unsigned long(ull / 60L / 60L);
}

}
}