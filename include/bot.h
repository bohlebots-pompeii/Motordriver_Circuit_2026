//
// Created by julius on 11.11.2025.
//

#ifndef MOTORDRIVERMB_BOT_H
#define MOTORDRIVERMB_BOT_H

class Bot {
public:
  static void init();

  static void update();

private:
  static void kick(int time);

  static void motor(int num, int motorSpeed);

  static void omnidrive(int vx, int vy, int rotation);

  static void onReceive(int numBytes);

  static int _vx;
  static int _vy;

  static int _rotation;

  static int _dribblerSpeed;
};
#endif //MOTORDRIVERMB_BOT_H