# Interactive Learning Board

**🏆 1st Place — LMU NSBE Hackathon**

An Arduino-based educational circuit board that teaches analog and digital circuit concepts through hands-on, interactive challenges. Built in 24 hours for the LMU NSBE Hackathon.

## Overview

The board presents users with three circuit challenges — Resistor, Diode, and Capacitor — navigated via a joystick-controlled OLED menu. Each level measures a real voltage from a physical circuit and gives the user live feedback on whether they've hit the target, reinforcing the real-world behavior of each component.

## Features

- Joystick-navigated menu with debounced input and scroll sound feedback
- 128×64 I²C OLED display (SSD1306) showing live voltage readings
- Green/red LED feedback for pass/fail
- Piezo buzzer with success jingle on level completion
- 3D-printed custom enclosure

## Levels

| Level | Component | Challenge |
|-------|-----------|-----------|
| 1 | Resistor | Use a resistor divider to hit a randomly selected target voltage (1.66 V, 3.58 V, or 4.15 V) within ±0.1 V |
| 2 | Diode | Bias a diode circuit to produce 2.68 V ± 0.10 V at the output |
| 3 | Capacitor | Connect a charged capacitor and let it discharge — if it takes >500 ms to drop below 0.2 V, it's a big cap and you win |

## Hardware

- Arduino Mega 2560
- SSD1306 128×64 OLED (I²C)
- Analog joystick module
- Piezo buzzer
- Green and red LEDs
- Breadboard + passive components (resistors, diodes, capacitors)
- 3D-printed enclosure

## Dependencies

- [SSD1306Ascii](https://github.com/greiman/SSD1306Ascii)
- Arduino `Wire.h` (built-in)
