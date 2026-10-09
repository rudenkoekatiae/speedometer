# Arduino Speedometer

An Arduino-based speed measurement system developed as a project for **UCU RoboClub**.

## Overview

This project is designed to measure the speed of a moving object using two HC-SR04 ultrasonic sensors. The system detects the object's movement between two measurement points and calculates its average speed based on the distance between the sensors and the elapsed time.

The project also includes an OLED display for visualizing measurements and a servo motor as an additional hardware component.

## Hardware Components

- Arduino Nano
- 2 × HC-SR04 ultrasonic sensors
- OLED display
- Servo motor
- Connecting wires and power supply
- Custom 3D-printed components

## How It Works

1. The first ultrasonic sensor detects a moving object.
2. The system starts measuring the elapsed time.
3. The second sensor detects the object.
4. Arduino calculates the object's average speed.
5. The result can be displayed on the OLED screen.

The speed is calculated using:

**Speed = Distance / Time**

where:
- **Distance** is the known distance between the measurement points.
- **Time** is the elapsed time between detections.

## Technologies

- Arduino
- C/C++
- Ultrasonic distance sensing
- Embedded systems
- OLED display
- 3D modeling and printing

## 3D Models

The repository includes 3D model files for the speedometer's mechanical components, available in STL format.

## Project Context

Developed for **UCU RoboClub** as a hands-on embedded systems project combining sensors, microcontroller programming, speed measurement, and mechanical design.
