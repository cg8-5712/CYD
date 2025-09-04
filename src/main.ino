#include <Arduino.h>

void setup() {
  pinMode(16, OUTPUT);
}

void loop() {
  digitalWrite(16, HIGH);
  delay(500);
  digitalWrite(16, LOW);
  delay(500);
}