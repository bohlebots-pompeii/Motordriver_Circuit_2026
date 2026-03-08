//
// Created by julius on 10.11.2025.
//
#ifndef MOTORDRIVERMB_CONFIG_H
#define MOTORDRIVERMB_CONFIG_H

#include <array>

constexpr std::array pwm = {25, 33, 5, 14, 2};

constexpr std::array dir = {26, 32, 19, 27, 4};

constexpr int kickPin = 23;
constexpr int ena = 13;

constexpr unsigned long kickDuration = 20;
constexpr unsigned long kickCooldown = 500;

#endif //MOTORDRIVERMB_CONFIG_H