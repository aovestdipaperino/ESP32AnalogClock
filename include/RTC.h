#ifndef RTC_H
#define RTC_H
#include "time.h"
#include <sys/time.h>

tm getRTCTime();
void setRTCTime(int sc, int mn, int hr);
#endif