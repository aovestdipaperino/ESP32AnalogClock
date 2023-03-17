#include "interface.h"
#include "motor.h"
#include "RTC.h"

void writeTime(uint8_t _hour, uint8_t _minute, bool writeToRtc)
{

  if (_minute <= 59)
  {
    if (minute.moveTo(_minute * 3))
    {
      if (_hour <= 23)
        hour.moveTo((_hour * 15 + (_minute / 4)) % 180);
    }
  }

  if (writeToRtc)
  {
    Serial.println("Setting the RTC.");
    setRTCTime(0, _minute, _hour);
  }
}

void query()
{
  
}

void setTimeRemote(uint8_t h, uint8_t m)
{
  writeTime(h, m, true);
}

void setHandsRemote(uint8_t h, uint8_t m)
{
  writeTime(h, m, false);
  freeze = true;
}

void setAngleRemote(uint8_t h, uint8_t m)
{
  writeTime(h, m, false);
  freeze = true;
}

void move(bool _hour, bool forward, uint8_t steps)
{
  if (!freeze)
  {
    return;
  }
  if (_hour)
  {
    if (forward)
      hour.forward(steps);
    else
      hour.backward(steps);
    hour.resetAngle();
  }
  else
  {
    if (forward)
      minute.forward(steps);
    else
      minute.backward(steps);
    hour.resetAngle();
  }
}

void go()
{
  freeze = false;
  unfreezeClock();
}
