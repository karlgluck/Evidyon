#ifndef __CALENDAR_H__
#define __CALENDAR_H__
#pragma once





//----[  CalendarDay  ]--------------------------------------------------------
enum CalendarDay {
  SUNDAY,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY,
  DAYS_IN_WEEK
};

const char* CalendarDayString(CalendarDay day);




//----[  CalendarWeek  ]-------------------------------------------------------
enum CalendarWeek {
  WEEK_FIRST,
  WEEK_SECOND,
  WEEK_THIRD,
  WEEK_LAST, // can be more than 7 days...
  WEEKS_IN_MONTH,
};

const char* CalendarWeekString(CalendarWeek week);



//----[  CalendarMonth  ]------------------------------------------------------
enum CalendarMonth {
  JANUARY,
  FEBRUARY,
  MARCH,
  APRIL,
  MAY,
  JUNE,
  JULY,
  AUGUST,
  SEPTEMBER,
  OCTOBER,
  NOVEMBER,
  DECEMBER,
  MONTHS_IN_YEAR,
};

const char* CalendarMonthString(CalendarMonth month);


#endif