# Interactive Learning Board

**🏆 1st Place — LMU NSBE Hackathon**

An Arduino-based educational circuit board that teaches analog and digital circuit concepts through hands-on, interactive challenges. Built by our four-person team in 8 hours for the LMU NSBE Hackathon, where the project earned 1st place.

## Overview

The board presents users with three circuit challenges — Resistor, Diode, and Capacitor — navigated through a joystick-controlled OLED menu. Each level measures a real voltage from a physical circuit and gives the user live feedback on whether the target condition has been met, reinforcing the real-world behavior of each component.

## Team

- Aisaiah Maguadog
- Max Bispham
- Josephine Baiden
- Wintana Araya

## Features

- Joystick-navigated menu with debounced input and scroll sound feedback
- 128×64 I²C OLED display (SSD1306) showing live voltage readings
- Green/red LED feedback for pass/fail status
- Piezo buzzer with success jingle on level completion
- Three interactive analog circuit challenges
- 3D-printed custom enclosure

## Levels

| Level | Component | Challenge |
|---|---|---|
| 1 | Resistor | Use a resistor divider to hit a randomly selected target voltage (1.66 V, 3.58 V, or 4.15 V) within ±0.1 V |
| 2 | Diode | Bias a diode circuit to produce 2.68 V ± 0.10 V at the output |
| 3 | Capacitor | Connect a charged capacitor and let it discharge; if it takes more than 500 ms to drop below 0.2 V, it is classified as a large capacitor and the level is completed |

## Hardware

- Arduino Mega 2560
- SSD1306 128×64 OLED display (I²C)
- Analog joystick module
- Piezo buzzer
- Green and red LEDs
- Breadboard
- Resistors
- Diodes
- Capacitors
- 3D-printed enclosure

## Dependencies

- [SSD1306Ascii](https://github.com/greiman/SSD1306Ascii)
- Arduino `Wire.h` library

## Project Context

This project was developed during an 8-hour LMU NSBE Hackathon. The goal was to create an interactive educational device that could help users understand basic analog circuit behavior through physical experimentation rather than simulation alone.

The finished board combined embedded programming, analog measurements, user-interface design, physical circuit construction, and rapid prototyping into a single working demonstration.
