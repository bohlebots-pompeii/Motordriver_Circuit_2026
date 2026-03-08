//
// Motordrive circuit by Julius Gerhardus on 27.10.25
//

#include <Arduino.h>
#include <bot.h>

void setup() {
  delay(300); // wait for the other i2c to register
  Bot::init();
}

void loop() {
  Bot::update();
}