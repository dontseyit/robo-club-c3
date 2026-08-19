#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

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

const char *AP_SSID = "RoboticsClub-C3";
const char *AP_PASSWORD = "roboticsclub1";  // min 8 chars

WebServer server(80);

const unsigned long WATCHDOG_MS = 3000;  // auto-stop if no command received in time
bool moving = false;
unsigned long lastCmdMs = 0;

// left/right: -255 (full reverse) .. 255 (full forward)
void setMotors(int left, int right) {
  digitalWrite(PIN_IN1, left >= 0 ? HIGH : LOW);
  digitalWrite(PIN_IN2, left >= 0 ? LOW : HIGH);
  analogWrite(PIN_ENA, abs(left));

  digitalWrite(PIN_IN3, right >= 0 ? LOW : HIGH);  // reversed: right motor is wired backward
  digitalWrite(PIN_IN4, right >= 0 ? HIGH : LOW);
  analogWrite(PIN_ENB, abs(right));

  digitalWrite(PIN_LED, (left == 0 && right == 0) ? HIGH : LOW);  // LED on while moving
}

const char PAGE[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>Robotics Club C3</title>
<style>
  body { font-family: sans-serif; text-align: center; background: #111; color: #eee; margin: 0; padding: 20px; }
  h1 { font-size: 20px; }
  .pad { display: grid; grid-template-columns: 80px 80px 80px; grid-template-rows: 80px 80px 80px; gap: 10px; justify-content: center; margin: 30px auto; }
  button { font-size: 28px; border-radius: 12px; border: none; background: #2a6; color: white; touch-action: manipulation; user-select: none; }
  button:active { background: #1a4; }
  #stop { background: #c33; }
  #stop:active { background: #a11; }
  .fwd { grid-column: 2; grid-row: 1; }
  .left { grid-column: 1; grid-row: 2; }
  .stopbtn { grid-column: 2; grid-row: 2; }
  .right { grid-column: 3; grid-row: 2; }
  .back { grid-column: 2; grid-row: 3; }
</style>
</head>
<body>
<h1>Robotics Club C3 Control</h1>
<div class="pad">
  <button class="fwd" data-d="F">&uarr;</button>
  <button class="left" data-d="L">&larr;</button>
  <button id="stop" class="stopbtn" data-d="S">&#9632;</button>
  <button class="right" data-d="R">&rarr;</button>
  <button class="back" data-d="B">&darr;</button>
</div>
<script>
function send(d) { fetch('/cmd?d=' + d); }
var holdTimer = null;
document.querySelectorAll('button[data-d]').forEach(function(btn) {
  var d = btn.getAttribute('data-d');
  if (d === 'S') {
    btn.addEventListener('click', function() { send('S'); });
    return;
  }
  var start = function(e) {
    e.preventDefault();
    send(d);
    clearInterval(holdTimer);
    holdTimer = setInterval(function() { send(d); }, 1000);  // keep-alive while held
  };
  var stop = function(e) {
    e.preventDefault();
    clearInterval(holdTimer);
    send('S');
  };
  btn.addEventListener('touchstart', start);
  btn.addEventListener('touchend', stop);
  btn.addEventListener('touchcancel', stop);
  btn.addEventListener('mousedown', start);
  btn.addEventListener('mouseup', stop);
  btn.addEventListener('mouseleave', stop);
});
</script>
</body>
</html>
)HTML";

void handleRoot() {
  server.send_P(200, "text/html", PAGE);
}

void handleCmd() {
  String d = server.arg("d");
  if (d == "F") setMotors(SPEED, SPEED);
  else if (d == "B") setMotors(-SPEED, -SPEED);
  else if (d == "L") setMotors(-SPEED, SPEED);
  else if (d == "R") setMotors(SPEED, -SPEED);
  else setMotors(0, 0);  // "S" or anything else = stop

  moving = (d == "F" || d == "B" || d == "L" || d == "R");
  lastCmdMs = millis();

  Serial.println(d);
  server.send(200, "text/plain", "ok");
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

  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("AP started. Connect to Wi-Fi \"");
  Serial.print(AP_SSID);
  Serial.println("\" and open http://192.168.4.1");

  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);
  server.begin();
}

void loop() {
  server.handleClient();

  if (moving && millis() - lastCmdMs > WATCHDOG_MS) {
    Serial.println("Watchdog: no command received, stopping");
    setMotors(0, 0);
    moving = false;
  }
}
