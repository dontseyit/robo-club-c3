#include <Arduino.h>

// ESP32-C3 SuperMini (no OLED) — status via Serial and onboard LED (GPIO 8, active LOW)
const int PIN_LED = 8;

// L298N wiring
const int PIN_ENA = 0;   // left speed (PWM)
const int PIN_IN1 = 1;   // left direction
const int PIN_IN2 = 3;   // left direction
const int PIN_IN3 = 4;   // right direction
const int PIN_IN4 = 7;   // right direction
const int PIN_ENB = 10;  // right speed (PWM)

const int SPEED = 200;   // 0..255

// left/right: -255 (full reverse) .. 255 (full forward)
void setMotors(int left, int right) {
  digitalWrite(PIN_IN1, left >= 0 ? HIGH : LOW);
  digitalWrite(PIN_IN2, left >= 0 ? LOW : HIGH);
  analogWrite(PIN_ENA, abs(left));

  digitalWrite(PIN_IN3, right >= 0 ? HIGH : LOW);
  digitalWrite(PIN_IN4, right >= 0 ? LOW : HIGH);
  analogWrite(PIN_ENB, abs(right));
}

void show(const char *msg, bool moving) {
  Serial.println(msg);
  digitalWrite(PIN_LED, moving ? LOW : HIGH);  // LED on while moving
}

void setup() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_ENB, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  setMotors(0, 0);

  Serial.begin(115200);
  show("Ready...", false);
  // blink while waiting so there is a visible "about to start" signal
  for (int i = 0; i < 10; i++) {
    digitalWrite(PIN_LED, i % 2 ? HIGH : LOW);
    delay(500);
  }
}

void loop() {
  show("FORWARD", true);
  setMotors(SPEED, SPEED);
  delay(2000);

  show("STOP", false);
  setMotors(0, 0);
  delay(1000);

  show("BACK", true);
  setMotors(-SPEED, -SPEED);
  delay(2000);

  show("STOP", false);
  setMotors(0, 0);
  delay(1000);

  show("SPIN L", true);
  setMotors(-SPEED, SPEED);
  delay(1500);

  show("SPIN R", true);
  setMotors(SPEED, -SPEED);
  delay(1500);

  show("REST", false);
  setMotors(0, 0);
  delay(3000);
}
