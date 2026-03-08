//
// Created by julius on 11.11.2025.
//

#ifndef MOTORDRIVERMB_BOT_H
#define MOTORDRIVERMB_BOT_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h" // alternative to https://docs.arduino.cc/language-reference/en/functions/interrupts/noInterrupts/https://docs.arduino.cc/language-reference/en/functions/interrupts/noInterrupts/
#include "freertos/task.h" // ^^

class Bot {
public:
  static void init();
  static void update();

private:
  struct __attribute__((packed)) MotorCmd {
    uint8_t flags;
    int8_t vx;
    int8_t vy;
    int8_t rot;
    int8_t drib;
  };

  static void motor(int num, int16_t motorSpeed);
  static void omnidrive(int8_t vx, int8_t vy, int8_t rotation);
  static void onReceive(int numBytes);
  static void handleKick();

  static volatile int8_t _vx;
  static volatile int8_t _vy;
  static volatile int8_t _rotation;
  static volatile int8_t _dribblerSpeed;

  static volatile bool _kickRequest;
  static volatile unsigned long _lastCmdTime;

  static portMUX_TYPE _mux;

  static bool _isKicking;
  static unsigned long _kickStartTime;
  static unsigned long _lastKickTime;
};

#endif