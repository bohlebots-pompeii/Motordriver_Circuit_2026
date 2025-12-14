//
// Created by julius on 11.11.2025.
//

#ifndef MOTORDRIVERMB_BOT_H
#define MOTORDRIVERMB_BOT_H
#include <Arduino.h>

class Bot {
public:
  static void init();

  static void update();

private:
  static void kick(int time);

  static void motor(int num, int motorSpeed);

  static void omnidrive(int8_t vx, int8_t vy, int rotation);

  static void onReceive(int numBytes);

  static int8_t _vx;
  static int8_t _vy;

  static int _rotation;

  static int _dribblerSpeed;
};
#endif //MOTORDRIVERMB_BOT_H