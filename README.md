# FlipWalker

An ESP32-based bipedal robot that locomotes by flipping its body segments forward using servos, guided by magnetic joint angle sensors.

---

## Hardware

- **MCU:** ESP32
- **Joint Sensors:** Adafruit MLX90393 3-axis magnetometer (x2)
  - Sensor 1: I2C address `0x18` (child-behind-parent side)
  - Sensor 2: I2C address `0x1A` (child-ahead-parent side)
- **Servos:** 2x (parent body, child body)
- **DC Motors:** 2x H-bridge driven

---

## Robot States

The robot determines its configuration by which magnetic sensor detects the magnet embedded in the joint.

| State | Sensor Active | Description |
|-------|--------------|-------------|
| `cBp` | Sensor 1 (0x18) | Child is **behind** parent |
| `cAp` | Sensor 2 (0x1A) | Child is **ahead** of parent |
| `UNKNOWN` | Both or neither | Mid-flip or error |

---

## Flip Forward Logic

Each call to `flip` executes one half of the full locomotion cycle. Call it again to complete the next half.

### Flip from `cBp` (child behind parent):
1. Child servo → 0°
2. Wait 1.5s (let the body swing)
3. Wait until both sensors stop detecting (body stacked on top of parent)
4. Child servo → 90° (return to neutral)
5. Wait 2s (settle)
6. Parent servo → 0°
7. Wait 1.5s (settle)
8. Robot is now in `cAp`

### Flip from `cAp` (child ahead of parent):
1. Parent servo → 180°
2. Wait 1.5s (let the body swing)
3. Wait until both sensors stop detecting (body stacked on top of child)
4. Parent servo → 90° (return to neutral)
5. Wait 2s (settle)
6. Child servo → 180°
7. Wait 1.5s (settle)
8. Robot is now in `cBp`

There is no flip backward — only flip forward is implemented.

> The servo that initiates the flip returns to 90° neutral during the stacked mid-point before the second servo finalises the position. This prevents mechanical conflict during the transition.

---

## Calibration Instructions

Calibration maps each sensor's raw magnetic heading values to known physical joint angles. **Run this after any physical reassembly or if sensor readings look wrong.**

### Steps

1. **Flash** `src/Calibration.cpp` as the active source file (comment out `src/main.cpp`).

2. Open the **Serial Monitor** at `115200` baud.

3. Press any key to start. The calibration will display:
   - Both sensor I2C addresses
   - Live magnitude readings for each sensor
   - The magnet detection threshold (`minMagnitude = 600`)

4. Type `1` or `2` to choose which sensor to calibrate first, then press Enter.

5. For each of the **7 angle steps** (`-90°, -60°, -30°, 0°, 30°, 60°, 90°`):
   - Physically move the joint to the indicated angle
   - Ensure the magnet magnitude is above 600 (sensor will warn you if not)
   - Press any key to capture 50 averaged readings

6. The calibration prints a ready-to-paste line:
   ```
   static const float RAW_READINGS_LEG_1[7] = { ... };
   ```

7. Repeat for the second sensor.

8. **Paste both arrays** into `include/params.hpp` under `Params::MagSensorValues`.

9. The shared `PHYSICAL_ANGLES` array stays the same — do not modify it.

### Verifying Calibration

Flash `test/magTesting.cpp` and move each joint through its range. The raw heading should span at least 20–30° across the full travel. If the range is less than 5°, the magnet is too far from the sensor.

---

## Serial Commands (`mainNoWifi.cpp`)

| Command | Action |
|---------|--------|
| `r` | Read magnitude + presence for both sensors |
| `state` | Print current robot state (`cBp`, `cAp`, or `UNKNOWN`) |
| `flip` | Execute one flip step based on current state |
| `p<angle>` | Set parent servo to angle (0–180) |
| `c<angle>` | Set child servo to angle (0–180) |

---

## Appendix — File Descriptions

### `src/`

| File | Description |
|------|-------------|
| `main.cpp` | Primary production firmware. Includes WiFi, web server, servo and motor control, and dual magnetometer calibration. |
| `Magnetometer.cpp` | MLX90393 sensor class. Handles initialization, raw heading, magnitude, magnet presence detection, 3-point calibration (`getHeading`), and 7-point LUT interpolation (`getLinearHeading`). Returns `NAN` on any heading call when no magnet is detected. |
| `MagnetometerCalibration.cpp` | Calibration engine. Captures 50-sample averaged readings at each angle, normalizes for atan2 wrap-around, sorts raw/angle pairs ascending, and prints copy-paste arrays for `params.hpp`. |
| `Calibration.cpp` | Calibration entry point. Zeros motors on boot, presents sensor selection menu with live magnitude readings, and runs `MagnetometerCalibration.cpp` for each sensor in sequence. |

### `include/`

| File | Description |
|------|-------------|
| `params.hpp` | Central configuration. I2C addresses, pin assignments, WiFi credentials, magnet detection threshold, shared physical angles array, and per-leg raw calibration readings. |
| `Magnetometer.hpp` | Magnetometer class interface. |
| `MagnetometerCalibrator.hpp` | Calibrator class interface. |
| `password.hpp` | WiFi credentials (gitignored). |

### `test/`

| File | Description |
|------|-------------|
| `mainNoWifi.cpp` | No-WiFi firmware with state machine. Detects `cBp`/`cAp` from sensor presence, executes flip-forward logic via serial `flip` command, and allows direct servo/motor control. |
| `magTesting.cpp` | Sensor diagnostic tool. Prints live magnitude and magnet presence for both sensors. Use to verify calibration and magnet proximity before running the robot. |
| `generalTest.cpp` | General hardware test. Accepts serial commands for both servos, both motors, and raw sensor reads with I2C address labels. |
| `MagnetCalibrationMain.cpp` | Standalone calibration verification sketch. Loads params and continuously prints calibrated joint angles. |
| `newMain.cpp` | WiFi-enabled variant of the main firmware with web server interface for remote servo, motor, and sensor control. |
| `comboTest.cpp` | Combined hardware test for early integration testing. |
| `servoTest.cpp` | Isolated servo sweep test. |
| `magTesting.cpp` | Magnetometer axis and heading diagnostic. |
| `findMagSensorAddy.cpp` | I2C scanner to identify connected magnetometer addresses. |
| `oldMain.cpp` | Archived earlier version of main firmware. |
