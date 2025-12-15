//
// Motordrive circuit by Julius Gerhardus on 27.10.25
//

#include <Arduino.h>
#include <bot.h>

void setup() {
  delay(300);
  Bot::init();
}

void loop() {
  Bot::update();
}