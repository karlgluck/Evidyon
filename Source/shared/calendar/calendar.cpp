#include "calendar.h"



//----[  CalendarDayString  ]--------------------------------------------------
const char* CalendarDayString(CalendarDay day) {
  switch (day) {
  case SUNDAY: return "Sunday";
  case MONDAY: return "Monday";
  case TUESDAY: return "Tuesday";
  case WEDNESDAY: return "Wednesday";
  case THURSDAY: return "Thursday";
  case FRIDAY: return "Friday";
  case SATURDAY: return "Saturday";
  }
}


//----[  CalendarWeekString  ]-------------------------------------------------
const char* CalendarWeekString(CalendarWeek week) {
  switch (week) {
    case WEEK_FIRST:  return "First";
    case WEEK_SECOND: return "Second";
    case WEEK_THIRD:  return "Third";
    case WEEK_LAST:   return "Last Weeks";
  }
}


//----[  CalendarMonthString  ]------------------------------------------------
const char* CalendarMonthString(CalendarMonth month) {
  switch (month) {
    case JANUARY: return "January";
    case FEBRUARY: return "February";
    case MARCH: return "March";
    case APRIL: return "April";
    case MAY: return "May";
    case JUNE: return "June";
    case JULY: return "July";
    case AUGUST: return "August";
    case SEPTEMBER: return "September";
    case OCTOBER: return "October";
    case NOVEMBER: return "November";
    case DECEMBER: return "December";
  }
}

