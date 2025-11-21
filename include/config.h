//
// Created by julius on 10.11.2025.
//
#ifndef MOTORDRIVERMB_CONFIG_H
#define MOTORDRIVERMB_CONFIG_H

#include <array>

constexpr std::array pwm = {25, 33, 5, 14};

constexpr std::array dir = {26, 32, 19, 27};

constexpr int kickPin = 23;

constexpr int ena = 13;

constexpr int kickDuration = 23;

#endif //MOTORDRIVERMB_CONFIG_H