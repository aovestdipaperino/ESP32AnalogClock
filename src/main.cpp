#include <Arduino.h>
#include <BLEDevice.h>
#include "interface.h"
#include "motor.h"
#include "globals.h"
#include "RTC.h"
#include "debug.h"
#include "driver/adc.h"

#define uS_TO_S_FACTOR 1000000

#define DELAY pdMS_TO_TICKS(1)

void timeStep()
{
  struct tm t = getRTCTime();
  writeTime((uint8_t)t.tm_hour, (uint8_t)t.tm_min, false);
}

void unfreezeClock()
{
  timeStep();
}

void timeStepTask(void *unused)
{
  while (true)
  {
    if (freeze || OTA::isUpdating())
    {
      delay(1000);
      continue;
    }

    timeStep();
    delay(5000); // wait 5 secs
  }
}

uint32_t time_alive;
const uint8_t SECS_AWAKE = 1;
const uint8_t SECS_ASLEEP = 59; // move once per minute.

RTC_DATA_ATTR uint currentHourAngle;
RTC_DATA_ATTR uint currentMinutesAngle;

void setup()
{
  // Set the motor state correctly;
  hour.resetAngle(currentHourAngle);
  minute.resetAngle(currentMinutesAngle);
  // Tic... tock...
  unfreezeClock();

  setCpuFrequencyMhz(80);

  // Turn off all the wifi.
  adc_power_off();
  WiFi.disconnect(true); // Disconnect from the network
  WiFi.mode(WIFI_OFF);   // Switch WiFi off

  Serial.begin(115200);

  esp_bt_controller_disable();

  xTaskCreate(
      timeStepTask, /* Task function. */
      "timeStep",   /* String with name of task. */
      10000,        /* Stack size in bytes. */
      NULL,         /* Parameter passed as input of the task */
      10,           /* Priority of the task. */
      NULL);

  time_alive = millis() + 1 * 1000;

  Serial.println("alive");
}

uint32_t lastTick = 0;
uint32_t minTick = UINT32_MAX, maxTick = 0;
bool movingManually = false;
const uint8_t MAX_SPEED = 30;
const uint32_t INERTIA_TIME_MS = 50;
const uint8_t DEFAULT_WAKEUP_DELAY = 59; // once per minute, to really save battery when not performing.

bool pin33;
bool pin34;
bool pin35;
RTC_DATA_ATTR int bootCount = 0;
void goToSleep()
{
  // save motor state
  // set wakeup time in N secs
  // go to deep sleep.
  Serial.printf("Sleeping for %d secs.\n", 59);
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  esp_sleep_enable_timer_wakeup(59 * uS_TO_S_FACTOR);

  // Save the current motor state.
  currentHourAngle = hour.currentAngle();
  currentMinutesAngle = minute.currentAngle();
  esp_deep_sleep_start();
}

void lightSleep(uint64_t millis)
{
  esp_sleep_enable_timer_wakeup(millis * 1000);
  esp_light_sleep_start();
}

void timeTick()
{
  static uint64_t lastStep = 0;
  if (freeze)
  {
    return;
  }
  if ((millis() - lastStep) > 5000)
  {
    lastStep = millis();
    timeStep();
  }
}

void loop()
{

  // PRIORITIES:
  // 1. do not interrupt OTA updates.
  // if (OTA::isUpdating())
  // {
  //   delay(100);
  //   return;
  // }
  // 2. handle current movements.
  if (minute.tick() || hour.tick())
  {
    // keep moving for the tick.
    return;
  }
  lightSleep(100);
  // 3. if handle encoder we are done.

  // 4. handle Serial.
  // handleSerial();

  timeTick();

  goToSleep();
}
