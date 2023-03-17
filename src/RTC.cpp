#include <Arduino.h>
#include "RTC.h"

/*!
    @brief  get the internal RTC time as a tm struct
*/
tm getRTCTime()
{
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  return timeinfo;
}

void setRTCTime(int sc, int mn, int hr)
{
  // seconds, minute, hour, day, month, year $ microseconds(optional)
  // ie setTime(20, 34, 8, 1, 4, 2021) = 8:34:20 1/4/2021
  struct tm t;     // Initalize to all 0's
  t.tm_year = 121; // This is year-1900, so 121 = 2021
  t.tm_mon = 11;
  t.tm_mday = 14;
  t.tm_hour = hr;
  t.tm_min = mn;
  t.tm_sec = sc;
  time_t timeSinceEpoch = mktime(&t);
  struct timeval tv;
  tv.tv_sec = timeSinceEpoch; // epoch time (seconds)
  tv.tv_usec = 0;             // microseconds
  settimeofday(&tv, NULL);
}