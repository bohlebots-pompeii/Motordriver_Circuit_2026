//
// Created by julius on 11.11.2025.
//

#ifndef MOTORDRIVERMB_BOT_H
#define MOTORDRIVERMB_BOT_H

class Bot {
public:
  Bot();
  void init();

  void update();

private:
  void kick();

  void motor(int num, int motorSpeed);

  void omnidrive();

  void onReceive(const int numBytes);
};
#endif //MOTORDRIVERMB_BOT_H