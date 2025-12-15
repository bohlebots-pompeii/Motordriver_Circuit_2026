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

int8_t Bot::_vx = 0;
int8_t Bot::_vy = 0;
int Bot::_rotation = 0;
int Bot::_dribblerSpeed = 0;

void Bot::kick(const int time) {
  if (lastKick <= 500) return; // safety check for last kick

  digitalWrite(kickPin, HIGH);
  delay(time);
  digitalWrite(kickPin, LOW);

  lastKick = 0;
}

void Bot::motor(const int num, const int motorSpeed) {
  if (num < 0 || num > 3) return;

  const int speed = std::clamp(motorSpeed, -100, 100); // assign result

  if (speed < 0) {
    digitalWrite(dir[num], LOW);
  } else {
    digitalWrite(dir[num], HIGH);
  }

  const int pwmVal = map(abs(speed), 0, 100, static_cast<long>(255*0.1), static_cast<long>(255*0.9));
  ledcWrite(pwm[num], pwmVal);
}


void Bot::omnidrive(const int vx, const int vy, const int rotation) {
  std::array<int16_t, 4> speeds{};

  speeds[0] = -vy - vx + rotation;
  speeds[1] =  vy - vx + rotation;
  speeds[2] =  vy + vx + rotation;
  speeds[3] = -vy + vx + rotation;

  for (int i = 0; i < 4; i++) {
    speeds[i] = constrain(speeds[i], -100, 100);
    motor(i, speeds[i]);
  }
}


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

void Bot::onReceive(const int numBytes) {
  std::vector<uint8_t> data;

  while (Wire.available()) {
    data.push_back(static_cast<uint8_t>(Wire.read()));
  }

  if (data.size() < 5) return; // safety

  const std::bitset<8> bits(data[0]);

  digitalWrite(ena, bits[0] ? HIGH : LOW);
  if (bits[1]) kick(kickDuration);

  const uint8_t raw_vx = data[1];
  const uint8_t raw_vy = data[2];
  _vx = map(static_cast<int>(raw_vx), 0, 255, -100, 100);
  _vy = map(static_cast<int>(raw_vy), 0, 255, -100, 100);

  _rotation = static_cast<int>(static_cast<int8_t>(data[3]));
  _dribblerSpeed = static_cast<int>(static_cast<int8_t>(data[4]));
}

void Bot::init() {
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

void Bot::update() {
  omnidrive(_vx, _vy, _rotation);
}