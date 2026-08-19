# Robo Club C3 — 2WD Robot Car

A beginner-friendly robot car built from a 2WD smart car chassis kit, an ESP32-C3 SuperMini, and an L298N motor driver. The robot hosts its own Wi-Fi network with a web page to drive it. This README walks you through the full wiring and how to flash the firmware.

## Parts

| Part | Notes |
|---|---|
| ESP32-C3 SuperMini | No OLED needed — control is via the web page |
| HW-566 expansion board | The SuperMini plugs into it; breaks out all pins |
| L298N motor driver module | Red module with heatsink and screw terminals |
| 2WD chassis kit (yourDroid) | Chassis, 2 TT gear motors, caster wheel, 4×AA battery box |
| Jumper wires | 6 signal wires + 2 power wires |

## Wiring

Three things to connect: battery → L298N, motors → L298N, and L298N → ESP32.

### 1. Battery pack → L298N

| Battery box | L298N terminal |
|---|---|
| Red (+) | `+12V` (sometimes labeled VCC/VMS) |
| Black (−) | `GND` |

Keep the **5V-EN jumper on the L298N in place** — it enables the onboard 5V regulator that powers the ESP32.

### 2. Motors → L298N

| Motor | L298N terminal |
|---|---|
| Left motor (2 wires) | `OUT1` + `OUT2` |
| Right motor (2 wires) | `OUT3` + `OUT4` |

Polarity only sets the spin direction. If a wheel turns the wrong way when driving, either swap that motor's two wires at the terminal, or flip its direction in software (see `setMotors()` in [src/main.cpp](src/main.cpp) — the OUT3/OUT4 motor is already reversed there to match how this robot is wired).

### 3. L298N → ESP32-C3 (via HW-566)

**Power:**

| L298N | HW-566 / SuperMini |
|---|---|
| `5V` | The **5V pin** of the SuperMini (not a 3.3V/VCC rail!) |
| `GND` | Any `G` pin |

The L298N has only one GND screw terminal — the battery black wire and the ESP32 ground wire share it (twist them together under the same screw, or use a spare GND header pin near IN1–IN4 if your module has one). This **common ground is mandatory**: without it, the control signals have no reference and nothing works.

**Signal wires** (chosen to avoid the C3 strapping pins 2, 8, 9 — GPIO 8 drives the onboard status LED):

| L298N pin | ESP32-C3 GPIO | Function |
|---|---|---|
| `ENA` | GPIO 0 | Left speed (PWM) |
| `IN1` | GPIO 1 | Left direction |
| `IN2` | GPIO 3 | Left direction |
| `IN3` | GPIO 4 | Right direction |
| `IN4` | GPIO 7 | Right direction |
| `ENB` | GPIO 10 | Right speed (PWM) |

## ⚠️ Warnings — read before powering on

- **Remove the two small jumper caps on ENA and ENB** before connecting the signal wires. With the caps on, the motors are locked at full speed and PWM speed control does nothing. (The 5V-EN jumper stays on.)
- **Never power the ESP32 from USB and the L298N 5V at the same time.** Before plugging in the USB cable to flash, switch the battery pack off (or pull out one battery). Unplug USB before switching the battery back on.
- **Never connect 5V to a 3.3V pin.** The L298N's 5V output goes to the SuperMini's 5V pin only — the ESP32-C3 chip itself runs on 3.3V and the 5V pin feeds its onboard regulator.
- **Common ground is not optional.** Battery (−), L298N GND, and ESP32 GND must all be connected together.
- **First test with the wheels off the ground**, so a wiring mistake doesn't send the car off the table.

## Power notes

- The 4×AA pack gives ~6V. The L298N is an old chip that drops about 2V internally, so the motors only see ~4V — they run, but leisurely. For more speed use 6×AA or a 2S (7.4V) battery on the same `+12V` terminal.
- If the board resets or the Wi-Fi drops while motors run, the batteries are sagging under load — use fresh batteries or the bigger pack.

## Building and flashing

This is a [PlatformIO](https://platformio.org/) project (VS Code + PlatformIO extension).
But the main program is compatible with Arduino IDE.

1. Open the project folder in VS Code.
2. **Switch the battery pack off**, then connect the board via USB.
3. Run **Upload** (PlatformIO: `pio run -t upload`).
4. Unplug USB, switch the battery on, and set the car down.

## Driving the robot

The ESP32 starts its own Wi-Fi access point instead of joining your home/club network:

- **Wi-Fi network:** `RoboticsClub-C3`
- **Password:** `roboticsclub1`
- **Control page:** connect to that network, then open `http://192.168.4.1` in a browser

The page shows a D-pad (Forward/Back/Left/Right + a center Stop button). Press and hold a direction to drive; release to stop. The onboard LED lights up while the motors are moving.

### Safety watchdog

If the phone loses the Wi-Fi connection (walks out of range, screen locks, browser backgrounds the tab) while a direction is held, the robot could otherwise keep driving forever on the last command it received. To prevent that, the firmware auto-stops the motors if no command arrives for **3 seconds**. While a button is held, the web page sends a keep-alive command once per second, so this only kicks in when the connection actually drops — a normal long press is never cut off.

## Troubleshooting

| Symptom | Likely cause / fix |
|---|---|
| Wheel spins the wrong way | Swap that motor's two wires at OUT1/OUT2 or OUT3/OUT4, or flip the sign logic for that side in `setMotors()` |
| Left/right swapped when turning | Motor pairs are swapped — exchange them at the terminals |
| Motor hums or doesn't move | ENA/ENB cap still on, loose signal wire, or weak batteries |
| Board resets / Wi-Fi drops under load | Battery voltage sag — fresh or bigger batteries |
| Can't see `RoboticsClub-C3` network | Board isn't powered, or crashed on boot — check Serial monitor at 115200 baud over USB |
| Robot keeps driving after you let go / lose connection | Should self-stop within 3 seconds via the watchdog — if not, check that the phone and board are actually exchanging `/cmd` requests (Serial monitor prints each command received) |
