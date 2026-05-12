# Lower Limb Rehabilitation Device

A wearable IoT-based rehabilitation system designed for lower limb motion tracking, gait analysis, and rehabilitation monitoring using IMU and pressure sensors.

---

# Overview

The Lower Limb Rehabilitation Device is developed to assist in physiotherapy and rehabilitation assessment by continuously monitoring lower limb movement and gait parameters in real time.

The system uses multiple inertial sensors and force sensing elements to capture:
- Knee joint angle
- Ankle joint angle
- Range of Motion (ROM)
- Gait phase
- Step count
- Cadence
- Foot pressure distribution

The collected data is processed using embedded firmware and transmitted wirelessly for visualization and analysis.

---

# Objectives

- Monitor lower limb rehabilitation exercises
- Measure joint angles accurately
- Detect gait phases during walking
- Analyze rehabilitation progress
- Provide real-time feedback
- Enable wireless monitoring and visualization

---

# Features

- Real-time knee angle tracking
- Real-time ankle angle tracking
- Gait phase detection
- Step counting
- Cadence estimation
- Heel and forefoot pressure monitoring
- BLE/WiFi wireless communication
- Embedded signal processing
- Live visualization dashboard
- Rehabilitation data logging

---

# System Architecture

```text
+-------------------+
|   IMU Sensors     |
| (BHI260AP / MPU)  |
+-------------------+
          |
          v
+-------------------+
|   STM32WB55 MCU   |
| Sensor Processing |
+-------------------+
          |
          v
+-------------------+
| BLE / WiFi Module |
+-------------------+
          |
          v
+-------------------+
| Dashboard / PC /  |
| Mobile Monitoring |
+-------------------+
```

---

# Hardware Components

| Component | Purpose |
|---|---|
| STM32WB55 | Main microcontroller |
| BHI260AP IMU | Motion sensing |
| BMM150 Magnetometer | Orientation correction |
| FSR Sensors | Foot pressure sensing |
| LiPo Battery | Portable power supply |
| BQ21040 | Battery charging IC |
| LP5907 | 1.8V voltage regulation |
| BLE Antenna | Wireless communication |

---

# Sensors Used

## IMU Sensors
Used for:
- Orientation tracking
- Angular velocity measurement
- Motion analysis

Parameters measured:
- Accelerometer data
- Gyroscope data
- Quaternion orientation

---

## FSR Sensors
Used for:
- Heel strike detection
- Toe-off detection
- Pressure distribution analysis

---

# Working Principle

1. IMU sensors capture lower limb motion.
2. FSR sensors detect foot-ground interaction.
3. STM32 processes sensor data.
4. Joint angles and gait parameters are calculated.
5. Processed data is transmitted wirelessly.
6. Dashboard visualizes rehabilitation metrics.

---

# Measured Parameters

| Parameter | Description |
|---|---|
| Knee Angle | Knee joint flexion angle |
| Ankle Angle | Ankle joint angle |
| Knee ROM | Knee range of motion |
| Ankle ROM | Ankle range of motion |
| Gait Phase | Current walking phase |
| Step Count | Number of steps |
| Cadence | Steps per minute |
| Heel Load | Pressure on heel |
| Forefoot Load | Pressure on forefoot |

---

# Gait Cycle Detection

The device detects major gait phases including:
- Heel Strike
- Foot Flat
- Mid Stance
- Heel Off
- Toe Off
- Swing Phase

---

# Software Stack

| Software | Purpose |
|---|---|
| STM32CubeIDE | Firmware development |
| MATLAB | Motion visualization |
| Arduino IDE | Sensor testing |
| BLE Stack | Wireless communication |
| GitHub | Version control |

---

# Data Output Format

```json
{
  "knee": 32.5,
  "ankle": 12.3,
  "knee_rom": 45.2,
  "ankle_rom": 18.1,
  "phase": "heel_strike",
  "steps": 120,
  "cadence": 92.5,
  "heel_load": 40.2,
  "forefoot_load": 58.7
}
```

---

# Firmware Workflow

```text
START
  ↓
Initialize Sensors
  ↓
Read IMU Data
  ↓
Read FSR Data
  ↓
Filter Sensor Signals
  ↓
Calculate Joint Angles
  ↓
Detect Gait Phase
  ↓
Compute Rehabilitation Metrics
  ↓
Transmit Data via BLE/WiFi
  ↓
Display on Dashboard
  ↓
Repeat
```

---

# Applications

- Physiotherapy rehabilitation
- Sports injury recovery
- Gait analysis
- Orthopedic assessment
- Elderly mobility monitoring
- Post-surgery rehabilitation

---

# Future Improvements

- AI-based gait classification
- Cloud data analytics
- Mobile application integration
- Real-time physiotherapist feedback
- Machine learning rehabilitation prediction
- Advanced biomechanical modeling

---

# Advantages

- Portable wearable system
- Wireless operation
- Real-time monitoring
- Low power consumption
- Compact design
- Continuous rehabilitation tracking

---

# Project Status

Current Development Stage:
- Sensor integration completed
- Motion tracking implemented
- Gait analysis under development
- Dashboard integration ongoing
- Prototype testing in progress

---

# Folder Structure

```text
Lower_Limb_Rehab_Device/
│
├── Firmware/
├── Hardware/
├── MATLAB/
├── Dashboard/
├── PCB_Design/
├── Documentation/
├── Images/
└── README.md
```

---

# Installation

## Clone Repository

```bash
git clone https://github.com/YOUR_USERNAME/Lower_Limb_Rehab_Device.git
```

---

# Build Firmware

1. Open STM32CubeIDE
2. Import project
3. Build firmware
4. Flash to STM32WB55

---

# MATLAB Visualization

1. Open MATLAB scripts
2. Connect serial/BLE device
3. Run visualization script

---

# Contributors

| Name | Contribution |
|---|---|
| Abinav S | Hardware design, embedded systems integration, sensor interfacing, PCB development, firmware development, ESP/STM32 programming, wireless communication |
| Arvind S | Software development, web dashboard design, data visualization, frontend integration, rehabilitation monitoring interface |

---

# License

This project is developed for academic and research purposes.

---
