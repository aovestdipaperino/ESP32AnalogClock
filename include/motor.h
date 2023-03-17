#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "globals.h"
const uint8_t STEPS[6][3] = {{HIGH, LOW, HIGH}, {HIGH, LOW, LOW}, {HIGH, HIGH, LOW}, {LOW, HIGH, LOW}, {LOW, HIGH, HIGH}, {LOW, LOW, HIGH}};

#define DELAY pdMS_TO_TICKS(1)
#define CCW -1
#define CW 1

class Motor
{
private:
  const char *motorName;
  uint8_t PIN1;
  uint8_t PIN2;
  uint8_t PIN3;
  uint16_t currentAngleDividedByTwo = 0;
  int8_t movementDirection = 0;
  int16_t macroStepToMove = 180;
  uint8_t currentStep = 0;

  void step()
  {
    digitalWrite(PIN1, STEPS[movementDirection == CW ? currentStep : 5 - currentStep][0]);
    digitalWrite(PIN2, STEPS[movementDirection == CW ? currentStep : 5 - currentStep][1]);
    digitalWrite(PIN3, STEPS[movementDirection == CW ? currentStep : 5 - currentStep][2]);
  }

public:
  Motor(const char *name, uint8_t p1, uint8_t p2, uint8_t p3) : motorName(name), PIN1(p1), PIN2(p2), PIN3(p3)
  {
    pinMode(PIN1, OUTPUT);
    pinMode(PIN2, OUTPUT);
    pinMode(PIN3, OUTPUT);
  }

  bool tick()
  {
    if (!movementDirection || macroStepToMove <= 0)
    {
      digitalWrite(PIN1, LOW);
      digitalWrite(PIN2, LOW);
      digitalWrite(PIN3, LOW);

      return false;
    }
    // Serial.print(".");
    //  Serial.printf("moving %s %d.%d\n", motorName, macroStepToMove, currentStep);
    step();
    currentStep++;
    if (currentStep == 6)
    {
      if (movementDirection == CW)
      {
        currentAngleDividedByTwo += 1;
        currentAngleDividedByTwo %= 180;
      }
      else
      {
        currentAngleDividedByTwo = (currentAngleDividedByTwo) ? currentAngleDividedByTwo - 1 : 179;
        currentAngleDividedByTwo %= 180;
      }
      macroStepToMove--;
      currentStep = 0;
    }

    return true;
  }

  void stop()
  {
    macroStepToMove = 0;
  }

  float remainder = 0;
  void relative(float n)
  {
    if (debug)
      Serial.printf("Relative %s by: %f\n", motorName, n);

    n = n > 0 ? (n + remainder) : -(n + remainder);
    macroStepToMove = n;
    remainder = n - (float)macroStepToMove;
    movementDirection = n > 0 ? CW : CCW;
  }

  void forward(uint16_t n)
  {

    if (debug)
      Serial.printf("Forward %s by: %d\n", motorName, n);
    macroStepToMove = n;
    movementDirection = CW;
  }
  void backward(uint16_t n)
  {
    if (debug)
      Serial.printf("Backward %s by: %d\n", motorName, n);
    macroStepToMove = n;
    movementDirection = CCW;
  }

  bool moveTo(uint16_t n)
  {

    if (n < 0 || n > 180 || (currentAngleDividedByTwo == n))
      return false;

    if (debug)
      Serial.printf("%s at: %d\n", motorName, currentAngleDividedByTwo * 2);
    if (debug)
      Serial.printf("Moving %s to: %d\n", motorName, n * 2);
    int16_t fSteps = n - currentAngleDividedByTwo;
    if (debug)
      Serial.printf("fSteps: %d\n", fSteps);
    if (fSteps < 0)
    {
      if (fSteps < -90)
      {
        forward(180 + fSteps);
      }
      else
      {
        backward(-fSteps);
      }
    }
    else
    {
      if (fSteps > 90)
      {
        backward(180 - fSteps);
      }
      else
      {
        forward(fSteps);
      }
    }
    return true;
  }

  void resetAngle(uint16_t angle = 0)
  {
    currentAngleDividedByTwo = angle;
  }

  void printAngle()
  {
    Serial.printf("Current %s angle: %d\n", motorName, currentAngleDividedByTwo * 2);
  }

  bool isMoving()
  {
    return macroStepToMove > 0;
  }

  uint16_t currentAngle()
  {
    return this->currentAngleDividedByTwo;
  }
};

extern Motor hour, minute;

#endif