//
// Motordrive circuit by Julius Gerhardus on 27.10.25
//

#include <Arduino.h>
#include <bot.h>

void setup() {
  Bot::init();
}

void loop() {
  Bot::update();
}