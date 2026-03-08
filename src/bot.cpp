//
// Created by julius on 11.11.2025.
//

#include "bot.h"
#include "config.h"
#include <Wire.h>
#include <algorithm>

volatile int8_t Bot::_vx = 0;
volatile int8_t Bot::_vy = 0;
volatile int8_t Bot::_rotation = 0;
volatile int8_t Bot::_dribblerSpeed = 0;
volatile bool Bot::_kickRequest = false;
volatile unsigned long Bot::_lastCmdTime = 0;

bool Bot::_isKicking = false;
unsigned long Bot::_kickStartTime = 0;
unsigned long Bot::_lastKickTime = 0;

portMUX_TYPE Bot::_mux = portMUX_INITIALIZER_UNLOCKED;

void Bot::handleKick() {
  const unsigned long currentMillis = millis();

  if (_kickRequest && !_isKicking && currentMillis - _lastKickTime >= kickCooldown) {
    _isKicking = true;
    _kickStartTime = currentMillis;
    digitalWrite(kickPin, HIGH);
    _kickRequest = false;
  } else {
    _kickRequest = false;
  }

  if (_isKicking && currentMillis - _kickStartTime >= kickDuration) {
    digitalWrite(kickPin, LOW);
    _isKicking = false;
    _lastKickTime = currentMillis;
  }
}

void Bot::motor(const int num, const int16_t motorSpeed) {
  if (num < 0 || num >= pwm.size()) return;

  const int16_t speed = std::clamp(motorSpeed, static_cast<int16_t>(-100), static_cast<int16_t>(100));

  digitalWrite(dir[num], speed < 0 ? LOW : HIGH);

  const long pwmVal = map(abs(speed), 0, 100, 25, 229); // 10% to 90% duty cycle
  ledcWrite(pwm[num], pwmVal);
}

void Bot::omnidrive(const int8_t vx, const int8_t vy, const int8_t rotation) {
  std::array<int16_t, 4> speeds{};

  speeds[0] = -vy - vx + rotation;
  speeds[1] =  vy - vx + rotation;
  speeds[2] =  vy + vx + rotation;
  speeds[3] = -vy + vx + rotation;

  for (size_t i = 0; i < 4; i++) {
    motor(i, speeds[i]);
  }
}

void Bot::onReceive(const int numBytes) {
  if (numBytes != sizeof(MotorCmd)) {
    while (Wire.available()) Wire.read();
    return;
  }

  MotorCmd cmd{};
  auto* p = reinterpret_cast<uint8_t*>(&cmd);

  for (size_t i = 0; i < sizeof(MotorCmd) && Wire.available(); i++) {
    p[i] = Wire.read();
  }

  digitalWrite(ena, cmd.flags & 0x01 ? HIGH : LOW);

  // ensure that ISR is not being modified in the main loop
  portENTER_CRITICAL_ISR(&_mux);

  if (cmd.flags & 0x02) {
    _kickRequest = true;
  }

  _vx = cmd.vx;
  _vy = cmd.vy;
  _rotation = cmd.rot;
  _dribblerSpeed = cmd.drib;

  portEXIT_CRITICAL_ISR(&_mux);

  _lastCmdTime = millis();
}

void Bot::init() {
  pinMode(ena, OUTPUT);
  pinMode(kickPin, OUTPUT);
  digitalWrite(kickPin, LOW);

  Wire.begin(0x69);
  Wire.onReceive(onReceive);

  for (size_t i = 0; i < pwm.size(); i++) {
    pinMode(pwm[i], OUTPUT);
    pinMode(dir[i], OUTPUT);
    digitalWrite(dir[i], HIGH);
    ledcAttach(pwm[i], 1000, 8);
    motor(i, 0);
  }
}

void Bot::update() {
  handleKick();

  // Failsafe timeout
  if (millis() - _lastCmdTime > 200) {
    omnidrive(0, 0, 0);
    motor(4, 0);
    return;
  }

  // safe data copy
  portENTER_CRITICAL(&_mux);
  const int8_t currentVx = _vx;
  const int8_t currentVy = _vy;
  const int8_t currentRot = _rotation;
  const int8_t currentDrib = _dribblerSpeed;
  portEXIT_CRITICAL(&_mux);

  omnidrive(currentVx, currentVy, currentRot);
  motor(4, currentDrib);
}