#pragma once
#include <stdint.h>

void writeTime(uint8_t _hour, uint8_t _minute, bool writeToRtc);
void query();
void setTimeRemote(uint8_t h, uint8_t m);
void setHandsRemote(uint8_t h, uint8_t m);
void setAngleRemote(uint8_t h, uint8_t m);
void move(bool hour, bool forward, uint8_t steps);
void go();
