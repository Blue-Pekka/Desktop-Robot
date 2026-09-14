# **Batmobile Robot**

A custom-built 3D-printed desktop robot designed and developed around an ESP32.

## Overview

I designed and built this robot as a hands-on electronics and robotics project. The chassis was designed in SolidWorks and 3D printed, while the electronics, wiring, programming, and assembly were completed as part of the build.

The robot combines Bluetooth remote control with autonomous obstacle avoidance and provides real-time information through an OLED display.

## Features

- ESP32-based control system
- Custom 3D-printed chassis
- SolidWorks CAD design
- Bluetooth Low Energy (BLE) control
- Autonomous obstacle avoidance
- HC-SR04 ultrasonic distance sensing
- 1.3-inch SH1106 OLED display
- TB6612FNG motor driver
- Two BO motors with 65 mm wheels
- Buzzer feedback
- Adjustable motor speeds
- Rechargeable 7.4V battery system

## How It Works

The ESP32 acts as the main controller of the robot.

In autonomous mode, the HC-SR04 ultrasonic sensor continuously measures the distance in front of the robot. When an obstacle is detected within the programmed threshold, the robot stops, gives an audible warning, reverses briefly, and performs a controlled turn before continuing.

In manual mode, commands received through BLE control the robot's movement.

The OLED displays the robot's current mode, measured distance, speed, and Bluetooth connection status.

## Hardware

| Component | Purpose |
|---|---|
| ESP32 DevKit | Main controller |
| TB6612FNG | Motor driver |
| HC-SR04 | Distance sensing |
| SH1106 OLED | Status display |
| BO Motors | Drive system |
| Buzzer | Obstacle warning |
| 7.4V battery pack | Power source |
| LM2596 Buck Converter | Voltage regulation |

## Software

- Arduino IDE
- C++ / Arduino
- U8g2 OLED library
- ESP32 BLE libraries

## Pin Configuration

### Motor Driver

| TB6612FNG | ESP32 |
|---|---|
| PWMA | GPIO 13 |
| AIN1 | GPIO 14 |
| AIN2 | GPIO 12 |
| PWMB | GPIO 33 |
| BIN1 | GPIO 26 |
| BIN2 | GPIO 25 |
| STBY | GPIO 27 |

### Ultrasonic Sensor

| HC-SR04 | ESP32 |
|---|---|
| TRIG | GPIO 22 |
| ECHO | GPIO 23 |

### OLED

| OLED | ESP32 |
|---|---|
| SDA | GPIO 2 |
| SCL | GPIO 15 |

### Buzzer

| Component | ESP32 |
|---|---|
| Buzzer | GPIO 4 |

## Project Images

The `images` folder contains photographs of the completed robot and screenshots of its CAD design.

## Code

The complete Arduino code used for the robot is available in:

`code/desktop_robot.ino`

## What I Learned

Building the robot involved more than assembling components. I had to work through mechanical design, motor control, sensor limitations, Bluetooth communication, power management, wiring, and debugging.

One of the challenges was dealing with the practical limitations of the ultrasonic sensor. Instead of assuming the sensor would behave ideally, I adapted the software around its reliable operating range.

The project also gave me experience taking a design from CAD to a physical prototype and then integrating hardware and software into a working system.

## Future Improvements

Possible future improvements include:

- More reliable distance sensing
- Improved autonomous navigation
- Additional OLED functions
- Better movement control
- Voice-based control
- More advanced autonomous behaviors
