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
  if (numBytes < sizeof(MotorCmd)) return;

  MotorCmd cmd{};

  // read bytes into struct from wire
  auto* p = reinterpret_cast<uint8_t*>(&cmd);
  for (int i = 0; i < sizeof(MotorCmd); i++) {
    if (!Wire.available()) return;
    p[i] = Wire.read();
  }

  digitalWrite(ena, (cmd.flags & 0x01) ? HIGH : LOW); // ena
  if (cmd.flags & 0x02) kick(kickDuration); // kick

  // different velos
  _vx = cmd.vx;
  _vy = cmd.vy;
  _rotation = cmd.rot;
  _dribblerSpeed = cmd.drib;
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