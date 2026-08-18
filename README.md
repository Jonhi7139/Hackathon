# Hackathon Project

A real-time telemetry and data acquisition system developed during the hackathon, designed to process multi-axis orientation and environmental data from a composite 13-degree-of-freedom (13DOF) sensor suite.

---

## Overview

This repository contains the source code, hardware configuration, and processing scripts for our hackathon project. The core system reads, filters, and processes real-time spatial motion and environmental telemetry using an integrated 13DOF sensor unit composed of three distinct physical sensors working together.

---

## Key Features

* **13-DOF Telemetry Fusion:** Collects real-time motion, orientation, heading, and environmental readings.
* **Integrated 3-Sensor Module:** Combines output from 3 distinct physical onboard sensors into a single unified stream.
* **Data Processing & Filtering:** Filters noise from raw sensor feeds for reliable output.
* **Lightweight & Modular:** Optimized code structure built for rapid deployment and hackathon prototyping.

---

## Hardware Architecture

The core tracking module uses a **13DOF Sensor Assembly** containing **3 distinct physical sensors**:

1. **6-Axis Motion Sensor (IMU):** 3-axis accelerometer + 3-axis gyroscope for motion and tilt tracking.
2. **3-Axis Magnetometer:** Digital compass for heading and magnetic field orientation.
3. **Barometric & Environmental Sensor:** Atmospheric pressure and altitude estimation (plus temperature).

---

## Repository Structure

```text
├── src/            # Source code (microcontroller firmware / main scripts)
├── lib/            # Custom hardware drivers and sensor libraries
├── docs/           # Hardware schematics and project documentation
└── README.md       # Project documentation
