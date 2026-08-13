#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 0.42" OLED (72x40, SSD1306) — I2C on SDA=5, SCL=6
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 6, /* data=*/ 5);

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

void show(const char *msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tr);
  u8g2.drawStr(2, 26, msg);
  u8g2.sendBuffer();
}

void setup() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_ENB, OUTPUT);
  setMotors(0, 0);

  u8g2.begin();
  show("Ready...");
  delay(5000);  // time to set the car down (or hold it up!)
}

void loop() {
  show("FORWARD");
  setMotors(SPEED, SPEED);
  delay(2000);

  show("STOP");
  setMotors(0, 0);
  delay(1000);

  show("BACK");
  setMotors(-SPEED, -SPEED);
  delay(2000);

  show("STOP");
  setMotors(0, 0);
  delay(1000);

  show("SPIN L");
  setMotors(-SPEED, SPEED);
  delay(1500);

  show("SPIN R");
  setMotors(SPEED, -SPEED);
  delay(1500);

  show("REST");
  setMotors(0, 0);
  delay(3000);
}
