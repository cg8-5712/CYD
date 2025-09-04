#include <Arduino.h>

// Define constants for pin and delay to avoid magic numbers
const int LED_PIN = 16;
const int BLINK_DELAY_MS = 500;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(BLINK_DELAY_MS);
  digitalWrite(LED_PIN, LOW);
  delay(BLINK_DELAY_MS);
}
