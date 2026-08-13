# Robo Club C3 — 2WD Robot Car

A beginner-friendly robot car built from a 2WD smart car chassis kit, an ESP32-C3 SuperMini (without OLED), and an L298N motor driver. This README walks you through the full wiring and how to flash the motor test program.

## Parts

| Part | Notes |
|---|---|
| ESP32-C3 SuperMini | The version without OLED on board |
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

Polarity only sets the spin direction. If a wheel turns the wrong way during the test, swap that motor's two wires at the terminal.

### 3. L298N → ESP32-C3 (via HW-566)

**Power:**

| L298N | HW-566 / SuperMini |
|---|---|
| `5V` | The **5V pin** of the SuperMini (not a 3.3V/VCC rail!) |
| `GND` | Any `G` pin |

The L298N has only one GND screw terminal — the battery black wire and the ESP32 ground wire share it (twist them together under the same screw, or use a spare GND header pin near IN1–IN4 if your module has one). This **common ground is mandatory**: without it, the control signals have no reference and nothing works.

**Signal wires** (chosen to avoid GPIO 5/6, which the OLED uses, and the C3 strapping pins 2, 8, 9):

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
- **First test with the wheels off the ground.** The program waits 5 seconds after boot before moving.

## Power notes

- The 4×AA pack gives ~6V. The L298N is an old chip that drops about 2V internally, so the motors only see ~4V — they run, but leisurely. For more speed use 6×AA or a 2S (7.4V) battery on the same `+12V` terminal.
- If the RBG LED on board doesn't light up or the board restarts while motors run, the batteries are sagging under load — use fresh batteries or the bigger pack.

## Building and flashing

This is a [PlatformIO](https://platformio.org/) project (VS Code + PlatformIO extension).
But the main program is compatible with Arduino IDE.

1. Open the project folder in VS Code.
2. **Switch the battery pack off**, then connect the board via USB.
3. Run **Upload** (PlatformIO: `pio run -t upload`).
4. Unplug USB, switch the battery on, and set the car down.

## What the test program does

[src/main.cpp](src/main.cpp) runs this loop forever:

```
FORWARD (2 s) → STOP → BACK (2 s) → STOP → SPIN L → SPIN R → REST (3 s)
```

Motor speed is set to 200 out of 255 (`SPEED` constant).

## Troubleshooting

| Symptom | Likely cause / fix |
|---|---|
| Wheel spins the wrong way | Swap that motor's two wires at OUT1/OUT2 or OUT3/OUT4 |
| Left/right swapped when spinning | Motor pairs are swapped — exchange them at the terminals |
| Motor hums or doesn't move | ENA/ENB cap still on, loose signal wire, or weak batteries |
| LED on board doesn't light up / board restarts under load | Battery voltage sag — fresh or bigger batteries |
| `bus is not initialized` / `NULL TX buffer` in serial monitor | Wrong I2C pins for this board check the board version |
