//
// Motordrive circuit by Julius Gerhardus on 27.10.25
//

#include <Arduino.h>
#include <memory>
#include <bot.h>

std::unique_ptr<Bot> bot;

void setup() {
  bot->init();
}

void loop() {
  bot->update();
}