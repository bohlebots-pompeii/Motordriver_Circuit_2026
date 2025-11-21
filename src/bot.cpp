//
// Created by julius on 11.11.2025.
//
#include <bot.h>
#include <Arduino.h>
#include <config.h>
#include <elapsedMillis.h>
#include <Wire.h>
#include <iostream>
#include <format>
#include <bitset>

elapsedMillis lastKick;

void kick(const int time) {
  if (lastKick <= 500) return; // safety check for last kick

  digitalWrite(kickPin, HIGH);
  delay(time);
  digitalWrite(kickPin, LOW);

  lastKick = 0;
}

void motor(const int num, const int motorSpeed) {
  if (num < 0 || num > 3) return;
  if (motorSpeed < 0) {
    digitalWrite(dir[num], LOW);
  } else {
    digitalWrite(dir[num], HIGH);
  }
  std::clamp(motorSpeed, -100, 100);
  ledcWrite(pwm[num], map(abs(motorSpeed), 0, 100, static_cast<long>(255*0.1), static_cast<long>(255*0.9)));
}

void omnidrive(int vx, int vy, const int rotation) {
  std::array<int, 4> speeds{};

  vx = map(0, 255, 100, -100, vx); // remap
  vy = map(0, 255, 100, -100, vy); // remap

  //vy = -vy;
  //vx = -vx;

  speeds[0] = -vy - vx + rotation;
  speeds[1] = vy - vx + rotation;
  speeds[2] = vy + vx + rotation;
  speeds[3] = -vy + vx + rotation;

  for (int i = 0; i < 4; i++) {
    motor(i, speeds[i]);
  }
}

void onReceive(const int numBytes) {
  std::vector<uint8_t> data;

  while (Wire.available()) {
    data.push_back(Wire.read());
  }

  for (int byte : data) {
    std::cout << std::format("{} ", byte);
  }
  std::cout << std::endl;

  //
  // Bytes that will be send by master
  // -------------------------------
  // byte | byte | byte | byte | byte
  // -------------------------------
  // 1. byte 0x01 > ena
  // 1. byte 0x02 > kick
  // 2. byte vx
  // 3. byte vy
  // 4. byte rotation
  // 5. byte dribbler
  //

  // byte 1
  const std::bitset<8> bits(data[0]);

  if (bits[0]) { // is bot enabled
    digitalWrite(ena, HIGH);
  } else {
    digitalWrite(ena, LOW);
  }
  if (bits[1]) { // should kick
    kick(kickDuration);
  }

  int vx = 0;
  int vy = 0;
  // byte 2 vx -> drive
  vx = static_cast<int>(data[1]);
  // byte 3 vy -> drive
  vy = static_cast<int>(data[2]);
  // byte 4 rotation -> drive
  const int rotation = data[3];
  // byte 5 dribbler -> dribbler
  const int dribbler = static_cast<int>(data[4]);

  // exec
  omnidrive(vx, vy, rotation);
}

void init() {
  pinMode(ena, OUTPUT);
  pinMode(kickPin, OUTPUT);

  Wire.begin(0x69);
  Wire.onReceive(onReceive);

  for (int i = 0; i < 4; i++) {
    pinMode(pwm[i], OUTPUT);
    pinMode(dir[i], OUTPUT);
    digitalWrite(dir[i], HIGH);
    ledcAttach(pwm[i], 1000, 8);
    motor(i, 0);
  }
}

void update() {

}